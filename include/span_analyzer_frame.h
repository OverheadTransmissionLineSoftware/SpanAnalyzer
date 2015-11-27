// This is free and unencumbered software released into the public domain.
// For more information, please refer to <http://unlicense.org/>

#ifndef OTLS_SPANANALYZER_SPANANALYZERFRAME_H_
#define OTLS_SPANANALYZER_SPANANALYZERFRAME_H_

#include "wx/docview.h"

/// \par OVERVIEW
///
/// This is the top-level SpanAnalyzer application frame.
class SpanAnalyzerFrame : public wxDocParentFrame {
 public:
  /// \brief Constructor.
  SpanAnalyzerFrame(wxDocManager* manager);

  /// \brief Destructor.
  ~SpanAnalyzerFrame();

  /// \brief Handles the edit->edit cable menu click event.
  /// \param[in] event
  ///   The event.
  void OnMenuEditCableEdit(wxCommandEvent& event);

  /// \brief Handles the edit->new cable menu click event.
  /// \param[in] event
  ///   The event.
  void OnMenuEditCableNew(wxCommandEvent& event);

  /// \brief Handles the edit->edit weather cases menu click event.
  /// \param[in] event
  ///   The event.
  void OnMenuEditWeatherCasesEdit(wxCommandEvent& event);

  /// \brief Handles the file->exit menu click event.
  /// \param[in] event
  ///   The event.
  void OnMenuFileExit(wxCommandEvent& event);

  /// \brief Handles the file->preferences menu click event.
  /// \param[in] event
  ///   The event.
  void OnMenuFilePreferences(wxCommandEvent& event);

  // DEBUG
  /// \brief Handles the debug->event menu click event.
  void OnMenuDebugEvent(wxCommandEvent& event);
  // DEBUG

 private:
  DECLARE_EVENT_TABLE()
};

#endif  // OTLS_SPANANALYZER_SPANANALYZERFRAME_H_
