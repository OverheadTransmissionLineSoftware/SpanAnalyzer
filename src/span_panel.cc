// This is free and unencumbered software released into the public domain.
// For more information, please refer to <http://unlicense.org/>

#include "span_panel.h"

#include "models/base/helper.h"
#include "wx/xrc/xmlres.h"

#include "span_analyzer_view.h"
#include "span.h"

BEGIN_EVENT_TABLE(SpanPanel, wxPanel)
  EVT_CHOICE(XRCID("choice_sagtension_weathercase"), SpanPanel::OnChoiceWeathercase)
  EVT_CHOICE(XRCID("choice_sagtension_condition"), SpanPanel::OnChoiceCondition)
END_EVENT_TABLE()

SpanPanel::SpanPanel(wxWindow* parent,
                     const SagTensionAnalysisResultSet* results) {
  wxXmlResource::Get()->LoadPanel(this, parent, "span_panel");

  results_ = results;

  // fills the reloaded condition
  wxChoice* choice = XRCCTRL(*this, "choice_sagtension_condition", wxChoice);
  choice->Append("Initial");
  choice->Append("Load");
}

SpanPanel::~SpanPanel() {
}

void SpanPanel::UpdateView(wxObject* hint) {
  if (hint == nullptr) {
    return;
  }

  // interprets hint type
  ViewUpdateHint* hint_update = (ViewUpdateHint*)hint;
  if (hint_update->type() == ViewUpdateHint::HintType::kModelSpansEdit) {
    UpdateWeathercases();
    UpdateData();
  } else if (hint_update->type() ==
       ViewUpdateHint::HintType::kModelWeathercaseEdit) {
    UpdateWeathercases();
    UpdateData();
  }
}

void SpanPanel::SetUnitsStaticText(const units::UnitSystem& units) {
  /// \todo
  ///  wxWidgets seems to have a bug when editing labels. The StaticText
  ///  controls are not re-sized

  //wxStaticText* statictext = nullptr;
  //wxString str;

  //if (model_->units_ == units::UnitSystem::kMetric) {
  //  statictext = XRCCTRL(*frame_,
  //                       "statictext_constraint_spacing_horizontal_units",
  //                       wxStaticText);
  //  statictext->SetLabel("[???]");

  //  statictext = XRCCTRL(*frame_,
  //                       "statictext_constraint_spacing_vertical_units",
  //                       wxStaticText);
  //  statictext->SetLabel("[???]");

  //  statictext = XRCCTRL(*frame_, "statictext_tension_horizontal_units",
  //                       wxStaticText);
  //  statictext->SetLabel("[???]");

  //  statictext = XRCCTRL(*frame_, "statictext_catenary_constant_units",
  //                       wxStaticText);
  //  statictext->SetLabel("[???]");

  //  statictext = XRCCTRL(*frame_, "statictext_tension_horizontal_shell_units",
  //                       wxStaticText);
  //  statictext->SetLabel("[???]");

  //  statictext = XRCCTRL(*frame_, "statictext_tension_horizontal_core_units",
  //                       wxStaticText);
  //  statictext->SetLabel("[???]");
  //} else if (model_->units_ == units::UnitSystem::kImperial) {
  //  statictext = XRCCTRL(*frame_,
  //                       "statictext_constraint_spacing_horizontal_units",
  //                       wxStaticText);
  //  statictext->SetLabel("[ft]");

  //  statictext = XRCCTRL(*frame_,
  //                       "statictext_constraint_spacing_vertical_units",
  //                       wxStaticText);
  //  statictext->SetLabel("[ft]");

  //  statictext = XRCCTRL(*frame_, "statictext_tension_horizontal_units",
  //                       wxStaticText);
  //  statictext->SetLabel("[lbs]");

  //  statictext = XRCCTRL(*frame_, "statictext_catenary_constant_units",
  //                       wxStaticText);
  //  statictext->SetLabel("[ft]");

  //  statictext = XRCCTRL(*frame_, "statictext_tension_horizontal_shell_units",
  //                       wxStaticText);
  //  statictext->SetLabel("[lbs]");

  //  statictext = XRCCTRL(*frame_, "statictext_tension_horizontal_core_units",
  //                       wxStaticText);
  //  statictext->SetLabel("[lbs]");
  //}
}

void SpanPanel::OnChoiceWeathercase(wxCommandEvent& event) {
  UpdateData();
}

void SpanPanel::OnChoiceCondition(wxCommandEvent& event) {
  UpdateData();
}

void SpanPanel::UpdateWeathercases() {
  // gets choice and current selection string
  wxChoice* choice = XRCCTRL(*this,
                             "choice_sagtension_weathercase",
                             wxChoice);
  wxString str_choice = choice->GetString(choice->GetSelection());

  // updates sag-tension weathercase choice and selection
  const std::list<std::string>& descriptions =
      results_->descriptions_weathercase;
  choice->Clear();
  for (auto iter = descriptions.cbegin(); iter != descriptions.cend();
        iter++) {
    const std::string& description = *iter;
    choice->Append(description);
  }
  choice->SetSelection(choice->FindString(str_choice));
}

void SpanPanel::UpdateData() {
  wxStaticText* statictext = nullptr;
  double value = 0;
  wxString str;

  // finds sag-tension result to display
  const std::list<SagTensionAnalysisResult>* results = nullptr;
  const SagTensionAnalysisResult* result = nullptr;

  wxChoice* choice = nullptr;
  wxString str_choice;

  // gets list of results for the specified condition
  choice = XRCCTRL(*this, "choice_sagtension_condition", wxChoice);
  str_choice = choice->GetString(choice->GetSelection());
  if (str_choice == "Initial") {
    results = &results_->results_initial;
  } else if (str_choice == "Load") {
    results = &results_->results_load;
  } else {
    return;
  }

  // searches results for the specified weathercase
  choice = XRCCTRL(*this, "choice_sagtension_weathercase", wxChoice);
  str_choice = choice->GetString(choice->GetSelection());

  const std::list<std::string>& descriptions =
      results_->descriptions_weathercase;
  auto iter = descriptions.cbegin();
  while (iter != descriptions.cend()) {
    const std::string& description = *iter;
    if (description == str_choice) {
      break;
    } else {
      iter++;
    }
  }

  const int kIndex = std::distance(descriptions.cbegin(), iter);

  // gets result with matching index
  auto iter_result = std::next(results->cbegin(), kIndex);
  result = &(*iter_result);

  // updates static text labels with results
  statictext = XRCCTRL(*this,
                       "statictext_tension_horizontal_value",
                       wxStaticText);
  value = result->tension_horizontal;
  str = helper::DoubleToFormattedString(value, 1);
  statictext->SetLabel(str);

  statictext = XRCCTRL(*this,
                       "statictext_tension_horizontal_shell_value",
                       wxStaticText);
  value = result->tension_horizontal_shell;
  str = helper::DoubleToFormattedString(value, 1);
  statictext->SetLabel(str);

  statictext = XRCCTRL(*this,
                       "statictext_tension_horizontal_core_value",
                       wxStaticText);
  value = result->tension_horizontal_core;
  str = helper::DoubleToFormattedString(value, 1);
  statictext->SetLabel(str);

  statictext = XRCCTRL(*this,
                       "statictext_catenary_constant_value",
                       wxStaticText);
  value = result->tension_horizontal / result->weight_unit.Magnitude();
  str = helper::DoubleToFormattedString(value, 1);
  statictext->SetLabel(str);
}
