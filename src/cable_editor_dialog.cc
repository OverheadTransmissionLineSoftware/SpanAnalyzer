// This is free and unencumbered software released into the public domain.
// For more information, please refer to <http://unlicense.org/>

#include "cable_editor_dialog.h"

#include "models/base/units.h"
#include "wx/valnum.h"
#include "wx/xrc/xmlres.h"

#include "cable_unit_converter.h"
#include "error_message_dialog.h"

BEGIN_EVENT_TABLE(CableEditorDialog, wxDialog)
  EVT_BUTTON(wxID_CANCEL, CableEditorDialog::OnCancel)
  EVT_BUTTON(wxID_OK, CableEditorDialog::OnOk)
  EVT_CLOSE(CableEditorDialog::OnClose)
END_EVENT_TABLE()

CableEditorDialog::CableEditorDialog(wxWindow* parent,
                                     Cable* cable,
                                     const units::UnitSystem& units) {
  // loads dialog from virtual xrc file system
  wxXmlResource::Get()->LoadDialog(this, parent, "cable_editor_dialog");

  // sets static text to display selected units
  SetUnitsStaticText(units);

  // saves unmodified cable reference, and copies to modified cable
  cable_ = cable;
  cable_modified_ = Cable(*cable);

  // sets variables not stored in modified cable
  name_ = cable_modified_.name;

  // sets form validators to transfer between controls data and controls
  SetValidators();

  // fits the dialog around the sizers
  this->Fit();
}

void CableEditorDialog::OnCancel(wxCommandEvent &event) {
  EndModal(wxID_CANCEL);
}

void CableEditorDialog::OnClose(wxCloseEvent &event) {
  EndModal(wxID_CLOSE);
}

void CableEditorDialog::OnOk(wxCommandEvent &event) {
  // validates data from form
  if (this->Validate() == false) {
    wxMessageBox("Errors on form");
    return;
  }

  // transfers data from dialog controls to modified cable
  this->TransferDataFromWindow();
  cable_modified_.name = name_.ToStdString();

  // validates cable data
  std::list<ErrorMessage> messages;
  if (cable_modified_.Validate(true, &messages) == true) {

    // updates original cable reference based on user form edits
    *cable_ = Cable(cable_modified_);

    // ends modal by returning ok indicator
    EndModal(wxID_OK);
  } else {
    // displays errors to user
    ErrorMessageDialog message(this, &messages);
    message.ShowModal();
    return;
  }
}

void CableEditorDialog::SetUnitsStaticText(const units::UnitSystem& units) {
  /// \todo
  ///  wxWidgets seems to have a bug when editing labels. The StaticText
  ///  controls are not re-sized
  
  //wxStaticText* statictext = nullptr;

  //if (units == units::UnitSystem::kMetric) {
  //  /// \todo
  //  /// Need to include metric form labeling.
  //  XRCCTRL(*this, "statictext_area_electrical_units", wxStaticText)->
  //      SetLabel("???");
  //  XRCCTRL(*this, "statictext_area_physical_units", wxStaticText)->
  //      SetLabel("???");
  //  XRCCTRL(*this, "statictext_diameter_units", wxStaticText)->
  //      SetLabel("???");
  //  XRCCTRL(*this, "statictext_weight_unit_units", wxStaticText)->
  //      SetLabel("???");
  //  XRCCTRL(*this, "statictext_strength_rated_units", wxStaticText)->
  //      SetLabel("???");
  //  XRCCTRL(*this, "statictext_temperature_properties_units", wxStaticText)->
  //      SetLabel("???");

  //  XRCCTRL(*this, "statictext_shell_coefficient_expansion_thermal_units",
  //          wxStaticText)->SetLabel("???");
  //  XRCCTRL(*this, "statictext_shell_modulus_tension_units", wxStaticText)->
  //      SetLabel("???");

  //  XRCCTRL(*this, "statictext_core_coefficient_expansion_thermal_units",
  //          wxStaticText)->SetLabel("???");
  //  XRCCTRL(*this, "statictext_core_modulus_tension_units", wxStaticText)->
  //      SetLabel("???");
  //} else if (units == units::UnitSystem::kImperial) {
  //  XRCCTRL(*this, "statictext_area_electrical_units", wxStaticText)->
  //      SetLabel("[kcmil]");
  //  XRCCTRL(*this, "statictext_area_physical_units", wxStaticText)->
  //      SetLabel("[in^2]");
  //  XRCCTRL(*this, "statictext_diameter_units", wxStaticText)->
  //      SetLabel("[in]");
  //  XRCCTRL(*this, "statictext_weight_unit_units", wxStaticText)->
  //      SetLabel("[lb/ft]");
  //  XRCCTRL(*this, "statictext_strength_rated_units", wxStaticText)->
  //      SetLabel("[lbs]");
  //  XRCCTRL(*this, "statictext_temperature_properties_units", wxStaticText)->
  //      SetLabel("[degF]");

  //  XRCCTRL(*this, "statictext_shell_coefficient_expansion_thermal_units",
  //          wxStaticText)->SetLabel("[degF]");
  //  XRCCTRL(*this, "statictext_shell_modulus_tension_units", wxStaticText)->
  //      SetLabel("[lb/in^2]");

  //  XRCCTRL(*this, "statictext_core_coefficient_expansion_thermal_units",
  //          wxStaticText)->SetLabel("[degF]");
  //  XRCCTRL(*this, "statictext_core_modulus_tension_units", wxStaticText)->
  //      SetLabel("[lb/in^2]");
  //}
}

void CableEditorDialog::SetValidators() {
  // variables used for creating validators
  wxString name;
  int style = 0;
  wxString* value_str = nullptr;
  double* value_num = nullptr;
  int precision = 0;
  wxTextCtrl* textctrl = nullptr;

  // cable name
  value_str = &name_;
  style = wxFILTER_NONE;
  textctrl = XRCCTRL(*this, "textctrl_name", wxTextCtrl);
  textctrl->SetValidator(wxTextValidator(style, value_str));

  // area-electrical
  precision = 1;
  value_num = &cable_modified_.area_electrical;
  style = wxNUM_VAL_NO_TRAILING_ZEROES;
  textctrl = XRCCTRL(*this, "textctrl_area_electrical", wxTextCtrl);
  textctrl->SetValidator(
      wxFloatingPointValidator<double>(precision, value_num, style));

  // area-physical
  precision = 4;
  value_num = &cable_modified_.area_physical;
  style = wxNUM_VAL_NO_TRAILING_ZEROES;
  textctrl = XRCCTRL(*this, "textctrl_area_physical", wxTextCtrl);
  textctrl->SetValidator(
      wxFloatingPointValidator<double>(precision, value_num, style));

  // diameter
  precision = 3;
  value_num = &cable_modified_.diameter;
  style = wxNUM_VAL_NO_TRAILING_ZEROES;
  textctrl = XRCCTRL(*this, "textctrl_diameter", wxTextCtrl);
  textctrl->SetValidator(
      wxFloatingPointValidator<double>(precision, value_num, style));

  // weight-unit
  precision = 3;
  value_num = &cable_modified_.weight_unit;
  style = wxNUM_VAL_NO_TRAILING_ZEROES;
  textctrl = XRCCTRL(*this, "textctrl_weight_unit", wxTextCtrl);
  textctrl->SetValidator(
      wxFloatingPointValidator<double>(precision, value_num, style));

  // strength-rated
  precision = 0;
  value_num = &cable_modified_.strength_rated;
  style = wxNUM_VAL_DEFAULT;
  textctrl = XRCCTRL(*this, "textctrl_strength_rated", wxTextCtrl);
  textctrl->SetValidator(
      wxFloatingPointValidator<double>(precision, value_num, style));

  // temperature-properties
  precision = 0;
  value_num = &cable_modified_.temperature_properties_components;
  style = wxNUM_VAL_DEFAULT;
  textctrl = XRCCTRL(*this, "textctrl_temperature_properties", wxTextCtrl);
  textctrl->SetValidator(
      wxFloatingPointValidator<double>(precision, value_num, style));

  // shell coefficient-expansion-thermal
  precision = 7;
  value_num = &cable_->component_shell.coefficient_expansion_linear_thermal;
  style = wxNUM_VAL_NO_TRAILING_ZEROES;
  textctrl = XRCCTRL(*this, "textctrl_shell_coefficient_expansion_thermal",
                     wxTextCtrl);
  textctrl->SetValidator(
      wxFloatingPointValidator<double>(precision, value_num, style));

  // shell modulus
  precision = 0;
  value_num = &cable_->component_shell.modulus_tension_elastic_area;
  style = wxNUM_VAL_THOUSANDS_SEPARATOR;
  textctrl = XRCCTRL(*this, "textctrl_shell_modulus_tension", wxTextCtrl);
  textctrl->SetValidator(
      wxFloatingPointValidator<double>(precision, value_num, style));

  // shell a0
  precision = 1;
  value_num = &cable_->component_shell.coefficients_polynomial_loadstrain.at(0);
  style = wxNUM_VAL_NO_TRAILING_ZEROES;
  textctrl = XRCCTRL(*this, "textctrl_shell_a0", wxTextCtrl);
  textctrl->SetValidator(
      wxFloatingPointValidator<double>(precision, value_num, style));

  // shell a1
  precision = 1;
  value_num = &cable_->component_shell.coefficients_polynomial_loadstrain.at(1);
  style = wxNUM_VAL_NO_TRAILING_ZEROES;
  textctrl = XRCCTRL(*this, "textctrl_shell_a1", wxTextCtrl);
  textctrl->SetValidator(
      wxFloatingPointValidator<double>(precision, value_num, style));

  // shell a2
  precision = 1;
  value_num = &cable_->component_shell.coefficients_polynomial_loadstrain.at(2);
  style = wxNUM_VAL_NO_TRAILING_ZEROES;
  textctrl = XRCCTRL(*this, "textctrl_shell_a2", wxTextCtrl);
  textctrl->SetValidator(
      wxFloatingPointValidator<double>(precision, value_num, style));

  // shell a3
  precision = 1;
  value_num = &cable_->component_shell.coefficients_polynomial_loadstrain.at(3);
  style = wxNUM_VAL_NO_TRAILING_ZEROES;
  textctrl = XRCCTRL(*this, "textctrl_shell_a3", wxTextCtrl);
  textctrl->SetValidator(
      wxFloatingPointValidator<double>(precision, value_num, style));

  // shell a4
  precision = 1;
  value_num = &cable_->component_shell.coefficients_polynomial_loadstrain.at(4);
  style = wxNUM_VAL_NO_TRAILING_ZEROES;
  textctrl = XRCCTRL(*this, "textctrl_shell_a4", wxTextCtrl);
  textctrl->SetValidator(
      wxFloatingPointValidator<double>(precision, value_num, style));

  // core coefficient-expansion-thermal
  precision = 7;
  value_num = &cable_->component_core.coefficient_expansion_linear_thermal;
  style = wxNUM_VAL_NO_TRAILING_ZEROES;
  textctrl = XRCCTRL(*this, "textctrl_core_coefficient_expansion_thermal",
                     wxTextCtrl);
  textctrl->SetValidator(
      wxFloatingPointValidator<double>(precision, value_num, style));

  // core elastic area modulus
  precision = 0;
  value_num = &cable_->component_core.modulus_tension_elastic_area;
  style = wxNUM_VAL_THOUSANDS_SEPARATOR;
  textctrl = XRCCTRL(*this, "textctrl_core_modulus_tension", wxTextCtrl);
  textctrl->SetValidator(
      wxFloatingPointValidator<double>(precision, value_num, style));

  // core a0
  precision = 1;
  value_num = &cable_->component_core.coefficients_polynomial_loadstrain.at(0);
  style = wxNUM_VAL_NO_TRAILING_ZEROES;
  textctrl = XRCCTRL(*this, "textctrl_core_a0", wxTextCtrl);
  textctrl->SetValidator(
      wxFloatingPointValidator<double>(precision, value_num, style));

  // core a1
  precision = 1;
  value_num = &cable_->component_core.coefficients_polynomial_loadstrain.at(1);
  style = wxNUM_VAL_NO_TRAILING_ZEROES;
  textctrl = XRCCTRL(*this, "textctrl_core_a1", wxTextCtrl);
  textctrl->SetValidator(
      wxFloatingPointValidator<double>(precision, value_num, style));

  // core a2
  precision = 1;
  value_num = &cable_->component_core.coefficients_polynomial_loadstrain.at(2);
  style = wxNUM_VAL_NO_TRAILING_ZEROES;
  textctrl = XRCCTRL(*this, "textctrl_core_a2", wxTextCtrl);
  textctrl->SetValidator(
      wxFloatingPointValidator<double>(precision, value_num, style));

  // core a3
  precision = 1;
  value_num = &cable_->component_core.coefficients_polynomial_loadstrain.at(3);
  style = wxNUM_VAL_NO_TRAILING_ZEROES;
  textctrl = XRCCTRL(*this, "textctrl_core_a3", wxTextCtrl);
  textctrl->SetValidator(
      wxFloatingPointValidator<double>(precision, value_num, style));

  // core a4
  precision = 1;
  value_num = &cable_->component_core.coefficients_polynomial_loadstrain.at(4);
  style = wxNUM_VAL_NO_TRAILING_ZEROES;
  textctrl = XRCCTRL(*this, "textctrl_core_a4", wxTextCtrl);
  textctrl->SetValidator(
      wxFloatingPointValidator<double>(precision, value_num, style));
}
