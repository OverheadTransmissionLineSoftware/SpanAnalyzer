// This is free and unencumbered software released into the public domain.
// For more information, please refer to <http://unlicense.org/>

#include "analysis_controller.h"

#include "wx/wx.h"

AnalysisController::AnalysisController() {
  data_ = nullptr;
  is_updated_ = false;
  span_ = nullptr;

  reloader_.set_length_unloaded_unstretched_adjustment(-999999);
  reloader_.set_line_cable(nullptr);
  reloader_.set_weathercase_reloaded(nullptr);
}

AnalysisController::~AnalysisController() {
}

void AnalysisController::ClearResults() {
  results_.clear();
}

/// \todo add a log message in here for the analysis time
void AnalysisController::RunAnalysis() const {
  // clears cached results
  results_.clear();

  // checks if span has been selected
  if (span_ == nullptr) {
    wxLogMessage("No span is selected. Aborting analysis.");
    return;
  }

  // validates span
  std::list <ErrorMessage> errors;
  if (span_->Validate(false, &errors) == false) {
    // logs errors
    for (auto iter = errors.cbegin(); iter != errors.cend(); iter++) {
      const ErrorMessage& error = *iter;
      std::string message = "Span: " + span_->name + "  --  "
                + error.description;
      wxLogError(message.c_str());
    }

    wxLogMessage("Span validation errors are present. Aborting analysis.");
    return;
  }

  wxLogMessage("Running sag-tension analysis.");

  // sets up reloader
  reloader_.set_line_cable(&span_->linecable);
  reloader_.set_length_unloaded_unstretched_adjustment(0);

  // runs analysis for each weathercase group
  const std::list<WeatherLoadCaseGroup>& groups = data_->groups_weathercase;
  for (auto iter = groups.cbegin(); iter != groups.cend(); iter++) {
    const WeatherLoadCaseGroup& group = *iter;
    AnalyzeWeatherCaseGroup(group);
  }

  // un-inits reloader
  reloader_.set_line_cable(&span_->linecable);
  reloader_.set_length_unloaded_unstretched_adjustment(0);

  // updates indicator
  is_updated_ = true;
}

const SpanAnalyzerData* AnalysisController::data() const {
  return data_;
}

const std::list<SagTensionAnalysisResultGroup>& AnalysisController::results()
    const {
  if (is_updated_ == false) {
    RunAnalysis();
  }

  return results_;
}

void AnalysisController::set_data(const SpanAnalyzerData* data) {
  data_ = data;

  is_updated_ = false;
}

void AnalysisController::set_span(const Span* span) {
  span_ = span;

  is_updated_ = false;
}

const Span* AnalysisController::span() const {
  return span_;
}

SagTensionAnalysisResult AnalysisController::Analyze(
    const WeatherLoadCase& weathercase,
    const CableConditionType& condition) const {
  // initializes
  SagTensionAnalysisResult result;

  // updates reloader
  reloader_.set_condition_reloaded(condition);
  reloader_.set_weathercase_reloaded(&weathercase);

  // validates reloader and logs any errors
  std::list <ErrorMessage> errors;
  if (reloader_.Validate(false, &errors) == false) {
    std::string message;
    if (condition == CableConditionType::kInitial) {
      message = "Reloader error for " + weathercase.description
                + " weathercase when determining the Initial condition.";
    } else if (condition == CableConditionType::kLoad) {
      message = "Reloader error for " + weathercase.description
                + " weathercase when determining the Load condition.";
    }
    wxLogError(message.c_str());

    for (auto iter = errors.cbegin(); iter != errors.cend(); iter++) {
      const ErrorMessage& error = *iter;
      std::string message = error.title + "  --  " + error.description;
      wxLogError(message.c_str());
    }

    return result;
  }

  // gets reloaded catenary and populates sag-tension result
  Catenary3d catenary = reloader_.CatenaryReloaded();

  result.tension_horizontal = catenary.tension_horizontal();
  result.tension_horizontal_core = reloader_.TensionHorizontalComponent(
      CableElongationModel::ComponentType::kCore);
  result.tension_horizontal_shell = reloader_.TensionHorizontalComponent(
      CableElongationModel::ComponentType::kShell);
  result.weight_unit = catenary.weight_unit();

  return result;
}

void AnalysisController::AnalyzeWeatherCaseGroup(
    const WeatherLoadCaseGroup& group) const {
  // creates a new analysis result group
  SagTensionAnalysisResultGroup group_results;
  group_results.group_weathercases = &group;

  // runs analysis for each weathercase
  const std::list<WeatherLoadCase>* weathercases = &group.weathercases;
  for (auto iter = weathercases->cbegin(); iter != weathercases->cend();
       iter++) {
    const WeatherLoadCase& weathercase = *iter;
    SagTensionAnalysisResult result;

    // appends a result for initial condition
    result = Analyze(weathercase, CableConditionType::kInitial);
    group_results.results_initial.push_back(result);

    // appends a result for load condition
    result = Analyze(weathercase, CableConditionType::kLoad);
    group_results.results_load.push_back(result);
  }

  // adds to cached result groups
  results_.push_back(group_results);
}
