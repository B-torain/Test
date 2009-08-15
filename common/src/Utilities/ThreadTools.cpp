/*  Pcsx2 - Pc Ps2 Emulator
 *  Copyright (C) 2002-2009  Pcsx2 Team
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *	Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA
 */

#include "PrecompiledHeader.h"
#include "Threading.h"

using namespace Threading;

namespace Threading
{
	PersistentThread::PersistentThread() :
		m_thread()
	,	m_returncode( 0 )
	,	m_running( false )
	,	m_post_event()
	{
	}

	// Perform a blocking termination of the thread, to ensure full object cleanup.
	PersistentThread::~PersistentThread()
	{
		Cancel( false );
	}

	void PersistentThread::Start()
	{
		if( pthread_create( &m_thread, NULL, _internal_callback, this ) != 0 )
			throw Exception::ThreadCreationError();

		m_running = true;
	}

	// Remarks:
	//   Provision of non-blocking Cancel() is probably academic, since destroying a PersistentThread
	//   object performs a blocking Cancel regardless of if you explicitly do a non-blocking Cancel()
	//   prior, since the ExecuteTask() method requires a valid object state.  If you really need
	//   fire-and-forget behavior on threads, use pthreads directly for now.
	//   (TODO : make a DetachedThread class?)
	//
	// Parameters:
	//   isBlocking - indicates if the Cancel action should block for thread completion or not.
	//
	void PersistentThread::Cancel( bool isBlocking )
	{
		if( !m_running ) return;
		m_running = false;

		pthread_cancel( m_thread );
		if( isBlocking )
			pthread_join( m_thread, (void**)&m_returncode );
		else
			pthread_detach( m_thread );
	}

	// Blocks execution of the calling thread until this thread completes its task.  The
	// caller should make sure to signal the thread to exit, or else blocking may deadlock the
	// calling thread.  Classes which extend PersistentThread should override this method
	// and signal any necessary thread exit variables prior to blocking.
	//
	// Returns the return code of the thread.
	// This method is roughly the equivalent of pthread_join().
	//
	sptr PersistentThread::Block()
	{
		bool isOwner = (pthread_self() == m_thread);
		DevAssert( !isOwner, "Thread deadlock detected; Block() should never be called by the owner thread." );

		pthread_join( m_thread, (void**)&m_returncode );
		return m_returncode;
	}
	
	bool PersistentThread::IsRunning() const
	{
		return ( m_running && (ESRCH != pthread_kill( m_thread, 0 )) );
	}

	// Exceptions:
	//   InvalidOperation - thrown if the thread is still running or has never been started.
	//
	sptr PersistentThread::GetReturnCode() const
	{
		if( !m_running )
			throw Exception::InvalidOperation( "Thread.GetReturnCode : thread has not been started." );

		if( IsRunning() )
			throw Exception::InvalidOperation( "Thread.GetReturnCode : thread is still running." );

		return m_returncode;
	}
	
	void* PersistentThread::_internal_callback( void* itsme )
	{
		jASSUME( itsme != NULL );

		PersistentThread& owner = *((PersistentThread*)itsme);
		owner.m_returncode = owner.ExecuteTask();
		return (void*)owner.m_returncode;
	}

// pthread Cond is an evil api that is not suited for Pcsx2 needs.
// Let's not use it. (Air)
#if 0
	WaitEvent::WaitEvent()
	{
		int err = 0;

		err = pthread_cond_init(&cond, NULL);
		err = pthread_mutex_init(&mutex, NULL);
	}

	WaitEvent::~WaitEvent()
	{
		pthread_cond_destroy( &cond );
		pthread_mutex_destroy( &mutex );
	}

	void WaitEvent::Set()
	{
		pthread_mutex_lock( &mutex );
		pthread_cond_signal( &cond );
		pthread_mutex_unlock( &mutex );
	}

	void WaitEvent::Wait()
	{
		pthread_mutex_lock( &mutex );
		pthread_cond_wait( &cond, &mutex );
		pthread_mutex_unlock( &mutex );
	}
#endif

	Semaphore::Semaphore()
	{
		sem_init( &sema, false, 0 );
	}

	Semaphore::~Semaphore()
	{
		sem_destroy( &sema );
	}

	void Semaphore::Reset()
	{
		sem_destroy( &sema );
		sem_init( &sema, false, 0 );
	}

	void Semaphore::Post()
	{
		sem_post( &sema );
	}

	void Semaphore::Post( int multiple )
	{
#if defined(_MSC_VER)
		sem_post_multiple( &sema, multiple );
#endif
	}

	void Semaphore::Wait()
	{
		sem_wait( &sema );
	}

	// Performs an uncancellable wait on a semaphore; restoring the thread's previous cancel state
	// after the wait has completed.  Useful for situations where the semaphore itself is stored on
	// the stack and passed to another thread via GUI message or such, avoiding complications where
	// the thread might be cancelled and the stack value becomes invalid.
	//
	// Performance note: this function has quite a bit more overhead compared to Semaphore::Wait(), so
	// consider manually specifying the thread as uncancellable and using Wait() instead if you need
	// to do a lot of no-cancel waits in a tight loop worker thread, for example.
	void Semaphore::WaitNoCancel()
	{
		int oldstate;
		pthread_setcancelstate( PTHREAD_CANCEL_DISABLE, &oldstate );
		Wait();
		pthread_setcancelstate( oldstate, NULL );
	}

	int Semaphore::Count()
	{
		int retval;
		sem_getvalue( &sema, &retval );
		return retval;
	}

	MutexLock::MutexLock()
	{
		int err = 0;
		err = pthread_mutex_init( &mutex, NULL );
	}

	MutexLock::MutexLock( bool isRecursive )
	{
		if( isRecursive )
		{
			pthread_mutexattr_t mutexAttribute; 
			int status = pthread_mutexattr_init( &mutexAttribute );
			if (status != 0) { /* ... */ } 
			status = pthread_mutexattr_settype( &mutexAttribute, PTHREAD_MUTEX_RECURSIVE); 
			if (status != 0) { /* ... */} 

			int err = 0;
			err = pthread_mutex_init( &mutex, &mutexAttribute );
		}
		else
		{
			int err = 0;
			err = pthread_mutex_init( &mutex, NULL );
		}
	}

	MutexLock::~MutexLock()
	{
		pthread_mutex_destroy( &mutex );
	}

	void MutexLock::Lock()
	{
		pthread_mutex_lock( &mutex );
	}

	void MutexLock::Unlock()
	{
		pthread_mutex_unlock( &mutex );
	}

	//////////////////////////////////////////////////////////////////////
	// define some overloads for InterlockedExchanges
	// for commonly used types, like u32 and s32.

	__forceinline long pcsx2_InterlockedExchange( volatile long* target, long srcval )
	{
		return _InterlockedExchange( target, srcval );
	}

	__forceinline long pcsx2_InterlockedCompareExchange( volatile long* target, long srcval, long comp )
	{
		// Use the pthreads-win32 implementation...
		return _InterlockedCompareExchange( target, srcval, comp );
	}

	__forceinline long pcsx2_InterlockedExchangeAdd( volatile long* target, long srcval )
	{
		return _InterlockedExchangeAdd( target, srcval );
	}

	__forceinline void AtomicExchange( volatile u32& Target, u32 value )
	{
		pcsx2_InterlockedExchange( (volatile long*)&Target, value );
	}

	__forceinline void AtomicExchangeAdd( volatile u32& Target, u32 value )
	{
		pcsx2_InterlockedExchangeAdd( (volatile long*)&Target, value );
	}

	__forceinline void AtomicIncrement( volatile u32& Target )
	{
		pcsx2_InterlockedExchangeAdd( (volatile long*)&Target, 1 );
	}

	__forceinline void AtomicDecrement( volatile u32& Target )
	{
		pcsx2_InterlockedExchangeAdd( (volatile long*)&Target, -1 );
	}

	__forceinline void AtomicExchange( volatile s32& Target, s32 value )
	{
		pcsx2_InterlockedExchange( (volatile long*)&Target, value );
	}

	__forceinline void AtomicExchangeAdd( s32& Target, u32 value )
	{
		pcsx2_InterlockedExchangeAdd( (volatile long*)&Target, value );
	}

	__forceinline void AtomicIncrement( volatile s32& Target )
	{
		pcsx2_InterlockedExchangeAdd( (volatile long*)&Target, 1 );
	}

	__forceinline void AtomicDecrement( volatile s32& Target )
	{
		pcsx2_InterlockedExchangeAdd( (volatile long*)&Target, -1 );
	}

	__forceinline void _AtomicExchangePointer( const void ** target, const void* value )
	{
		pcsx2_InterlockedExchange( (volatile long*)target, (long)value );
	}

	__forceinline void _AtomicCompareExchangePointer( const void ** target, const void* value, const void* comparand )
	{
		pcsx2_InterlockedCompareExchange( (volatile long*)target, (long)value, (long)comparand );
	}

}
