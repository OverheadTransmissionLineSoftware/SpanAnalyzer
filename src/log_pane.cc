// This is free and unencumbered software released into the public domain.
// For more information, please refer to <http://unlicense.org/>

#include "log_pane.h"

#include "wx/clipbrd.h"
#include "wx/xrc/xmlres.h"

/// context menu enum
enum {
  kClear = 0,
  kCopy,
};

BEGIN_EVENT_TABLE(LogPaneTextCtrl, wxPanel)
  EVT_MENU(wxID_ANY, LogPaneTextCtrl::OnContextMenuSelect)
  EVT_RIGHT_DOWN(LogPaneTextCtrl::OnMouse)
END_EVENT_TABLE()

LogPaneTextCtrl::LogPaneTextCtrl(wxWindow* parent)
  : wxTextCtrl(parent, wxID_ANY, wxEmptyString, wxDefaultPosition,
               wxSize(600, 400), wxTE_MULTILINE | wxTE_READONLY) {
}

LogPaneTextCtrl::~LogPaneTextCtrl() {
}

void LogPaneTextCtrl::CopyLogToClipboard() {
  if (wxTheClipboard->Open()) {
    // gets the textctrl contents
    std::string str = GetValue();

    // creates a text data object to store clipboard information
    wxTextDataObject* data = new wxTextDataObject();
    data->SetText(str);

    // copies to the clipboard and closes
    wxTheClipboard->SetData(data);
    wxTheClipboard->Close();
  }
}

void LogPaneTextCtrl::OnContextMenuSelect(wxCommandEvent& event) {
  // gets context menu selection and sends to handler function
  const int id_event = event.GetId();
  if (id_event == kClear) {
    Clear();
  } else if (id_event == kCopy) {
    CopyLogToClipboard();
  }
}

void LogPaneTextCtrl::OnMouse(wxMouseEvent& event) {
  if (event.RightDown() == true) {
    // builds a context menu
    wxMenu menu;

    menu.Append(kClear, "Clear");
    menu.Append(kCopy, "Copy");

    // shows context menu
    // the event is caught by the pane
    PopupMenu(&menu, event.GetPosition());
  }
}


/// \todo This pane should eventually be declared in an XRC file.
///    wxWidgets is throwing an assert when specifying a multline style,
///    which is required for a logging textctrl.
LogPane::LogPane(wxWindow* parent)
    : wxPanel(parent, wxID_ANY, wxDefaultPosition, wxDefaultSize,
              wxTAB_TRAVERSAL) {
  // creates textctrl
  textctrl_ = new LogPaneTextCtrl(this);

  // adds to pane and fits
  wxBoxSizer* sizer = new wxBoxSizer(wxVERTICAL);
  sizer->Add(textctrl_, 1, wxEXPAND);
  this->SetSizerAndFit(sizer);
}

LogPane::~LogPane() {
}

wxTextCtrl* LogPane::textctrl() {
  return dynamic_cast<wxTextCtrl*>(textctrl_);
}
