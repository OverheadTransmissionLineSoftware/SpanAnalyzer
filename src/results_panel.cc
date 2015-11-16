// This is free and unencumbered software released into the public domain.
// For more information, please refer to <http://unlicense.org/>

#include "results_panel.h"

#include "models/base/helper.h"
#include "wx/xrc/xmlres.h"

#include "edit_panel.h"
#include "span.h"
#include "span_analyzer_app.h"
#include "span_analyzer_doc.h"
#include "span_analyzer_view.h"

BEGIN_EVENT_TABLE(ResultsPanel, wxPanel)
  EVT_CHOICE(XRCID("choice_sagtension_weathercase"), ResultsPanel::OnChoiceWeathercase)
  EVT_CHOICE(XRCID("choice_sagtension_condition"), ResultsPanel::OnChoiceCondition)
END_EVENT_TABLE()

ResultsPanel::ResultsPanel(wxWindow* parent, SpanAnalyzerView* view) {
  // loads dialog from virtual xrc file system
  wxXmlResource::Get()->LoadPanel(this, parent, "results_panel");

  // saves view reference
  view_ = view;

  // fills the reloaded condition
  wxChoice* choice = XRCCTRL(*parent, "choice_sagtension_condition", wxChoice);
  choice->Append("Initial");
  choice->Append("Final-Load");
}

ResultsPanel::~ResultsPanel() {
}

void ResultsPanel::Update(wxObject* hint) {
  if (hint == nullptr) {
    return;
  }

  // gets weathercases
  SpanAnalyzerDoc* doc = (SpanAnalyzerDoc*)view_->GetDocument();
  const std::vector<WeatherLoadCase>& weathercases = *doc->weathercases();

  wxChoice* choice = nullptr;
  wxString str_choice;

  // updates sag-tension weathercase choice and selection
  choice = XRCCTRL(*view_->GetFrame(), "choice_sagtension_weathercase",
                   wxChoice);
  str_choice = choice->GetString(choice->GetSelection());

  choice->Clear();
  for (auto iter = weathercases.cbegin(); iter != weathercases.cend();
        iter++) {
    const WeatherLoadCase& weathercase = *iter;
    choice->Append(weathercase.description);
  }
  choice->SetSelection(choice->FindString(str_choice));

  // gets activated span
  Span* span = view_->panel_edit()->ActivatedSpan();
  if (span != nullptr) {
    reloader_.set_line_cable(&span->linecable);
  }

  reloader_.set_length_unloaded_unstretched_adjustment(0);
  UpdateSagTensionResults();
}

void ResultsPanel::SetUnitsStaticText(const units::UnitSystem& units) {
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

void ResultsPanel::OnChoiceWeathercase(wxCommandEvent& event) {
  // gets weathercases
  SpanAnalyzerDoc* doc = (SpanAnalyzerDoc*)view_->GetDocument();
  const std::vector<WeatherLoadCase>& weathercases = *doc->weathercases();

  // gets selection and updates reloader
  wxChoice* choice = XRCCTRL(*view_->GetFrame(),
                             "choice_sagtension_weathercase", wxChoice);
  const int index_selection = choice->GetSelection();
  const WeatherLoadCase& weathercase = weathercases.at(index_selection);
  reloader_.set_weathercase_reloaded(&weathercase);

  // updates sag-tension results
  UpdateSagTensionResults();
}

void ResultsPanel::OnChoiceCondition(wxCommandEvent& event) {
  // gets condition selection and updates reloader
  wxChoice* choice = XRCCTRL(*view_->GetFrame(),
                             "choice_sagtension_condition", wxChoice);
  const wxString str_selection = choice->GetStringSelection();
  if (str_selection == "Initial") {
    reloader_.set_condition_reloaded(CableConditionType::kInitial);
  } else if (str_selection == "Final-Load") {
    reloader_.set_condition_reloaded(CableConditionType::kLoad);
  }

  // updates sag-tension results
  UpdateSagTensionResults();
}

void ResultsPanel::UpdateSagTensionResults() {
  wxStaticText* statictext = nullptr;
  double value = 0;
  wxString str;

  // validates reloader
  std::list<ErrorMessage> messages;
  bool status_validate = reloader_.Validate(false, &messages);

  statictext = XRCCTRL(*view_->GetFrame(),
                       "statictext_tension_horizontal_value",
                       wxStaticText);
  if (status_validate == false) {
    statictext->SetLabel("");
  } else if (status_validate == true) {
    value = reloader_.TensionHorizontal();
    str = helper::DoubleToFormattedString(value, 1);
    statictext->SetLabel(str);
  }

  statictext = XRCCTRL(*view_->GetFrame(), "statictext_catenary_constant_value",
                       wxStaticText);
  if (status_validate == false) {
    statictext->SetLabel("");
  } else if (status_validate == true) {
    value = reloader_.CatenaryReloaded().Constant();
    str = helper::DoubleToFormattedString(value, 1);
    statictext->SetLabel(str);
  }

  statictext = XRCCTRL(*view_->GetFrame(), "statictext_tension_horizontal_shell_value",
                       wxStaticText);
  if (status_validate == false) {
    statictext->SetLabel("");
  } else if (status_validate == true) {
    value = reloader_.TensionHorizontalComponent(
        CableElongationModel::ComponentType::kShell);
    str = helper::DoubleToFormattedString(value, 1);
    statictext->SetLabel(str);
  }

  statictext = XRCCTRL(*view_->GetFrame(), "statictext_tension_horizontal_core_value",
                       wxStaticText);
  if (status_validate == false) {
    statictext->SetLabel("");
  } else if (status_validate == true) {
    value = reloader_.TensionHorizontalComponent(
        CableElongationModel::ComponentType::kCore);
    str = helper::DoubleToFormattedString(value, 1);
    statictext->SetLabel(str);
  }
}

  //std::vector<std::string> columns;
  //columns.push_back("Weathercase");
  //columns.push_back("Vertical");
  //columns.push_back("Transverse");
  //columns.push_back("Resultant");
  //columns.push_back("Horizontal Tension");
  //columns.push_back("Core Tension");
  //columns.push_back("Shell Tension");
  //columns.push_back("H/w");
  //columns.push_back("Sag");

  //std::vector<ReportRow> rows;
  //const std::vector<WeatherLoadCase>& weathercases = model_->weathercases_;
  //LineCableReloader reloader;
  //reloader.set_line_cable(&model_->line_cable_);
  //reloader.set_length_unloaded_unstretched_adjustment(0);
  //reloader.set_condition_reloaded(CableConditionType::kInitial);

  //for (auto iter = weathercases.cbegin(); iter != weathercases.cend();
  //     iter++) {
  //  const WeatherLoadCase& weathercase = *iter;
  //
  //  // updates reloader
  //  reloader.set_weathercase_reloaded(&weathercase);
  //  Catenary3d catenary = reloader.CatenaryReloaded();

  //  // calculate row values
  //  ReportRow row;
  //  double value;
  //  std::string str;

  //  str = weathercase.description;
  //  row.values.push_back(str);

  //  value = catenary.weight_unit().z();
  //  str = helper::DoubleToFormattedString(value, 3);
  //  row.values.push_back(str);

  //  value = catenary.weight_unit().y();
  //  str = helper::DoubleToFormattedString(value, 3);
  //  row.values.push_back(str);

  //  value = catenary.weight_unit().Magnitude();
  //  str = helper::DoubleToFormattedString(value, 3);
  //  row.values.push_back(str);

  //  value = reloader.TensionHorizontal();
  //  str = helper::DoubleToFormattedString(value, 1);
  //  row.values.push_back(str);

  //  value = reloader.TensionHorizontalComponent(
  //      CableElongationModel::ComponentType::kCore);
  //  str = helper::DoubleToFormattedString(value, 1);
  //  row.values.push_back(str);

  //  value = reloader.TensionHorizontalComponent(
  //      CableElongationModel::ComponentType::kShell);
  //  str = helper::DoubleToFormattedString(value, 1);
  //  row.values.push_back(str);

  //  value = catenary.Constant();
  //  str = helper::DoubleToFormattedString(value, 1);
  //  row.values.push_back(str);

  //  value = catenary.Sag();
  //  str = helper::DoubleToFormattedString(value, 1);
  //  row.values.push_back(str);

  //  rows.push_back(row);
  //}

  //ReportDialog report(frame_, &columns, &rows);
  //report.ShowModal();
//}
