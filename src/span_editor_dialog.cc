// This is free and unencumbered software released into the public domain.
// For more information, please refer to <http://unlicense.org/>

#include "span_editor_dialog.h"

#include "models/base/helper.h"
#include "wx/valnum.h"
#include "wx/xrc/xmlres.h"

#include "error_message_dialog.h"

BEGIN_EVENT_TABLE(SpanEditorDialog, wxDialog)
  EVT_BUTTON(wxID_CANCEL, SpanEditorDialog::OnCancel)
  EVT_BUTTON(wxID_OK, SpanEditorDialog::OnOk)
  EVT_CLOSE(SpanEditorDialog::OnClose)
  EVT_CHECKBOX(XRCID("checkbox_match_constraint_geometry"), SpanEditorDialog::OnCheckboxMatchGeometry)
  EVT_CHOICE(XRCID("choice_span_type"), SpanEditorDialog::OnChoiceType)
END_EVENT_TABLE()

SpanEditorDialog::SpanEditorDialog(
    wxWindow* parent,
    const std::list<Cable>* cables,
    const std::list<WeatherLoadCase>* weathercases,
    const units::UnitSystem& units,
    Span* span) {
  // loads dialog from virtual xrc file system
  wxXmlResource::Get()->LoadDialog(this, parent, "span_editor_dialog");

  // saves constructor parameters to class
  SetUnitsStaticText(units);
  cables_ = cables;
  weathercases_ = weathercases;

  // saves unmodified span reference, and copies to modified span
  span_ = span;
  span_modified_ = Span(*span);

  // sets form validators to transfer between controls data and controls
  SetValidators();

  // inits choice controls on form
  InitChoiceControls();

  TransferCustomDataToWindow();

  // fits the dialog around the sizers
  this->Fit();
}

SpanEditorDialog::~SpanEditorDialog() {
}

void SpanEditorDialog::InitChoiceControls() {
  wxChoice* choice = nullptr;

  // populates type choice control
  choice = XRCCTRL(*this, "choice_span_type", wxChoice);
  choice->Append("Deadend");
  choice->Append("Ruling Span");

  // populates cable choice control
  choice = XRCCTRL(*this, "choice_cable", wxChoice);
  for (auto iter = cables_->cbegin(); iter != cables_->cend(); iter++) {
    const Cable& cable = *iter;
    choice->Append(cable.name);
  }

  // populates constraint type choice control
  choice = XRCCTRL(*this, "choice_constraint_type", wxChoice);
  choice->Append("Support");
  choice->Append("Horizontal");
  choice->Append("H/w");

  // populates constraint weathercase choice control
  choice = XRCCTRL(*this, "choice_constraint_weathercase", wxChoice);
  for (auto iter = weathercases_->cbegin(); iter != weathercases_->cend();
       iter++) {
    const WeatherLoadCase& weathercase = *iter;
    choice->Append(weathercase.description);
  }

  // populates constraint condition choice control
  choice = XRCCTRL(*this, "choice_constraint_condition", wxChoice);
  choice->Append("Initial");
  choice->Append("Final-Load");

  // populates stretch-load weathercase choice control
  choice = XRCCTRL(*this, "choice_stretch_weathercase", wxChoice);
  for (auto iter = weathercases_->cbegin(); iter != weathercases_->cend();
       iter++) {
    const WeatherLoadCase& weathercase = *iter;
    choice->Append(weathercase.description);
  }
}

void SpanEditorDialog::OnCancel(wxCommandEvent &event) {
  EndModal(wxID_CANCEL);
}

void SpanEditorDialog::OnClose(wxCloseEvent &event) {
  EndModal(wxID_CLOSE);
}

void SpanEditorDialog::OnCheckboxMatchGeometry(wxCommandEvent& event) {
  // gets the type of span and exits if it isn't a ruling span
  wxChoice* choice = XRCCTRL(*this, "choice_span_type", wxChoice);
  wxString str_selection = choice->GetStringSelection();
  if (str_selection != "Ruling Span") {
    return;
  }

  // gets checkbox
  wxCheckBox* checkbox = XRCCTRL(*this, "checkbox_match_constraint_geometry",
                                wxCheckBox);

  // gets textctrls associated with catenary spacing
  wxTextCtrl* textctrl_horizontal =
      XRCCTRL(*this, "textctrl_catenary_spacing_horizontal", wxTextCtrl);
  wxTextCtrl* textctrl_vertical =
      XRCCTRL(*this, "textctrl_catenary_spacing_vertical", wxTextCtrl);

  // enables/disables catenary spacing textctrls
  if (checkbox->IsChecked() == false) {
    textctrl_horizontal->Enable(true);
    textctrl_vertical->Enable(true);
  } else {
    textctrl_horizontal->SetValue("");
    textctrl_horizontal->Enable(false);

    textctrl_vertical->SetValue("");
    textctrl_vertical->Enable(false);
  }
}

void SpanEditorDialog::OnChoiceType(wxCommandEvent& event) {
  wxCheckBox* checkbox = nullptr;
  wxTextCtrl* textctrl = nullptr;

  // gets selected span type
  wxChoice* choice = XRCCTRL(*this, "choice_span_type", wxChoice);
  wxString str_selection = choice->GetStringSelection();
  if (str_selection == "Deadend") {
    // enables constraint geometry controls
    textctrl = XRCCTRL(*this, "textctrl_constraint_spacing_vertical",
                       wxTextCtrl);
    textctrl->Enable(true);

    // clears and disables catenary geometry controls
    checkbox = XRCCTRL(*this, "checkbox_match_constraint_geometry", wxCheckBox);
    checkbox->Enable(false);

    textctrl = XRCCTRL(*this, "textctrl_catenary_spacing_horizontal",
                       wxTextCtrl);
    textctrl->SetValue("");
    textctrl->Enable(false);

    textctrl = XRCCTRL(*this, "textctrl_catenary_spacing_vertical",
                       wxTextCtrl);
    textctrl->SetValue("");
    textctrl->Enable(false);
  } else if (str_selection == "Ruling Span") {
    // clears and disables constraint geometry controls
    textctrl = XRCCTRL(*this, "textctrl_constraint_spacing_vertical",
                       wxTextCtrl);
    textctrl->SetValue("");
    textctrl->Enable(false);

    // enables catenary geometry match checkbox
    wxCheckBox* checkbox = XRCCTRL(*this, "checkbox_match_constraint_geometry",
                                   wxCheckBox);
    checkbox = XRCCTRL(*this, "checkbox_match_constraint_geometry",
                        wxCheckBox);
    checkbox->Enable(true);

    if (checkbox->IsChecked() == false) {
      // enables catenary geometry textctrls
      textctrl = XRCCTRL(*this, "textctrl_catenary_spacing_horizontal",
                         wxTextCtrl);
      textctrl->Enable(true);

      textctrl = XRCCTRL(*this, "textctrl_catenary_spacing_vertical",
                         wxTextCtrl);
      textctrl->Enable(true);
    } else {
      // disables catenary geometry textctrls
      textctrl = XRCCTRL(*this, "textctrl_catenary_spacing_horizontal",
                         wxTextCtrl);
      textctrl->Enable(false);

      textctrl = XRCCTRL(*this, "textctrl_catenary_spacing_vertical",
                         wxTextCtrl);
      textctrl->Enable(false);
    }
  }
}

void SpanEditorDialog::OnOk(wxCommandEvent &event) {
  // validates data from form
  if (this->Validate() == false) {
    wxMessageBox("Errors on form");
    return;
  }

  // transfers data from dialog controls
  TransferDataFromWindow();
  TransferCustomDataFromWindow();

  // validates span data
  std::list<ErrorMessage> messages;

  /// \todo Validate if catenary can be solved for.

  if (span_modified_.Validate(true, &messages) == true) {

    // updates original cable reference based on user form edits
    *span_ = Span(span_modified_);

    // ends modal by returning ok indicator
    EndModal(wxID_OK);
  } else {
    // displays errors to user
    ErrorMessageDialog message(this, &messages);
    message.ShowModal();
    return;
  }
}

void SpanEditorDialog::SetUnitsStaticText(const units::UnitSystem& units) {
  /// \todo
  ///  wxWidgets seems to have a bug when editing labels. The StaticText
  ///  controls are not re-sized
  if (units == units::UnitSystem::kMetric) {

  } else if (units == units::UnitSystem::kImperial) {

  }

}

void SpanEditorDialog::SetValidators() {
  // variables used for creating validators
  wxString name;
  int style = 0;
  double* value_num = nullptr;
  int precision = 0;
  wxTextCtrl* textctrl = nullptr;

  // constraint limit
  precision = 1;
  value_num = &span_modified_.linecable.constraint.limit;
  style = wxNUM_VAL_NO_TRAILING_ZEROES;
  textctrl = XRCCTRL(*this, "textctrl_constraint_limit", wxTextCtrl);
  textctrl->SetValidator(
      wxFloatingPointValidator<double>(precision, value_num, style));

  // constraint spacing-horizontal
  precision = 1;
  style = wxNUM_VAL_NO_TRAILING_ZEROES;
  textctrl = XRCCTRL(*this, "textctrl_constraint_spacing_horizontal",
                     wxTextCtrl);
  textctrl->SetValidator(
      wxFloatingPointValidator<double>(precision, nullptr, style));

  // constraint spacing-vertical
  precision = 1;
  style = wxNUM_VAL_NO_TRAILING_ZEROES;
  textctrl = XRCCTRL(*this, "textctrl_constraint_spacing_vertical",
                     wxTextCtrl);
  textctrl->SetValidator(
      wxFloatingPointValidator<double>(precision, nullptr, style));

  // catenary spacing-horizontal
  precision = 1;
  style = wxNUM_VAL_NO_TRAILING_ZEROES;
  textctrl = XRCCTRL(*this, "textctrl_catenary_spacing_horizontal",
                     wxTextCtrl);
  textctrl->SetValidator(
      wxFloatingPointValidator<double>(precision, nullptr, style));

  // catenary spacing-vertical
  precision = 1;
  style = wxNUM_VAL_NO_TRAILING_ZEROES;
  textctrl = XRCCTRL(*this, "textctrl_catenary_spacing_vertical",
                     wxTextCtrl);
  textctrl->SetValidator(
      wxFloatingPointValidator<double>(precision, nullptr, style));
}

void SpanEditorDialog::TransferCustomDataFromWindow() {
  wxChoice* choice = nullptr;
  wxTextCtrl* textctrl = nullptr;
  wxString str;
  int index = -9999;
  double value = -999999;

  // transfers name
  textctrl = XRCCTRL(*this, "textctrl_name", wxTextCtrl);
  span_modified_.name = textctrl->GetValue();

  // transfers type
  choice = XRCCTRL(*this, "choice_span_type", wxChoice);
  if (choice->GetSelection() == 0) {
    span_modified_.type = Span::Type::kDeadendSpan;
  } else if (choice->GetSelection() == 1) {
    span_modified_.type = Span::Type::kRulingSpan;
  }

  // transfers cable
  choice = XRCCTRL(*this, "choice_cable", wxChoice);
  index = choice->GetSelection();
  if (index == wxNOT_FOUND) {
    span_modified_.linecable.cable = nullptr;
  } else {
    str = choice->GetString(index);
    auto iter = std::next(cables_->cbegin(), index);
    span_modified_.linecable.cable = &(*iter);
  }

  // transfers constraint type
  choice = XRCCTRL(*this, "choice_constraint_type", wxChoice);
  index = choice->GetSelection();
  str = choice->GetString(index);

  if (str == "Support") {
    span_modified_.linecable.constraint.type_limit =
        CableConstraint::LimitType::kSupportTension;
  } else if (str == "Horizontal") {
    span_modified_.linecable.constraint.type_limit =
        CableConstraint::LimitType::kHorizontalTension;
  } else if (str == "H/w") {
    span_modified_.linecable.constraint.type_limit =
        CableConstraint::LimitType::kCatenaryConstant;
  }

  // transfers constraint weathercase
  choice = XRCCTRL(*this, "choice_constraint_weathercase", wxChoice);
  index = choice->GetSelection();
  if (index == wxNOT_FOUND) {
    span_modified_.linecable.constraint.case_weather = nullptr;
  } else {
    auto iter = std::next(weathercases_->cbegin(), index);
    const WeatherLoadCase* weathercase = &(*iter);
    span_modified_.linecable.constraint.case_weather = weathercase;
  }

  // transfers constraint condition
  choice = XRCCTRL(*this, "choice_constraint_condition", wxChoice);
  index = choice->GetSelection();
  if (index == wxNOT_FOUND) {
    // can't set enum class to invalid, do nothing
  } else {
    str = choice->GetString(index);

    if (str == "Initial") {
      span_modified_.linecable.constraint.condition =
          CableConditionType::kInitial;
    } else if (str == "Final-Load") {
      span_modified_.linecable.constraint.condition =
          CableConditionType::kLoad;
    }
  }

  // transfers constraint spacing
  Vector3d spacing(0, 0, 0);

  textctrl = XRCCTRL(*this, "textctrl_constraint_spacing_horizontal",
                     wxTextCtrl);
  str = textctrl->GetValue();
  str.ToDouble(&value);
  spacing.set_x(value);

  if (span_modified_.type == Span::Type::kDeadendSpan) {
    textctrl = XRCCTRL(*this, "textctrl_constraint_spacing_vertical",
                       wxTextCtrl);
    str = textctrl->GetValue();
    str.ToDouble(&value);
    spacing.set_z(value);
  } else if (span_modified_.type == Span::Type::kRulingSpan) {
    spacing.set_z(0);
  }

  span_modified_.linecable.spacing_attachments_ruling_span = spacing;

  // transfers stretch weathercases
  choice = XRCCTRL(*this, "choice_stretch_weathercase", wxChoice);
  index = choice->GetSelection();
  if (index == wxNOT_FOUND) {
    span_modified_.linecable.weathercase_stretch_creep = nullptr;
    span_modified_.linecable.weathercase_stretch_load = nullptr;
  } else {
    auto iter = std::next(weathercases_->cbegin(), index);
    const WeatherLoadCase* weathercase = &(*iter);
    span_modified_.linecable.weathercase_stretch_creep = weathercase;
    span_modified_.linecable.weathercase_stretch_load = weathercase;
  }

  // transfers catenary spacing
  if (span_modified_.type == Span::Type::kDeadendSpan) {
    spacing = span_modified_.linecable.spacing_attachments_ruling_span;
  } else if (span_modified_.type == Span::Type::kRulingSpan) {
    wxCheckBox* checkbox = XRCCTRL(*this, "checkbox_match_constraint_geometry",
                                   wxCheckBox);
    if (checkbox->IsChecked() == true) {
      spacing = span_modified_.linecable.spacing_attachments_ruling_span;
    } else {
      spacing = Vector3d(0, 0, 0);
      textctrl = XRCCTRL(*this, "textctrl_catenary_spacing_horizontal",
                         wxTextCtrl);
      str = textctrl->GetValue();
      str.ToDouble(&value);
      spacing.set_x(value);

      textctrl = XRCCTRL(*this, "textctrl_catenary_spacing_vertical",
                         wxTextCtrl);
      str = textctrl->GetValue();
      str.ToDouble(&value);
      spacing.set_z(value);
    }
  }

  span_modified_.spacing_catenary = spacing;
}

void SpanEditorDialog::TransferCustomDataToWindow() {
  wxChoice* choice = nullptr;
  wxTextCtrl* textctrl = nullptr;
  wxString str;
  int index = -9999;

  // transfers name
  textctrl = XRCCTRL(*this, "textctrl_name", wxTextCtrl);
  textctrl->SetValue(span_modified_.name);

  // transfers type
  choice = XRCCTRL(*this, "choice_span_type", wxChoice);
  if (span_modified_.type == Span::Type::kDeadendSpan) {
    choice->SetSelection(0);
  } else if (span_modified_.type == Span::Type::kRulingSpan) {
    choice->SetSelection(1);
  }

  // transfers cable
  if (span_modified_.linecable.cable != nullptr) {
    choice = XRCCTRL(*this, "choice_cable", wxChoice);
    index = choice->FindString(span_modified_.linecable.cable->name, true);
    choice->SetSelection(index);
  }

  // transfers constraint type
  choice = XRCCTRL(*this, "choice_constraint_type", wxChoice);
  CableConstraint::LimitType type_limit =
      span_modified_.linecable.constraint.type_limit;
  if (type_limit == CableConstraint::LimitType::kSupportTension) {
    choice->SetSelection(0);
  } else if (type_limit == CableConstraint::LimitType::kHorizontalTension) {
    choice->SetSelection(1);
  } else if (type_limit == CableConstraint::LimitType::kCatenaryConstant) {
    choice->SetSelection(2);
  }

  // transfers constraint weathercase
  if (span_modified_.linecable.constraint.case_weather != nullptr) {
    choice = XRCCTRL(*this, "choice_constraint_weathercase", wxChoice);
    index = choice->FindString(
        span_modified_.linecable.constraint.case_weather->description,
        true);
    choice->SetSelection(index);
  }

  // transfers constraint condition
  choice = XRCCTRL(*this, "choice_constraint_condition", wxChoice);
  CableConditionType type_condition =
      span_modified_.linecable.constraint.condition;
  if (type_condition == CableConditionType::kInitial) {
    choice->SetSelection(0);
  } else if (type_condition == CableConditionType::kLoad) {
    choice->SetSelection(1);
  }

  // transfers constraint-spacing-horizontal
  textctrl = XRCCTRL(*this, "textctrl_constraint_spacing_horizontal",
                     wxTextCtrl);
  str = wxString::FromDouble(
      span_modified_.linecable.spacing_attachments_ruling_span.x(),
      1);
  textctrl->SetValue(str);

  // transfers constraint-spacing-vertical
  textctrl = XRCCTRL(*this, "textctrl_constraint_spacing_vertical",
                      wxTextCtrl);
  if (span_modified_.type == Span::Type::kDeadendSpan) {
    str = wxString::FromDouble(
        span_modified_.linecable.spacing_attachments_ruling_span.z(),
        1);
    textctrl->SetValue(str);
  } else if (span_modified_.type == Span::Type::kRulingSpan) {
    textctrl->SetValue("0");
  }

  // transfers stretch-load weathercase
  if (span_modified_.linecable.weathercase_stretch_load != nullptr) {
    choice = XRCCTRL(*this, "choice_stretch_weathercase", wxChoice);
    index = choice->FindString(
        span_modified_.linecable.weathercase_stretch_load->description,
        true);
    choice->SetSelection(index);
  }

  // transfers catenary geometry
  if (span_modified_.type == Span::Type::kDeadendSpan) {
    // disables catenary geometry controls
    wxCheckBox* checkbox = XRCCTRL(*this, "checkbox_match_constraint_geometry",
                                    wxCheckBox);
    checkbox->Enable(false);

    textctrl = XRCCTRL(*this, "textctrl_catenary_spacing_horizontal",
                       wxTextCtrl);
    textctrl->Enable(false);

    textctrl = XRCCTRL(*this, "textctrl_catenary_spacing_vertical",
                       wxTextCtrl);
    textctrl->Enable(false);
  } else if (span_modified_.type == Span::Type::kRulingSpan) {
    const Vector3d& spacing_constraint =
        span_modified_.linecable.spacing_attachments_ruling_span;
    const Vector3d& spacing_catenary = span_modified_.spacing_catenary;

    /// \todo add equals operator to Models library to simplify this
    if ((spacing_constraint.x() == spacing_catenary.x())
        && (spacing_constraint.y() == spacing_catenary.y())
        && (spacing_constraint.z() == spacing_catenary.z())) {
      wxCheckBox* checkbox = XRCCTRL(*this, "checkbox_match_constraint_geometry",
                                     wxCheckBox);
      checkbox->SetValue(true);

      textctrl = XRCCTRL(*this, "textctrl_catenary_spacing_horizontal",
                         wxTextCtrl);
      textctrl->Enable(false);

      textctrl = XRCCTRL(*this, "textctrl_catenary_spacing_vertical",
                         wxTextCtrl);
      textctrl->Enable(false);
    } else {
      textctrl = XRCCTRL(*this, "textctrl_catenary_spacing_horizontal",
                         wxTextCtrl);
      str = helper::DoubleToFormattedString(spacing_catenary.x(), 1);
      textctrl->SetValue(str);

      textctrl = XRCCTRL(*this, "textctrl_catenary_spacing_vertical",
                         wxTextCtrl);
      str = helper::DoubleToFormattedString(spacing_catenary.z(), 1);
      textctrl->SetValue(str);
    }
  }
}
