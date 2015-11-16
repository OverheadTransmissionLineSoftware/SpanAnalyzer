// This is free and unencumbered software released into the public domain.
// For more information, please refer to <http://unlicense.org/>

#ifndef OTLS_SPANANALYZER_PREFERENCESDIALOG_H_
#define OTLS_SPANANALYZER_PREFERENCESDIALOG_H_

#include "wx/wx.h"
#include "wx/filepicker.h"

#include "span_analyzer_config.h"

class PreferencesDialog : public wxDialog {
 public:
  /// \brief Constructor.
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
  /// \brief config_
  ///   The config data reference.
  SpanAnalyzerConfig* config_;

  DECLARE_EVENT_TABLE()
};

#endif  // OTLS_SPANANALYZER_PREFERENCESDIALOG_H_
