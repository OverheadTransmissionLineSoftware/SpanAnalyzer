// This is free and unencumbered software released into the public domain.
// For more information, please refer to <http://unlicense.org/>

#include "weather_load_case_editor_dialog.h"

#include "wx/valnum.h"
#include "wx/xrc/xmlres.h"

BEGIN_EVENT_TABLE(WeatherLoadCaseEditorDialog, wxDialog)
  EVT_BUTTON(wxID_CANCEL, WeatherLoadCaseEditorDialog::OnCancel)
  EVT_BUTTON(wxID_OK, WeatherLoadCaseEditorDialog::OnOk)
  EVT_CLOSE(WeatherLoadCaseEditorDialog::OnClose)
END_EVENT_TABLE()

WeatherLoadCaseEditorDialog::WeatherLoadCaseEditorDialog(
    wxWindow* parent,
    WeatherLoadCase* weathercase,
    const units::UnitSystem units) {
  // loads dialog from virtual xrc file system
  wxXmlResource::Get()->LoadDialog(this, parent,
                                   "weather_load_case_editor_dialog");

  // sets static text to display selected units
  SetUnitsStaticText(units);

  // saves unmodified cable reference, and copies to modified cable
  weathercase_ = weathercase;
  weathercase_modified_ = WeatherLoadCase(*weathercase);

  // sets variables not stored in modified cable
  name_ = weathercase_modified_.description;

  // sets form validators to transfer between controls data and controls
  SetValidators();

  // fits the dialog around the sizers
  this->Fit();
}

WeatherLoadCaseEditorDialog::~WeatherLoadCaseEditorDialog() {
}

void WeatherLoadCaseEditorDialog::OnCancel(wxCommandEvent &event) {
  EndModal(wxID_CANCEL);
}

void WeatherLoadCaseEditorDialog::OnClose(wxCloseEvent &event) {
  EndModal(wxID_CLOSE);
}

void WeatherLoadCaseEditorDialog::OnOk(wxCommandEvent &event) {
  // validates data from form
  if (this->Validate() == false) {
    wxMessageBox("Errors on form");
    return;
  }

  // transfers data from dialog controls to converted cable
  this->TransferDataFromWindow();
  weathercase_modified_.description = name_.ToStdString();

  // validates weathercase data
  // creates a temp cable for data validation
  std::list<ErrorMessage> messages;
  if (weathercase_modified_.Validate(true, &messages) == true) {
    
    // updates original cable reference based on user form edits
    *weathercase_ = WeatherLoadCase(weathercase_modified_);

    // ends modal by returning ok indicator
    EndModal(wxID_OK);
  } else {
    /// \todo
    /// do something to notify user of the errors
    wxMessageDialog message(this, "Errors present");
    message.ShowModal();
  }
}

void WeatherLoadCaseEditorDialog::SetUnitsStaticText(
    const units::UnitSystem& units) {
  /// \todo
  ///  wxWidgets seems to have a bug when editing labels. The StaticText
  ///  controls are not re-sized

  //if (units == units::UnitSystem::kMetric) {
  //  /// \todo
  //  /// Need to include metric form labeling.
  //  XRCCTRL(*this, "statictext_thickness_ice_units", wxStaticText)->
  //      SetLabel("[???]");
  //  XRCCTRL(*this, "statictext_density_ice_units", wxStaticText)->
  //      SetLabel("[???]");
  //  XRCCTRL(*this, "statictext_pressure_wind_units", wxStaticText)->
  //      SetLabel("[???]");
  //  XRCCTRL(*this, "statictext_temperature_cable_units", wxStaticText)->
  //      SetLabel("[???]");
  //} else if (units == units::UnitSystem::kImperial) {
  //  XRCCTRL(*this, "statictext_thickness_ice_units", wxStaticText)->
  //      SetLabel("[in]");
  //  XRCCTRL(*this, "statictext_density_ice_units", wxStaticText)->
  //      SetLabel("[lb/ft^3]");
  //  XRCCTRL(*this, "statictext_pressure_wind_units", wxStaticText)->
  //      SetLabel("[lb/ft^2]");
  //  XRCCTRL(*this, "statictext_temperature_cable_units", wxStaticText)->
  //      SetLabel("[deg F]");
  //}
}

void WeatherLoadCaseEditorDialog::SetValidators() {
  // variables used for creating validators
  int style = 0;
  wxString* value_str = nullptr;
  double* value_num = nullptr;
  int precision = 0;
  wxTextCtrl* textctrl = nullptr;

  // description
  value_str = &name_;
  style = wxFILTER_NONE;
  textctrl = XRCCTRL(*this, "textctrl_description", wxTextCtrl);
  textctrl->SetValidator(wxTextValidator(style, value_str));

  // thickness-ice
  precision = 2;
  value_num = &weathercase_modified_.thickness_ice;
  style = wxNUM_VAL_NO_TRAILING_ZEROES;
  textctrl = XRCCTRL(*this, "textctrl_thickness_ice", wxTextCtrl);
  textctrl->SetValidator(
      wxFloatingPointValidator<double>(precision, value_num, style));

  // density-ice
  precision = 1;
  value_num = &weathercase_modified_.density_ice;
  style = wxNUM_VAL_NO_TRAILING_ZEROES;
  textctrl = XRCCTRL(*this, "textctrl_density_ice", wxTextCtrl);
  textctrl->SetValidator(
      wxFloatingPointValidator<double>(precision, value_num, style));

  // pressure-wind
  precision = 1;
  value_num = &weathercase_modified_.pressure_wind;
  style = wxNUM_VAL_NO_TRAILING_ZEROES;
  textctrl = XRCCTRL(*this, "textctrl_pressure_wind", wxTextCtrl);
  textctrl->SetValidator(
      wxFloatingPointValidator<double>(precision, value_num, style));

  // temperature-cable
  precision = 0;
  value_num = &weathercase_modified_.temperature_cable;
  style = wxNUM_VAL_DEFAULT;
  textctrl = XRCCTRL(*this, "textctrl_temperature_cable", wxTextCtrl);
  textctrl->SetValidator(
      wxFloatingPointValidator<double>(precision, value_num, style));
}
