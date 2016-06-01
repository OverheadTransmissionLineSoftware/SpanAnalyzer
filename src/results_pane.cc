// This is free and unencumbered software released into the public domain.
// For more information, please refer to <http://unlicense.org/>

#include "results_pane.h"

#include "models/sagtension/line_cable_reloader.h"
#include "wx/notebook.h"
#include "wx/xrc/xmlres.h"

#include "edit_pane.h"
#include "span_analyzer_app.h"
#include "span_analyzer_doc.h"

BEGIN_EVENT_TABLE(ResultsPane, wxPanel)
  EVT_CHOICE(XRCID("choice_weathercase_set"), ResultsPane::OnChoiceWeathercaseSet)
END_EVENT_TABLE()

ResultsPane::ResultsPane(wxWindow* parent, SpanAnalyzerView* view) {
  // loads dialog from virtual xrc file system
  wxXmlResource::Get()->LoadPanel(this, parent, "results_pane");

  // saves view reference
  view_ = view;

  // initializes weathercase set choice
  UpdateAnalysisWeathercaseSetChoice();

  // creates notebook and adds child notebook pages
  wxNotebook* notebook = XRCCTRL(*this, "notebook_results", wxNotebook);

  //panel_span_ = new SpanPanel(notebook, &results_);
  //notebook->AddPage(panel_span_, "Span");

  panel_table_sagtension_ = new SagTensionTablePanel(notebook, &results_);
  notebook->AddPage(panel_table_sagtension_, "Sag-Tension Table");

  panel_table_catenary_ = new CatenaryTablePanel(notebook, &results_);
  notebook->AddPage(panel_table_catenary_, "Catenary Table");
}

ResultsPane::~ResultsPane() {
}

void ResultsPane::Update(wxObject* hint) {
  // interprets hint
  ViewUpdateHint* hint_update = (ViewUpdateHint*)hint;
  if (hint_update == nullptr) {
    // do nothing, this is only passed when pane is created
  } else if (hint_update->type() ==
       ViewUpdateHint::HintType::kModelAnalysisWeathercaseEdit) {
    UpdateAnalysisWeathercaseSetChoice();
    UpdateSagTensionResults();
  } else if (hint_update->type() ==
       ViewUpdateHint::HintType::kModelPreferencesEdit) {
     UpdateSagTensionResults();
  } else if (hint_update->type() ==
       ViewUpdateHint::HintType::kModelSpansEdit) {
    UpdateSagTensionResults();
  } else if (hint_update->type() ==
       ViewUpdateHint::HintType::kModelWeathercaseEdit) {
    UpdateSagTensionResults();
  } else if (hint_update->type() ==
      ViewUpdateHint::HintType::kViewWeathercasesSetChange) {
    UpdateSagTensionResults();
  }

  //panel_span_->UpdateView(hint);
  panel_table_sagtension_->UpdateView(hint);
  panel_table_catenary_->UpdateView(hint);
}

const SagTensionAnalysisResultSet& ResultsPane::results() const {
  return results_;
}

void ResultsPane::OnChoiceWeathercaseSet(wxCommandEvent& event) {
  // gets weathercase set from application data
  wxChoice* choice = XRCCTRL(*this, "choice_weathercase_set", wxChoice);
  wxString str_selection = choice->GetStringSelection();

  // updates the selected/cached weathercases
  UpdateSelectedWeathercases();

  // updates views
  ViewUpdateHint hint;
  hint.set_type(ViewUpdateHint::HintType::kViewWeathercasesSetChange);
  view_->GetDocument()->UpdateAllViews(nullptr, &hint);
}

void ResultsPane::UpdateAnalysisWeathercaseSetChoice() {
  // gets choice control
  wxChoice* choice = XRCCTRL(*this, "choice_weathercase_set", wxChoice);

  // saves current choice string
  std::string str_choice = choice->GetStringSelection();

  // clears choice control
  choice->Clear();

  // appends project weathercase set, and any stored in the application data
  choice->Append("Project");

  const std::list<std::string>& descriptions =
      wxGetApp().data()->descriptions_weathercases_analysis;
  for (auto iter = descriptions.cbegin(); iter != descriptions.cend();
       iter ++) {
    const std::string& description = *iter;
    choice->Append(description);
  }

  // attempts to find the old weathercase set
  choice->SetSelection(choice->FindString(str_choice));

  // updates the selected/cached weathercases
  UpdateSelectedWeathercases();
}

void ResultsPane::UpdateSagTensionResults() {
  // gets activated span
  const Span* span = view_->pane_edit()->SpanActivated();
  if (span == nullptr) {
    results_.descriptions_weathercase.clear();
    results_.results_initial.clear();
    results_.results_load.clear();

    return;
  }

  // checks if weathercases have been selected
  if (weathercases_selected_ == nullptr) {
    results_.descriptions_weathercase.clear();
    results_.results_initial.clear();
    results_.results_load.clear();

    return;
  }

  // sets up reloader
  LineCableReloader reloader;
  reloader.set_line_cable(&span->linecable);
  reloader.set_length_unloaded_unstretched_adjustment(0);
  reloader.set_condition_reloaded(CableConditionType::kInitial);

  // updates span reference
  results_.span = span;

  // updates weathercase descriptions
  results_.descriptions_weathercase.clear();
  for (auto iter = weathercases_selected_->cbegin();
       iter != weathercases_selected_->cend(); iter++) {
    const WeatherLoadCase& weathercase = *iter;
    results_.descriptions_weathercase.push_back(weathercase.description);
  }

  // updates initial results
  results_.results_initial.clear();
  reloader.set_condition_reloaded(CableConditionType::kInitial);
  for (auto iter = weathercases_selected_->cbegin();
       iter != weathercases_selected_->cend(); iter++) {
    const WeatherLoadCase& weathercase = *iter;

    // updates reloader with weathercase
    reloader.set_weathercase_reloaded(&weathercase);

    // calculates results and adds to list
    Catenary3d catenary = reloader.CatenaryReloaded();

    SagTensionAnalysisResult result;
    result.tension_horizontal = catenary.tension_horizontal();
    result.tension_horizontal_core = reloader.TensionHorizontalComponent(
        CableElongationModel::ComponentType::kCore);
    result.tension_horizontal_shell = reloader.TensionHorizontalComponent(
        CableElongationModel::ComponentType::kShell);
    result.weight_unit = catenary.weight_unit();

    results_.results_initial.push_back(result);
  }

  // updates load results
  results_.results_load.clear();
  reloader.set_condition_reloaded(CableConditionType::kLoad);
  for (auto iter = weathercases_selected_->cbegin();
       iter != weathercases_selected_->cend(); iter++) {
    const WeatherLoadCase& weathercase = *iter;

    // updates reloader with weathercase
    reloader.set_weathercase_reloaded(&weathercase);

    // calculates results and adds to list
    Catenary3d catenary = reloader.CatenaryReloaded();

    SagTensionAnalysisResult result;
    result.tension_horizontal = catenary.tension_horizontal();
    result.tension_horizontal_core = reloader.TensionHorizontalComponent(
        CableElongationModel::ComponentType::kCore);
    result.tension_horizontal_shell = reloader.TensionHorizontalComponent(
        CableElongationModel::ComponentType::kShell);
    result.weight_unit = catenary.weight_unit();

    results_.results_load.push_back(result);
  }
}

void ResultsPane::UpdateSelectedWeathercases() {
  wxChoice* choice = XRCCTRL(*this, "choice_weathercase_set", wxChoice);
  std::string str_selection = choice->GetStringSelection();
  if (str_selection == "Project") {
    // gets weathercases from document
    const SpanAnalyzerDoc* doc = (SpanAnalyzerDoc*)view_->GetDocument();
    weathercases_selected_ = &doc->weathercases();
  } else {
    // gets weathercases from app data
    // searches descriptions to find a match with the choice control
    const SpanAnalyzerData* data = wxGetApp().data();
    const std::list<std::string>& descriptions =
        data->descriptions_weathercases_analysis;
    std::list<std::string>::const_iterator iter;
    for (iter = descriptions.cbegin(); iter != descriptions.cend(); iter ++) {
      const std::string& description = *iter;
      if (description == str_selection) {
        break;
      }
    }

    // advances the weathercase set iterator the same distance as the
    // description iterator to find the match
    if (iter != descriptions.cend()) {
      const int index = std::distance(descriptions.cbegin(), iter);
      auto it = std::next(data->weathercases_analysis.cbegin(), index);
      weathercases_selected_ = &(*it);
    } else {
      weathercases_selected_ = nullptr;
    }
  }
}
