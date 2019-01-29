// This is free and unencumbered software released into the public domain.
// For more information, please refer to <http://unlicense.org/>

#ifndef SPANANALYZER_ANALYSIS_CONTROLLER_H_
#define SPANANALYZER_ANALYSIS_CONTROLLER_H_

#include <list>
#include <vector>

#include "models/sagtension/line_cable_reloader.h"
#include "models/sagtension/line_cable_unloader.h"
#include "wx/wx.h"

#include "spananalyzer/sag_tension_analysis_result.h"
#include "spananalyzer/span.h"

/// \par OVERVIEW
///
/// This struct is an analysis job, which includes any inputs that change and
/// the result to calculate.
struct AnalysisJob {
  /// \var condition
  ///   The reloaded condition.
  CableConditionType condition;

  /// \var result
  ///   The sag-tension result to calculate.
  SagTensionAnalysisResult* result;

  /// \var weathercase
  ///   The reloaded weathercase.
  const WeatherLoadCase* weathercase;
};

/// \par OVERVIEW
///
/// This class is a worker thread for an analysis.
///
/// \par JOBS
///
/// This thread can accept multiple jobs to run. Once all of the jobs are
/// completed, this thread will pause and wait to be joined back into the main
/// application thread.
///
/// \par RESULTS
///
/// The results are calculated primarily using a LineCableReloader from the
/// OTLS-Models calculation library.
///
/// \par MESSAGES
///
/// This thread logs any error messages from the analysis.
class AnalysisThread : public wxThread {
 public:
  /// \brief Constructor.
  AnalysisThread();

  /// \brief Adds an analysis job to the thread.
  /// \param[in] job
  ///   The job.
  /// This must be done before the Run() method.
  void AddAnalysisJob(AnalysisJob* job);

  /// \brief Gets the line cable.
  /// \return The line cable.
  const LineCable* line_cable() const;

  /// \brief Gets the error messages.
  /// \return The error messages.
  const std::list<ErrorMessage>* messages() const;

  /// \brief Gets the reloader.
  /// \return The reloader.
  const LineCableReloader* reloader() const;

  /// \brief Sets the line cable.
  /// \param[in] line_cable
  ///   The line cable.
  void set_line_cable(const LineCable* line_cable);

  /// \brief Sets the catenary spacing.
  /// \param[in] spacing_catenary
  ///   The catenary spacing.
  void set_spacing_catenary(const Vector3d* spacing_catenary);

  /// \brief Gets the catenary spacing.
  /// \return The catenary spacing.
  const Vector3d* spacing_catenary() const;

 protected:
  /// \brief Does an analysis job.
  /// \param[in] index
  ///   The job index.
  void DoAnalysisJob(const int& index);

  /// \brief Begins the thread processing.
  /// \return The thread exit code.
  /// This function is called directly after Run().
  virtual ExitCode Entry();

  /// \var jobs_
  ///   The analysis jobs.
  std::list<AnalysisJob*> jobs_;

  /// \var line_cable_
  ///   The line cable to reload.
  const LineCable* line_cable_;

  /// \var messages_
  ///   Error messages encountered during the analysis.
  mutable std::list<ErrorMessage> messages_;

  /// \var reloader_
  ///   The line cable reloader that is used in the analysis. This is used solve
  ///   for the sag-tension results.
  LineCableReloader reloader_;

  /// \var spacing_catenary_
  ///   The catenary end point spacing.
  const Vector3d* spacing_catenary_;

  /// \var unloader_
  ///   The line cable unloader that is used in the analysis. This is used solve
  ///   for the unloaded length result.
  LineCableUnloader unloader_;
};

/// \par OVERVIEW
///
/// This class handles the sag-tension analysis and generates cached data
/// for the application to display.
///
/// \par CACHED RESULTS
///
/// This class stores all of the analysis results. The results do not update
/// automatically. Results can only be generated by using the RunAnalysis()
/// method. This is done because:
///  - a span is not always selected
///  - results are computationally expensive and need to be calculated only as
///    needed
///
/// MULTI-THREADING
///
/// This class uses worker threads to calculate the results. The number of
/// threads depends on the available CPUs. A list of analysis jobs are generated
/// and then split evenly among the available threads. This speeds things up so
/// the calculations are less of a bottleneck to the main application thread.
class AnalysisController {
 public:
  /// \brief Constructor.
  AnalysisController();

  /// \brief Destructor.
  ~AnalysisController();

  /// \brief Clears the sag-tension results.
  void ClearResults();

  /// \brief Gets the sag-tension analyis result.
  /// \param[in] index_weathercase
  ///   The weathercase index.
  /// \param[in] condition
  ///   The condition.
  /// \return The sag-tension analysis result. If the index did not match up to
  ///   the results list, or the result is not a valid sag-tension result, a
  ///   nullptr is returned.
  const SagTensionAnalysisResult* Result(
      const int& index_weathercase,
      const CableConditionType& condition) const;

  /// \brief Gets the sag-tension analysis results.
  /// \param[in] condition
  ///   The condition.
  /// \return The sag-tension analysis results.
  const std::vector<SagTensionAnalysisResult>* Results(
      const CableConditionType& condition) const;

  /// \brief Runs the sag-tension analysis.
  void RunAnalysis();

  /// \brief Gets the analysis stretch state for the specified condition.
  /// \param[in] condition
  ///   The condition.
  /// \return The stretch state for the specified condition. If no stretch
  ///   state is available, a nullptr is returned.
  const CableStretchState* StretchState(const CableConditionType& condition);

  /// \brief Sets the activated span.
  /// \param[in] span
  ///   The span.
  void set_span(const Span* span);

  /// \brief Sets the weathercases.
  /// \param[in] weathercases
  ///   The reference data.
  void set_weathercases(const std::list<WeatherLoadCase*>* weathercases);

  /// \brief Gets the span.
  /// \return The span. If no span is set, a nullptr is returned.
  const Span* span() const;

  /// \brief Gets the weathercases.
  /// \return The weathercases.
  const std::list<WeatherLoadCase*>* weathercases() const;

 private:
  /// \var max_threads_
  ///   The maximum number of threads that can be used in the analysis.
  int max_threads_;

  /// \var results_creep_
  ///   The analysis results for the creep condition.
  mutable std::vector<SagTensionAnalysisResult> results_creep_;

  /// \var results_initial_
  ///   The analysis results for the initial condition.
  mutable std::vector<SagTensionAnalysisResult> results_initial_;

  /// \var results_load_
  ///   The analysis results for the load condition.
  mutable std::vector<SagTensionAnalysisResult> results_load_;

  /// \var span_
  ///   The span being analyzed.
  const Span* span_;

  /// \var state_stretch_creep_
  ///   The calculated stretch state for the creep condition.
  mutable CableStretchState state_stretch_creep_;

  /// \var state_stretch_initial_
  ///   The stretch state for the initial condition.
  CableStretchState state_stretch_initial_;

  /// \var state_stretch_load_
  ///   The calculated stretch state for the load condition.
  mutable CableStretchState state_stretch_load_;

  /// \var weathercases_
  ///   The weathercases to be analyzed.
  const std::list<WeatherLoadCase*>* weathercases_;
};

#endif  // SPANANALYZER_ANALYSIS_CONTROLLER_H_