// This is free and unencumbered software released into the public domain.
// For more information, please refer to <http://unlicense.org/>

#ifndef SPANANALYZER_PREFERENCES_DIALOG_H_
#define SPANANALYZER_PREFERENCES_DIALOG_H_

#include "wx/wx.h"

#include "spananalyzer/span_analyzer_config.h"

/// \par OVERVIEW
///
/// This class is an application preferences editor dialog.
class PreferencesDialog : public wxDialog {
 public:
  /// \brief Constructor.
  /// \param[in] parent
  ///   The parent window.
  /// \param[in] config
  ///   The application configuration settings.
  PreferencesDialog(wxWindow* parent, SpanAnalyzerConfig* config);

  /// \brief Destructor.
  ~PreferencesDialog();

  /// \brief Handles the cancel button event.
  /// \param[in] event
  ///   The event.
  void OnButtonCancel(wxCommandEvent& event);

  /// \brief Handles the Ok button event.
  /// \param[in] event
  ///   The event.
  void OnButtonOk(wxCommandEvent& event);

  /// \brief Handles the close event.
  /// \param[in] event
  ///   The event.
  void OnClose(wxCloseEvent& event);

 private:
  /// \var config_
  ///   The application configuration data.
  SpanAnalyzerConfig* config_;

  DECLARE_EVENT_TABLE()
};

#endif  // SPANANALYZER_PREFERENCES_DIALOG_H_
