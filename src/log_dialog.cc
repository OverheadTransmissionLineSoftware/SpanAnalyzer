// This is free and unencumbered software released into the public domain.
// For more information, please refer to <http://unlicense.org/>

#include "log_dialog.h"

#include "wx/xrc/xmlres.h"

BEGIN_EVENT_TABLE(LogDialog, wxDialog)
END_EVENT_TABLE()

LogDialog::LogDialog(wxWindow* parent) {
  // loads dialog from virtual xrc file system
  wxXmlResource::Get()->LoadDialog(this, parent, "log_dialog");

  /// \todo This eventually should be declared in an XRC file.
  ///    wxWidgets is throwing an assert when specifying a multline style,
  ///    which is required for a logging textctrl.
  // creates a textctrl and adds to pane
  wxBoxSizer* sizer = new wxBoxSizer(wxVERTICAL);
  textctrl_ = new wxTextCtrl(this, wxID_ANY, wxEmptyString,
                             wxDefaultPosition, wxDefaultSize,
                             wxTE_MULTILINE|wxTE_READONLY);
  sizer->Add(textctrl_, 1, wxEXPAND);
  this->SetSizer(sizer);
}

LogDialog::~LogDialog() {
}

wxTextCtrl* LogDialog::textctrl() {
  return textctrl_;
}
