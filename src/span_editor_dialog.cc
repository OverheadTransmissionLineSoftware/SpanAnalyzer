// This is free and unencumbered software released into the public domain.
// For more information, please refer to <http://unlicense.org/>

#include "span_editor_dialog.h"

#include "wx/valnum.h"
#include "wx/xrc/xmlres.h"

#include "error_message_dialog.h"

BEGIN_EVENT_TABLE(SpanEditorDialog, wxDialog)
  EVT_BUTTON(wxID_CANCEL, SpanEditorDialog::OnCancel)
  EVT_BUTTON(wxID_OK, SpanEditorDialog::OnOk)
  EVT_CLOSE(SpanEditorDialog::OnClose)
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

void SpanEditorDialog::OnCancel(wxCommandEvent &event) {
  EndModal(wxID_CANCEL);
}

void SpanEditorDialog::OnClose(wxCloseEvent &event) {
  EndModal(wxID_CLOSE);
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
}

void SpanEditorDialog::InitChoiceControls() {
  wxChoice* choice = nullptr;

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

void SpanEditorDialog::TransferCustomDataFromWindow() {
  wxChoice* choice = nullptr;
  wxTextCtrl* textctrl = nullptr;
  wxString str;
  int index = -9999;
  double value = -999999;

  // transfers name
  textctrl = XRCCTRL(*this, "textctrl_name", wxTextCtrl);
  span_modified_.name = textctrl->GetValue();

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

  // transfers spacing
  Vector3d spacing(0, 0, 0);

  textctrl = XRCCTRL(*this, "textctrl_constraint_spacing_horizontal",
                     wxTextCtrl);
  str = textctrl->GetValue();
  str.ToDouble(&value);
  spacing.set_x(value);

  textctrl = XRCCTRL(*this, "textctrl_constraint_spacing_vertical",
                     wxTextCtrl);
  str = textctrl->GetValue();
  str.ToDouble(&value);
  spacing.set_z(value);

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
}

void SpanEditorDialog::TransferCustomDataToWindow() {
  wxChoice* choice = nullptr;
  wxTextCtrl* textctrl = nullptr;
  wxString str;
  int index = -9999;

  // transfers name
  // transfers name
  textctrl = XRCCTRL(*this, "textctrl_name", wxTextCtrl);
  textctrl->SetValue(span_modified_.name);

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

  // transfers spacing-horizontal
  textctrl = XRCCTRL(*this, "textctrl_constraint_spacing_horizontal",
                     wxTextCtrl);
  str = wxString::FromDouble(
      span_modified_.linecable.spacing_attachments_ruling_span.x(),
      1);
  textctrl->SetValue(str);

  // transfers spacing-vertical
  textctrl = XRCCTRL(*this, "textctrl_constraint_spacing_vertical",
                     wxTextCtrl);
  str = wxString::FromDouble(
      span_modified_.linecable.spacing_attachments_ruling_span.z(),
      1);
  textctrl->SetValue(str);

  // transfers stretch-load weathercase
  if (span_modified_.linecable.weathercase_stretch_load != nullptr) {
    choice = XRCCTRL(*this, "choice_stretch_weathercase", wxChoice);
    index = choice->FindString(
        span_modified_.linecable.weathercase_stretch_load->description,
        true);
    choice->SetSelection(index);
  }
}
