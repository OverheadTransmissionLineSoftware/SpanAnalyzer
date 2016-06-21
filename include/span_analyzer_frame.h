// This is free and unencumbered software released into the public domain.
// For more information, please refer to <http://unlicense.org/>

#ifndef OTLS_SPANANALYZER_SPANANALYZERFRAME_H_
#define OTLS_SPANANALYZER_SPANANALYZERFRAME_H_

#include "wx/docview.h"
#include "wx/dnd.h"

#include "log_dialog.h"

/// \par OVERVIEW
///
/// This class is the drop target for an application document file.
///
/// If an application file is dropped onto the main application frame
/// associated with this target, it will notify the document manager to open
/// the document.
class DocumentFileDropTarget : public wxFileDropTarget {
 public:
  /// \brief Constructor.
  /// \param[in] parent
  ///   The parent window.
  DocumentFileDropTarget(wxWindow* parent);

  /// \brief Handles the drop file event.
  /// \param[in] x
  ///   The x position of the mouse.
  /// \param[in] y
  ///   The y position of the mouse.
  /// \param[in] filenames
  ///   The list of files that are dropped. This should only contain one
  ///   file.
  /// \return Return true to accept data, or false to veto the operation.
  virtual bool OnDropFiles(wxCoord x, wxCoord y,
                           const wxArrayString& filenames);

 private:
  /// \var parent_
  ///   The parent window.
  wxWindow* parent_;
};

/// \par OVERVIEW
///
/// This is the top-level SpanAnalyzer application frame.
class SpanAnalyzerFrame : public wxDocParentFrame {
 public:
  /// \brief Constructor.
  SpanAnalyzerFrame(wxDocManager* manager);

  /// \brief Destructor.
  ~SpanAnalyzerFrame();

  /// \brief Handles the edit->edit weather cases menu click event.
  /// \param[in] event
  ///   The event.
  void OnMenuEditAnalysisWeathercases(wxCommandEvent& event);

  /// \brief Handles the edit->cables menu click event.
  /// \param[in] event
  ///   The event.
  void OnMenuEditCables(wxCommandEvent& event);

  /// \brief Handles the file->preferences menu click event.
  /// \param[in] event
  ///   The event.
  void OnMenuFilePreferences(wxCommandEvent& event);

  /// \brief Handles the help->about menu click event.
  /// \param[in] event
  ///   The event.
  void OnMenuHelpAbout(wxCommandEvent& event);

  /// \brief Handles the view->log menu click event.
  /// \param[in] event
  ///   The event.
  void OnMenuViewLog(wxCommandEvent& event);

  /// \brief Gets the log dialog.
  /// \return The log dialog.
  LogDialog* dialog_log();

 private:
  /// \var dialog_log
  ///   The log dialog.
  LogDialog* dialog_log_;

  DECLARE_EVENT_TABLE()
};

#endif  // OTLS_SPANANALYZER_SPANANALYZERFRAME_H_
