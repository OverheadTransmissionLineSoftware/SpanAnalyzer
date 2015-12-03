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
  wxXmlResource::Get()->LoadDialog(this, parent,
                                   "span_analyzer_preferences_dialog");

  // saves doc reference
  config_ = config;

  // updates the cable directory
  wxDirPickerCtrl* dirpickerctrl = XRCCTRL(*this, "dirpickerctrl_cable",
                                           wxDirPickerCtrl);

  // sets path for directory picker ctrl
  dirpickerctrl->SetPath(config_->cable_directory);

  // disables the metric option and selects imperial
  wxRadioBox* radiobox = XRCCTRL(*this, "radiobox_units", wxRadioBox);
  radiobox->Enable(0, false);
  radiobox->SetSelection(1);

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
  // transfers units
  wxRadioBox* radiobox = XRCCTRL(*this, "radiobox_units", wxRadioBox);
  if (radiobox->GetSelection() == 0) {
    config_->units = units::UnitSystem::kMetric;
  } else if (radiobox->GetSelection() == 1) {
    config_->units = units::UnitSystem::kImperial;
  }

  // transfers cable directory
  wxDirPickerCtrl* dirpickerctrl = XRCCTRL(*this, "dirpickerctrl_cable",
                                           wxDirPickerCtrl);
  config_->cable_directory = dirpickerctrl->GetPath();

  EndModal(wxID_OK);
}

/// \brief Handles the close event.
/// \param[in] event
///   The event.
void PreferencesDialog::OnClose(wxCloseEvent& event) {
  EndModal(wxID_CLOSE);
}
