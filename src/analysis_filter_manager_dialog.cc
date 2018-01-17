// This is free and unencumbered software released into the public domain.
// For more information, please refer to <http://unlicense.org/>

#include "analysis_filter_manager_dialog.h"

#include "wx/spinbutt.h"
#include "wx/xrc/xmlres.h"

#include "analysis_filter_editor_dialog.h"

BEGIN_EVENT_TABLE(AnalysisFilterManagerDialog, wxDialog)
  EVT_BUTTON(XRCID("button_filters_add"), AnalysisFilterManagerDialog::OnButtonFilterAdd)
  EVT_BUTTON(XRCID("button_filters_delete"), AnalysisFilterManagerDialog::OnButtonFilterDelete)
  EVT_BUTTON(XRCID("button_filters_edit"), AnalysisFilterManagerDialog::OnButtonFilterEdit)
  EVT_BUTTON(XRCID("button_groups_add"), AnalysisFilterManagerDialog::OnButtonGroupAdd)
  EVT_BUTTON(XRCID("button_groups_delete"), AnalysisFilterManagerDialog::OnButtonGroupDelete)
  EVT_BUTTON(wxID_CANCEL, AnalysisFilterManagerDialog::OnCancel)
  EVT_BUTTON(wxID_OK, AnalysisFilterManagerDialog::OnOk)
  EVT_CLOSE(AnalysisFilterManagerDialog::OnClose)
  EVT_LISTBOX(XRCID("listbox_groups"), AnalysisFilterManagerDialog::OnListBoxGroupSelect)
  EVT_LISTBOX_DCLICK(XRCID("listbox_groups"), AnalysisFilterManagerDialog::OnListBoxGroupDoubleClick)
  EVT_LIST_ITEM_SELECTED(wxID_ANY, AnalysisFilterManagerDialog::OnListCtrlFilterSelect)
  EVT_SPIN_DOWN(XRCID("spinbutton_filters"), AnalysisFilterManagerDialog::OnSpinButtonFilterDown)
  EVT_SPIN_DOWN(XRCID("spinbutton_groups"), AnalysisFilterManagerDialog::OnSpinButtonGroupDown)
  EVT_SPIN_UP(XRCID("spinbutton_filters"), AnalysisFilterManagerDialog::OnSpinButtonFilterUp)
  EVT_SPIN_UP(XRCID("spinbutton_groups"), AnalysisFilterManagerDialog::OnSpinButtonGroupUp)
END_EVENT_TABLE()

AnalysisFilterManagerDialog::AnalysisFilterManagerDialog(
    wxWindow* parent,
    const std::list<WeatherLoadCase*>* weathercases,
    std::list<AnalysisFilterGroup>* groups_filters) {
  // loads dialog from virtual xrc file system
  wxXmlResource::Get()->LoadDialog(this, parent,
                                   "analysis_filter_manager_dialog");

  // gets listbox references
  listbox_groups_ = XRCCTRL(*this, "listbox_groups", wxListBox);
  listctrl_filters_ = XRCCTRL(*this, "listctrl_filters", wxListCtrl);

  // saves references to data
  groups_ = groups_filters;
  weathercases_ = weathercases;

  // makes copies so user can modify
  groups_modified_ = *groups_;

  // fills the listboxes with the group descriptions
  for (auto iter = groups_modified_.cbegin();
       iter != groups_modified_.cend(); iter++) {
    const AnalysisFilterGroup& group = *iter;
    listbox_groups_->Append(group.name);
  }

  index_group_selected_ = wxNOT_FOUND;
  index_filter_selected_ = wxNOT_FOUND;

  // fits the dialog around the sizers
  this->Fit();
}

AnalysisFilterManagerDialog::~AnalysisFilterManagerDialog() {
}

void AnalysisFilterManagerDialog::OnButtonFilterAdd(wxCommandEvent& event) {
  // checks group index
  if (index_group_selected_ == wxNOT_FOUND) {
    return;
  }

  // gets the filter group
  auto iter = std::next(groups_modified_.begin(), index_group_selected_);
  AnalysisFilterGroup& group = *iter;

  // creates a filter
  AnalysisFilter filter;

  // creates a dialog to let the user edit
  AnalysisFilterEditorDialog dialog(this, weathercases_, &filter);
  if (dialog.ShowModal() != wxID_OK) {
    return;
  }

  wxBusyCursor cursor;

  // adds filter to group
  if (index_filter_selected_ == wxNOT_FOUND) {
    // nothing is selected so the cable is appended
    group.filters.push_back(filter);
  } else {
    // an index is selected so the filter is inserted after selection
    auto iter = std::next(group.filters.begin(), index_filter_selected_ + 1);
    group.filters.insert(iter, filter);
  }

  // adds to listctrl
  long index = -1;
  if (index_filter_selected_ == wxNOT_FOUND) {
    index = listctrl_filters_->GetItemCount();
  } else {
    index = index_filter_selected_ + 1;
  }

  wxListItem item;
  item.SetId(index);
  listctrl_filters_->InsertItem(item);
  UpdateListCtrlRow(index, filter);
}

void AnalysisFilterManagerDialog::OnButtonFilterDelete(wxCommandEvent& event) {
  // checks group index
  if (index_group_selected_ == wxNOT_FOUND) {
    return;
  }

  // checks filter index
  if (index_filter_selected_ == wxNOT_FOUND) {
    return;
  }

  // gets the filter group from the group list
  auto iter_group = std::next(groups_modified_.begin(), index_group_selected_);
  AnalysisFilterGroup& group = *iter_group;

  // gets the filter from the group
  auto iter = std::next(group.filters.begin(), index_filter_selected_);

  wxBusyCursor cursor;

  // erases from filter group
  group.filters.erase(iter);

  // updates filter listctrl
  listctrl_filters_->DeleteItem(index_filter_selected_);

  // reselects listctrl item
  const int kSize = listctrl_filters_->GetItemCount();
  if (kSize == 0) {
    // no items left in listctrl
    index_filter_selected_ = wxNOT_FOUND;
    return;
  } else if (index_filter_selected_ < kSize) {
    // reselects index
    listctrl_filters_->SetItemState(index_filter_selected_,
                                    wxLIST_STATE_SELECTED,
                                    wxLIST_STATE_SELECTED);
  } else {
    // last item in the listctrl was deleted
    // decrements index and reselects
    index_filter_selected_--;
    listctrl_filters_->SetItemState(index_filter_selected_,
                                    wxLIST_STATE_SELECTED,
                                    wxLIST_STATE_SELECTED);
  }
}

void AnalysisFilterManagerDialog::OnButtonFilterEdit(wxCommandEvent& event) {
  // checks group index
  if (index_group_selected_ == wxNOT_FOUND) {
    return;
  }

  // checks filter index
  if (index_filter_selected_ == wxNOT_FOUND) {
    return;
  }

  // gets the filter group
  auto iter_groups = std::next(groups_modified_.begin(),
                               index_group_selected_);
  AnalysisFilterGroup& group = *iter_groups;

  // gets the filter and copies
  auto iter_filter = std::next(group.filters.begin(),
                               index_filter_selected_);
  AnalysisFilter filter = *iter_filter;

  // creates a dialog to let the user edit
  AnalysisFilterEditorDialog dialog(this, weathercases_, &filter);
  if (dialog.ShowModal() != wxID_OK) {
    return;
  }

  wxBusyCursor cursor;

  // saves filter to modified list
  *iter_filter = filter;

  // updates listctrl
  UpdateListCtrlRow(index_filter_selected_, filter);
}

void AnalysisFilterManagerDialog::OnButtonGroupAdd(
    wxCommandEvent& event) {
  // gets filter group name from user
  std::string name = wxGetTextFromUser("Enter a filter group name",
                                       "Analysis Filter Group Name");
  if (name.empty() == true) {
    return;
  }

  wxBusyCursor cursor;

  // adds to the group list
  AnalysisFilterGroup group;
  group.name = name;
  groups_modified_.push_back(group);

  // updates the listbox
  listbox_groups_->Append(group.name);
}

void AnalysisFilterManagerDialog::OnButtonGroupDelete(
    wxCommandEvent& event) {
  // checks index
  if (index_group_selected_ == wxNOT_FOUND) {
    return;
  }

  wxBusyCursor cursor;

  // erases from the groups
  auto iter = std::next(groups_modified_.begin(), index_group_selected_);
  groups_modified_.erase(iter);

  // updates the group listbox
  listbox_groups_->Delete(index_group_selected_);
  const int kSize = listbox_groups_->GetCount();
  if (index_group_selected_ < kSize) {
    listbox_groups_->SetSelection(index_group_selected_);
  } else {
    index_group_selected_ = wxNOT_FOUND;
  }

  // updates the filter listctrl
  listctrl_filters_->DeleteAllItems();
  index_filter_selected_ = wxNOT_FOUND;
}

void AnalysisFilterManagerDialog::OnCancel(wxCommandEvent& event) {
  EndModal(wxID_CANCEL);
}

void AnalysisFilterManagerDialog::OnClose(wxCloseEvent& event) {
  EndModal(wxID_CLOSE);
}

void AnalysisFilterManagerDialog::OnListBoxGroupDoubleClick(
    wxCommandEvent& event) {
  // gets group name
  auto iter = std::next(groups_modified_.begin(), index_group_selected_);
  AnalysisFilterGroup& group = *iter;

  // shows a dialog an allows user to edit set name
  std::string str = wxGetTextFromUser("Enter a filter group name:",
                                      "Weathercase Group Name Edit",
                                      group.name);

  wxBusyCursor cursor;

  // implements change if user accepts name change
  if (str.empty() == false) {
    // updates list
    group.name = str;

    // updates listbox
    listbox_groups_->SetString(index_group_selected_, str);
  }
}

void AnalysisFilterManagerDialog::OnListBoxGroupSelect(
    wxCommandEvent& event) {
  // updates the group index
  index_group_selected_ = listbox_groups_->GetSelection();
  if (index_group_selected_ == wxNOT_FOUND) {
    return;
  }

  wxBusyCursor cursor;

  // gets the filter group from the list
  auto iter_groups = std::next(groups_modified_.cbegin(),
                               index_group_selected_);
  const AnalysisFilterGroup& group = *iter_groups;

  // fills the filter listctrl
  listctrl_filters_->DeleteAllItems();
  for (auto iter = group.filters.cbegin(); iter != group.filters.cend();
       iter++) {
    const AnalysisFilter& filter = *iter;

    // adds list item (row) to listctrl
    const long index = listctrl_filters_->GetItemCount();
    wxListItem item;
    item.SetId(index);
    listctrl_filters_->InsertItem(item);
    UpdateListCtrlRow(index, filter);
  }
}

void AnalysisFilterManagerDialog::OnListCtrlFilterSelect(wxListEvent& event) {
  index_filter_selected_ = event.GetIndex();
}

void AnalysisFilterManagerDialog::OnOk(wxCommandEvent& event) {
  wxBusyCursor cursor;

  // overwrites original data with modified data
  *groups_ = groups_modified_;

  // ends modal by returning ok indicator
  EndModal(wxID_OK);
}

void AnalysisFilterManagerDialog::OnSpinButtonFilterDown(wxSpinEvent& event) {
  // exits if nothing is selected
  if (index_filter_selected_ == wxNOT_FOUND) {
    return;
  }

  // exits if the selected index is the last filter
  const long kCount = listctrl_filters_->GetItemCount() - 1;
  if (index_filter_selected_ == kCount) {
    return;
  }

  wxBusyCursor cursor;

  // gets the filter group
  auto iter_groups = std::next(groups_modified_.begin(), index_group_selected_);
  AnalysisFilterGroup& group = *iter_groups;

  // switches filters in list using iterators
  auto iter_selection = std::next(group.filters.begin(),
                                  index_filter_selected_);
  auto iter_position = std::next(iter_selection, 2);
  group.filters.splice(iter_position, group.filters, iter_selection);

  // updates listctrl
  const AnalysisFilter& filter = *iter_selection;

  listctrl_filters_->DeleteItem(index_filter_selected_);

  index_filter_selected_ += 1;
  wxListItem item;
  item.SetId(index_filter_selected_);
  item.SetState(wxLIST_STATE_SELECTED);
  listctrl_filters_->InsertItem(item);
  UpdateListCtrlRow(index_filter_selected_, filter);
}

void AnalysisFilterManagerDialog::OnSpinButtonFilterUp(wxSpinEvent& event) {
  // exits if nothing is selected
  if (index_filter_selected_ == wxNOT_FOUND) {
    return;
  }

  // exits if the selected index is the first filter
  if (index_filter_selected_ == 0) {
    return;
  }

  wxBusyCursor cursor;

  // gets the filter group
  auto iter_groups = std::next(groups_modified_.begin(), index_group_selected_);
  AnalysisFilterGroup& group = *iter_groups;

  // switches weathercases in list using iterators
  auto iter_selection = std::next(group.filters.begin(),
                                  index_filter_selected_);
  auto iter_position = std::prev(iter_selection, 1);
  group.filters.splice(iter_position, group.filters, iter_selection);

  const AnalysisFilter& filter = *iter_selection;

  listctrl_filters_->DeleteItem(index_filter_selected_);

  index_filter_selected_ -= 1;
  wxListItem item;
  item.SetId(index_filter_selected_);
  item.SetState(wxLIST_STATE_SELECTED);
  listctrl_filters_->InsertItem(item);
  UpdateListCtrlRow(index_filter_selected_, filter);
}

void AnalysisFilterManagerDialog::OnSpinButtonGroupDown(
    wxSpinEvent& event) {
  // checks index
  if (index_group_selected_ == wxNOT_FOUND) {
    return;
  }

  // exits if the group index is the last index
  const int kCount = listbox_groups_->GetCount() - 1;
  if (index_group_selected_ == kCount) {
    return;
  }

  wxBusyCursor cursor;

  // switches groups list using iterators
  auto iter_selection = std::next(groups_modified_.begin(),
                                  index_group_selected_);
  auto iter_position = std::next(iter_selection, 2);
  groups_modified_.splice(iter_position, groups_modified_, iter_selection);

  // updates listbox
  const AnalysisFilterGroup& group = *iter_selection;
  listbox_groups_->Delete(index_group_selected_);
  listbox_groups_->Insert(group.name, index_group_selected_ + 1);
  listbox_groups_->SetSelection(index_group_selected_ + 1);

  // updates activated index
  index_group_selected_ += 1;
}

void AnalysisFilterManagerDialog::OnSpinButtonGroupUp(
    wxSpinEvent& event) {
  // checks index
  if (index_group_selected_ == wxNOT_FOUND) {
    return;
  }

  // exits if the selected index is the first index
  if (index_group_selected_ == 0) {
    return;
  }

  wxBusyCursor cursor;

  // switches group list using iterators
  auto iter_selection = std::next(groups_modified_.begin(),
                                  index_group_selected_);
  auto iter_position = std::prev(iter_selection, 1);
  groups_modified_.splice(iter_position, groups_modified_, iter_selection);

  // updates listbox
  const AnalysisFilterGroup& group = *iter_selection;
  listbox_groups_->Delete(index_group_selected_);
  listbox_groups_->Insert(group.name, index_group_selected_ - 1);
  listbox_groups_->SetSelection(index_group_selected_ - 1);

  // updates activated index
  index_group_selected_ -= 1;
}

void AnalysisFilterManagerDialog::UpdateListCtrlRow(const long& index,
                                                    const AnalysisFilter& filter) {
  // checks index
  if (index < 0) {
    return;
  } else if (listctrl_filters_->GetItemCount() <= index) {
    return;
  }

  // gets a condition string
  std::string str_condition;
  if (filter.condition == CableConditionType::kCreep) {
    str_condition = "Creep";
  } else if (filter.condition == CableConditionType::kInitial) {
    str_condition = "Initial";
  } else if (filter.condition == CableConditionType::kLoad) {
    str_condition = "Load";
  }

  listctrl_filters_->SetItem(index, 0, filter.weathercase->description);
  listctrl_filters_->SetItem(index, 1, str_condition);
}
