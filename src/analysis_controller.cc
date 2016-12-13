// This is free and unencumbered software released into the public domain.
// For more information, please refer to <http://unlicense.org/>

#include "analysis_controller.h"

#include "wx/wx.h"

AnalysisController::AnalysisController() {
  span_ = nullptr;
  weathercases_ = nullptr;

  reloader_.set_length_unloaded_unstretched_adjustment(-999999);
  reloader_.set_line_cable(nullptr);
  reloader_.set_weathercase_reloaded(nullptr);
}

AnalysisController::~AnalysisController() {
}

void AnalysisController::ClearResults() {
  results_creep_.clear();
  results_initial_.clear();
  results_load_.clear();
}

const SagTensionAnalysisResult* AnalysisController::Result(
    const int& index_weathercase,
    const CableConditionType& condition) const {
  // checks index
  const unsigned int kSizeResults = results_creep_.size();
  if ((index_weathercase < 0) || (kSizeResults < index_weathercase)) {
    return nullptr;
  }

  if (condition == CableConditionType::kCreep) {
    return &results_creep_.at(index_weathercase);
  } else if (condition == CableConditionType::kInitial) {
    return &results_initial_.at(index_weathercase);
  } else if (condition == CableConditionType::kLoad) {
    return &results_load_.at(index_weathercase);
  } else {
    return nullptr;
  }
}

const std::vector<SagTensionAnalysisResult>* AnalysisController::Results(
    const CableConditionType& condition) const {
  if (condition == CableConditionType::kCreep) {
    return &results_creep_;;
  } else if (condition == CableConditionType::kInitial) {
    return &results_initial_;
  } else if (condition == CableConditionType::kLoad) {
    return &results_load_;
  } else {
    return nullptr;
  }
}

/// \todo add a log message in here for the analysis time
void AnalysisController::RunAnalysis() const {
  // clears cached results
  results_creep_.clear();
  results_initial_.clear();
  results_load_.clear();

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

  // runs analysis for each weathercase
  for (auto iter = weathercases_->cbegin(); iter != weathercases_->cend();
       iter++) {
    const WeatherLoadCase* weathercase = *iter;

    SagTensionAnalysisResult result;

    // appends result for creep condition
    result = Analyze(*weathercase, CableConditionType::kCreep);
    results_creep_.push_back(result);

    // appends a result for initial condition
    result = Analyze(*weathercase, CableConditionType::kInitial);
    results_initial_.push_back(result);

    // appends a result for load condition
    result = Analyze(*weathercase, CableConditionType::kLoad);
    results_load_.push_back(result);
  }

  // un-inits reloader
  reloader_.set_line_cable(&span_->linecable);
  reloader_.set_length_unloaded_unstretched_adjustment(0);
}

void AnalysisController::set_span(const Span* span) {
  span_ = span;
}

void AnalysisController::set_weathercases(
    const std::list<WeatherLoadCase*>* weathercases) {
  weathercases_ = weathercases;
}

const Span* AnalysisController::span() const {
  return span_;
}

const std::list<WeatherLoadCase*>* AnalysisController::weathercases() const {
  return weathercases_;
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

  result.condition = condition;
  result.weathercase = &weathercase;

  return result;
}