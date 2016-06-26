// This is free and unencumbered software released into the public domain.
// For more information, please refer to <http://unlicense.org/>

#include "preferences_dialog.h"

#include "wx/filepicker.h"
#include "wx/xrc/xmlres.h"

BEGIN_EVENT_TABLE(PreferencesDialog, wxDialog)
  EVT_BUTTON(wxID_CANCEL, PreferencesDialog::OnButtonCancel)
  EVT_BUTTON(wxID_OK, PreferencesDialog::OnButtonOk)
  EVT_CLOSE(PreferencesDialog::OnClose)
END_EVENT_TABLE()

PreferencesDialog::PreferencesDialog(
    wxWindow* parent,
    SpanAnalyzerConfig* config) {
  // loads dialog from virtual xrc file system
  wxXmlResource::Get()->LoadDialog(this, parent, "preferences_dialog");

  // saves doc reference
  config_ = config;

  // updates the cable directory
  // sets application data path for file picker ctrl
  wxFilePickerCtrl* filepickerctrl = XRCCTRL(*this, "filepickerctrl_data",
                                             wxFilePickerCtrl);
  filepickerctrl->SetPath(config_->filepath_data);

  // sets the unit system in the radio control
  wxRadioBox* radiobox = nullptr;

  radiobox = XRCCTRL(*this, "radiobox_units", wxRadioBox);
  if (config_->units == units::UnitSystem::kImperial) {
    radiobox->SetSelection(0);
  } else if (config_->units == units::UnitSystem::kMetric) {
    radiobox->SetSelection(1);
  }

  // sets the logging level in the radio control
  radiobox = XRCCTRL(*this, "radiobox_logging", wxRadioBox);
  if (config_->level_log == wxLOG_Error) {
    radiobox->SetSelection(0);
  } else if (config_->level_log == wxLOG_Message) {
    radiobox->SetSelection(1);
  }

  // fits the dialog around the sizers
  this->Fit();
}

PreferencesDialog::~PreferencesDialog() {
}

/// \brief Handles the cancel button event.
/// \param[in] event
///   The event.
void PreferencesDialog::OnButtonCancel(wxCommandEvent& event) {
  EndModal(wxID_CANCEL);
}

/// \brief Handles the Ok button event.
/// \param[in] event
///   The event.
void PreferencesDialog::OnButtonOk(wxCommandEvent& event) {
  wxRadioBox* radiobox = nullptr;

  // transfers units
  radiobox = XRCCTRL(*this, "radiobox_units", wxRadioBox);
  if (radiobox->GetSelection() == 0) {
    config_->units = units::UnitSystem::kImperial;
  } else if (radiobox->GetSelection() == 1) {
    config_->units = units::UnitSystem::kMetric;
  }

  // transfers logging level
  radiobox = XRCCTRL(*this, "radiobox_logging", wxRadioBox);
  if (radiobox->GetSelection() == 0) {
    config_->level_log = wxLOG_Error;
  } else if (radiobox->GetSelection() == 1) {
    config_->level_log = wxLOG_Message;
  }

  // transfers application data path
  wxFilePickerCtrl* filepickerctrl = XRCCTRL(*this, "filepickerctrl_data",
                                             wxFilePickerCtrl);
  config_->filepath_data = filepickerctrl->GetPath();

  EndModal(wxID_OK);
}

/// \brief Handles the close event.
/// \param[in] event
///   The event.
void PreferencesDialog::OnClose(wxCloseEvent& event) {
  EndModal(wxID_CLOSE);
}
