// This is free and unencumbered software released into the public domain.
// For more information, please refer to <http://unlicense.org/>

#include "spananalyzer/cable_constraint_editor_dialog.h"

#include "appcommon/widgets/error_message_dialog.h"
#include "wx/valnum.h"
#include "wx/xrc/xmlres.h"

BEGIN_EVENT_TABLE(CableConstraintEditorDialog, wxDialog)
  EVT_BUTTON(wxID_CANCEL, CableConstraintEditorDialog::OnCancel)
  EVT_BUTTON(wxID_OK, CableConstraintEditorDialog::OnOk)
  EVT_CHOICE(XRCID("choice_type"), CableConstraintEditorDialog::OnChoiceType)
  EVT_CLOSE(CableConstraintEditorDialog::OnClose)
END_EVENT_TABLE()

CableConstraintEditorDialog::CableConstraintEditorDialog(
    wxWindow* parent,
    const std::list<CableFile*>* cablefiles,
    const std::list<WeatherLoadCase*>* weathercases,
    const units::UnitSystem& units,
    CableConstraint* constraint) {
  // loads dialog from virtual xrc file system
  wxXmlResource::Get()->LoadDialog(this, parent,
                                   "cable_constraint_editor_dialog");

  // saves constructor parameters
  cablefiles_ = cablefiles;
  weathercases_ = weathercases;
  units_ = &units;

  // saves unmodified reference and makes a duplicate for modifying
  constraint_ = constraint;
  constraint_modified_ = CableConstraint(*constraint);

  // inits form controls
  InitChoiceControls();

  // sets form validators to transfer between controls data and controls
  SetValidators();

  // transfers non-validator data to the window
  TransferCustomDataToWindow();

  // updates unit system labels
  SetUnitsStaticText(*units_);

  // fits the dialog around the sizers
  this->Fit();
}

CableConstraintEditorDialog::~CableConstraintEditorDialog() {
}

void CableConstraintEditorDialog::InitChoiceControls() {
  wxChoice* choice = nullptr;

  // populates cable choice control
  choice = XRCCTRL(*this, "choice_cable", wxChoice);
  choice->Append("");
  for (auto iter = cablefiles_->cbegin(); iter != cablefiles_->cend();
       iter++) {
    const CableFile* cablefile = *iter;
    choice->Append(cablefile->cable.name);
  }

  // populates weathercase choice control
  choice = XRCCTRL(*this, "choice_weathercase", wxChoice);
  for (auto iter = weathercases_->cbegin(); iter != weathercases_->cend();
       iter++) {
    const WeatherLoadCase* weathercase = *iter;
    choice->Append(weathercase->description);
  }

  // populates condition choice control
  choice = XRCCTRL(*this, "choice_condition", wxChoice);
  choice->Append("Initial");
  choice->Append("Load");
  choice->Append("Creep");

  // populates type choice control
  choice = XRCCTRL(*this, "choice_type", wxChoice);
  choice->Append("Support");
  choice->Append("Horizontal");
  choice->Append("H/w");
  choice->Append("Length");
  choice->Append("Sag");
}

void CableConstraintEditorDialog::OnCancel(wxCommandEvent &event) {
  EndModal(wxID_CANCEL);
}

void CableConstraintEditorDialog::OnChoiceType(wxCommandEvent& event) {
  // updates units on form
  SetUnitsStaticText(*units_);
}

void CableConstraintEditorDialog::OnClose(wxCloseEvent &event) {
  EndModal(wxID_CLOSE);
}

void CableConstraintEditorDialog::OnOk(wxCommandEvent &event) {
  // validates data from form
  if (this->Validate() == false) {
    wxMessageBox("Errors on form");
    return;
  }

  wxBusyCursor cursor;

  // transfers data from dialog controls
  TransferDataFromWindow();
  TransferCustomDataFromWindow();

  // validates constraint data
  std::list<ErrorMessage> messages;
  if (constraint_modified_.Validate(true, &messages) == true) {
    // updates original constraint reference based on user form edits
    *constraint_ = CableConstraint(constraint_modified_);

    // ends modal by returning ok indicator
    EndModal(wxID_OK);
  } else {
    // displays errors to user
    ErrorMessageDialog message(this, &messages);
    message.ShowModal();
    return;
  }
}

void CableConstraintEditorDialog::SetUnitsStaticText(
    const units::UnitSystem& units) {
  // updates constraint limit units based on type
  wxChoice* choice_type = XRCCTRL(*this, "choice_type", wxChoice);
  const std::string str = choice_type->GetStringSelection();

  wxStaticText* statictext =
      XRCCTRL(*this, "statictext_limit_units", wxStaticText);

  if (units == units::UnitSystem::kImperial) {
    if (str == "Support") {
      statictext->SetLabel("[lbs]");
    } else if (str == "Horizontal") {
      statictext->SetLabel("[lbs]");
    } else if (str == "H/w") {
      statictext->SetLabel("[ft]");
    } else if (str == "Length") {
      statictext->SetLabel("[ft]");
    } else if (str == "Sag") {
      statictext->SetLabel("[ft]");
    }
  } else if (units == units::UnitSystem::kMetric) {
    if (str == "Support") {
      statictext->SetLabel("[N]");
    } else if (str == "Horizontal") {
      statictext->SetLabel("[N]");
    } else if (str == "H/w") {
      statictext->SetLabel("[m]");
    } else if (str == "Length") {
      statictext->SetLabel("[m]");
    } else if (str == "Sag") {
      statictext->SetLabel("[m]");
    }
  } else {
    statictext->SetLabel("");
  }
}

void CableConstraintEditorDialog::SetValidators() {
  // variables used for creating validators
  int style = 0;
  double* value_num = nullptr;
  int precision = 0;
  wxTextCtrl* textctrl = nullptr;

  // constraint limit
  precision = 2;
  value_num = &constraint_modified_.limit;
  style = wxNUM_VAL_NO_TRAILING_ZEROES;
  textctrl = XRCCTRL(*this, "textctrl_limit", wxTextCtrl);
  textctrl->SetValidator(
      wxFloatingPointValidator<double>(precision, value_num, style));
}

void CableConstraintEditorDialog::TransferCustomDataFromWindow() {
  wxChoice* choice = nullptr;
  wxString str;
  int index = -9999;

  // transfers cable
  choice = XRCCTRL(*this, "choice_cable", wxChoice);
  index = choice->GetSelection();
  if (index == wxNOT_FOUND) {
    constraint_modified_.note = "";
  } else {
    constraint_modified_.note = choice->GetStringSelection();
  }

  // transfers weathercase
  choice = XRCCTRL(*this, "choice_weathercase", wxChoice);
  index = choice->GetSelection();
  if (index == wxNOT_FOUND) {
    constraint_modified_.case_weather = nullptr;
  } else {
    auto iter = std::next(weathercases_->cbegin(), index);
    const WeatherLoadCase* weathercase = *iter;
    constraint_modified_.case_weather = weathercase;
  }

  // transfers constraint condition
  choice = XRCCTRL(*this, "choice_condition", wxChoice);
  index = choice->GetSelection();
  if (index == wxNOT_FOUND) {
    constraint_modified_.condition = CableConditionType::kNull;
  } else {
    str = choice->GetString(index);
    if (str == "Initial") {
      constraint_modified_.condition = CableConditionType::kInitial;
    } else if (str == "Load") {
      constraint_modified_.condition = CableConditionType::kLoad;
    } else if (str == "Creep") {
      constraint_modified_.condition = CableConditionType::kCreep;
    }
  }

  // transfers constraint type
  choice = XRCCTRL(*this, "choice_type", wxChoice);
  index = choice->GetSelection();
  str = choice->GetString(index);

  if (str == "Support") {
    constraint_modified_.type_limit =
        CableConstraint::LimitType::kSupportTension;
  } else if (str == "Horizontal") {
    constraint_modified_.type_limit =
        CableConstraint::LimitType::kHorizontalTension;
  } else if (str == "H/w") {
    constraint_modified_.type_limit =
        CableConstraint::LimitType::kCatenaryConstant;
  } else if (str == "Length") {
    constraint_modified_.type_limit = CableConstraint::LimitType::kLength;
  } else if (str == "Sag") {
    constraint_modified_.type_limit = CableConstraint::LimitType::kSag;
  } else {
    constraint_modified_.type_limit = CableConstraint::LimitType::kNull;
  }
}

void CableConstraintEditorDialog::TransferCustomDataToWindow() {
  wxChoice* choice = nullptr;
  int index = -9999;

  // transfers cable
  if (constraint_modified_.note != "") {
    choice = XRCCTRL(*this, "choice_cable", wxChoice);
    index = choice->FindString(constraint_modified_.note, true);
    choice->SetSelection(index);
  }

  // transfers weathercase
  if (constraint_modified_.case_weather != nullptr) {
    choice = XRCCTRL(*this, "choice_weathercase", wxChoice);
    index = choice->FindString(
        constraint_modified_.case_weather->description,
        true);
    choice->SetSelection(index);
  }

  // transfers condition
  choice = XRCCTRL(*this, "choice_condition", wxChoice);
  const CableConditionType kTypeCondition = constraint_modified_.condition;
  if (kTypeCondition == CableConditionType::kInitial) {
    choice->SetSelection(0);
  } else if (kTypeCondition == CableConditionType::kLoad) {
    choice->SetSelection(1);
  } else if (kTypeCondition == CableConditionType::kCreep) {
    choice->SetSelection(2);
  }

  // transfers type
  choice = XRCCTRL(*this, "choice_type", wxChoice);
  const CableConstraint::LimitType kTypeLimit = constraint_modified_.type_limit;
  if (kTypeLimit == CableConstraint::LimitType::kSupportTension) {
    choice->SetSelection(0);
  } else if (kTypeLimit == CableConstraint::LimitType::kHorizontalTension) {
    choice->SetSelection(1);
  } else if (kTypeLimit == CableConstraint::LimitType::kCatenaryConstant) {
    choice->SetSelection(2);
  } else if (kTypeLimit == CableConstraint::LimitType::kLength) {
    choice->SetSelection(3);
  } else if (kTypeLimit == CableConstraint::LimitType::kSag) {
    choice->SetSelection(4);
  }
}
