// This is free and unencumbered software released into the public domain.
// For more information, please refer to <http://unlicense.org/>

#include "weather_load_case_manager_dialog.h"

#include "wx/spinbutt.h"
#include "wx/xrc/xmlres.h"

#include "weather_load_case_editor_dialog.h"
#include "weather_load_case_unit_converter.h"

BEGIN_EVENT_TABLE(WeatherLoadCaseManagerDialog, wxDialog)
  EVT_BUTTON(XRCID("button_groups_add"), WeatherLoadCaseManagerDialog::OnButtonGroupAdd)
  EVT_BUTTON(XRCID("button_groups_delete"), WeatherLoadCaseManagerDialog::OnButtonGroupDelete)
  EVT_BUTTON(XRCID("button_weathercases_add"), WeatherLoadCaseManagerDialog::OnButtonWeathercaseAdd)
  EVT_BUTTON(XRCID("button_weathercases_delete"), WeatherLoadCaseManagerDialog::OnButtonWeathercaseDelete)
  EVT_BUTTON(wxID_CANCEL, WeatherLoadCaseManagerDialog::OnCancel)
  EVT_BUTTON(wxID_OK, WeatherLoadCaseManagerDialog::OnOk)
  EVT_CLOSE(WeatherLoadCaseManagerDialog::OnClose)
  EVT_LISTBOX(XRCID("listbox_groups"), WeatherLoadCaseManagerDialog::OnListBoxGroupSelect)
  EVT_LISTBOX_DCLICK(XRCID("listbox_groups"), WeatherLoadCaseManagerDialog::OnListBoxGroupDoubleClick)
  EVT_LISTBOX_DCLICK(XRCID("listbox_weathercases"), WeatherLoadCaseManagerDialog::OnListBoxWeatherCaseDoubleClick)
  EVT_SPIN_DOWN(XRCID("spinbutton_groups"), WeatherLoadCaseManagerDialog::OnSpinButtonGroupDown)
  EVT_SPIN_DOWN(XRCID("spinbutton_weathercases"), WeatherLoadCaseManagerDialog::OnSpinButtonWeathercaseDown)
  EVT_SPIN_UP(XRCID("spinbutton_groups"), WeatherLoadCaseManagerDialog::OnSpinButtonGroupUp)
  EVT_SPIN_UP(XRCID("spinbutton_weathercases"), WeatherLoadCaseManagerDialog::OnSpinButtonWeathercaseUp)
END_EVENT_TABLE()

WeatherLoadCaseManagerDialog::WeatherLoadCaseManagerDialog(
    wxWindow* parent,
    const units::UnitSystem& units,
    std::list<WeatherLoadCaseGroup>* groups_weathercase) {
  // loads dialog from virtual xrc file system
  wxXmlResource::Get()->LoadDialog(this, parent,
                                   "weather_load_case_manager_dialog");

  // gets listbox references
  listbox_groups_ = XRCCTRL(*this, "listbox_groups", wxListBox);
  listbox_weathercases_ = XRCCTRL(*this, "listbox_weathercases", wxListBox);

  // saves references to data
  groups_ = groups_weathercase;
  units_ = &units;

  // makes copies so user can modify
  groups_modified_ = *groups_;

  // fills the listboxes with the set descriptions
  wxListBox* listbox = nullptr;
  listbox = XRCCTRL(*this, "listbox_groups", wxListBox);
  for (auto iter = groups_modified_.cbegin();
       iter != groups_modified_.cend(); iter++) {
    const WeatherLoadCaseGroup& group = *iter;
    listbox->Append(group.name);
  }

  // fits the dialog around the sizers
  this->Fit();
}

WeatherLoadCaseManagerDialog::~WeatherLoadCaseManagerDialog() {
}

void WeatherLoadCaseManagerDialog::OnButtonGroupAdd(
    wxCommandEvent& event) {
  // gets weathercase group name from user
  std::string name = wxGetTextFromUser("Enter a weathercase group name",
                                       "Weathercase Group Name");
  if (name.empty() == true) {
    return;
  }

  // adds to the group list
  WeatherLoadCaseGroup group;
  groups_modified_.push_back(group);

  // updates the listbox
  listbox_groups_->Append(group.name);
}

void WeatherLoadCaseManagerDialog::OnButtonGroupDelete(
    wxCommandEvent& event) {
  if (index_group_activated_ == wxNOT_FOUND) {
    // exits if nothing is selected
    return;
  } else if (index_group_activated_ == 0) {
    // exits if default group is selected
    return;
  }

  // erases from the groups
  auto iter = std::next(groups_modified_.begin(), index_group_activated_);
  groups_modified_.erase(iter);

  // updates the listboxes
  listbox_groups_->Delete(index_group_activated_);
  listbox_weathercases_->Clear();
}

void WeatherLoadCaseManagerDialog::OnButtonWeathercaseAdd(
    wxCommandEvent& event) {
  // gets the weathercase group
  auto iter = std::next(groups_modified_.begin(), index_group_activated_);
  WeatherLoadCaseGroup& group = *iter;

  // creates a weathercase and shows an editor
  WeatherLoadCase weathercase;
  WeatherLoadCaseEditorDialog dialog(this, &weathercase, *units_);
  if (dialog.ShowModal() == wxID_OK) {
    // converts weathercase to consistent unit style
    WeatherLoadCaseUnitConverter::ConvertUnitStyle(
        *units_,
        units::UnitStyle::kDifferent,
        units::UnitStyle::kConsistent,
        weathercase);

    // adds weathercase to set
    group.weathercases.push_back(weathercase);

    // updates weathercase listbox
    listbox_weathercases_->Append(weathercase.description);
  }
}

void WeatherLoadCaseManagerDialog::OnButtonWeathercaseDelete(
    wxCommandEvent& event) {
  // gets weathercase listbox index
  const int index = listbox_weathercases_->GetSelection();

  // gets the weathercase group from the set list
  auto iter_group = std::next(groups_modified_.begin(), index_group_activated_);
  WeatherLoadCaseGroup& group = *iter_group;

  // erases from weathercase set
  auto iter = std::next(group.weathercases.begin(), index);
  group.weathercases.erase(iter);

  // updates weathercase listbox
  listbox_weathercases_->Delete(index);
}

void WeatherLoadCaseManagerDialog::OnCancel(wxCommandEvent& event) {
  EndModal(wxID_CANCEL);
}

void WeatherLoadCaseManagerDialog::OnClose(wxCloseEvent& event) {
  EndModal(wxID_CLOSE);
}

void WeatherLoadCaseManagerDialog::OnListBoxGroupDoubleClick(
    wxCommandEvent& event) {
  // exits if default group is selected
  if (index_group_activated_ == 0) {
    return;
  }

  // gets group name
  auto iter = std::next(groups_modified_.begin(), index_group_activated_);
  WeatherLoadCaseGroup& group = *iter;

  // shows a dialog an allows user to edit set name
  std::string str = wxGetTextFromUser("Enter a weathercase group name:",
                                      "Weathercase Group Name Edit",
                                      group.name);

  // implements change if user accepts name change
  if (str.empty() == false) {
    // updates list
    group.name = str;

    // updates listbox
    listbox_groups_->SetString(index_group_activated_, str);
  }
}

void WeatherLoadCaseManagerDialog::OnListBoxGroupSelect(
    wxCommandEvent& event) {
  // gets the set listbox index
  index_group_activated_ = listbox_groups_->GetSelection();
  if (index_group_activated_ == wxNOT_FOUND) {
    return;
  }

  // gets the weathercase set from the set list
  auto iter_groups = std::next(groups_modified_.cbegin(),
                               index_group_activated_);
  const WeatherLoadCaseGroup& group = *iter_groups;

  // fills the weathercase listbox
  listbox_weathercases_->Clear();
  for (auto iter = group.weathercases.cbegin();
       iter != group.weathercases.cend(); iter++) {
    const WeatherLoadCase& weathercase = *iter;
    listbox_weathercases_->Append(weathercase.description);
  }
}

void WeatherLoadCaseManagerDialog::OnListBoxWeatherCaseDoubleClick(
    wxCommandEvent& event) {
  // gets the weathercase group
  auto iter_groups = std::next(groups_modified_.begin(),
                               index_group_activated_);
  WeatherLoadCaseGroup& group = *iter_groups;

  // gets the weathercase listbox index
  const int index_weathercases = listbox_weathercases_->GetSelection();
  if (index_weathercases == wxNOT_FOUND) {
    return;
  }

  // gets the weathercase, copies, and converts to different unit style
  auto iter_weathercase = std::next(group.weathercases.begin(),
                                    index_weathercases);
  WeatherLoadCase weathercase = *iter_weathercase;
  WeatherLoadCaseUnitConverter::ConvertUnitStyle(*units_,
                                                 units::UnitStyle::kConsistent,
                                                 units::UnitStyle::kDifferent,
                                                 weathercase);

  // shows an editor
  WeatherLoadCaseEditorDialog dialog(this, &weathercase, *units_);
  if (dialog.ShowModal() == wxID_OK) {
    // converts weathercase to consistent unit style
    WeatherLoadCaseUnitConverter::ConvertUnitStyle(
        *units_,
        units::UnitStyle::kDifferent,
        units::UnitStyle::kConsistent,
        weathercase);

    // saves weathercase to modified cases
    *iter_weathercase = weathercase;

    // updates listbox of description change
    listbox_weathercases_->SetString(index_weathercases,
                                     weathercase.description);
  }
}

void WeatherLoadCaseManagerDialog::OnOk(wxCommandEvent& event) {
  // overwrites original data with modified data
  *groups_ = groups_modified_;

  // ends modal by returning ok indicator
  EndModal(wxID_OK);
}

void WeatherLoadCaseManagerDialog::OnSpinButtonGroupDown(
    wxSpinEvent& event) {
  if (index_group_activated_ == wxNOT_FOUND) {
    // exits if nothing is selected
    return;
  } else if (index_group_activated_ == 0) {
    // exits if default group is selected
    return;
  }

  // exits if the activated set is the last set
  const int kCount = listbox_groups_->GetCount() - 1;
  if (index_group_activated_ == kCount) {
    return;
  }

  // switches groups list using iterators
  auto iter_selection = std::next(groups_modified_.begin(),
                                  index_group_activated_);
  auto iter_position = std::next(iter_selection, 2);
  groups_modified_.splice(iter_position, groups_modified_, iter_selection);

  // updates listbox
  const WeatherLoadCaseGroup& group = *iter_selection;
  listbox_groups_->Delete(index_group_activated_);
  listbox_groups_->Insert(group.name, index_group_activated_ + 1);
  listbox_groups_->SetSelection(index_group_activated_ + 1);

  // updates activated index
  index_group_activated_ += 1;
}

void WeatherLoadCaseManagerDialog::OnSpinButtonGroupUp(
    wxSpinEvent& event) {
  if (index_group_activated_ == wxNOT_FOUND) {
    // exits if nothing is selected
    return;
  } else if (index_group_activated_ == 0) {
    // exits if default group is selected
    return;
  } else if (index_group_activated_ == 1) {
    // exits if group below default is selected
    return;
  }

  // exits if the activated set is the first set
  if (index_group_activated_ == 0) {
    return;
  }

  // switches group list using iterators
  auto iter_selection = std::next(groups_modified_.begin(),
                                  index_group_activated_);
  auto iter_position = std::prev(iter_selection, 1);
  groups_modified_.splice(iter_position, groups_modified_, iter_selection);

  // updates listbox
  const WeatherLoadCaseGroup& group = *iter_selection;
  listbox_groups_->Delete(index_group_activated_);
  listbox_groups_->Insert(group.name, index_group_activated_ - 1);
  listbox_groups_->SetSelection(index_group_activated_ - 1);

  // updates activated index
  index_group_activated_ -= 1;
}

void WeatherLoadCaseManagerDialog::OnSpinButtonWeathercaseDown(
    wxSpinEvent& event) {
  // gets weathercase index
  const int index = listbox_weathercases_->GetSelection();

  // exits if nothing is selected
  if (index == wxNOT_FOUND) {
    return;
  }

  // exits if the selected index is the first weathercase
  const int kCount = listbox_groups_->GetCount() - 1;
  if (index == kCount) {
    return;
  }

  // gets the weathercase set
  auto iter_groups = std::next(groups_modified_.begin(), index_group_activated_);
  WeatherLoadCaseGroup& group = *iter_groups;

  // switches weathercases in list using iterators
  auto iter_selection = std::next(group.weathercases.begin(), index);
  auto iter_position = std::next(iter_selection, 2);
  group.weathercases.splice(iter_position, group.weathercases, iter_selection);

  // updates listbox
  const WeatherLoadCase& weathercase = *iter_selection;
  listbox_weathercases_->Delete(index);
  listbox_weathercases_->Insert(weathercase.description, index + 1);
  listbox_weathercases_->SetSelection(index + 1);
}

void WeatherLoadCaseManagerDialog::OnSpinButtonWeathercaseUp(
    wxSpinEvent& event) {
  // gets weathercase index
  const int index = listbox_weathercases_->GetSelection();

  // exits if nothing is selected
  if (index == wxNOT_FOUND) {
    return;
  }

  // exits if the selected index is the first weathercase
  if (index == 0) {
    return;
  }

  // gets the weathercase set
  auto iter_groups = std::next(groups_modified_.begin(), index_group_activated_);
  WeatherLoadCaseGroup& group = *iter_groups;

  // switches weathercases in list using iterators
  auto iter_selection = std::next(group.weathercases.begin(), index);
  auto iter_position = std::prev(iter_selection, 1);
  group.weathercases.splice(iter_position, group.weathercases, iter_selection);

  // updates listbox
  const WeatherLoadCase& weathercase = *iter_selection;
  listbox_weathercases_->Delete(index);
  listbox_weathercases_->Insert(weathercase.description, index - 1);
  listbox_weathercases_->SetSelection(index - 1);
}
