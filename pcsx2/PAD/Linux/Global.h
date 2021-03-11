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

#pragma once

#define GAMEPAD_NUMBER 2 // numbers of gamepad

#include <stdio.h>
#include <assert.h>
#include <array>
#include <vector>
#include <map>
#include <string>
#include <memory>
#include <mutex>
#include <queue>

#include "Utilities/pxStreams.h"
#include "Utilities/Console.h"
#include "App.h"
#include "DebugTools/Debug.h"

#include "Pcsx2Defs.h"
#include "Config.h"
#include "bitwise.h"

#define PADdefs

enum gamePadValues
{
	PAD_L2 = 0,   // L2 button
	PAD_R2,       // R2 button
	PAD_L1,       // L1 button
	PAD_R1,       // R1 button
	PAD_TRIANGLE, // Triangle button ▲
	PAD_CIRCLE,   // Circle button ●
	PAD_CROSS,    // Cross button ✖
	PAD_SQUARE,   // Square button ■
	PAD_SELECT,   // Select button
	PAD_L3,       // Left joystick button (L3)
	PAD_R3,       // Right joystick button (R3)
	PAD_START,    // Start button
	PAD_UP,       // Directional pad ↑
	PAD_RIGHT,    // Directional pad →
	PAD_DOWN,     // Directional pad ↓
	PAD_LEFT,     // Directional pad ←
	PAD_L_UP,     // Left joystick (Up) ↑
	PAD_L_RIGHT,  // Left joystick (Right) →
	PAD_L_DOWN,   // Left joystick (Down) ↓
	PAD_L_LEFT,   // Left joystick (Left) ←
	PAD_R_UP,     // Right joystick (Up) ↑
	PAD_R_RIGHT,  // Right joystick (Right) →
	PAD_R_DOWN,   // Right joystick (Down) ↓
	PAD_R_LEFT    // Right joystick (Left) ←
};

static const u32 MAX_KEYS = 24;

static const std::array<gamePadValues, MAX_KEYS> all_keys =
{
	PAD_L2,       // L2 button
	PAD_R2,       // R2 button
	PAD_L1,       // L1 button
	PAD_R1,       // R1 button
	PAD_TRIANGLE, // Triangle button ▲
	PAD_CIRCLE,   // Circle button ●
	PAD_CROSS,    // Cross button ✖
	PAD_SQUARE,   // Square button ■
	PAD_SELECT,   // Select button
	PAD_L3,       // Left joystick button (L3)
	PAD_R3,       // Right joystick button (R3)
	PAD_START,    // Start button
	PAD_UP,       // Directional pad ↑
	PAD_RIGHT,    // Directional pad →
	PAD_DOWN,     // Directional pad ↓
	PAD_LEFT,     // Directional pad ←
	PAD_L_UP,     // Left joystick (Up) ↑
	PAD_L_RIGHT,  // Left joystick (Right) →
	PAD_L_DOWN,   // Left joystick (Down) ↓
	PAD_L_LEFT,   // Left joystick (Left) ←
	PAD_R_UP,     // Right joystick (Up) ↑
	PAD_R_RIGHT,  // Right joystick (Right) →
	PAD_R_DOWN,   // Right joystick (Down) ↓
	PAD_R_LEFT    // Right joystick (Left) ←

};

static bool IsAnalogKey(int index)
{
	return ((index >= PAD_L_UP) && (index <= PAD_R_LEFT));
}

#include "KeyStatus.h"

void __LogToConsole(const char* fmt, ...);
void PADLoadConfig();
void PADSaveConfig();
