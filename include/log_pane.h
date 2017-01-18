// This is free and unencumbered software released into the public domain.
// For more information, please refer to <http://unlicense.org/>

#ifndef OTLS_SPANANALYZER_LOGPANE_H_
#define OTLS_SPANANALYZER_LOGPANE_H_

#include "wx/textctrl.h"
#include "wx/wx.h"

/// \par OVERVIEW
///
/// This class is a custom wxTextCtrl used for logging.
class LogPaneTextCtrl : public wxTextCtrl {
 public:
  /// \brief Constructor.
  /// \param[in] parent
  ///   The parent window.
  LogPaneTextCtrl(wxWindow* parent);

  /// \brief Destructor.
  ~LogPaneTextCtrl();

 private:
  /// \brief Copies the log to the clipboard.
  void CopyLogToClipboard();

  /// \brief Handles the context menu select event.
  /// \param[in] event
  ///   The event.
  void OnContextMenuSelect(wxCommandEvent& event);

  /// \brief Handles the mouse events.
  /// \param[in] event
  ///   The event.
  void OnMouse(wxMouseEvent& event);

  DECLARE_EVENT_TABLE()
};

/// \par OVERVIEW
///
/// This class is a wxAUI pane that displays the log information.
class LogPane : public wxPanel {
 public:
  /// \brief Constructor.
  /// \param[in] parent
  ///   The parent window.
  LogPane(wxWindow* parent);

  /// \brief Destructor.
  ~LogPane();

  /// \brief Gets the textctrl.
  wxTextCtrl* textctrl();

 private:
  /// \var textctrl_
  ///   The textctrl.
  LogPaneTextCtrl* textctrl_;
};

# endif //  OTLS_SPANANALYZER_LOGPANE_H_
