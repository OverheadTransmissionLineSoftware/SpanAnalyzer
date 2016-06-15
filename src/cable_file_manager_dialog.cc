// This is free and unencumbered software released into the public domain.
// For more information, please refer to <http://unlicense.org/>

#include "cable_file_manager_dialog.h"

#include "wx/xrc/xmlres.h"

#include "cable_editor_dialog.h"
#include "cable_unit_converter.h"
#include "file_handler.h"
#include "span_analyzer_app.h"

BEGIN_EVENT_TABLE(CableFileManagerDialog, wxDialog)
  EVT_BUTTON(XRCID("button_add"), CableFileManagerDialog::OnButtonAdd)
  EVT_BUTTON(wxID_CANCEL, CableFileManagerDialog::OnButtonCancel)
  EVT_BUTTON(XRCID("button_edit"), CableFileManagerDialog::OnButtonEdit)
  EVT_BUTTON(wxID_OK, CableFileManagerDialog::OnButtonOk)
  EVT_BUTTON(XRCID("button_new"), CableFileManagerDialog::OnButtonNew)
  EVT_BUTTON(XRCID("button_remove"), CableFileManagerDialog::OnButtonRemove)
  EVT_CLOSE(CableFileManagerDialog::OnClose)
  EVT_LIST_ITEM_SELECTED(wxID_ANY, CableFileManagerDialog::OnItemSelect)
END_EVENT_TABLE()

CableFileManagerDialog::CableFileManagerDialog(
    wxWindow* parent,
    const units::UnitSystem& units,
    std::list<CableFile>* cablefiles) {
  // loads dialog from virtual xrc file system
  wxXmlResource::Get()->LoadDialog(this, parent, "cable_file_manager_dialog");

  // saves references to members
  cablefiles_ = cablefiles;
  listctrl_ = XRCCTRL(*this, "listctrl_cables", wxListCtrl);
  units_ = units;

  index_selected_ = -1;

  // copies list of cables to be modified by manager
  cablefiles_modified_ = *cablefiles_;

  // populates listctrl with cable files
  for (auto iter = cablefiles_->cbegin(); iter != cablefiles_->cend();
       iter++) {
    long index_row = std::distance(cablefiles_->cbegin(), iter);
    const CableFile& cablefile = *iter;

    // adds list item (row) to listctrl
    wxListItem item;
    item.SetId(index_row);
    listctrl_->InsertItem(item);

    listctrl_->SetItem(index_row, 0, cablefile.cable.name);
    listctrl_->SetItem(index_row, 1, cablefile.filepath);
  }

  // fits the dialog around the sizers
  this->Fit();
}

CableFileManagerDialog::~CableFileManagerDialog() {
}

void CableFileManagerDialog::OnButtonAdd(wxCommandEvent& event) {
  // opens file dialog to select files to add
  wxFileDialog dialog_file(this, "Open Cable File", "", ".cable",
                           "Cable Files (*.cable) | *.cable",
                           wxFD_OPEN|wxFD_MULTIPLE);
  if (dialog_file.ShowModal() != wxID_OK) {
    return;
  }

  // gets user selected cable file paths
  wxArrayString paths;
  dialog_file.GetPaths(paths);

  // adds all selected files
  for (auto iter = paths.begin(); iter != paths.end(); iter++) {
    const wxString path = *iter;

    // determines if cable file is already loaded
    bool is_loaded = false;
    for (auto iter = cablefiles_modified_.cbegin();
         iter != cablefiles_modified_.cend(); iter++) {
      const CableFile& cablefile = *iter;
      if (path == cablefile.filepath) {
        is_loaded = true;
        break;
      }
    }

    // adds cable file if not currently loaded
    if (is_loaded == false) {
      // loads selected path into cable file object
      CableFile cablefile;
      cablefile.filepath = path;
      const int status = FileHandler::LoadCable(cablefile.filepath,
        units_, cablefile.cable);
      if (status != 0) {
        continue;
      }

      // converts cable units to consistent unit style
      CableUnitConverter::ConvertUnitStyle(
        units_,
        units::UnitStyle::kDifferent,
        units::UnitStyle::kConsistent,
        cablefile.cable);

      // adds to app data
      cablefiles_modified_.push_back(cablefile);

      // adds to listctrl
      const long index = listctrl_->GetItemCount();
      wxListItem item;
      item.SetId(index);
      listctrl_->InsertItem(item);

      listctrl_->SetItem(index, 0, cablefile.cable.name);
      listctrl_->SetItem(index, 1, cablefile.filepath);
    }
  }
}

void CableFileManagerDialog::OnButtonCancel(wxCommandEvent &event) {
  EndModal(wxID_CANCEL);
}

void CableFileManagerDialog::OnButtonEdit(wxCommandEvent& event) {
  if (index_selected_ < 0) {
    return;
  }

  // gets cablefile and copies
  auto iter = std::next(cablefiles_modified_.begin(), index_selected_);
  CableFile cablefile = *iter;

  // converts units to different unit style
  CableUnitConverter::ConvertUnitStyle(
      units_,
      units::UnitStyle::kConsistent,
      units::UnitStyle::kDifferent,
      cablefile.cable);

  // creates dialog to edit cable
  CableEditorDialog dialog_edit(this, &cablefile.cable, units_);
  if (dialog_edit.ShowModal() != wxID_OK) {
    return;
  }

  // converts units to consistent unit style
  CableUnitConverter::ConvertUnitStyle(
      units_,
      units::UnitStyle::kDifferent,
      units::UnitStyle::kConsistent,
      cablefile.cable);

  // saves cable to filesystem
  FileHandler::SaveCable(cablefile.filepath, cablefile.cable, units_);

  // transfers copied cable to original
  *iter = cablefile;
}

void CableFileManagerDialog::OnButtonNew(wxCommandEvent& event) {
  // creates new cable file
  CableFile cablefile;

  // creates dialog to edit cable
  CableEditorDialog dialog_edit(this, &cablefile.cable, units_);
  if (dialog_edit.ShowModal() != wxID_OK) {
    return;
  }

  // converts units to consistent unit style
  CableUnitConverter::ConvertUnitStyle(
    units_,
    units::UnitStyle::kDifferent,
    units::UnitStyle::kConsistent,
    cablefile.cable);

  // gets filepath to save cable
  wxFileDialog dialog_file(this, "Save Cable File", "", ".cable",
                           "Cable Files (*.cable) | *.cable", wxFD_SAVE);
  if (dialog_file.ShowModal() != wxID_OK) {
    return;
  }
  cablefile.filepath = dialog_file.GetPath();

  // saves file
  FileHandler::SaveCable(cablefile.filepath, cablefile.cable, units_);

  // determines if new cable was saved over loaded cable file
  int index_existing = -1;
  for (auto iter = cablefiles_modified_.cbegin();
    iter != cablefiles_modified_.cend(); iter++) {
    const CableFile& cablefile_ref = *iter;
    if (cablefile.filepath == cablefile_ref.filepath) {
      index_existing = std::distance(cablefiles_modified_.cbegin(), iter);
      break;
    }
  }

  // merges cable file into application data and listctrl
  if (index_existing == -1) {
    // adds to end of application data
    cablefiles_modified_.push_back(cablefile);

    // adds to end of listctrl
    const long index = listctrl_->GetItemCount();
    wxListItem item;
    item.SetId(index);
    listctrl_->InsertItem(item);

    listctrl_->SetItem(index, 0, cablefile.cable.name);
    listctrl_->SetItem(index, 1, cablefile.filepath);
  } else {
    // replaces existing cable file in application data
    auto iter = std::next(cablefiles_modified_.begin(), index_existing);
    *iter = cablefile;

    // replaces existing listctrl entry
    listctrl_->SetItem(index_existing, 0, cablefile.cable.name);
    listctrl_->SetItem(index_existing, 1, cablefile.filepath);
  }
}

void CableFileManagerDialog::OnButtonOk(wxCommandEvent &event) {
  // copies modified cablefiles to original cablefile list
  *cablefiles_ = cablefiles_modified_;

  EndModal(wxID_OK);
}

void CableFileManagerDialog::OnButtonRemove(wxCommandEvent& event) {
  if (index_selected_ < 0) {
    return;
  }

  // gets iterator to selected cablefile
  auto iter = std::next(cablefiles_modified_.begin(), index_selected_);

  // removes from application data
  cablefiles_modified_.erase(iter);

  // removes from listctrl
  listctrl_->DeleteItem(index_selected_);
}

void CableFileManagerDialog::OnClose(wxCloseEvent &event) {
  EndModal(wxID_CLOSE);
}

void CableFileManagerDialog::OnItemSelect(wxListEvent& event) {
  index_selected_ = event.GetIndex();
}
