// This is free and unencumbered software released into the public domain.
// For more information, please refer to <http://unlicense.org/>

#include "cable_file_manager_dialog.h"

#include "wx/spinbutt.h"
#include "wx/xrc/xmlres.h"

#include "cable_editor_dialog.h"
#include "cable_unit_converter.h"
#include "file_handler.h"
#include "span_analyzer_app.h"
#include "span_analyzer_doc.h"

BEGIN_EVENT_TABLE(CableFileManagerDialog, wxDialog)
  EVT_BUTTON(XRCID("button_add"), CableFileManagerDialog::OnButtonAdd)
  EVT_BUTTON(wxID_CANCEL, CableFileManagerDialog::OnButtonCancel)
  EVT_BUTTON(XRCID("button_edit"), CableFileManagerDialog::OnButtonEdit)
  EVT_BUTTON(wxID_OK, CableFileManagerDialog::OnButtonOk)
  EVT_BUTTON(XRCID("button_new"), CableFileManagerDialog::OnButtonNew)
  EVT_BUTTON(XRCID("button_remove"), CableFileManagerDialog::OnButtonRemove)
  EVT_CLOSE(CableFileManagerDialog::OnClose)
  EVT_LIST_ITEM_SELECTED(wxID_ANY, CableFileManagerDialog::OnItemSelect)
  EVT_SPIN_DOWN(XRCID("spinbutton_cablefiles"), CableFileManagerDialog::OnSpinButtonDown)
  EVT_SPIN_UP(XRCID("spinbutton_cablefiles"), CableFileManagerDialog::OnSpinButtonUp)
END_EVENT_TABLE()

CableFileManagerDialog::CableFileManagerDialog(
    wxWindow* parent,
    const units::UnitSystem& units,
    std::list<CableFile*>* cablefiles) {
  // loads dialog from virtual xrc file system
  wxXmlResource::Get()->LoadDialog(this, parent, "cable_file_manager_dialog");

  // saves references to members
  cablefiles_ = cablefiles;
  cablefiles_original_ = *cablefiles;

  listctrl_ = XRCCTRL(*this, "listctrl_cables", wxListCtrl);
  units_ = units;

  index_selected_ = wxNOT_FOUND;

  // copies list of cables to be modified by manager
  cablefiles_modified_ = *cablefiles_;

  // populates listctrl with cable files
  for (auto iter = cablefiles_->cbegin(); iter != cablefiles_->cend();
       iter++) {
    long index_row = std::distance(cablefiles_->cbegin(), iter);
    const CableFile* cablefile = *iter;

    // adds list item (row) to listctrl
    wxListItem item;
    item.SetId(index_row);
    listctrl_->InsertItem(item);

    listctrl_->SetItem(index_row, 0, cablefile->cable.name);
    listctrl_->SetItem(index_row, 1, cablefile->filepath);
  }

  // fits the dialog around the sizers
  this->Fit();
}

CableFileManagerDialog::~CableFileManagerDialog() {
}

void CableFileManagerDialog::DeleteExtraCableFiles(
    const std::list<CableFile*>* list_keep,
    std::list<CableFile*>* list_master) {
  // iterates over master list
  for (auto iter = list_master->begin(); iter != list_master->end();) {
    CableFile* cablefile = *iter;

    // searches keep list for a match
    std::list<CableFile*>::const_iterator iter_keep = list_keep->cbegin();
    while (iter_keep != list_keep->cend()) {
      CableFile* cablefile_keep = *iter_keep;
      if (cablefile == cablefile_keep) {
        break;
      } else {
        iter_keep++;
      }
    }

    // increments master list iterator
    if (iter_keep == list_keep->cend()) {
      // the cablefile is deallocated
      iter = list_master->erase(iter);
      delete cablefile;
    } else {
      iter++;
    }
  }
}

bool CableFileManagerDialog::IsReferencedByDocument(
    const std::string& name) const {
  // gets document
  const SpanAnalyzerDoc* doc =
      (SpanAnalyzerDoc*)wxGetApp().manager_doc()->GetCurrentDocument();
  if (doc == nullptr) {
    return false;
  }

  // scans all spans to see if cable name is referenced
  for (auto iter = doc->spans().cbegin(); iter != doc->spans().cend(); iter++) {
    const Span& span = *iter;
    if (span.linecable.cable->name == name) {
      return true;
    }
  }

  return false;
}

bool CableFileManagerDialog::IsUniqueName(const std::string& name,
                                          const int& index_ignore) const {
  // scans modified cablefiles to see if the cable name is a duplicate
  for (auto iter = cablefiles_modified_.cbegin();
       iter != cablefiles_modified_.cend(); iter++) {
    const CableFile* cablefile = *iter;

    const int index = std::distance(cablefiles_modified_.cbegin(), iter);
    if (index == index_ignore) {
      continue;
    } else {
      if (cablefile->cable.name == name) {
        return false;
      }
    }
  }

  return true;
}

void CableFileManagerDialog::OnButtonAdd(wxCommandEvent& event) {
  // opens file dialog to select files to add
  wxFileDialog dialog_file(this, "Open Cable File", "", "",
                           "Cable Files (*.cable)|*.cable",
                           wxFD_OPEN|wxFD_MULTIPLE);
  if (dialog_file.ShowModal() != wxID_OK) {
    return;
  }

  wxBusyCursor cursor;

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
      const CableFile* cablefile = *iter;
      if (path == cablefile->filepath) {
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

      // determines if cable name is unique
      bool is_unique = IsUniqueName(cablefile.cable.name);
      if (is_unique == false) {
        std::string message = cablefile.filepath + "  --  "
                              "Cable name is a duplicate. Skipping.";
        wxLogError(message.c_str());
        continue;
      }

      // adds to app data
      CableFile* cablefile_new = new CableFile(cablefile);
      cablefiles_->push_back(cablefile_new);
      cablefiles_modified_.push_back(cablefile_new);

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
  // deletes any extra cable files
  DeleteExtraCableFiles(&cablefiles_original_, cablefiles_);

  EndModal(wxID_CANCEL);
}

void CableFileManagerDialog::OnButtonEdit(wxCommandEvent& event) {
  if (index_selected_ < 0) {
    return;
  }

  // gets cablefile and copies
  auto iter = std::next(cablefiles_modified_.begin(), index_selected_);
  CableFile cablefile = **iter;

  // converts units to different unit style
  CableUnitConverter::ConvertUnitStyle(
      units_,
      units::UnitStyle::kConsistent,
      units::UnitStyle::kDifferent,
      cablefile.cable);

  // lets user edit cable file
  // ensures that the cable name is unique
  bool is_unique = false;
  while (is_unique == false) {
    // shows editor dialog
    CableEditorDialog dialog_edit(this, &cablefile.cable, units_);
    if (dialog_edit.ShowModal() != wxID_OK) {
      return;
    }

    // checks if the cable name is unique
    is_unique = IsUniqueName(cablefile.cable.name, index_selected_);
    if (is_unique == false) {
      std::string message = "Cable name is a duplicate. Please provide "
                            "another name.";
      wxMessageBox(message);
    }
  }

  wxBusyCursor cursor;

  // converts units to consistent unit style
  CableUnitConverter::ConvertUnitStyle(
      units_,
      units::UnitStyle::kDifferent,
      units::UnitStyle::kConsistent,
      cablefile.cable);

  // saves cable to filesystem
  FileHandler::SaveCable(cablefile.filepath, cablefile.cable, units_);

  // transfers copied cable to original
  **iter = cablefile;

  // updates listctrl
  listctrl_->SetItem(index_selected_, 0, cablefile.cable.name);
}

void CableFileManagerDialog::OnButtonNew(wxCommandEvent& event) {
  // creates new cable file
  CableFile cablefile;

  // lets user edit cable file
  // ensures that the cable name is unique
  bool is_unique = false;
  while (is_unique == false) {
    // shows editor dialog
    CableEditorDialog dialog_edit(this, &cablefile.cable, units_);
    if (dialog_edit.ShowModal() != wxID_OK) {
      return;
    }

    // checks if the cable name is unique
    is_unique = IsUniqueName(cablefile.cable.name);
    if (is_unique == false) {
      std::string message = "Cable name is a duplicate. Please provide "
                            "another name.";
      wxMessageBox(message);
    }
  }

  wxBusyCursor cursor;

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
  int index_existing = wxNOT_FOUND;
  for (auto iter = cablefiles_modified_.cbegin();
    iter != cablefiles_modified_.cend(); iter++) {
    const CableFile* cablefile_ref = *iter;
    if (cablefile.filepath == cablefile_ref->filepath) {
      index_existing = std::distance(cablefiles_modified_.cbegin(), iter);
      break;
    }
  }

  // merges cable file into application data and listctrl
  if (index_existing == wxNOT_FOUND) {
    // adds to end of application data
    CableFile* cablefile_new = new CableFile(cablefile);
    cablefiles_->push_back(cablefile_new);
    cablefiles_modified_.push_back(cablefile_new);

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
    **iter = cablefile;

    // replaces existing listctrl entry
    listctrl_->SetItem(index_existing, 0, cablefile.cable.name);
    listctrl_->SetItem(index_existing, 1, cablefile.filepath);
  }
}

void CableFileManagerDialog::OnButtonOk(wxCommandEvent &event) {
  wxBusyCursor cursor;

  // deletes any extra cable files
  DeleteExtraCableFiles(&cablefiles_modified_, cablefiles_);

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
  const CableFile* cablefile = *iter;

  // checks if the cablefile is referenced by the document
  bool is_referenced = IsReferencedByDocument(cablefile->cable.name);
  if (is_referenced == true) {
    std::string message = cablefile->filepath + "  --  "
                          "Cable file is currently referenced by the open "
                          "document.";
    wxMessageBox(message);

    return;
  }

  wxBusyCursor cursor;

  // removes from modified list only
  cablefiles_modified_.erase(iter);

  // removes from listctrl
  listctrl_->DeleteItem(index_selected_);

  // resets cached index
  index_selected_ = wxNOT_FOUND;
}

void CableFileManagerDialog::OnClose(wxCloseEvent &event) {
  // deletes any extra cable files
  DeleteExtraCableFiles(&cablefiles_original_, cablefiles_);

  EndModal(wxID_CLOSE);
}

void CableFileManagerDialog::OnItemSelect(wxListEvent& event) {
  index_selected_ = event.GetIndex();
}

void CableFileManagerDialog::OnSpinButtonDown(wxSpinEvent& event) {
  // exits if nothing is selected
  if (index_selected_ < 0) {
    return;
  }

  // exits if the selected index is the last cable file
  const int kCount = listctrl_->GetItemCount() - 1;
  if (index_selected_ == kCount) {
    return;
  }

  wxBusyCursor cursor;

  // switches cable files in list using iterators
  auto iter_selection = std::next(cablefiles_modified_.begin(),
                                  index_selected_);
  auto iter_position = std::next(iter_selection, 2);
  cablefiles_modified_.splice(iter_position, cablefiles_modified_,
                              iter_selection);

  // updates listctrl
  const CableFile* cablefile = *iter_selection;

  listctrl_->DeleteItem(index_selected_);

  index_selected_ += 1;
  wxListItem item;
  item.SetId(index_selected_);
  item.SetState(wxLIST_STATE_SELECTED);
  listctrl_->InsertItem(item);
  listctrl_->SetItem(index_selected_, 0, cablefile->cable.name);
  listctrl_->SetItem(index_selected_, 1, cablefile->filepath);
}

void CableFileManagerDialog::OnSpinButtonUp(wxSpinEvent& event) {
  // exits if nothing is selected
  if (index_selected_ < 0) {
    return;
  }

  // exits if the selected index is the first cable file
  if (index_selected_ == 0) {
    return;
  }

  wxBusyCursor cursor;

  // switches cable files in list using iterators
  auto iter_selection = std::next(cablefiles_modified_.begin(),
    index_selected_);
  auto iter_position = std::prev(iter_selection, 1);
  cablefiles_modified_.splice(iter_position, cablefiles_modified_,
                              iter_selection);

  // updates listctrl
  const CableFile* cablefile = *iter_selection;

  listctrl_->DeleteItem(index_selected_);

  index_selected_ -= 1;
  wxListItem item;
  item.SetId(index_selected_);
  item.SetState(wxLIST_STATE_SELECTED);
  listctrl_->InsertItem(item);
  listctrl_->SetItem(index_selected_, 0, cablefile->cable.name);
  listctrl_->SetItem(index_selected_, 1, cablefile->filepath);
}
