// This is free and unencumbered software released into the public domain.
// For more information, please refer to <http://unlicense.org/>

#include "results_pane.h"

#include "models/sagtension/line_cable_reloader.h"
#include "wx/notebook.h"
#include "wx/xrc/xmlres.h"

#include "edit_pane.h"
#include "span_analyzer_doc.h"

ResultsPane::ResultsPane(wxWindow* parent, SpanAnalyzerView* view) {
  // loads dialog from virtual xrc file system
  wxXmlResource::Get()->LoadPanel(this, parent, "results_pane");

  // saves view reference
  view_ = view;

  // creates notebook and adds child notebook pages
  wxNotebook* notebook = XRCCTRL(*this, "notebook_results", wxNotebook);

  panel_span_ = new SpanPanel(notebook, &results_);
  notebook->AddPage(panel_span_, "Span");

  panel_table_sagtension_ = new SagTensionTablePanel(notebook, &results_);
  notebook->AddPage(panel_table_sagtension_, "Sag-Tension Table");

  panel_table_catenary_ = new CatenaryTablePanel(notebook, &results_);
  notebook->AddPage(panel_table_catenary_, "Catenary Table");
}

ResultsPane::~ResultsPane() {
}

void ResultsPane::Update(wxObject* hint) {
  if (hint != nullptr) {
    UpdateSagTensionResults();
  }

  panel_span_->UpdateView(hint);
  panel_table_sagtension_->UpdateView(hint);
  panel_table_catenary_->UpdateView(hint);
}

const SagTensionAnalysisResultSet& ResultsPane::results() const {
  return results_;
}

void ResultsPane::UpdateSagTensionResults() {
  // gets info from document/view
  const SpanAnalyzerDoc* doc = (SpanAnalyzerDoc*)view_->GetDocument();
  const Span* span = view_->pane_edit()->SpanActivated();
  const std::list<WeatherLoadCase>& weathercases = doc->weathercases();

  // sets up reloader
  LineCableReloader reloader;
  reloader.set_line_cable(&span->linecable);
  reloader.set_length_unloaded_unstretched_adjustment(0);
  reloader.set_condition_reloaded(CableConditionType::kInitial);

  // updates weathercase descriptions
  results_.descriptions_weathercase.clear();
  for (auto iter = weathercases.cbegin(); iter != weathercases.cend(); iter++) {
    const WeatherLoadCase& weathercase = *iter;
    results_.descriptions_weathercase.push_back(weathercase.description);
  }

  // updates initial results
  results_.results_initial.clear();
  reloader.set_condition_reloaded(CableConditionType::kInitial);
  for (auto iter = weathercases.cbegin(); iter != weathercases.cend(); iter++) {
    const WeatherLoadCase& weathercase = *iter;

    // updates reloader with weathercase
    reloader.set_weathercase_reloaded(&weathercase);

    // calculates results and adds to list
    Catenary3d catenary = reloader.CatenaryReloaded();

    SagTensionAnalysisResult result;
    result.spacing_endpoints = catenary.spacing_endpoints();
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
  for (auto iter = weathercases.cbegin(); iter != weathercases.cend(); iter++) {
    const WeatherLoadCase& weathercase = *iter;

    // updates reloader with weathercase
    reloader.set_weathercase_reloaded(&weathercase);

    // calculates results and adds to list
    Catenary3d catenary = reloader.CatenaryReloaded();

    SagTensionAnalysisResult result;
    result.spacing_endpoints = catenary.spacing_endpoints();
    result.tension_horizontal = catenary.tension_horizontal();
    result.tension_horizontal_core = reloader.TensionHorizontalComponent(
        CableElongationModel::ComponentType::kCore);
    result.tension_horizontal_shell = reloader.TensionHorizontalComponent(
        CableElongationModel::ComponentType::kShell);
    result.weight_unit = catenary.weight_unit();

    results_.results_load.push_back(result);
  }
}
