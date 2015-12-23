// This is free and unencumbered software released into the public domain.
// For more information, please refer to <http://unlicense.org/>

#include "cable_directory_editor_dialog.h"

#include "wx/xrc/xmlres.h"

BEGIN_EVENT_TABLE(CableDirectoryEditorDialog, wxDialog)
  EVT_BUTTON(wxID_CANCEL, CableDirectoryEditorDialog::OnButtonCancel)
  EVT_BUTTON(wxID_OK, CableDirectoryEditorDialog::OnButtonOk)
  EVT_CLOSE(CableDirectoryEditorDialog::OnClose)
END_EVENT_TABLE()

CableDirectoryEditorDialog::CableDirectoryEditorDialog(wxWindow* parent,
                                                       std::string* directory) {
  // loads dialog from virtual xrc file system
  wxXmlResource::Get()->LoadDialog(this, parent,
                                   "cable_directory_editor_dialog");

  // saves references to members
  directory_ = directory;

  // sets directory in directory picker ctrl and disables textctrl so it has to
  // be selected with picker button
  wxDirPickerCtrl* dirpickerctrl = XRCCTRL(*this, "dirpickerctrl_directory",
                                           wxDirPickerCtrl);
  dirpickerctrl->SetPath(*directory_);

  // fits the dialog around the sizers
  this->Fit();
}

CableDirectoryEditorDialog::~CableDirectoryEditorDialog() {
}

void CableDirectoryEditorDialog::OnButtonCancel(wxCommandEvent &event) {
  EndModal(wxID_CANCEL);
}

void CableDirectoryEditorDialog::OnButtonOk(wxCommandEvent &event) {
  // gets directory from picker ctrl
  wxDirPickerCtrl* dirpickerctrl = XRCCTRL(*this, "dirpickerctrl_directory",
                                           wxDirPickerCtrl);

  // checks if directory exists
  std::string directory = dirpickerctrl->GetPath();
  if (wxFileName::Exists(directory) == true) {
    // updates directory and ends modal
    *directory_ = dirpickerctrl->GetPath();
    EndModal(wxID_OK);
  } else {
    // notifies user of invalid directory
    wxMessageBox("Directory does not exist, try again.");
  }
}

void CableDirectoryEditorDialog::OnClose(wxCloseEvent &event) {
  EndModal(wxID_CLOSE);
}
