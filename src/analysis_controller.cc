// This is free and unencumbered software released into the public domain.
// For more information, please refer to <http://unlicense.org/>

#include "spananalyzer/analysis_controller.h"

#include "appcommon/widgets/status_bar_log.h"
#include "appcommon/widgets/timer.h"
#include "models/base/helper.h"
#include "wx/wx.h"

AnalysisThread::AnalysisThread() : wxThread(wxTHREAD_JOINABLE) {
  line_cable_ = nullptr;
  spacing_catenary_ = nullptr;
}

void AnalysisThread::AddAnalysisJob(AnalysisJob* job) {
  jobs_.push_back(job);
}

const LineCable* AnalysisThread::line_cable() const {
  return line_cable_;
}

const std::list<ErrorMessage>* AnalysisThread::messages() const {
  return &messages_;
}

const LineCableReloader* AnalysisThread::reloader() const {
  return &reloader_;
}

void AnalysisThread::set_line_cable(const LineCable* line_cable) {
  line_cable_ = line_cable;
}

void AnalysisThread::set_spacing_catenary(const Vector3d* spacing_catenary) {
  spacing_catenary_ = spacing_catenary;
}

const Vector3d* AnalysisThread::spacing_catenary() const {
  return spacing_catenary_;
}

void AnalysisThread::DoAnalysisJob(const int& index) {
  std::string message;
  AnalysisJob* job = *std::next(jobs_.begin(), index);

  // sets up loaders for job
  reloader_.set_condition_reloaded(job->condition);
  reloader_.set_weathercase_reloaded(job->weathercase);

  unloader_.set_condition_unloaded(job->condition);
  unloader_.set_temperature_unloaded(job->weathercase->temperature_cable);

  // validates reloader and logs any errors
  std::list<ErrorMessage> messages;
  const bool status_reloader = reloader_.Validate(false, &messages);
  if (status_reloader == false) {
    // errors were present
    // adds analysis controller error message to give context
    ErrorMessage message;
    message.title = "ANALYSIS THREAD";
    if (job->condition == CableConditionType::kCreep) {
      message.description = "No reloader solution for "
                            + job->weathercase->description
                            + " Creep.";
    } else if (job->condition == CableConditionType::kInitial) {
      message.description = "No solution for "
                            + job->weathercase->description
                            + " Initial.";
    } else if (job->condition == CableConditionType::kLoad) {
      message.description = "No solution for "
                            + job->weathercase->description
                            + " Load.";
    }

    messages_.push_back(message);

    // adds reloader error messages
    messages_.splice(messages_.cend(), messages);
  }

  // validates unloader and logs any errors
  const bool status_unloader = unloader_.Validate(false, &messages);
  if (status_unloader == false) {
    // errors were present
    // adds analysis controller error message to give context
    ErrorMessage message;
    message.title = "ANALYSIS THREAD";
    if (job->condition == CableConditionType::kCreep) {
      message.description = "No unloader solution for "
                            + job->weathercase->description
                            + " Creep.";
    } else if (job->condition == CableConditionType::kInitial) {
      message.description = "No solution for "
                            + job->weathercase->description
                            + " Initial.";
    } else if (job->condition == CableConditionType::kLoad) {
      message.description = "No solution for "
                            + job->weathercase->description
                            + " Load.";
    }

    messages_.push_back(message);

    // adds unloader error messages
    messages_.splice(messages_.cend(), messages);
  }

  // populates result
  SagTensionAnalysisResult& result = *job->result;
  result.weathercase = job->weathercase;

  if ((status_reloader == true) && (status_unloader == true)) {
    // no errors were present
    result.condition = job->condition;

    // gets reloaded catenary and populates sag-tension result
    Catenary3d catenary = reloader_.CatenaryReloaded();

    result.length_unloaded = unloader_.LengthUnloaded();

    result.state = reloader_.StateReloaded();

    result.tension_average = catenary.TensionAverage();
    result.tension_average_core = reloader_.TensionAverageComponent(
        CableElongationModel::ComponentType::kCore);
    result.tension_average_shell = reloader_.TensionAverageComponent(
        CableElongationModel::ComponentType::kShell);

    result.tension_horizontal = catenary.tension_horizontal();
    result.tension_horizontal_core = reloader_.TensionHorizontalComponent(
        CableElongationModel::ComponentType::kCore);
    result.tension_horizontal_shell = reloader_.TensionHorizontalComponent(
        CableElongationModel::ComponentType::kShell);

    result.weight_unit = catenary.weight_unit();
  } else {
    // flags this as an invalid result
    result.condition = CableConditionType::kNull;

    result.length_unloaded = -999999;

    result.state = CableStretchState();

    result.tension_average = -999999;
    result.tension_average_core = -999999;
    result.tension_average_shell = -999999;

    result.tension_horizontal = -999999;
    result.tension_horizontal_core = -999999;
    result.tension_horizontal_shell = -999999;

    result.weight_unit = Vector3d();
  }
}

wxThread::ExitCode AnalysisThread::Entry() {
  // initializes loaders
  reloader_.set_line_cable(line_cable_);

  unloader_.set_line_cable(line_cable_);
  unloader_.set_spacing_attachments(*spacing_catenary_);

  // does all jobs in the list
  const int kSizeJobs = jobs_.size();
  for (int i = 0; i < kSizeJobs; i++) {
    DoAnalysisJob(i);
  }

  // exits
  return (wxThread::ExitCode)0;
}


AnalysisController::AnalysisController() {
  span_ = nullptr;
  weathercases_ = nullptr;

  max_threads_ = wxThread::GetCPUCount();
  if (max_threads_ == -1) {
    max_threads_ = 1;
  }

  state_stretch_initial_.load = 0;
  state_stretch_initial_.temperature = 0;
  state_stretch_initial_.type_polynomial =
      SagTensionCableComponent::PolynomialType::kLoadStrain;
}

AnalysisController::~AnalysisController() {
}

void AnalysisController::ClearResults() {
  results_creep_.clear();
  results_initial_.clear();
  results_load_.clear();
  state_stretch_creep_ = CableStretchState();
  state_stretch_load_ = CableStretchState();

  status_bar_log::SetText("Ready", 0);
}

const SagTensionAnalysisResult* AnalysisController::Result(
    const int& index_weathercase,
    const CableConditionType& condition) const {
  // checks index
  const int kSizeResults = results_creep_.size();
  if ((index_weathercase < 0) || (kSizeResults <= index_weathercase)) {
    return nullptr;
  }

  // gets the results container to pull from
  const std::vector<SagTensionAnalysisResult>* results = nullptr;
  if (condition == CableConditionType::kCreep) {
    results = &results_creep_;
  } else if (condition == CableConditionType::kInitial) {
    results = &results_initial_;
  } else if (condition == CableConditionType::kLoad) {
    results = &results_load_;
  } else {
    return nullptr;
  }

  // checks if result is valid before returning pointer
  if (results->at(index_weathercase).condition == CableConditionType::kNull) {
    return nullptr;
  } else {
    return &results->at(index_weathercase);
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

void AnalysisController::RunAnalysis() {
  std::string message;

  // clears cached results
  ClearResults();

  // checks if span has been selected
  if (span_ == nullptr) {
    wxLogVerbose("No span is selected. Aborting analysis.");
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

    wxLogError("Span validation errors are present. Aborting analysis.");
    status_bar_log::SetText("Span validation error(s) present, see logs", 0);

    return;
  }

  // starts analysis timer
  Timer timer;
  timer.Start();

  // creates empty set of results that will be populated by worker threads
  results_creep_.resize(weathercases_->size(), SagTensionAnalysisResult());
  results_initial_.resize(weathercases_->size(), SagTensionAnalysisResult());
  results_load_.resize(weathercases_->size(), SagTensionAnalysisResult());

  // creates a job list
  std::list<AnalysisJob> jobs;
  for (auto iter = weathercases_->cbegin(); iter != weathercases_->cend();
       iter++) {
    const int index = std::distance(weathercases_->cbegin(), iter);

    AnalysisJob job;
    job.weathercase = *iter;

    job.condition = CableConditionType::kCreep;
    job.result = &results_creep_[index];
    jobs.push_back(job);

    job.condition = CableConditionType::kInitial;
    job.result = &results_initial_[index];
    jobs.push_back(job);

    job.condition = CableConditionType::kLoad;
    job.result = &results_load_[index];
    jobs.push_back(job);
  }

  // determines the number of analysis threads to use
  int num_threads = -1;
  const int num_jobs = jobs.size();

  if (num_jobs < max_threads_) {
    num_threads = num_jobs;
  } else {
    num_threads = max_threads_;
  }

  // creates analysis threads
  std::list<AnalysisThread*> threads;
  for (int i = 0; i < num_threads; i++) {
    AnalysisThread* thread = new AnalysisThread();
    thread->set_line_cable(&span_->linecable);
    thread->set_spacing_catenary(&span_->spacing_attachments);
    threads.push_back(thread);
  }

  // adds jobs to threads round robin style
  auto iter_jobs = jobs.begin();
  while (iter_jobs != jobs.end()) {
    for (auto iter = threads.begin(); iter != threads.end(); iter++) {
      AnalysisThread* thread = *iter;

      if (iter_jobs == jobs.cend()) {
        break;
      } else {
        AnalysisJob* job = &(*iter_jobs);
        thread->AddAnalysisJob(job);

        iter_jobs++;
      }
    }
  }

  // logs analysis start
  message = "Calculating " + std::to_string(num_jobs)
            + " sag-tension solutions using "
            + std::to_string(num_threads) + " threads.";
  wxLogVerbose(message.c_str());
  status_bar_log::PushText("Running sag-tension analysis...", 0);

  // starts threads
  for (auto iter = threads.begin(); iter != threads.end(); iter++) {
    AnalysisThread* thread = *iter;
    wxThreadError status = thread->Run();
    if (status != wxTHREAD_NO_ERROR) {
      wxLogError("Couldn't start analysis thread");
    }
  }

  // waits for threads to complete (i.e. joins worker threads back into main)
  bool is_errors = false;
  for (auto iter_thread = threads.begin(); iter_thread != threads.end();
       iter_thread++) {
    AnalysisThread* thread = *iter_thread;
    thread->Wait();

    // collects any thread errors and logs
    const std::list<ErrorMessage>* messages = thread->messages();
    for (auto iter_message = messages->cbegin();
         iter_message != messages->cend(); iter_message++) {
      is_errors = true;
      const ErrorMessage& message_error = *iter_message;
      std::string str = message_error.title + " - " + message_error.description;
      wxLogError(str.c_str());
    }
  }

  // updates stretch states
  // the stretch states are similar for all threads, so it grabs from first one
  AnalysisThread* thread = *threads.cbegin();
  state_stretch_creep_ = thread->reloader()->StretchStateCreep();
  state_stretch_load_ = thread->reloader()->StretchStateLoad();

  // deletes threads
  for (auto iter = threads.begin(); iter != threads.end(); iter++) {
    AnalysisThread* thread = *iter;
    delete thread;
  }

  // stops timer and logs
  timer.Stop();
  message = "Analysis time = "
            + helper::DoubleToString(timer.Duration(), 3, true) + "s.";
  wxLogVerbose(message.c_str());

  // notifies user of any errors
  if (is_errors == true) {
    // notifies user of error
    message = "Analysis encountered error(s). Check logs.";
    wxMessageBox(message);
  }

  // clears status bar
  status_bar_log::PopText(0);
  status_bar_log::SetText("Ready", 0);
}

const CableStretchState* AnalysisController::StretchState(
    const CableConditionType& condition) {
  // checks if analysis has been ran
  if (weathercases_->size() != results_creep_.size()) {
    return nullptr;
  }

  if (condition == CableConditionType::kCreep) {
    return &state_stretch_creep_;
  } else if (condition == CableConditionType::kInitial) {
    return &state_stretch_initial_;
  } else if (condition == CableConditionType::kLoad) {
    return &state_stretch_load_;
  } else {
    return nullptr;
  }
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
