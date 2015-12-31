// This is free and unencumbered software released into the public domain.
// For more information, please refer to <http://unlicense.org/>

#include "analysis_weather_load_case_manager_dialog.h"

#include "wx/spinbutt.h"
#include "wx/xrc/xmlres.h"

#include "weather_load_case_editor_dialog.h"
#include "weather_load_case_unit_converter.h"

BEGIN_EVENT_TABLE(AnalysisWeatherLoadCaseManagerDialog, wxDialog)
  EVT_BUTTON(XRCID("button_sets_add"), AnalysisWeatherLoadCaseManagerDialog::OnButtonSetAdd)
  EVT_BUTTON(XRCID("button_sets_delete"), AnalysisWeatherLoadCaseManagerDialog::OnButtonSetDelete)
  EVT_BUTTON(XRCID("button_weathercases_add"), AnalysisWeatherLoadCaseManagerDialog::OnButtonWeathercaseAdd)
  EVT_BUTTON(XRCID("button_weathercases_delete"), AnalysisWeatherLoadCaseManagerDialog::OnButtonWeathercaseDelete)
  EVT_BUTTON(wxID_CANCEL, AnalysisWeatherLoadCaseManagerDialog::OnCancel)
  EVT_BUTTON(wxID_OK, AnalysisWeatherLoadCaseManagerDialog::OnOk)
  EVT_CLOSE(AnalysisWeatherLoadCaseManagerDialog::OnClose)
  EVT_LISTBOX(XRCID("listbox_sets"), AnalysisWeatherLoadCaseManagerDialog::OnListBoxSetSelect)
  EVT_LISTBOX_DCLICK(XRCID("listbox_sets"), AnalysisWeatherLoadCaseManagerDialog::OnListBoxSetDoubleClick)
  EVT_LISTBOX_DCLICK(XRCID("listbox_weathercases"), AnalysisWeatherLoadCaseManagerDialog::OnListBoxWeatherCaseDoubleClick)
  EVT_SPIN_DOWN(XRCID("spinbutton_sets"), AnalysisWeatherLoadCaseManagerDialog::OnSpinButtonSetDown)
  EVT_SPIN_DOWN(XRCID("spinbutton_weathercases"), AnalysisWeatherLoadCaseManagerDialog::OnSpinButtonWeathercaseDown)
  EVT_SPIN_UP(XRCID("spinbutton_sets"), AnalysisWeatherLoadCaseManagerDialog::OnSpinButtonSetUp)
  EVT_SPIN_UP(XRCID("spinbutton_weathercases"), AnalysisWeatherLoadCaseManagerDialog::OnSpinButtonWeathercaseUp)
END_EVENT_TABLE()

AnalysisWeatherLoadCaseManagerDialog::AnalysisWeatherLoadCaseManagerDialog(
    wxWindow* parent,
    const units::UnitSystem& units,
    std::list<std::string>* descriptions_weathercases_analysis,
    std::list<std::list<WeatherLoadCase>>* weathercases_analysis) {
  // loads dialog from virtual xrc file system
  wxXmlResource::Get()->LoadDialog(this, parent,
                                   "analysis_weather_load_case_manager_dialog");

  // gets listbox references
  listbox_sets_ = XRCCTRL(*this, "listbox_sets", wxListBox);
  listbox_weathercases_ = XRCCTRL(*this, "listbox_weathercases", wxListBox);

  // saves references to data
  descriptions_ = descriptions_weathercases_analysis;
  sets_ = weathercases_analysis;
  units_ = &units;

  // makes copies so user can modify
  descriptions_modified_ = *descriptions_;
  sets_modified_ = *sets_;

  // fills the listboxes with the set descriptions
  wxListBox* listbox = nullptr;
  listbox = XRCCTRL(*this, "listbox_sets", wxListBox);
  for (auto iter = descriptions_modified_.cbegin();
       iter != descriptions_modified_.cend(); iter++) {
    const std::string& description = *iter;
    listbox->Append(description);
  }

  // fits the dialog around the sizers
  this->Fit();
}

AnalysisWeatherLoadCaseManagerDialog::~AnalysisWeatherLoadCaseManagerDialog() {
}

void AnalysisWeatherLoadCaseManagerDialog::OnButtonSetAdd(
    wxCommandEvent& event) {
  // gets weathercase set name from user
  std::string name = wxGetTextFromUser("Enter a weathercase set name",
                                       "Weathercase Set Name");
  if (name.empty() == true) {
    return;
  }

  // adds to the description list
  descriptions_modified_.push_back(name);

  // adds to the set list
  std::list<WeatherLoadCase> weathercases;
  sets_modified_.push_back(weathercases);

  // updates the listbox
  listbox_sets_->Append(name);
}

void AnalysisWeatherLoadCaseManagerDialog::OnButtonSetDelete(
    wxCommandEvent& event) {
  // exits if nothing is selected
  if (index_set_activated_ == wxNOT_FOUND) {
    return;
  }

  // erases from the description list
  auto iter_description = std::next(descriptions_modified_.begin(),
                                    index_set_activated_);
  descriptions_modified_.erase(iter_description);

  // erases from the set list
  auto iter_set = std::next(sets_modified_.begin(), index_set_activated_);
  sets_modified_.erase(iter_set);

  // updates the listboxes
  listbox_sets_->Delete(index_set_activated_);
  listbox_weathercases_->Clear();
}

void AnalysisWeatherLoadCaseManagerDialog::OnButtonWeathercaseAdd(
    wxCommandEvent& event) {
  // gets the weathercase set
  auto iter_sets = std::next(sets_modified_.begin(), index_set_activated_);
  std::list<WeatherLoadCase>& set = *iter_sets;

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
    set.push_back(weathercase);

    // updates weathercase listbox
    listbox_weathercases_->Append(weathercase.description);
  }
}

void AnalysisWeatherLoadCaseManagerDialog::OnButtonWeathercaseDelete(
    wxCommandEvent& event) {
  // gets weathercase listbox index
  const int index = listbox_weathercases_->GetSelection();

  // gets the weathercase set from the set list
  auto iter_sets = std::next(sets_modified_.begin(), index_set_activated_);
  std::list<WeatherLoadCase>& set = *iter_sets;

  // erases from weathercase set
  auto iter = std::next(set.begin(), index);
  set.erase(iter);

  // updates weathercase listbox
  listbox_weathercases_->Delete(index);
}

void AnalysisWeatherLoadCaseManagerDialog::OnCancel(wxCommandEvent& event) {
  EndModal(wxID_CANCEL);
}

void AnalysisWeatherLoadCaseManagerDialog::OnClose(wxCloseEvent& event) {
  EndModal(wxID_CLOSE);
}

void AnalysisWeatherLoadCaseManagerDialog::OnListBoxSetDoubleClick(
    wxCommandEvent& event) {
  // gets set name
  auto iter = std::next(descriptions_modified_.begin(), index_set_activated_);

  // shows a dialog an allows user to edit set name
  std::string str = wxGetTextFromUser("Enter a weathercase set name:",
                                      "Weathercase Set Name Edit",
                                      *iter);

  // implements change if user accepts name change
  if (str.empty() == false) {
    // updates list
    *iter = str;

    // updates listbox
    listbox_sets_->SetString(index_set_activated_, str);
  }
}

void AnalysisWeatherLoadCaseManagerDialog::OnListBoxSetSelect(
    wxCommandEvent& event) {
  // gets the set listbox index
  index_set_activated_ = listbox_sets_->GetSelection();
  if (index_set_activated_ == wxNOT_FOUND) {
    return;
  }

  // gets the weathercase set from the set list
  auto iter_sets = std::next(sets_modified_.cbegin(), index_set_activated_);
  const std::list<WeatherLoadCase>& set = *iter_sets;

  // fills the weathercase listbox
  listbox_weathercases_->Clear();
  for (auto iter = set.cbegin(); iter != set.cend(); iter++) {
    const WeatherLoadCase& weathercase = *iter;
    listbox_weathercases_->Append(weathercase.description);
  }
}

void AnalysisWeatherLoadCaseManagerDialog::OnListBoxWeatherCaseDoubleClick(
    wxCommandEvent& event) {
  // gets the weathercase set
  auto iter_sets = std::next(sets_modified_.begin(), index_set_activated_);
  std::list<WeatherLoadCase>& set = *iter_sets;

  // gets the weathercase listbox index
  const int index_weathercases = listbox_weathercases_->GetSelection();
  if (index_weathercases == wxNOT_FOUND) {
    return;
  }

  // gets the weathercase, copies, and converts to different unit style
  auto iter_weathercase = std::next(set.begin(), index_weathercases);
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
    listbox_weathercases_->SetStringSelection(weathercase.description);
  }
}

void AnalysisWeatherLoadCaseManagerDialog::OnOk(wxCommandEvent& event) {
  // overwrites original data with modified data
  *descriptions_ = descriptions_modified_;
  *sets_ = sets_modified_;

  // ends modal by returning ok indicator
  EndModal(wxID_OK);
}

void AnalysisWeatherLoadCaseManagerDialog::OnSpinButtonSetDown(
    wxSpinEvent& event) {
  // exits if nothing is selected
  if (index_set_activated_ == wxNOT_FOUND) {
    return;
  }

  // exits if the activated set is the last set
  const int kCount = listbox_sets_->GetCount() - 1;
  if (index_set_activated_ == kCount) {
    return;
  }

  // switches description list using iterators
  auto iter_selection_desc = std::next(descriptions_modified_.begin(),
                                       index_set_activated_);
  auto iter_position_desc = std::next(iter_selection_desc, 2);
  descriptions_modified_.splice(iter_position_desc, descriptions_modified_,
                                iter_selection_desc);

  // switches set list using iterators
  auto iter_selection_set = std::next(sets_modified_.begin(),
                                      index_set_activated_);
  auto iter_position_set = std::next(iter_selection_set, 2);
  sets_modified_.splice(iter_position_set, sets_modified_, iter_selection_set);

  // updates listbox
  listbox_sets_->Delete(index_set_activated_);
  listbox_sets_->Insert(*iter_selection_desc, index_set_activated_ + 1);
  listbox_sets_->SetSelection(index_set_activated_ + 1);

  // updates activated index
  index_set_activated_ += 1;
}

void AnalysisWeatherLoadCaseManagerDialog::OnSpinButtonSetUp(
    wxSpinEvent& event) {
  // exits if nothing is selected
  if (index_set_activated_ == wxNOT_FOUND) {
    return;
  }

  // exits if the activated set is the first set
  if (index_set_activated_ == 0) {
    return;
  }

  // switches description list using iterators
  auto iter_selection_desc = std::next(descriptions_modified_.begin(),
                                       index_set_activated_);
  auto iter_position_desc = std::prev(iter_selection_desc, 1);
  descriptions_modified_.splice(iter_position_desc, descriptions_modified_,
                                iter_selection_desc);

  // switches set list using iterators
  auto iter_selection_set = std::next(sets_modified_.begin(),
                                      index_set_activated_);
  auto iter_position_set = std::prev(iter_selection_set, 1);
  sets_modified_.splice(iter_position_set, sets_modified_, iter_selection_set);

  // updates listbox
  listbox_sets_->Delete(index_set_activated_);
  listbox_sets_->Insert(*iter_selection_desc, index_set_activated_ - 1);
  listbox_sets_->SetSelection(index_set_activated_ - 1);

  // updates activated index
  index_set_activated_ -= 1;
}

void AnalysisWeatherLoadCaseManagerDialog::OnSpinButtonWeathercaseDown(
    wxSpinEvent& event) {
  // gets weathercase index
  const int index = listbox_weathercases_->GetSelection();

  // exits if nothing is selected
  if (index == wxNOT_FOUND) {
    return;
  }

  // exits if the selected index is the first weathercase
  const int kCount = listbox_sets_->GetCount() - 1;
  if (index == kCount) {
    return;
  }

  // gets the weathercase set
  auto iter_sets = std::next(sets_modified_.begin(), index_set_activated_);
  std::list<WeatherLoadCase>& set = *iter_sets;

  // switches weathercases in list using iterators
  auto iter_selection = std::next(set.begin(), index);
  auto iter_position = std::next(iter_selection, 2);
  set.splice(iter_position, set, iter_selection);

  // updates listbox
  const WeatherLoadCase& weathercase = *iter_selection;
  listbox_weathercases_->Delete(index);
  listbox_weathercases_->Insert(weathercase.description, index + 1);
  listbox_weathercases_->SetSelection(index + 1);
}

void AnalysisWeatherLoadCaseManagerDialog::OnSpinButtonWeathercaseUp(
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
  auto iter_sets = std::next(sets_modified_.begin(), index_set_activated_);
  std::list<WeatherLoadCase>& set = *iter_sets;

  // switches weathercases in list using iterators
  auto iter_selection = std::next(set.begin(), index);
  auto iter_position = std::prev(iter_selection, 1);
  set.splice(iter_position, set, iter_selection);

  // updates listbox
  const WeatherLoadCase& weathercase = *iter_selection;
  listbox_weathercases_->Delete(index);
  listbox_weathercases_->Insert(weathercase.description, index - 1);
  listbox_weathercases_->SetSelection(index - 1);
}
