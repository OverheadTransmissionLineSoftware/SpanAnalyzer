// This is free and unencumbered software released into the public domain.
// For more information, please refer to <http://unlicense.org/>

#ifndef SPANANALYZER_SAG_TENSION_ANALYSIS_RESULT_H_
#define SPANANALYZER_SAG_TENSION_ANALYSIS_RESULT_H_

#include "models/base/vector.h"
#include "models/sagtension/cable_state.h"
#include "models/transmissionline/cable_constraint.h"
#include "models/transmissionline/weather_load_case.h"

/// \par OVERVIEW
///
/// This represents a single sag-tension analysis result.
///
/// The sag-tension solution is fairly computationally expensive. Using these
/// results, a catenary can be created to generate further analysis results.
struct SagTensionAnalysisResult {
  /// \var condition
  ///   The condition of the cable.
  CableConditionType condition;

  /// \var length_unloaded
  ///   The unloaded length of cable. This is based on the catenary spacing,
  ///   not the ruling span spacing.
  double length_unloaded;

  /// \var state
  ///   The cable state.
  CableState state;

  /// \var tension_average
  ///   The average tension of the cable.
  double tension_average;

  /// \var tension_average_core
  ///   The average tension of the cable core.
  double tension_average_core;

  /// \var tension_average_shell
  ///   The average tension of the cable shell.
  double tension_average_shell;

  /// \var tension_horizontal
  ///   The horizontal tension of the cable.
  double tension_horizontal;

  /// \var tension_horizontal_core
  ///   The horizontal tension of the cable core.
  double tension_horizontal_core;

  /// \var tension_horizontal_shell
  ///   The horizontal tension of the cable shell.
  double tension_horizontal_shell;

  /// \var weathercase
  ///   The weathercase used in the calculation.
  const WeatherLoadCase* weathercase;

  /// \var weight_unit
  ///   The unit weight of the weathercase and cable loading.
  Vector3d weight_unit;
};

#endif  // SPANANALYZER_SAG_TENSION_ANALYSIS_RESULT_H_
