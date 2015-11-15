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

#pragma once

#include <wx/wx.h>
#include <wx/propdlg.h>

#include "AppCommon.h"
#include "ApplyState.h"
#include "App.h"

namespace Panels
{
	class BaseSelectorPanel;
	class McdConfigPanel_Toggles;
	class BaseMcdListPanel;
}

BEGIN_DECLARE_EVENT_TYPES()
	DECLARE_EVENT_TYPE( pxEvt_SomethingChanged, -1 );
END_DECLARE_EVENT_TYPES()

namespace Dialogs
{
	// --------------------------------------------------------------------------------------
	//  BaseConfigurationDialog
	// --------------------------------------------------------------------------------------
	class BaseConfigurationDialog : public BaseApplicableDialog
	{
		typedef BaseApplicableDialog _parent;
	
	protected:
		wxListbook*			m_listbook;
		wxArrayString		m_labels;
		bool				m_allowApplyActivation;

	public:
		virtual ~BaseConfigurationDialog() throw();
		BaseConfigurationDialog(wxWindow* parent, const wxString& title, int idealWidth);

	public:
		void AddOkCancel( wxSizer* sizer=NULL );
		void AddListbook( wxSizer* sizer=NULL );
		void CreateListbook( wxImageList& bookicons );

		virtual void SomethingChanged();

		template< typename T >
		void AddPage( const wxChar* label, int iconid );

		void AllowApplyActivation( bool allow=true );
		virtual bool SomethingChanged_StateModified_IsChanged(){ //returns the state of the apply button.
			if( wxWindow* apply = FindWindow( wxID_APPLY ) )
				return apply->IsEnabled();
			return false;
		}

	protected:
		void OnOk_Click( wxCommandEvent& evt );
		void OnCancel_Click( wxCommandEvent& evt );
		void OnApply_Click( wxCommandEvent& evt );
		void OnScreenshot_Click( wxCommandEvent& evt );

		void OnSetSettingsPage( wxCommandEvent& evt );
		void OnSomethingChanged( wxCommandEvent& evt );

		virtual wxString& GetConfSettingsTabName() const=0;

		virtual void Apply() {};
		virtual void Cancel() {};
	};

	// --------------------------------------------------------------------------------------
	//  SysConfigDialog
	// --------------------------------------------------------------------------------------
	class SysConfigDialog : public BaseConfigurationDialog
	{
	public:
		virtual ~SysConfigDialog() throw() {}
		SysConfigDialog(wxWindow* parent=NULL);
		static wxString GetNameStatic() { return L"CoreSettings"; }
		wxString GetDialogName() const { return GetNameStatic(); }
   		void Apply();
		void Cancel();

		//Stores the state of the apply button in a global var.
		//This var will be used by KB shortcuts commands to decide if the gui should be modified (only when no intermediate changes)
		virtual bool SomethingChanged_StateModified_IsChanged(){
			return g_ConfigPanelChanged = BaseConfigurationDialog::SomethingChanged_StateModified_IsChanged();
		}

	protected:
		virtual wxString& GetConfSettingsTabName() const { return g_Conf->SysSettingsTabName; }

		pxCheckBox*		m_check_presets;
		wxSlider*		m_slider_presets;
		pxStaticText*	m_msg_preset;
		void AddPresetsControl();
		void Preset_Scroll(wxScrollEvent &event);
		void Presets_Toggled(wxCommandEvent &event);
		void UpdateGuiForPreset ( int presetIndex, bool presetsEnabled );
	};

	// --------------------------------------------------------------------------------------
	//  InterfaceConfigDialog
	// --------------------------------------------------------------------------------------
	class InterfaceConfigDialog : public BaseConfigurationDialog
	{
	public:
		virtual ~InterfaceConfigDialog() throw() {}
		InterfaceConfigDialog(wxWindow* parent=NULL);
		static wxString GetNameStatic() { return L"InterfaceConfig"; }
		wxString GetDialogName() const { return GetNameStatic(); }

	protected:
		virtual wxString& GetConfSettingsTabName() const { return g_Conf->AppSettingsTabName; }
	};

	// --------------------------------------------------------------------------------------
	//  InterfaceLanguageDialog
	// --------------------------------------------------------------------------------------
	class InterfaceLanguageDialog : public BaseConfigurationDialog
	{
	public:
		InterfaceLanguageDialog(wxWindow* parent = NULL);
		virtual ~InterfaceLanguageDialog() throw() { }

		static wxString GetNameStatic() { return L"InterfaceLanguage"; }
		wxString GetDialogName() const { return GetNameStatic(); }

	protected:
		virtual wxString& GetConfSettingsTabName() const { return g_Conf->AppSettingsTabName; }
	};


	// --------------------------------------------------------------------------------------
	//  McdConfigDialog
	// --------------------------------------------------------------------------------------
	class McdConfigDialog : public BaseConfigurationDialog
	{
		typedef BaseConfigurationDialog _parent;

	protected:
		Panels::BaseMcdListPanel*	m_panel_mcdlist;

	public:
		virtual ~McdConfigDialog() throw() {}
		McdConfigDialog(wxWindow* parent=NULL);
		static wxString GetNameStatic() { return L"McdConfig"; }
		wxString GetDialogName() const { return GetNameStatic(); }

	protected:
		virtual wxString& GetConfSettingsTabName() const { return g_Conf->McdSettingsTabName; }
		//void OnMultitapClicked( wxCommandEvent& evt );
	};

	// --------------------------------------------------------------------------------------
	//  GameDatabaseDialog
	// --------------------------------------------------------------------------------------
	class GameDatabaseDialog : public BaseConfigurationDialog
	{
	public:
		virtual ~GameDatabaseDialog() throw() {}
		GameDatabaseDialog(wxWindow* parent=NULL);
		static wxString GetNameStatic() { return L"GameDatabase"; }
		wxString GetDialogName() const { return GetNameStatic(); }

	protected:
		virtual wxString& GetConfSettingsTabName() const { return g_Conf->GameDatabaseTabName; }
	};

	// --------------------------------------------------------------------------------------
	//  ComponentsConfigDialog
	// --------------------------------------------------------------------------------------
	class ComponentsConfigDialog : public BaseConfigurationDialog
	{
	protected:

	public:
		virtual ~ComponentsConfigDialog() throw() {}
		ComponentsConfigDialog(wxWindow* parent=NULL);
		static wxString GetNameStatic() { return L"AppSettings"; }
		wxString GetDialogName() const { return GetNameStatic(); }

	protected:
		virtual wxString& GetConfSettingsTabName() const { return g_Conf->ComponentsTabName; }
	};

	// --------------------------------------------------------------------------------------
	//  CreateMemoryCardDialog
	// --------------------------------------------------------------------------------------
	class CreateMemoryCardDialog : public wxDialogWithHelpers
	{
	protected:
		wxDirName	m_mcdpath;
		wxString	m_mcdfile;
		wxTextCtrl*	m_text_filenameInput;

		//wxFilePickerCtrl*	m_filepicker;
		pxRadioPanel*		m_radio_CardSize;

	#ifdef __WXMSW__
		pxCheckBox*			m_check_CompressNTFS;
	#endif

	public:
		virtual ~CreateMemoryCardDialog()  throw() {}
		CreateMemoryCardDialog( wxWindow* parent, const wxDirName& mcdpath, const wxString& suggested_mcdfileName);
	
		//duplicate of MemoryCardFile::Create. Don't know why the existing method isn't used. - avih
		static bool CreateIt( const wxString& mcdFile, uint sizeInMB );
		wxString result_createdMcdFilename;
		//wxDirName GetPathToMcds() const;


	protected:
		void CreateControls();
		void OnOk_Click( wxCommandEvent& evt );
	};

	// --------------------------------------------------------------------------------------
	//  ConvertMemoryCardDialog
	// --------------------------------------------------------------------------------------
	class ConvertMemoryCardDialog : public wxDialogWithHelpers
	{
	protected:
		wxDirName     m_mcdPath;
		wxString      m_mcdSourceFilename;
		wxTextCtrl*   m_text_filenameInput;
		pxRadioPanel* m_radio_CardType;

	public:
		virtual ~ConvertMemoryCardDialog()  throw() {}
		ConvertMemoryCardDialog( wxWindow* parent, const wxDirName& mcdPath, const AppConfig::McdOptions& mcdSourceConfig );
	
	protected:
		void CreateControls( const MemoryCardType sourceType );
		void OnOk_Click( wxCommandEvent& evt );
		bool ConvertToFile( const wxFileName& sourcePath, const wxFileName& targetPath, const u32 sizeInMB );
		bool ConvertToFolder( const wxFileName& sourcePath, const wxFileName& targetPath );
	};
}
