// This is free and unencumbered software released into the public domain.
// For more information, please refer to <http://unlicense.org/>

#include "log_pane.h"

#include "wx/xrc/xmlres.h"

BEGIN_EVENT_TABLE(LogPane, wxPanel)
END_EVENT_TABLE()

/// \todo This pane should eventually be declared in an XRC file.
///    wxWidgets is throwing an assert when specifying a multline style,
///    which is required for a logging textctrl.
LogPane::LogPane(wxWindow* parent)
    : wxPanel(parent, wxID_ANY, wxDefaultPosition, wxDefaultSize,
              wxTAB_TRAVERSAL) {
  // creates a textctrl and adds to pane
  wxBoxSizer* sizer = new wxBoxSizer(wxVERTICAL);
  textctrl_ = new wxTextCtrl(this, wxID_ANY, wxEmptyString,
                             wxDefaultPosition, wxSize(600, 400),
                             wxTE_MULTILINE|wxTE_READONLY);
  sizer->Add(textctrl_, 1, wxEXPAND);
  this->SetSizerAndFit(sizer);
}

LogPane::~LogPane() {
}

wxTextCtrl* LogPane::textctrl() {
  return textctrl_;
}
