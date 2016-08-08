// This is free and unencumbered software released into the public domain.
// For more information, please refer to <http://unlicense.org/>

#include "span_analyzer_view.h"

#include "models/sagtension/line_cable_reloader.h"

#include "span_analyzer_app.h"

IMPLEMENT_DYNAMIC_CLASS(SpanAnalyzerView, wxView)

SpanAnalyzerView::SpanAnalyzerView() {
}

SpanAnalyzerView::~SpanAnalyzerView() {
}

bool SpanAnalyzerView::OnCreate(wxDocument *doc, long flags) {
  if (!wxView::OnCreate(doc, flags)) {
    return false;
  }

  // initializes cached references
  span_ = nullptr;
  group_weathercases_ = nullptr;
  condition_ = CableConditionType::kInitial;
  index_weathercase_ = -1;

  // gets main application frame
  wxFrame* frame = ((wxFrame *)wxGetApp().GetTopWindow());

  // notify application of new view
  SetFrame(frame);
  Activate(true);

  // gets aui manager
  wxAuiManager* manager = wxAuiManager::GetManager(frame);

  // creates AUI windows and adds to manager
  wxAuiPaneInfo info;

  info = wxAuiPaneInfo();
  info.Name("Plot");
  info.CenterPane();
  pane_plot_ = new PlotPane(frame, this);
  manager->AddPane(pane_plot_, info);

  info = wxAuiPaneInfo();
  info.Name("Results");
  info.Floatable(true);
  info.Bottom();
  info.Caption("Results");
  info.CloseButton(false);
  pane_results_ = new ResultsPane(frame, this);
  manager->AddPane(pane_results_, info);

  info = wxAuiPaneInfo();
  info.Name("Edit");
  info.Floatable(true);
  info.Left();
  info.Caption("Edit");
  info.CloseButton(false);
  pane_edit_ = new EditPane(frame, this);
  manager->AddPane(pane_edit_, info);

  // loads perspective and updates
  std::string perspective = wxGetApp().config()->perspective;
  if (perspective == "") {
    manager->Update();
  } else {
    manager->LoadPerspective(wxGetApp().config()->perspective);
  }

  return true;
}

bool SpanAnalyzerView::OnClose(bool WXUNUSED(deleteWindow)) {
  if (!GetDocument()->Close()) {
    return false;
  }

  // gets aui manager from main frame
  wxAuiManager* manager = wxAuiManager::GetManager(GetFrame());

  // saves AUI perspective
  wxGetApp().config()->perspective = manager->SavePerspective();

  // detaches panes and un-init manager
  manager->DetachPane(pane_edit_);
  manager->DetachPane(pane_plot_);
  manager->DetachPane(pane_results_);
  manager->Update();

  // destroys panes
  pane_edit_->Destroy();
  pane_plot_->Destroy();
  pane_results_->Destroy();

  // resets frame to document-less state
  SpanAnalyzerFrame* frame = wxGetApp().frame();
  frame->Refresh();
  frame->SetTitle(wxGetApp().GetAppDisplayName());

  return true;
}

void SpanAnalyzerView::OnDraw(wxDC *dc) {
}

void SpanAnalyzerView::OnUpdate(wxView* sender, wxObject* hint) {
  // passes to base class first
  wxView::OnUpdate(sender, hint);

  // updates sag-tension results
  UpdateSagTensionResults();

  // don't need to distinguish sender - all frames are grouped under one view
  pane_edit_->Update(hint);
  pane_plot_->Update(hint);
  pane_results_->Update(hint);
}

const CableConditionType& SpanAnalyzerView::condition() const {
  return condition_;
}

const WeatherLoadCaseGroup* SpanAnalyzerView::group_weathercases() const {
  return group_weathercases_;
}

const int SpanAnalyzerView::index_weathercase() const {
  return index_weathercase_;
}

EditPane* SpanAnalyzerView::pane_edit() {
  return pane_edit_;
}

ResultsPane* SpanAnalyzerView::pane_results() {
  return pane_results_;
}

const SagTensionAnalysisResultSet& SpanAnalyzerView::results() const {
  return results_;
}

void SpanAnalyzerView::set_condition(const CableConditionType& condition) {
  condition_ = condition;
}

void SpanAnalyzerView::set_group_weathercase(
    const WeatherLoadCaseGroup* group) {
  group_weathercases_ = group;
}

void SpanAnalyzerView::set_index_weathercase(const int& index_weathercase) {
  index_weathercase_ = index_weathercase;
}

void SpanAnalyzerView::set_span(const Span* span) {
  span_ = span;
}

const Span* SpanAnalyzerView::span() const {
  return span_;
}

/// \todo add a log message in here for the analysis time
void SpanAnalyzerView::UpdateSagTensionResults() {
  std::list <ErrorMessage> errors;
  std::string message;

  // clears cached results
  results_.descriptions_weathercase.clear();
  results_.results_initial.clear();
  results_.results_load.clear();

  // checks if span has been selected
  results_.span = span_;
  if (span_ == nullptr) {
    return;
  }

  // validates span
  if (span_->Validate(false, &errors) == false) {
    // logs errors
    for (auto iter = errors.cbegin(); iter != errors.cend(); iter++) {
      const ErrorMessage& error = *iter;
      message = "Span: " + span_->name + "  --  "
                + error.description;
      wxLogError(message.c_str());
    }

    wxLogMessage("Span validation errors are present. Aborting analysis.");
    return;
  }

  // checks if weathercase group has been selected
  if (group_weathercases_ == nullptr) {
    return;
  }

  wxLogMessage("Running sag-tension analysis.");

  // sets up reloader
  LineCableReloader reloader;
  reloader.set_line_cable(&results_.span->linecable);
  reloader.set_length_unloaded_unstretched_adjustment(0);

  // runs analysis for each weathercase
  const std::list<WeatherLoadCase>* weathercases =
      &group_weathercases_->weathercases;
  for (auto iter = weathercases->cbegin(); iter != weathercases->cend();
       iter++) {
    const WeatherLoadCase& weathercase = *iter;


    // validates and logs any errors
    errors.clear();
    bool is_valid_weathercase = weathercase.Validate(false, &errors);
    for (auto iter = errors.cbegin(); iter != errors.cend(); iter++) {
      const ErrorMessage& error = *iter;
      message = weathercase.description + " weathercase  --  "
                + error.description;
      wxLogError(message.c_str());
    }

    // caches description
    results_.descriptions_weathercase.push_back(weathercase.description);

    // updates reloader with weathercase
    reloader.set_weathercase_reloaded(&weathercase);

    // if valid, calculates value and adds to cache
    // if invalid, adds invalid result to cache
    Catenary3d catenary;
    SagTensionAnalysisResult result;
    if (is_valid_weathercase == true) {
      // analyzes and caches initial condition results
      // validates reloader and logs any errors
      reloader.set_condition_reloaded(CableConditionType::kInitial);
      errors.clear();
      if (reloader.Validate(false, &errors) == false) {
        message = "Reloader error for " + weathercase.description
                  + " weathercase when determining the Initial condition.";
        wxLogError(message.c_str());

        for (auto iter = errors.cbegin(); iter != errors.cend(); iter++) {
          const ErrorMessage& error = *iter;
          std::string message = error.title + "  --  "
                                + error.description;
          wxLogError(message.c_str());
        }
      } else {
        catenary = reloader.CatenaryReloaded();

        result.tension_horizontal = catenary.tension_horizontal();
        result.tension_horizontal_core = reloader.TensionHorizontalComponent(
            CableElongationModel::ComponentType::kCore);
        result.tension_horizontal_shell = reloader.TensionHorizontalComponent(
            CableElongationModel::ComponentType::kShell);
        result.weight_unit = catenary.weight_unit();

        results_.results_initial.push_back(result);
      }

      // analyzes and caches initial condition results
      // validates reloader and logs any errors
      reloader.set_condition_reloaded(CableConditionType::kLoad);
      errors.clear();
      if (reloader.Validate(false, &errors) == false) {
        message = "Reloader error for " + weathercase.description
                  + "weathercase when determining the Final-Load condition.";
        wxLogError(message.c_str());

        for (auto iter = errors.cbegin(); iter != errors.cend(); iter++) {
          const ErrorMessage& error = *iter;
          std::string message = error.title + "  --  "
                                + error.description;
          wxLogError(message.c_str());
        }
      } else {
        catenary = reloader.CatenaryReloaded();

        result.tension_horizontal = catenary.tension_horizontal();
        result.tension_horizontal_core = reloader.TensionHorizontalComponent(
            CableElongationModel::ComponentType::kCore);
        result.tension_horizontal_shell = reloader.TensionHorizontalComponent(
            CableElongationModel::ComponentType::kShell);
        result.weight_unit = catenary.weight_unit();

        results_.results_load.push_back(result);
      }
    } else {
      result.tension_horizontal = -999999;
      result.tension_horizontal_core = -999999;
      result.tension_horizontal_shell = -999999;
      result.weight_unit = Vector3d();

      results_.results_initial.push_back(result);
      results_.results_load.push_back(result);
    }
  }
}
