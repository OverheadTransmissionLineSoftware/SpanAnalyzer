// This is free and unencumbered software released into the public domain.
// For more information, please refer to <http://unlicense.org/>

#ifndef OTLS_SPANANALYZER_SAGTENSIONANALYSISRESULTS_H_
#define OTLS_SPANANALYZER_SAGTENSIONANALYSISRESULTS_H_

#include <list>

#include "models/base/vector.h"

/// \par OVERVIEW
///
/// This represents a single sag-tension analysis result.
///
/// The sag-tension solution is fairly computationally expensive. Using these
/// results, a catenary can be created to generate further analsyis results.
struct SagTensionAnalysisResult {
  /// \var spacing_endpoints
  ///   The spacing between support endpoints.
  Vector3d spacing_endpoints;

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
/// This represents the sag-tension analysis results for a single set of
/// weathercases.
struct SagTensionAnalysisResultSet {
  /// \var descriptions_weathercase
  ///   The descriptions of the analysis weathercases.
  std::list<std::string> descriptions_weathercase;

  /// \var results_initial
  ///   The sag-tension results for the initial cable condition.
  std::list<SagTensionAnalysisResult> results_initial;

  /// \var results_load
  ///   The sag-tension results for the load cable condition.
  std::list<SagTensionAnalysisResult> results_load;
};

#endif  // OTLS_SPANANALYZER_SAGTENSIONANALYSISRESULTS_H_