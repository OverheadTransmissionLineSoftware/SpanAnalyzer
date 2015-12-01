// This is free and unencumbered software released into the public domain.
// For more information, please refer to <http://unlicense.org/>

#include "error_message_dialog.h"

#include "wx/xrc/xmlres.h"

ErrorMessageDialog::ErrorMessageDialog(
    wxWindow* parent,
    const std::list<ErrorMessage>* messages) {
  // loads dialog from virtual xrc file system
  wxXmlResource::Get()->LoadDialog(this, parent, "error_message_dialog");

  // gets the listbox and adds error messages
  wxListBox* listbox = XRCCTRL(*this, "listbox_messages", wxListBox);
  for (auto iter = messages->cbegin(); iter != messages->cend(); iter++) {
    const ErrorMessage& message = *iter;
    listbox->Append(message.title + " - " + message.description);
  }

  // fits the dialog around the sizers
  this->Fit();
}

ErrorMessageDialog::~ErrorMessageDialog() {
}
