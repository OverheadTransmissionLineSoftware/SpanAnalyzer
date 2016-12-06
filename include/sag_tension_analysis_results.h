// This is free and unencumbered software released into the public domain.
// For more information, please refer to <http://unlicense.org/>

#ifndef OTLS_SPANANALYZER_SAGTENSIONANALYSISRESULTS_H_
#define OTLS_SPANANALYZER_SAGTENSIONANALYSISRESULTS_H_

#include <list>

#include "models/base/vector.h"

#include "span_analyzer_data.h"

/// \par OVERVIEW
///
/// This represents a single sag-tension analysis result.
///
/// The sag-tension solution is fairly computationally expensive. Using these
/// results, a catenary can be created to generate further analysis results.
struct SagTensionAnalysisResult {
  /// \var tension_horizontal
  ///   The horizontal tension of the cable.
  double tension_horizontal;

  /// \var tension_horizontal_core
  ///   The horizontal tension of the cable core.
  double tension_horizontal_core;

  /// \var tension_horizontal_shell
  ///   The horizontal tension of the cable shell.
  double tension_horizontal_shell;

  /// \var weight_unit
  ///   The unit weight of the weathercase and cable loading.
  Vector3d weight_unit;
};

/// \par OVERVIEW
///
/// This represents the sag-tension analysis results for a single group of
/// weathercases.
struct SagTensionAnalysisResultGroup {
  /// \var group_weathercases
  ///   The weathercase group. This is for reference only.
  const WeatherLoadCaseGroup* group_weathercases;

  /// \var results_creep
  ///   The sag-tension results for the creep cable condition.
  std::list<SagTensionAnalysisResult> results_creep;

  /// \var results_initial
  ///   The sag-tension results for the initial cable condition.
  std::list<SagTensionAnalysisResult> results_initial;

  /// \var results_load
  ///   The sag-tension results for the load cable condition.
  std::list<SagTensionAnalysisResult> results_load;
};

#endif  // OTLS_SPANANALYZER_SAGTENSIONANALYSISRESULTS_H_
