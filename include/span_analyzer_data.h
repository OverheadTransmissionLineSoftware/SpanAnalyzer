// This is free and unencumbered software released into the public domain.
// For more information, please refer to <http://unlicense.org/>

#ifndef OTLS_SPANANALYZER_SPANANALYZERDATA_H_
#define OTLS_SPANANALYZER_SPANANALYZERDATA_H_

#include <list>
#include <string>

#include "models/transmissionline/cable.h"
#include "models/transmissionline/weather_load_case.h"

/// \par OVERVIEW
///
/// This object pairs a cable with an external file.
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
/// This struct stores the application data. This information is referenced by a
/// SpanAnalyzerDocument.
struct SpanAnalyzerData {
 public:
  /// \var cablefiles
  ///   The cable files.
  std::list<CableFile> cablefiles;

  /// \var descriptions_weathercases_analysis
  ///   The analysis weathercase set descriptions.
  std::list<std::string> descriptions_weathercases_analysis;

  /// \var weathercases_analysis
  ///   The analysis weathercase sets.
  std::list<std::list<WeatherLoadCase>> weathercases_analysis;
};

#endif //  OTLS_SPANANALYZER_SPANANALYZERDATA_H_
