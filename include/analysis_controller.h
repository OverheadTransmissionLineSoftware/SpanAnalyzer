// This is free and unencumbered software released into the public domain.
// For more information, please refer to <http://unlicense.org/>

#ifndef OTLS_SPANANALYZER_ANALYSISCONTROLLER_H_
#define OTLS_SPANANALYZER_ANALYSISCONTROLLER_H_

#include <list>

#include "models/sagtension/line_cable_reloader.h"

#include "sag_tension_analysis_results.h"
#include "span.h"
#include "span_analyzer_data.h"

/// \par OVERVIEW
///
/// This class handles the sag-tension analysis and generates cached data
/// for the application to display.
class AnalysisController {
 public:
  /// \brief Constructor.
  AnalysisController();

  /// \brief Destructor.
  ~AnalysisController();

  /// \brief Clears the sag-tension results.
  void ClearResults();

  /// \brief Runs the sag-tension analysis.
  void RunAnalysis() const;

  /// \brief Gets the reference data.
  /// \return The reference data.
  /// If the results are not updated, this will cause an analysis to run.
  const SpanAnalyzerData* data() const;

  /// \brief Gets the sag-tension analysis results.
  /// \return The sag-tension analysis results.
  const std::list<SagTensionAnalysisResultGroup>& results() const;

  /// \brief Sets the reference data.
  /// \param[in] data
  ///   The reference data.
  void set_data(const SpanAnalyzerData* data);

  /// \brief Sets the activated span.
  /// \param[in] span
  ///   The span.
  void set_span(const Span* span);

  /// \brief Gets the span.
  /// \return The span. If no span is set, a nullptr is returned.
  const Span* span() const;

 private:
  /// \brief Analyzes a weathercase.
  /// \param[in] weathercase
  ///   The weathercase.
  /// \param[in] condition
  ///   The cable condition.
  /// \return A sag-tension analysis result.
  SagTensionAnalysisResult Analyze(const WeatherLoadCase& weathercase,
                                   const CableConditionType& condition) const;

  /// \brief Analyzes a weathercase group.
  /// Results are appended for all weathercase and condition combinations.
  void AnalyzeWeatherCaseGroup(const WeatherLoadCaseGroup& group) const;

  /// \var data_
  ///   The reference data.
  const SpanAnalyzerData* data_;

  /// \var is_updated_
  ///   An indicator that tells if the analysis is up-to-date.
  mutable bool is_updated_;

  /// \var reloader_
  ///   The line cable reloader. This solves for the sag-tension results. When
  ///   not running an analysis, this is set to an invalid state.
  mutable LineCableReloader reloader_;

  /// \var results_
  ///   The sag-tension analysis results.
  mutable std::list<SagTensionAnalysisResultGroup> results_;

  /// \var span_
  ///   The span being analyzed.
  const Span* span_;
};

#endif  // OTLS_SPANANALYZER_ANALYSISCONTROLLER_H_
