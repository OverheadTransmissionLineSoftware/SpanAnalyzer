// This is free and unencumbered software released into the public domain.
// For more information, please refer to <http://unlicense.org/>

#include "spananalyzer/span_editor_dialog.h"

#include "appcommon/widgets/error_message_dialog.h"
#include "models/base/helper.h"
#include "wx/valnum.h"
#include "wx/xrc/xmlres.h"

BEGIN_EVENT_TABLE(SpanEditorDialog, wxDialog)
  EVT_BUTTON(wxID_CANCEL, SpanEditorDialog::OnCancel)
  EVT_BUTTON(wxID_OK, SpanEditorDialog::OnOk)
  EVT_CLOSE(SpanEditorDialog::OnClose)
  EVT_CHECKBOX(XRCID("checkbox_match_line_section_geometry"), SpanEditorDialog::OnCheckboxMatchGeometry)
  EVT_CHOICE(XRCID("choice_span_type"), SpanEditorDialog::OnChoiceType)
END_EVENT_TABLE()

SpanEditorDialog::SpanEditorDialog(
    wxWindow* parent,
    const std::list<CableFile*>* cablefiles,
    const std::list<WeatherLoadCase*>* weathercases,
    const units::UnitSystem& units,
    Span* span) {
  // loads dialog from virtual xrc file system
  wxXmlResource::Get()->LoadDialog(this, parent, "span_editor_dialog");
  this->Fit();

  // saves constructor parameters to class
  SetUnitsStaticText(units);
  cablefiles_ = cablefiles;
  weathercases_ = weathercases;

  // saves unmodified span reference, and copies to modified span
  span_ = span;
  span_modified_ = Span(*span);

  // sets form validators to transfer between controls data and controls
  SetValidators();

  // inits form controls
  InitChoiceControls();
  if (span_modified_.type == Span::Type::kDeadendSpan) {
    ToggleDeadEndSpanModeControls();
  } else if (span_modified_.type == Span::Type::kRulingSpan) {
    ToggleRulingSpanModeControls();
  }

  TransferCustomDataToWindow();
}

SpanEditorDialog::~SpanEditorDialog() {
}

void SpanEditorDialog::InitChoiceControls() {
  wxChoice* choice = nullptr;

  // populates type choice control
  choice = XRCCTRL(*this, "choice_span_type", wxChoice);
  choice->Append("Dead-End");
  choice->Append("Ruling Span");

  // populates cable choice control
  choice = XRCCTRL(*this, "choice_cable", wxChoice);
  for (auto iter = cablefiles_->cbegin(); iter != cablefiles_->cend();
       iter++) {
    const CableFile* cablefile = *iter;
    choice->Append(cablefile->cable.name);
  }

  // populates constraint type choice control
  choice = XRCCTRL(*this, "choice_constraint_type", wxChoice);
  choice->Append("Support");
  choice->Append("Horizontal");
  choice->Append("H/w");
  choice->Append("Length");
  choice->Append("Sag");

  // populates constraint weathercase choice control
  choice = XRCCTRL(*this, "choice_constraint_weathercase", wxChoice);
  for (auto iter = weathercases_->cbegin(); iter != weathercases_->cend();
       iter++) {
    const WeatherLoadCase* weathercase = *iter;
    choice->Append(weathercase->description);
  }

  // populates constraint condition choice control
  choice = XRCCTRL(*this, "choice_constraint_condition", wxChoice);
  choice->Append("Initial");
  choice->Append("Load");
  choice->Append("Creep");

  // populates stretch-load weathercase choice control
  choice = XRCCTRL(*this, "choice_stretch_load_weathercase", wxChoice);
  for (auto iter = weathercases_->cbegin(); iter != weathercases_->cend();
       iter++) {
    const WeatherLoadCase* weathercase = *iter;
    choice->Append(weathercase->description);
  }

  // populates stretch-creep weathercase choice control
  choice = XRCCTRL(*this, "choice_stretch_creep_weathercase", wxChoice);
  for (auto iter = weathercases_->cbegin(); iter != weathercases_->cend();
       iter++) {
    const WeatherLoadCase* weathercase = *iter;
    choice->Append(weathercase->description);
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
  wxCheckBox* checkbox = XRCCTRL(*this, "checkbox_match_line_section_geometry",
                                 wxCheckBox);

  // enables/disables catenary spacing textctrls
  if (checkbox->IsChecked() == false) {
    ToggleSpanGeometrySpacingControls(true);
  } else {
    ToggleSpanGeometrySpacingControls(false);
  }
}

void SpanEditorDialog::OnChoiceType(wxCommandEvent& event) {
  // gets selected span type
  wxChoice* choice = XRCCTRL(*this, "choice_span_type", wxChoice);
  wxString str_selection = choice->GetStringSelection();
  if (str_selection == "Dead-End") {
    ToggleDeadEndSpanModeControls();
  } else if (str_selection == "Ruling Span") {
    ToggleRulingSpanModeControls();
  }
}

void SpanEditorDialog::OnOk(wxCommandEvent &event) {
  // validates data from form
  if (this->Validate() == false) {
    wxMessageBox("Errors on form");
    return;
  }

  wxBusyCursor cursor;

  // transfers data from dialog controls
  TransferDataFromWindow();
  TransferCustomDataFromWindow();

  // validates span data
  std::list<ErrorMessage> messages;
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
  if (units == units::UnitSystem::kMetric) {
    XRCCTRL(*this, "statictext_line_section_spacing_horizontal_units",
            wxStaticText)->SetLabel("[m]");
    XRCCTRL(*this, "statictext_line_section_spacing_vertical_units",
            wxStaticText)->SetLabel("[m]");
    XRCCTRL(*this, "statictext_span_spacing_horizontal_units",
            wxStaticText)->SetLabel("[m]");
    XRCCTRL(*this, "statictext_span_spacing_vertical_units",
            wxStaticText)->SetLabel("[m]");
  } else if (units == units::UnitSystem::kImperial) {
    XRCCTRL(*this, "statictext_line_section_spacing_horizontal_units",
            wxStaticText)->SetLabel("[ft]");
    XRCCTRL(*this, "statictext_line_section_spacing_vertical_units",
            wxStaticText)->SetLabel("[ft]");
    XRCCTRL(*this, "statictext_span_spacing_horizontal_units",
            wxStaticText)->SetLabel("[ft]");
    XRCCTRL(*this, "statictext_span_spacing_vertical_units",
            wxStaticText)->SetLabel("[ft]");
  }
}

void SpanEditorDialog::SetValidators() {
  // variables used for creating validators
  int style = 0;
  int precision = 0;
  wxTextCtrl* textctrl = nullptr;

  // constraint limit
  precision = 2;
  style = wxNUM_VAL_NO_TRAILING_ZEROES;
  textctrl = XRCCTRL(*this, "textctrl_constraint_limit", wxTextCtrl);
  textctrl->SetValidator(
      wxFloatingPointValidator<double>(precision, nullptr, style));

  // line section spacing-horizontal
  precision = 2;
  style = wxNUM_VAL_NO_TRAILING_ZEROES;
  textctrl = XRCCTRL(*this, "textctrl_line_section_spacing_horizontal",
                     wxTextCtrl);
  textctrl->SetValidator(
      wxFloatingPointValidator<double>(precision, nullptr, style));

  // line section spacing-vertical
  precision = 2;
  style = wxNUM_VAL_NO_TRAILING_ZEROES;
  textctrl = XRCCTRL(*this, "textctrl_line_section_spacing_vertical",
                     wxTextCtrl);
  textctrl->SetValidator(
      wxFloatingPointValidator<double>(precision, nullptr, style));

  // span spacing-horizontal
  precision = 2;
  style = wxNUM_VAL_NO_TRAILING_ZEROES;
  textctrl = XRCCTRL(*this, "textctrl_span_spacing_horizontal",
                     wxTextCtrl);
  textctrl->SetValidator(
      wxFloatingPointValidator<double>(precision, nullptr, style));

  // span spacing-vertical
  precision = 2;
  style = wxNUM_VAL_NO_TRAILING_ZEROES;
  textctrl = XRCCTRL(*this, "textctrl_span_spacing_vertical",
                     wxTextCtrl);
  textctrl->SetValidator(
      wxFloatingPointValidator<double>(precision, nullptr, style));
}

void SpanEditorDialog::ToggleSpanGeometrySpacingControls(
    const bool& is_enabled) {
  wxStaticText* statictext = nullptr;
  wxTextCtrl* textctrl = nullptr;

  // toggles horizontal spacing controls
  statictext = XRCCTRL(*this, "statictext_span_spacing_horizontal",
                       wxStaticText);
  statictext->Enable(is_enabled);
  statictext = XRCCTRL(*this, "statictext_span_spacing_horizontal_units",
                       wxStaticText);
  statictext->Enable(is_enabled);
  textctrl = XRCCTRL(*this, "textctrl_span_spacing_horizontal",
                     wxTextCtrl);
  if (is_enabled == false) {
    textctrl->Clear();
  }
  textctrl->Enable(is_enabled);

  // toggles vertical spacing controls
  statictext = XRCCTRL(*this, "statictext_span_spacing_vertical",
                       wxStaticText);
  statictext->Enable(is_enabled);
  statictext = XRCCTRL(*this, "statictext_span_spacing_vertical_units",
                       wxStaticText);
  statictext->Enable(is_enabled);
  textctrl = XRCCTRL(*this, "textctrl_span_spacing_vertical",
                     wxTextCtrl);
  if (is_enabled == false) {
    textctrl->Clear();
  }
  textctrl->Enable(is_enabled);
}

void SpanEditorDialog::ToggleDeadEndSpanModeControls() {
  wxCheckBox* checkbox = nullptr;
  wxStaticText* statictext = nullptr;
  wxTextCtrl* textctrl = nullptr;

  // enables vertical line section geometry controls
  statictext = XRCCTRL(*this, "statictext_line_section_spacing_vertical",
                       wxStaticText);
  statictext->Enable(true);
  statictext = XRCCTRL(*this, "statictext_line_section_spacing_vertical_units",
                       wxStaticText);
  statictext->Enable(true);
  textctrl = XRCCTRL(*this, "textctrl_line_section_spacing_vertical",
                     wxTextCtrl);
  textctrl->Clear();
  textctrl->Enable(true);

  // disables span geometry controls
  checkbox = XRCCTRL(*this, "checkbox_match_line_section_geometry",
                     wxCheckBox);
  checkbox->Enable(false);

  ToggleSpanGeometrySpacingControls(false);
}

void SpanEditorDialog::ToggleRulingSpanModeControls() {
  wxCheckBox* checkbox = nullptr;
  wxStaticText* statictext = nullptr;
  wxTextCtrl* textctrl = nullptr;

  // disables vertical line section geometry controls
  statictext = XRCCTRL(*this, "statictext_line_section_spacing_vertical",
                       wxStaticText);
  statictext->Enable(false);
  statictext = XRCCTRL(*this, "statictext_line_section_spacing_vertical_units",
                       wxStaticText);
  statictext->Enable(false);
  textctrl = XRCCTRL(*this, "textctrl_line_section_spacing_vertical",
                     wxTextCtrl);
  textctrl->Clear();
  textctrl->Enable(false);

  // enables span geometry controls
  checkbox = XRCCTRL(*this, "checkbox_match_line_section_geometry",
                     wxCheckBox);
  checkbox->Enable(true);

  if (checkbox->IsChecked() == false) {
    ToggleSpanGeometrySpacingControls(true);
  } else {
    ToggleSpanGeometrySpacingControls(false);
  }
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
    span_modified_.linecable.set_cable(nullptr);
  } else {
    str = choice->GetString(index);
    auto iter = std::next(cablefiles_->cbegin(), index);
    span_modified_.linecable.set_cable(&(*iter)->cable);
  }

  // transfers constraint limit
  CableConstraint constraint;
  textctrl = XRCCTRL(*this, "textctrl_constraint_limit", wxTextCtrl);
  str = textctrl->GetValue();
  str.ToDouble(&constraint.limit);

  // transfers constraint type
  choice = XRCCTRL(*this, "choice_constraint_type", wxChoice);
  index = choice->GetSelection();
  str = choice->GetString(index);

  if (str == "Support") {
    constraint.type_limit = CableConstraint::LimitType::kSupportTension;
  } else if (str == "Horizontal") {
    constraint.type_limit = CableConstraint::LimitType::kHorizontalTension;
  } else if (str == "H/w") {
    constraint.type_limit = CableConstraint::LimitType::kCatenaryConstant;
  } else if (str == "Length") {
    constraint.type_limit = CableConstraint::LimitType::kLength;
  } else if (str == "Sag") {
    constraint.type_limit = CableConstraint::LimitType::kSag;
  } else {
    constraint.type_limit = CableConstraint::LimitType::kNull;
  }

  // transfers constraint weathercase
  choice = XRCCTRL(*this, "choice_constraint_weathercase", wxChoice);
  index = choice->GetSelection();
  if (index == wxNOT_FOUND) {
    constraint.case_weather = nullptr;
  } else {
    auto iter = std::next(weathercases_->cbegin(), index);
    const WeatherLoadCase* weathercase = *iter;
    constraint.case_weather = weathercase;
  }

  // transfers constraint condition
  choice = XRCCTRL(*this, "choice_constraint_condition", wxChoice);
  index = choice->GetSelection();
  if (index == wxNOT_FOUND) {
    constraint.condition = CableConditionType::kNull;
  } else {
    str = choice->GetString(index);
    if (str == "Initial") {
      constraint.condition = CableConditionType::kInitial;
    } else if (str == "Load") {
      constraint.condition = CableConditionType::kLoad;
    } else if (str == "Creep") {
      constraint.condition = CableConditionType::kCreep;
    }
  }

  span_modified_.linecable.set_constraint(constraint);

  // transfers stretch-load weathercase
  choice = XRCCTRL(*this, "choice_stretch_load_weathercase", wxChoice);
  index = choice->GetSelection();
  if (index == wxNOT_FOUND) {
    span_modified_.linecable.set_weathercase_stretch_load(nullptr);
  } else {
    auto iter = std::next(weathercases_->cbegin(), index);
    const WeatherLoadCase* weathercase = *iter;
    span_modified_.linecable.set_weathercase_stretch_load(weathercase);
  }

  // transfers stretch-creep weathercase
  choice = XRCCTRL(*this, "choice_stretch_creep_weathercase", wxChoice);
  index = choice->GetSelection();
  if (index == wxNOT_FOUND) {
    span_modified_.linecable.set_weathercase_stretch_creep(nullptr);
  } else {
    auto iter = std::next(weathercases_->cbegin(), index);
    const WeatherLoadCase* weathercase = *iter;
    span_modified_.linecable.set_weathercase_stretch_creep(weathercase);
  }

  // transfers line section spacing
  Vector3d spacing(0, 0, 0);

  textctrl = XRCCTRL(*this, "textctrl_line_section_spacing_horizontal",
                     wxTextCtrl);
  str = textctrl->GetValue();
  str.ToDouble(&value);
  spacing.set_x(value);

  if (span_modified_.type == Span::Type::kDeadendSpan) {
    textctrl = XRCCTRL(*this, "textctrl_line_section_spacing_vertical",
                       wxTextCtrl);
    str = textctrl->GetValue();
    str.ToDouble(&value);
    spacing.set_z(value);
  } else if (span_modified_.type == Span::Type::kRulingSpan) {
    spacing.set_z(0);
  }

  span_modified_.linecable.set_spacing_attachments_ruling_span(spacing);

  // transfers span spacing
  if (span_modified_.type == Span::Type::kDeadendSpan) {
    spacing = span_modified_.linecable.spacing_attachments_ruling_span();
  } else if (span_modified_.type == Span::Type::kRulingSpan) {
    wxCheckBox* checkbox = XRCCTRL(*this,
                                   "checkbox_match_line_section_geometry",
                                   wxCheckBox);
    if (checkbox->IsChecked() == true) {
      spacing = span_modified_.linecable.spacing_attachments_ruling_span();
    } else {
      spacing = Vector3d(0, 0, 0);
      textctrl = XRCCTRL(*this, "textctrl_span_spacing_horizontal",
                         wxTextCtrl);
      str = textctrl->GetValue();
      str.ToDouble(&value);
      spacing.set_x(value);

      textctrl = XRCCTRL(*this, "textctrl_span_spacing_vertical",
                         wxTextCtrl);
      str = textctrl->GetValue();
      str.ToDouble(&value);
      spacing.set_z(value);
    }
  }

  span_modified_.spacing_attachments = spacing;
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
  if (span_modified_.linecable.cable() != nullptr) {
    choice = XRCCTRL(*this, "choice_cable", wxChoice);
    index = choice->FindString(span_modified_.linecable.cable()->name, true);
    choice->SetSelection(index);
  }

  // transfers constraint type
  choice = XRCCTRL(*this, "choice_constraint_type", wxChoice);
  CableConstraint::LimitType type_limit =
      span_modified_.linecable.constraint().type_limit;
  if (type_limit == CableConstraint::LimitType::kSupportTension) {
    choice->SetSelection(0);
  } else if (type_limit == CableConstraint::LimitType::kHorizontalTension) {
    choice->SetSelection(1);
  } else if (type_limit == CableConstraint::LimitType::kCatenaryConstant) {
    choice->SetSelection(2);
  } else if (type_limit == CableConstraint::LimitType::kLength) {
    choice->SetSelection(3);
  } else if (type_limit == CableConstraint::LimitType::kSag) {
    choice->SetSelection(4);
  }

  // transfers constraint limit
  textctrl = XRCCTRL(*this, "textctrl_constraint_limit", wxTextCtrl);
  str = wxString::FromDouble(span_modified_.linecable.constraint().limit, 2);
  textctrl->SetValue(str);

  // transfers constraint weathercase
  if (span_modified_.linecable.constraint().case_weather != nullptr) {
    choice = XRCCTRL(*this, "choice_constraint_weathercase", wxChoice);
    index = choice->FindString(
        span_modified_.linecable.constraint().case_weather->description,
        true);
    choice->SetSelection(index);
  }

  // transfers constraint condition
  choice = XRCCTRL(*this, "choice_constraint_condition", wxChoice);
  CableConditionType type_condition =
      span_modified_.linecable.constraint().condition;
  if (type_condition == CableConditionType::kInitial) {
    choice->SetSelection(0);
  } else if (type_condition == CableConditionType::kLoad) {
    choice->SetSelection(1);
  } else if (type_condition == CableConditionType::kCreep) {
    choice->SetSelection(2);
  }

  // transfers stretch-load weathercase
  if (span_modified_.linecable.weathercase_stretch_load() != nullptr) {
    choice = XRCCTRL(*this, "choice_stretch_load_weathercase", wxChoice);
    index = choice->FindString(
        span_modified_.linecable.weathercase_stretch_load()->description,
        true);
    choice->SetSelection(index);
  }

  // transfers stretch-creep weathercase
  if (span_modified_.linecable.weathercase_stretch_creep() != nullptr) {
    choice = XRCCTRL(*this, "choice_stretch_creep_weathercase", wxChoice);
    index = choice->FindString(
        span_modified_.linecable.weathercase_stretch_creep()->description,
        true);
    choice->SetSelection(index);
  }

  // transfers line section geometry
  textctrl = XRCCTRL(*this, "textctrl_line_section_spacing_horizontal",
                     wxTextCtrl);
  str = wxString::FromDouble(
      span_modified_.linecable.spacing_attachments_ruling_span().x(),
      2);
  textctrl->SetValue(str);

  textctrl = XRCCTRL(*this, "textctrl_line_section_spacing_vertical",
                      wxTextCtrl);
  str = wxString::FromDouble(
      span_modified_.linecable.spacing_attachments_ruling_span().z(),
      2);
  textctrl->SetValue(str);

  // transfers span geometry
  const Vector3d& spacing_section =
      span_modified_.linecable.spacing_attachments_ruling_span();
  const Vector3d& spacing_span = span_modified_.spacing_attachments;

  const bool is_match =
      ((spacing_section.x() == spacing_span.x())
        && (spacing_section.y() == spacing_span.y())
        && (spacing_section.z() == spacing_span.z()));
  if (is_match == true) {
    wxCheckBox* checkbox = XRCCTRL(*this,
                                   "checkbox_match_line_section_geometry",
                                   wxCheckBox);
    checkbox->SetValue(true);

    ToggleSpanGeometrySpacingControls(false);
  } else {
    // toggles checkbox
    wxCheckBox* checkbox = XRCCTRL(*this,
                                   "checkbox_match_line_section_geometry",
                                   wxCheckBox);
    checkbox->SetValue(false);

    // toggles catenary spacing controls
    ToggleSpanGeometrySpacingControls(true);

    // updates textctrls
    textctrl = XRCCTRL(*this, "textctrl_span_spacing_horizontal",
                       wxTextCtrl);
    str = helper::DoubleToString(spacing_span.x(), 2, true);
    textctrl->SetValue(str);

    textctrl = XRCCTRL(*this, "textctrl_span_spacing_vertical",
                       wxTextCtrl);
    str = helper::DoubleToString(spacing_span.z(), 2, true);
    textctrl->SetValue(str);
  }
}
