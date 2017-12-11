// This is free and unencumbered software released into the public domain.
// For more information, please refer to <http://unlicense.org/>

#ifndef OTLS_SPANANALYZER_SPANANALYZERCONFIG_H_
#define OTLS_SPANANALYZER_SPANANALYZERCONFIG_H_

#include <string>

#include "models/base/units.h"
#include "wx/cmndata.h"
#include "wx/wx.h"

/// \par OVERVIEW
///
/// This class has the options for the CablePlotPane.
struct CablePlotOptions {
 public:
  /// \var color_core
  ///   The core component color.
  wxColour color_core;

  /// \var color_markers
  ///   The marker colors.
  wxColour color_markers;

  /// \var color_shell
  ///   The shell component color.
  wxColour color_shell;

  /// \var color_total
  ///   The total cable color.
  wxColour color_total;

  /// \var thickness_line
  ///   The line thickness.
  int thickness_line;
};

/// \par OVERVIEW
///
/// This class has the options for the ProfilePlotPane.
struct ProfilePlotOptions {
 public:
  /// \var color_catenary
  ///   The catenary color.
  wxColour color_catenary;

  /// \var scale_horizontal
  ///   The horizontal scale factor.
  int scale_horizontal;

  /// \var scale_vertical
  ///   The vertical scale factor.
  int scale_vertical;

  /// \var thickness_line
  ///   The line thickness.
  int thickness_line;
};

/// \par OVERVIEW
///
/// This struct stores the application configuration settings.
struct SpanAnalyzerConfig {
 public:
  /// \var color_background
  ///   The background color.
  wxColour color_background;

  /// \var data_page
  ///   The page setup data for printing.
  wxPageSetupDialogData* data_page;

  /// \var filepath_data
  ///   The file path for the application data.
  std::string filepath_data;

  /// \var level_log
  ///   The log level of the application.
  wxLogLevelValues level_log;

  /// \var options_plot_cable
  ///   The cable elongation model plot pane options.
  CablePlotOptions options_plot_cable;

  /// \var options_plot_profile
  ///   The profile plot pane options.
  ProfilePlotOptions options_plot_profile;

  /// \var perspective
  ///   The AUI perspective, which is based on the user layout of panes. The
  ///   perspective is loaded/saved on application startup/exit, and modified
  ///   on view open/close.
  std::string perspective;

  /// \var size_frame
  ///   The main application frame size.
  wxSize size_frame;

  /// \var units
  ///   The measurement unit system.
  units::UnitSystem units;
};

#endif  // OTLS_SPANANALYZER_SPANANALYZERCONFIG_H_
