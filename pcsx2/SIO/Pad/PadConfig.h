/*  PCSX2 - PS2 Emulator for PCs
 *  Copyright (C) 2002-2023  PCSX2 Dev Team
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

#include "SIO/Pad/PadTypes.h"

#include "Config.h"

class SettingsInterface;
enum class GenericInputBinding : u8;

namespace Pad
{
	struct ControllerInfo
	{
		ControllerType type;
		const char* name;
		const char* display_name;
		const InputBindingInfo* bindings;
		u32 num_bindings;
		const SettingInfo* settings;
		u32 num_settings;
		VibrationCapabilities vibration_caps;

		// Returns localized controller type name.
		const char* GetLocalizedName() const;
	};

	// Returns the default type for the specified port.
	const char* GetDefaultPadType(u32 pad);
	// Reloads configuration.
	void LoadConfig(const SettingsInterface& si);
	// Restores default configuration.
	void SetDefaultControllerConfig(SettingsInterface& si);
	void SetDefaultHotkeyConfig(SettingsInterface& si);
	// Clears all bindings for a given port.
	void ClearPortBindings(SettingsInterface& si, u32 port);
	// Copies pad configuration from one interface (ini) to another.
	void CopyConfiguration(SettingsInterface* dest_si, const SettingsInterface& src_si,
		bool copy_pad_config = true, bool copy_pad_bindings = true, bool copy_hotkey_bindings = true);
	// Returns a list of controller type names. Pair of [name, display name].
	const std::vector<std::pair<const char*, const char*>> GetControllerTypeNames();
	// Returns the list of binds for the specified controller type.
	std::vector<std::string> GetControllerBinds(const std::string_view& type);
	// Returns general information for the specified controller type.
	const ControllerInfo* GetControllerInfo(Pad::ControllerType type);
	const ControllerInfo* GetControllerInfo(const std::string_view& name);
	const char* GetControllerTypeName(Pad::ControllerType type);
	// Performs automatic controller mapping with the provided list of generic mappings.
	bool MapController(SettingsInterface& si, u32 controller,
		const std::vector<std::pair<GenericInputBinding, std::string>>& mapping);
	// Returns a list of input profiles available.
	std::vector<std::string> GetInputProfileNames();
	std::string GetConfigSection(u32 pad_index);
	void LoadMacroButtonConfig(const SettingsInterface& si, u32 pad, const std::string_view& type, const std::string& section);
};
