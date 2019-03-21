// This is free and unencumbered software released into the public domain.
// For more information, please refer to <http://unlicense.org/>

#include "spananalyzer/plan_plot_options_dialog.h"

#include "wx/clrpicker.h"
#include "wx/spinctrl.h"
#include "wx/xrc/xmlres.h"

BEGIN_EVENT_TABLE(PlanPlotOptionsDialog, wxDialog)
  EVT_BUTTON(wxID_CANCEL, PlanPlotOptionsDialog::OnCancel)
  EVT_BUTTON(wxID_OK, PlanPlotOptionsDialog::OnOk)
  EVT_CLOSE(PlanPlotOptionsDialog::OnClose)
END_EVENT_TABLE()

PlanPlotOptionsDialog::PlanPlotOptionsDialog(
    wxWindow* parent,
    PlanPlotOptions* options) {
  // loads dialog from virtual xrc file system
  wxXmlResource::Get()->LoadDialog(this, parent, "plan_plot_options_dialog");

  // saves unmodified reference, and copies to modified
  options_ = options;
  options_modified_ = PlanPlotOptions(*options_);

  // transfers non-validator data to the window
  TransferCustomDataToWindow();

  // fits the dialog around the sizers
  this->Fit();
}

PlanPlotOptionsDialog::~PlanPlotOptionsDialog() {
}

void PlanPlotOptionsDialog::OnCancel(wxCommandEvent &event) {
  EndModal(wxID_CANCEL);
}

void PlanPlotOptionsDialog::OnClose(wxCloseEvent &event) {
  EndModal(wxID_CLOSE);
}

void PlanPlotOptionsDialog::OnOk(wxCommandEvent &event) {
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

void PlanPlotOptionsDialog::TransferCustomDataFromWindow() {
  wxColourPickerCtrl* pickerctrl = nullptr;
  wxSpinCtrl* spinctrl = nullptr;

  // transfers color-catenary
  pickerctrl = XRCCTRL(*this, "colorpicker_catenary", wxColourPickerCtrl);
  options_modified_.color_catenary = pickerctrl->GetColour();

  // transfers scale-horizontal
  spinctrl = XRCCTRL(*this, "spinctrl_horizontal_scale", wxSpinCtrl);
  options_modified_.scale_horizontal = spinctrl->GetValue();

  // transfers scale-vertical
  spinctrl = XRCCTRL(*this, "spinctrl_vertical_scale", wxSpinCtrl);
  options_modified_.scale_vertical = spinctrl->GetValue();

  // transfers thickness-line
  spinctrl = XRCCTRL(*this, "spinctrl_line_thickness", wxSpinCtrl);
  options_modified_.thickness_line = spinctrl->GetValue();
}

void PlanPlotOptionsDialog::TransferCustomDataToWindow() {
  wxColourPickerCtrl* pickerctrl = nullptr;
  wxSpinCtrl* spinctrl = nullptr;

  // transfers color-catenary
  pickerctrl = XRCCTRL(*this, "colorpicker_catenary", wxColourPickerCtrl);
  pickerctrl->SetColour(options_modified_.color_catenary);

  // transfers scale-horizontal
  spinctrl = XRCCTRL(*this, "spinctrl_horizontal_scale", wxSpinCtrl);
  spinctrl->SetValue(options_modified_.scale_horizontal);

  // transfers scale-vertical
  spinctrl = XRCCTRL(*this, "spinctrl_vertical_scale", wxSpinCtrl);
  spinctrl->SetValue(options_modified_.scale_vertical);

  // transfers thickness-line
  spinctrl = XRCCTRL(*this, "spinctrl_line_thickness", wxSpinCtrl);
  spinctrl->SetValue(options_modified_.thickness_line);
}
