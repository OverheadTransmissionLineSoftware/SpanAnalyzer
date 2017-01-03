// This is free and unencumbered software released into the public domain.
// For more information, please refer to <http://unlicense.org/>

#include "status_bar_log.h"

#include "span_analyzer_app.h"
#include "span_analyzer_frame.h"

namespace status_bar_log {

/// \brief This function gets the status bar associated with the top level
///   window in the application.
/// \return The status bar. If one cannot be found, a nullptr is returned.
wxStatusBar* GetStatusBar() {
  // checks for GUI application
  wxClassInfo* info = wxGetApp().GetClassInfo();
  const wxString kGraphicalApp = "wxApp";
  if (info->GetClassName() != kGraphicalApp) {
    return nullptr;
  }

  // checks for a top level window
  wxFrame* frame = dynamic_cast<wxFrame*>(wxGetApp().GetTopWindow());
  if (frame == nullptr) {
    return nullptr;
  }

  // returns statusbar
  return frame->GetStatusBar();
}

void PushText(const std::string& text, const int& index) {
  // gets status bar
  wxStatusBar* status_bar = status_bar_log::GetStatusBar();
  if (status_bar == nullptr) {
    return;
  }

  // logs
  status_bar->PushStatusText(text, index);
}

void PopText(const int& index) {
  // gets status bar
  wxStatusBar* status_bar = status_bar_log::GetStatusBar();
  if (status_bar == nullptr) {
    return;
  }

  // logs
  status_bar->PopStatusText(index);
}

void SetText(const std::string& text, const int& index) {
  // gets status bar
  wxStatusBar* status_bar = status_bar_log::GetStatusBar();
  if (status_bar == nullptr) {
    return;
  }

  // logs
  status_bar->SetStatusText(text, index);
}

}
