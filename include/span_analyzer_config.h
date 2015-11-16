// This is free and unencumbered software released into the public domain.
// For more information, please refer to <http://unlicense.org/>

#ifndef OTLS_SPANANALYZER_SPANANALYZERCONFIG_H_
#define OTLS_SPANANALYZER_SPANANALYZERCONFIG_H_

#include <string>

#include "models/base/units.h"
#include "wx/wx.h"

/// \par OVERVIEW
///
/// This struct stores the application configuration settings.
struct SpanAnalyzerConfig {
 public:
  /// \var cable_directory
  ///   The directory to search for cable files.
  std::string cable_directory;

  /// \var units_
  ///   The measurement unit system.
  units::UnitSystem units;

  /// \var perspective
  ///   The AUI perspective, which is based on the user layout of panes.
  std::string perspective;

  /// \var size_frame
  ///   The main application frame size.
  wxSize size_frame;
};

#endif //  OTLS_SPANANALYZER_SPANANALYZERCONFIG_H_
