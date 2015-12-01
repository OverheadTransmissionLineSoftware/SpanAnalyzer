// This is free and unencumbered software released into the public domain.
// For more information, please refer to <http://unlicense.org/>

#ifndef OTLS_SPANANALYZER_ERRORMESSAGEDIALOG_H_
#define OTLS_SPANANALYZER_ERRORMESSAGEDIALOG_H_

#include <list>

#include "models/base/error_message.h"
#include "wx/wx.h"

/// \par OVERVIEW
///
/// This class is an error message dialog. No editing is provided, just
/// displaying only.
class ErrorMessageDialog : public wxDialog {
 public:
  /// \brief Constructor.
  /// \param[in] parent
  ///   The parent window.
  /// \param[in] messages
  ///   The error messages.
  ErrorMessageDialog(wxWindow* parent,
                     const std::list<ErrorMessage>* messages);

  /// \brief Destructor.
  ~ErrorMessageDialog();
};

#endif  // OTLS_SPANANALYZER_ERRORMESSAGEDIALOG_H_