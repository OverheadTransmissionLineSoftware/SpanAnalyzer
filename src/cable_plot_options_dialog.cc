// This is free and unencumbered software released into the public domain.
// For more information, please refer to <http://unlicense.org/>

#include "spananalyzer/cable_plot_options_dialog.h"

#include "wx/clrpicker.h"
#include "wx/spinctrl.h"
#include "wx/xrc/xmlres.h"

BEGIN_EVENT_TABLE(CablePlotOptionsDialog, wxDialog)
  EVT_BUTTON(wxID_CANCEL, CablePlotOptionsDialog::OnCancel)
  EVT_BUTTON(wxID_OK, CablePlotOptionsDialog::OnOk)
  EVT_CLOSE(CablePlotOptionsDialog::OnClose)
END_EVENT_TABLE()

CablePlotOptionsDialog::CablePlotOptionsDialog(
    wxWindow* parent, CablePlotOptions* options) {
  // loads dialog from virtual xrc file system
  wxXmlResource::Get()->LoadDialog(this, parent, "cable_plot_options_dialog");

  // saves unmodified reference, and copies to modified
  options_ = options;
  options_modified_ = CablePlotOptions(*options_);

  // transfers non-validator data to the window
  TransferCustomDataToWindow();

  // fits the dialog around the sizers
  this->Fit();
}

CablePlotOptionsDialog::~CablePlotOptionsDialog() {
}

void CablePlotOptionsDialog::OnCancel(wxCommandEvent &event) {
  EndModal(wxID_CANCEL);
}

void CablePlotOptionsDialog::OnClose(wxCloseEvent &event) {
  EndModal(wxID_CLOSE);
}

void CablePlotOptionsDialog::OnOk(wxCommandEvent &event) {
  // validates data from form
  if (this->Validate() == false) {
    wxMessageBox("Errors on form");
    return;
  }

  wxBusyCursor cursor;

  // transfers data from dialog controls
  TransferCustomDataFromWindow();

  // commits changes and exits the dialog
  *options_ = options_modified_;
  EndModal(wxID_OK);
}

void CablePlotOptionsDialog::TransferCustomDataFromWindow() {
  wxColourPickerCtrl* pickerctrl = nullptr;
  wxSpinCtrl* spinctrl = nullptr;

  // transfers color-core
  pickerctrl = XRCCTRL(*this, "colorpicker_core", wxColourPickerCtrl);
  options_modified_.color_core = pickerctrl->GetColour();

  // transfers color-shell
  pickerctrl = XRCCTRL(*this, "colorpicker_shell", wxColourPickerCtrl);
  options_modified_.color_shell = pickerctrl->GetColour();

  // transfers color-total
  pickerctrl = XRCCTRL(*this, "colorpicker_total", wxColourPickerCtrl);
  options_modified_.color_total = pickerctrl->GetColour();

  // transfers color-markers
  pickerctrl = XRCCTRL(*this, "colorpicker_markers", wxColourPickerCtrl);
  options_modified_.color_markers = pickerctrl->GetColour();

  // transfers thickness-line
  spinctrl = XRCCTRL(*this, "spinctrl_line_thickness", wxSpinCtrl);
  options_modified_.thickness_line = spinctrl->GetValue();
}

void CablePlotOptionsDialog::TransferCustomDataToWindow() {
  wxColourPickerCtrl* pickerctrl = nullptr;
  wxSpinCtrl* spinctrl = nullptr;

  // transfers color-core
  pickerctrl = XRCCTRL(*this, "colorpicker_core", wxColourPickerCtrl);
  pickerctrl->SetColour(options_modified_.color_core);

  // transfers color-shell
  pickerctrl = XRCCTRL(*this, "colorpicker_shell", wxColourPickerCtrl);
  pickerctrl->SetColour(options_modified_.color_shell);

  // transfers color-total
  pickerctrl = XRCCTRL(*this, "colorpicker_total", wxColourPickerCtrl);
  pickerctrl->SetColour(options_modified_.color_total);

  // transfers color-markers
  pickerctrl = XRCCTRL(*this, "colorpicker_markers", wxColourPickerCtrl);
  pickerctrl->SetColour(options_modified_.color_markers);

  // transfers thickness-line
  spinctrl = XRCCTRL(*this, "spinctrl_line_thickness", wxSpinCtrl);
  spinctrl->SetValue(options_modified_.thickness_line);
}
