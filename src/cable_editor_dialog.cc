// This is free and unencumbered software released into the public domain.
// For more information, please refer to <http://unlicense.org/>

#include "cable_editor_dialog.h"

#include "wx/valnum.h"
#include "wx/xrc/xmlres.h"

BEGIN_EVENT_TABLE(CableEditorDialog, wxDialog)
  EVT_BUTTON(wxID_CANCEL, CableEditorDialog::OnCancel)
  EVT_BUTTON(wxID_OK, CableEditorDialog::OnOk)
  EVT_CLOSE(CableEditorDialog::OnClose)
END_EVENT_TABLE()

CableEditorDialog::CableEditorDialog(wxWindow* parent,
                                     Cable* cable,
                                     const UnitSystem& units,
                                     const bool& convert_units) { 
  // set variable
  cable_ = cable;
  name_ = wxString(cable_->name);

  // loads dialog from virtual xrc file system
  wxXmlResource::Get()->LoadDialog(this, parent, "cable_editor_dialog");

  // updates static text to display selected units
  units_ = units;
  UpdateUnitsStaticText(units_);

  // set validators on form

  //// cable name
  //wxTextValidator validator_name = wxTextValidator(wxFILTER_NONE,
  //                                                 &name_);
  //XRCCTRL(*this, "textctrl_name", wxTextCtrl)->SetValidator(validator_name);

  //// area-electrical
  //wxFloatingPointValidator<double> validator_area_electrical(
  //    0,
  //    &cable_->area_electrical,
  //    wxNUM_VAL_NO_TRAILING_ZEROES);
  //XRCCTRL(*this, "textctrl_area_electrical", wxTextCtrl)->SetValidator(
  //    validator_area_electrical);

  //// area-physical
  //wxFloatingPointValidator<double> validator_area_physical(
  //    4,
  //    &cable_->area_physical,
  //    wxNUM_VAL_NO_TRAILING_ZEROES);
  //XRCCTRL(*this, "textctrl_area_physical", wxTextCtrl)->SetValidator(
  //    validator_area_physical);

  //// diameter
  //wxFloatingPointValidator<double> validator_diameter(
  //    3,
  //    &cable_->diameter,
  //    wxNUM_VAL_NO_TRAILING_ZEROES);
  //XRCCTRL(*this, "textctrl_diameter", wxTextCtrl)->SetValidator(
  //    validator_diameter);

  //// weight-unit
  //wxFloatingPointValidator<double> validator_weight_unit(
  //    3,
  //    &cable_->weight_unit,
  //    wxNUM_VAL_NO_TRAILING_ZEROES);
  //XRCCTRL(*this, "textctrl_weight_unit", wxTextCtrl)->SetValidator(
  //    validator_weight_unit);

  //// strength-rated
  //wxFloatingPointValidator<double> validator_strength_rated(
  //    0,
  //    &cable_->strength_rated,
  //    wxNUM_VAL_THOUSANDS_SEPARATOR);
  //XRCCTRL(*this, "textctrl_strength_rated", wxTextCtrl)->SetValidator(
  //    validator_strength_rated);

  //// temperature-properties
  //wxFloatingPointValidator<double> validator_temperature_properties(
  //    0,
  //    &cable_->temperature_properties_components,
  //    wxNUM_VAL_NO_TRAILING_ZEROES);
  //XRCCTRL(*this, "textctrl_temperature_properties", wxTextCtrl)->SetValidator(
  //    validator_temperature_properties);

  //// shell coefficient-expansion-thermal
  //wxFloatingPointValidator<double> validator_shell_coefficient_expansion_thermal(
  //    6,
  //    &cable_->component_shell.coefficient_expansion_linear_thermal,
  //    wxNUM_VAL_NO_TRAILING_ZEROES);
  //XRCCTRL(*this, "textctrl_shell_coefficient_expansion_thermal", wxTextCtrl)
  //    ->SetValidator(validator_shell_coefficient_expansion_thermal);

  //// shell modulus
  //wxFloatingPointValidator<double> validator_shell_modulus_tension(
  //    0,
  //    &cable_->component_shell.modulus_tension_elastic_area,
  //    wxNUM_VAL_THOUSANDS_SEPARATOR);
  //XRCCTRL(*this, "textctrl_shell_modulus_tension", wxTextCtrl)->SetValidator(
  //    validator_shell_modulus_tension);

  //// shell a0
  //wxFloatingPointValidator<double> validator_shell_a0(
  //    2,
  //    &cable_->component_shell.coefficients_polynomial_loadstrain.at(0),
  //    wxNUM_VAL_NO_TRAILING_ZEROES);
  //XRCCTRL(*this, "textctrl_shell_a0", wxTextCtrl)->SetValidator(
  //    validator_shell_a0);

  //// shell a1
  //wxFloatingPointValidator<double> validator_shell_a1(
  //    2,
  //    &cable_->component_shell.coefficients_polynomial_loadstrain.at(1),
  //    wxNUM_VAL_NO_TRAILING_ZEROES);
  //XRCCTRL(*this, "textctrl_shell_a1", wxTextCtrl)->SetValidator(
  //    validator_shell_a1);

  //// shell a2
  //wxFloatingPointValidator<double> validator_shell_a2(
  //    2,
  //    &cable_->component_shell.coefficients_polynomial_loadstrain.at(2),
  //    wxNUM_VAL_NO_TRAILING_ZEROES);
  //XRCCTRL(*this, "textctrl_shell_a2", wxTextCtrl)->SetValidator(
  //    validator_shell_a2);

  //// shell a3
  //wxFloatingPointValidator<double> validator_shell_a3(
  //    2,
  //    &cable_->component_shell.coefficients_polynomial_loadstrain.at(3),
  //    wxNUM_VAL_NO_TRAILING_ZEROES);
  //XRCCTRL(*this, "textctrl_shell_a3", wxTextCtrl)->SetValidator(
  //    validator_shell_a3);

  //// shell a4
  //wxFloatingPointValidator<double> validator_shell_a4(
  //    2,
  //    &cable_->component_shell.coefficients_polynomial_loadstrain.at(4),
  //    wxNUM_VAL_NO_TRAILING_ZEROES);
  //XRCCTRL(*this, "textctrl_shell_a4", wxTextCtrl)->SetValidator(
  //    validator_shell_a4);

  //// core coefficient-expansion-thermal
  //wxFloatingPointValidator<double> validator_core_coefficient_expansion_thermal(
  //    6,
  //    &cable_->component_core.coefficient_expansion_linear_thermal,
  //    wxNUM_VAL_NO_TRAILING_ZEROES);
  //XRCCTRL(*this, "textctrl_core_coefficient_expansion_thermal", wxTextCtrl)
  //    ->SetValidator(validator_core_coefficient_expansion_thermal);

  //// core elastic area modulus
  //wxFloatingPointValidator<double> validator_core_modulus_tension(
  //    0,
  //    &cable_->component_core.modulus_tension_elastic_area,
  //    wxNUM_VAL_THOUSANDS_SEPARATOR);
  //XRCCTRL(*this, "textctrl_core_modulus_tension", wxTextCtrl)->SetValidator(
  //    validator_core_modulus_tension);

  //// core a0
  //wxFloatingPointValidator<double> validator_core_a0(
  //    2,
  //    &cable_->component_core.coefficients_polynomial_loadstrain.at(0),
  //    wxNUM_VAL_NO_TRAILING_ZEROES);
  //XRCCTRL(*this, "textctrl_core_a0", wxTextCtrl)->SetValidator(
  //    validator_core_a0);

  //// core a1
  //wxFloatingPointValidator<double> validator_core_a1(
  //    2,
  //    &cable_->component_core.coefficients_polynomial_loadstrain.at(1),
  //    wxNUM_VAL_NO_TRAILING_ZEROES);
  //XRCCTRL(*this, "textctrl_core_a1", wxTextCtrl)->SetValidator(
  //    validator_core_a1);

  //// core a2
  //wxFloatingPointValidator<double> validator_core_a2(
  //    2,
  //    &cable_->component_core.coefficients_polynomial_loadstrain.at(2),
  //    wxNUM_VAL_NO_TRAILING_ZEROES);
  //XRCCTRL(*this, "textctrl_core_a2", wxTextCtrl)->SetValidator(
  //    validator_core_a2);

  //// core a3
  //wxFloatingPointValidator<double> validator_core_a3(
  //    2,
  //    &cable_->component_core.coefficients_polynomial_loadstrain.at(3),
  //    wxNUM_VAL_NO_TRAILING_ZEROES);
  //XRCCTRL(*this, "textctrl_core_a3", wxTextCtrl)->SetValidator(
  //    validator_core_a3);

  //// core a4
  //wxFloatingPointValidator<double> validator_core_a4(
  //    2,
  //    &cable_->component_core.coefficients_polynomial_loadstrain.at(4),
  //    wxNUM_VAL_NO_TRAILING_ZEROES);
  //XRCCTRL(*this, "textctrl_core_a4", wxTextCtrl)->SetValidator(
  //    validator_core_a4);
}

void CableEditorDialog::OnClose(wxCloseEvent &event) {
  EndModal(wxID_CLOSE);
}

void CableEditorDialog::OnCancel(wxCommandEvent &event) {
  EndModal(wxID_CANCEL);
}

void CableEditorDialog::OnOk(wxCommandEvent &event) {
  // validates data from form
  if (this->Validate() == false) {
    wxMessageBox("Errors on form");
    return;
  }

  // transfers data from dialog controls to cable
  this->TransferDataFromWindow();
  cable_->name = name_.ToStdString();

  // ends modal by returning ok indicator
  EndModal(wxID_OK);
}

void CableEditorDialog::UpdateUnitsStaticText(UnitSystem& units) {

  if (units == UnitSystem::Metric) {

  } else {
    XRCCTRL(*this, "statictext_area_electrical_units", wxStaticText)->
        SetLabel("[kcmil]");
    XRCCTRL(*this, "statictext_area_physical_units", wxStaticText)->
        SetLabel("[in^2]");
    XRCCTRL(*this, "statictext_diameter_units", wxStaticText)->
        SetLabel("[in]");
    XRCCTRL(*this, "statictext_weight_unit_units", wxStaticText)->
        SetLabel("[lb/ft]");
    XRCCTRL(*this, "statictext_strength_rated_units", wxStaticText)->
        SetLabel("[lbs]");
    XRCCTRL(*this, "statictext_temperature_properties_units", wxStaticText)->
        SetLabel("[degF]");

    XRCCTRL(*this, "statictext_shell_coefficient_expansion_thermal_units", wxStaticText)->
        SetLabel("[degF]");
    XRCCTRL(*this, "statictext_shell_modulus_tension_units", wxStaticText)->
        SetLabel("[lb/in^2]");

    XRCCTRL(*this, "statictext_core_coefficient_expansion_thermal_units", wxStaticText)->
        SetLabel("[degF]");
    XRCCTRL(*this, "statictext_core_modulus_tension_units", wxStaticText)->
        SetLabel("[lb/in^2]");
  }
}
