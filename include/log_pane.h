// This is free and unencumbered software released into the public domain.
// For more information, please refer to <http://unlicense.org/>

#ifndef OTLS_SPANANALYZER_LOGPANE_H_
#define OTLS_SPANANALYZER_LOGPANE_H_

#include "wx/textctrl.h"
#include "wx/wx.h"

/// \par OVERVIEW
///
/// This class is a wxAUI pane that holds all of the results display/controls.
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
  wxTextCtrl* textctrl_;

  DECLARE_EVENT_TABLE()
};

# endif //  OTLS_SPANANALYZER_LOGPANE_H_
