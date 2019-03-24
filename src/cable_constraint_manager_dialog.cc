// This is free and unencumbered software released into the public domain.
// For more information, please refer to <http://unlicense.org/>

#include "spananalyzer/cable_constraint_manager_dialog.h"

#include "appcommon/units/cable_constraint_unit_converter.h"
#include "models/base/helper.h"
#include "wx/spinbutt.h"
#include "wx/xrc/xmlres.h"

#include "spananalyzer/cable_constraint_editor_dialog.h"

BEGIN_EVENT_TABLE(CableConstraintManagerDialog, wxDialog)
  EVT_BUTTON(XRCID("button_add"), CableConstraintManagerDialog::OnButtonAdd)
  EVT_BUTTON(XRCID("button_delete"), CableConstraintManagerDialog::OnButtonDelete)
  EVT_BUTTON(XRCID("button_edit"), CableConstraintManagerDialog::OnButtonEdit)
  EVT_BUTTON(wxID_CANCEL, CableConstraintManagerDialog::OnCancel)
  EVT_BUTTON(wxID_OK, CableConstraintManagerDialog::OnOk)
  EVT_CLOSE(CableConstraintManagerDialog::OnClose)
  EVT_LIST_ITEM_SELECTED(wxID_ANY, CableConstraintManagerDialog::OnListCtrlSelect)
  EVT_SPIN_DOWN(XRCID("spinbutton"), CableConstraintManagerDialog::OnSpinButtonDown)
  EVT_SPIN_UP(XRCID("spinbutton"), CableConstraintManagerDialog::OnSpinButtonUp)
END_EVENT_TABLE()

CableConstraintManagerDialog::CableConstraintManagerDialog(
    wxWindow* parent,
    const std::list<CableFile*>* cablefiles,
    const std::list<WeatherLoadCase*>* weathercases,
    const units::UnitSystem& units,
    std::list<CableConstraint>* constraints) {
  // loads dialog from virtual xrc file system
  wxXmlResource::Get()->LoadDialog(this, parent,
                                   "cable_constraint_manager_dialog");

  // gets listctrl references
  listctrl_ = XRCCTRL(*this, "listctrl", wxListCtrl);

  // saves references to data
  cablefiles_ = cablefiles;
  weathercases_ = weathercases;
  units_ = &units;
  constraints_ = constraints;

  // makes copies so user can modify
  constraints_modified_ = *constraints;

  // fills the listctrl
  for (auto iter = constraints_modified_.cbegin();
       iter != constraints_modified_.cend(); iter++) {
    const CableConstraint& constraint = *iter;

    // adds list item (row) to listctrl
    const long index = listctrl_->GetItemCount();
    wxListItem item;
    item.SetId(index);
    listctrl_->InsertItem(item);
    UpdateListCtrlRow(index, constraint);
  }

  index_selected_ = wxNOT_FOUND;

  // fits the dialog around the sizers
  this->Fit();
}

CableConstraintManagerDialog::~CableConstraintManagerDialog() {
}

void CableConstraintManagerDialog::OnButtonAdd(wxCommandEvent& event) {
  // creates a constraint
  // initializes values to zero
  CableConstraint constraint;
  constraint.limit = 0;

  // creates a dialog to let the user edit
  CableConstraintEditorDialog dialog(this, cablefiles_, weathercases_, *units_,
                                     &constraint);
  if (dialog.ShowModal() != wxID_OK) {
    return;
  }

  wxBusyCursor cursor;

  // converts units to consistent unit style
  CableConstraintUnitConverter::ConvertUnitStyleToConsistent(
      0, *units_, constraint);

  // adds to modified list
  if (index_selected_ == wxNOT_FOUND) {
    // nothing is selected so the constraint is appended
    constraints_modified_.push_back(constraint);
  } else {
    // an index is selected so the filter is inserted after selection
    auto iter = std::next(constraints_modified_.begin(), index_selected_ + 1);
    constraints_modified_.insert(iter, constraint);
  }

  // adds to listctrl
  long index = -1;
  if (index_selected_ == wxNOT_FOUND) {
    index = listctrl_->GetItemCount();
  } else {
    index = index_selected_ + 1;
  }

  wxListItem item;
  item.SetId(index);
  listctrl_->InsertItem(item);
  UpdateListCtrlRow(index, constraint);
}

void CableConstraintManagerDialog::OnButtonDelete(wxCommandEvent& event) {
  // checks index
  if (index_selected_ == wxNOT_FOUND) {
    return;
  }

  // gets iterator to selected cablefile
  auto iter = std::next(constraints_modified_.begin(), index_selected_);

  wxBusyCursor cursor;

  // erases modified list only
  constraints_modified_.erase(iter);

  // updates listctrl
  listctrl_->DeleteItem(index_selected_);

  // reselects listctrl item
  const int kSize = listctrl_->GetItemCount();
  if (kSize == 0) {
    // no items left in listctrl
    index_selected_ = wxNOT_FOUND;
    return;
  } else if (index_selected_ < kSize) {
    // reselects index
    listctrl_->SetItemState(index_selected_,
                            wxLIST_STATE_SELECTED,
                            wxLIST_STATE_SELECTED);
  } else {
    // last item in the listctrl was deleted
    // decrements index and reselects
    index_selected_--;
    listctrl_->SetItemState(index_selected_,
                            wxLIST_STATE_SELECTED,
                            wxLIST_STATE_SELECTED);
  }
}

void CableConstraintManagerDialog::OnButtonEdit(wxCommandEvent& event) {
  if (index_selected_ < 0) {
    return;
  }

  // gets constraint and copies
  auto iter = std::next(constraints_modified_.begin(), index_selected_);
  CableConstraint constraint = *iter;

  // converts units to different unit style
  CableConstraintUnitConverter::ConvertUnitStyleToDifferent(
      *units_, constraint);

  // shows editor dialog
  CableConstraintEditorDialog dialog(this, cablefiles_, weathercases_, *units_,
                                     &constraint);
  if (dialog.ShowModal() != wxID_OK) {
    return;
  }

  wxBusyCursor cursor;

  // converts units to consistent unit style
  CableConstraintUnitConverter::ConvertUnitStyleToConsistent(
      0, *units_, constraint);

  // transfers edited constraint to list
  *iter = constraint;

  // updates listctrl
  UpdateListCtrlRow(index_selected_, constraint);
}

void CableConstraintManagerDialog::OnCancel(wxCommandEvent& event) {
  EndModal(wxID_CANCEL);
}

void CableConstraintManagerDialog::OnClose(wxCloseEvent& event) {
  EndModal(wxID_CLOSE);
}

void CableConstraintManagerDialog::OnListCtrlSelect(wxListEvent& event) {
  index_selected_ = event.GetIndex();
}

void CableConstraintManagerDialog::OnOk(wxCommandEvent& event) {
  wxBusyCursor cursor;

  // overwrites original data with modified data
  *constraints_ = constraints_modified_;

  // ends modal by returning ok indicator
  EndModal(wxID_OK);
}

void CableConstraintManagerDialog::OnSpinButtonDown(wxSpinEvent& event) {
  // exits if nothing is selected
  if (index_selected_ == wxNOT_FOUND) {
    return;
  }

  // exits if the selected index is the last filter
  const long kCount = listctrl_->GetItemCount() - 1;
  if (index_selected_ == kCount) {
    return;
  }

  wxBusyCursor cursor;

  // switches constraints in list using iterators
  auto iter_selection = std::next(constraints_modified_.begin(),
                                  index_selected_);
  auto iter_position = std::next(iter_selection, 2);
  constraints_modified_.splice(iter_position, constraints_modified_,
                               iter_selection);

  // updates listctrl
  const CableConstraint& constraint = *iter_selection;

  listctrl_->DeleteItem(index_selected_);

  index_selected_ += 1;
  wxListItem item;
  item.SetId(index_selected_);
  item.SetState(wxLIST_STATE_SELECTED);
  listctrl_->InsertItem(item);
  UpdateListCtrlRow(index_selected_, constraint);
}

void CableConstraintManagerDialog::OnSpinButtonUp(wxSpinEvent& event) {
  // exits if nothing is selected
  if (index_selected_ == wxNOT_FOUND) {
    return;
  }

  // exits if the selected index is the first filter
  if (index_selected_ == 0) {
    return;
  }

  wxBusyCursor cursor;

  // switches constraints in list using iterators
  auto iter_selection = std::next(constraints_modified_.begin(),
                                  index_selected_);
  auto iter_position = std::prev(iter_selection, 1);
  constraints_modified_.splice(iter_position, constraints_modified_,
                               iter_selection);

  // updates listctrl
  const CableConstraint& constraint = *iter_selection;

  listctrl_->DeleteItem(index_selected_);

  index_selected_ -= 1;
  wxListItem item;
  item.SetId(index_selected_);
  item.SetState(wxLIST_STATE_SELECTED);
  listctrl_->InsertItem(item);
  UpdateListCtrlRow(index_selected_, constraint);
}

void CableConstraintManagerDialog::UpdateListCtrlRow(
    const long& index,
    const CableConstraint& constraint) {
  // checks index
  if (index < 0) {
    return;
  } else if (listctrl_->GetItemCount() <= index) {
    return;
  }

  // gets condition string
  std::string str_condition;
  if (constraint.condition == CableConditionType::kCreep) {
    str_condition = "Creep";
  } else if (constraint.condition == CableConditionType::kInitial) {
    str_condition = "Initial";
  } else if (constraint.condition == CableConditionType::kLoad) {
    str_condition = "Load";
  }

  // gets type string
  std::string str_type;
  if (constraint.type_limit == CableConstraint::LimitType::kCatenaryConstant) {
    str_type = "H/w";
  } else if (constraint.type_limit ==
             CableConstraint::LimitType::kHorizontalTension) {
    str_type = "Horizontal";
  } else if (constraint.type_limit == CableConstraint::LimitType::kLength) {
    str_type = "Length";
  } else if (constraint.type_limit == CableConstraint::LimitType::kSag) {
    str_type = "Sag";
  } else if (constraint.type_limit ==
             CableConstraint::LimitType::kSupportTension) {
    str_type = "Support";
  }

  // gets limit string
  std::string str_limit = helper::DoubleToString(constraint.limit, 2, true);

  // modifies listctrl
  listctrl_->SetItem(index, 0, constraint.note);
  listctrl_->SetItem(index, 1, constraint.case_weather->description);
  listctrl_->SetItem(index, 2, str_condition);
  listctrl_->SetItem(index, 3, str_type);
  listctrl_->SetItem(index, 4, str_limit);
}
