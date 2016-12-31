// This is free and unencumbered software released into the public domain.
// For more information, please refer to <http://unlicense.org/>

#ifndef OTLS_SPANANALYZER_STATUSBARLOG_H_
#define OTLS_SPANANALYZER_STATUSBARLOG_H_

#include <string>

/// \par OVERVIEW
///
/// This namespace handles status bar logging for the application. This
/// namespace was created because the wxLogStatus() function does not work with
/// redirected log target which is done with this application for error and
/// message logging.
///
/// This namespace can be used when a statusbar is not present in the
/// application, although any text will be dropped.
namespace status_bar_log {

/// \brief Pushes the string to the statusbar stack to update the status
///   message.
/// \param[in] text
///   The status text.
/// \param[in] index
///   The status field index.
void PushText(const std::string& text, const int& index);

/// \brief Pops the current status from the statusbar and returns to the
///   previous status message.
/// \param[in] index
///   The status field index.
void PopText(const int& index);

/// \brief Sets the current status message.
/// \param[in] text
///   The status text.
/// \param[in] index
///   The status field index.
/// This will overwrite the current status message in the stack.
void SetText(const std::string& text, const int& index);

}

# endif //  OTLS_SPANANALYZER_STATUSBARLOG_H_
