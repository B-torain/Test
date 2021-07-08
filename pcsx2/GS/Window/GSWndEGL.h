/*  PCSX2 - PS2 Emulator for PCs
 *  Copyright (C) 2002-2021 PCSX2 Dev Team
 *
 *  PCSX2 is free software: you can redistribute it and/or modify it under the terms
 *  of the GNU Lesser General Public License as published by the Free Software Found-
 *  ation, either version 3 of the License, or (at your option) any later version.
 *
 *  PCSX2 is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY;
 *  without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 *  PURPOSE.  See the GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License along with PCSX2.
 *  If not, see <http://www.gnu.org/licenses/>.
 */

#include "GSWnd.h"

#if defined(__unix__)
#include <EGL/egl.h>
#include <EGL/eglext.h>

#define GS_EGL_X11 1
#define GS_EGL_WL 0

class GSWndEGL : public GSWndGL
{
	void* m_native_window;

	EGLDisplay m_eglDisplay;
	EGLSurface m_eglSurface;
	EGLContext m_eglContext;

	int m_platform;

	void PopulateWndGlFunction();
	void CreateContext(int major, int minor);
	void BindAPI();

	void SetSwapInterval() final;
	bool HasLateVsyncSupport() final { return false; }

	void OpenEGLDisplay();
	void CloseEGLDisplay();

public:
	GSWndEGL(int platform);
	virtual ~GSWndEGL(){};

	static std::shared_ptr<GSWndEGL> CreateForPlatform(const WindowInfo& wi);

	bool Attach(const WindowInfo& wi) final;
	void Detach() final;

	virtual void* AttachNativeWindow(const WindowInfo& wi) = 0;
	virtual void DestroyNativeResources() = 0;

	GSVector4i GetClientRect();

	void AttachContext() final;
	void DetachContext() final;
	void* GetProcAddress(const char* name, bool opt = false) final;

	void Flip() final;
};

#if GS_EGL_X11

#include <xcb/xcb.h>
#include <X11/Xlib.h>
#include <X11/Xlib-xcb.h>

class GSWndEGL_X11 final : public GSWndEGL
{
	Window m_NativeWindow;

public:
	GSWndEGL_X11();
	virtual ~GSWndEGL_X11() final = default;

	void* GetHandle() final { return reinterpret_cast<void*>(m_NativeWindow); }

	void* AttachNativeWindow(const WindowInfo& wi) final;
	void DestroyNativeResources() final;
};

#endif

#if GS_EGL_WL

// Which include ?
#include <wayland-client.h>
#include <wayland-server.h>
#include <wayland-client-protocol.h>
#include <wayland-egl.h>

class GSWndEGL_WL final : public GSWndEGL
{
	wl_egl_window* m_NativeWindow;

public:
	GSWndEGL_WL();
	virtual ~GSWndEGL_WL() final = default;

	void* GetHandle() final { return reinterpret_cast<void*>(m_NativeWindow); }

	void* AttachNativeWindow(const WindowInfo& wi) final;
	void DestroyNativeResources() final;
};

#endif

#endif
