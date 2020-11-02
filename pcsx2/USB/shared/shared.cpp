/*  PCSX2 - PS2 Emulator for PCs
 *  Copyright (C) 2002-2020  PCSX2 Dev Team
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

#include "shared.h"
#include <stdexcept>

#if defined(BUILD_RAW)
#include "rawinput.h"
#endif

namespace shared
{

	void Initialize(void* ptr)
	{
// Keeping it simple, for now
#if defined(BUILD_RAW)
		if (!shared::rawinput::Initialize(ptr))
			throw std::runtime_error("Failed to initialize raw input!");
#endif
	}

	void Uninitialize(/*void *ptr*/)
	{
#if defined(BUILD_RAW)
		shared::rawinput::Uninitialize(/*ptr*/);
#endif
	}

}; // namespace shared
