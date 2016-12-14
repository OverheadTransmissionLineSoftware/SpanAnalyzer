// This is free and unencumbered software released into the public domain.
// For more information, please refer to <http://unlicense.org/>

#ifndef OTLS_SPANANALYZER_SPANANALYZERDATA_H_
#define OTLS_SPANANALYZER_SPANANALYZERDATA_H_

#include <list>
#include <string>

#include "models/transmissionline/cable.h"
#include "models/transmissionline/cable_constraint.h"
#include "models/transmissionline/weather_load_case.h"

/// \par OVERVIEW
///
/// This struct is a filter for the analysis results.
struct AnalysisFilter {
 public:
  /// \var condition
  ///   The cable condition.
  CableConditionType condition;

  /// \var weathercase
  ///   The weathercase.
  const WeatherLoadCase* weathercase;
};

/// \par OVERVIEW
///
/// This struct pairs a list of weathercases with a name.
struct AnalysisFilterGroup {
 public:
  /// \var name
  ///   The name of the group.
  std::string name;

  /// \var filters
  ///   The filters in the group.
  std::list<AnalysisFilter> filters;
};

/// \par OVERVIEW
///
/// This struct pairs a cable with an external file.
struct CableFile {
 public:
  /// \var cable
  ///   The cable.
  Cable cable;

  /// \var filepath
  ///   The file path.
  std::string filepath;
};

/// \par OVERVIEW
///
/// This struct stores the application data. This information is available for
/// reference by the SpanAnalyzerDoc.
struct SpanAnalyzerData {
 public:
  /// \var cablefiles
  ///   The cable files.
  /// Pointers are used so the list can be sorted and rearranged while keeping
  /// static memory addresses. This allows the document to consistently
  /// reference the data. This list has ownership of the pointers.
  std::list<CableFile*> cablefiles;

  /// \var groups_filters
  ///   The analysis filter groups.
  std::list<AnalysisFilterGroup> groups_filters;

  /// \var weathercases
  ///   The weathercases.
  /// Pointers are used so the list can be sorted and rearranged while keeping
  /// static memory addresses. This allows the document to consistently
  /// reference the data. This list has ownership of the pointers.
  std::list<WeatherLoadCase*> weathercases;
};

#endif //  OTLS_SPANANALYZER_SPANANALYZERDATA_H_
