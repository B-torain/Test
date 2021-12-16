/*  PCSX2 - PS2 Emulator for PCs
 *  Copyright (C) 2002-2010  PCSX2 Dev Team
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

#include <wx/wx.h>
#include "common/Pcsx2Defs.h" // __fastcall

bool pxIsEnglish(int id)
{
	return (id == wxLANGUAGE_ENGLISH || id == wxLANGUAGE_ENGLISH_US);
}

// --------------------------------------------------------------------------------------
//  pxExpandMsg  -- an Iconized Text Translator
//  Was replaced by a standard implementation of wxGetTranslation
// --------------------------------------------------------------------------------------
const wxChar* __fastcall pxExpandMsg(const wxChar* message)
{
	return wxGetTranslation(message).wc_str();
}

const wxChar* __fastcall pxGetTranslation(const wxChar* message)
{
	return wxGetTranslation(message).wc_str();
}

std::string __fastcall pxSt(const char* message)
{
	wxScopedCharBuffer str = wxGetTranslation(wxString::FromUTF8(message)).ToUTF8();
	return std::string(str.data(), str.data() + str.length());
}
