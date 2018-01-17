// This is free and unencumbered software released into the public domain.
// For more information, please refer to <http://unlicense.org/>

#include "weather_load_case_manager_dialog.h"

#include "appcommon/editors/weather_load_case_editor_dialog.h"
#include "appcommon/units/weather_load_case_unit_converter.h"
#include "wx/spinbutt.h"
#include "wx/xrc/xmlres.h"

#include "span_analyzer_app.h"
#include "span_analyzer_doc.h"

BEGIN_EVENT_TABLE(WeatherLoadCaseManagerDialog, wxDialog)
  EVT_BUTTON(XRCID("button_weathercases_add"), WeatherLoadCaseManagerDialog::OnButtonAdd)
  EVT_BUTTON(XRCID("button_weathercases_delete"), WeatherLoadCaseManagerDialog::OnButtonDelete)
  EVT_BUTTON(wxID_CANCEL, WeatherLoadCaseManagerDialog::OnCancel)
  EVT_BUTTON(wxID_OK, WeatherLoadCaseManagerDialog::OnOk)
  EVT_CLOSE(WeatherLoadCaseManagerDialog::OnClose)
  EVT_LISTBOX(XRCID("listbox_weathercases"), WeatherLoadCaseManagerDialog::OnListBoxSelect)
  EVT_LISTBOX_DCLICK(XRCID("listbox_weathercases"), WeatherLoadCaseManagerDialog::OnListBoxDoubleClick)
  EVT_SPIN_DOWN(XRCID("spinbutton_weathercases"), WeatherLoadCaseManagerDialog::OnSpinButtonDown)
  EVT_SPIN_UP(XRCID("spinbutton_weathercases"), WeatherLoadCaseManagerDialog::OnSpinButtonUp)
END_EVENT_TABLE()

WeatherLoadCaseManagerDialog::WeatherLoadCaseManagerDialog(
    wxWindow* parent,
    const units::UnitSystem& units,
    std::list<WeatherLoadCase*>* weathercases) {
  // loads dialog from virtual xrc file system
  wxXmlResource::Get()->LoadDialog(this, parent,
                                   "weather_load_case_manager_dialog");

  // gets listbox references
  listbox_weathercases_ = XRCCTRL(*this, "listbox_weathercases", wxListBox);

  // saves references to data
  weathercases_ = weathercases;
  weathercases_original_ = *weathercases;
  units_ = &units;

  // makes copies so user can modify
  weathercases_modified_ = *weathercases_;

  // fills the listbox with the set descriptions
  for (auto iter = weathercases_modified_.cbegin();
       iter != weathercases_modified_.cend(); iter++) {
    const WeatherLoadCase* weathercase = *iter;
    listbox_weathercases_->Append(weathercase->description);
  }

  // initializes index
  index_selected_ = wxNOT_FOUND;

  // fits the dialog around the sizers
  this->Fit();
}

WeatherLoadCaseManagerDialog::~WeatherLoadCaseManagerDialog() {
}

void WeatherLoadCaseManagerDialog::DeleteExtraWeathercases(
    const std::list<WeatherLoadCase*>* list_keep,
    std::list<WeatherLoadCase*>* list_master) {
  // iterates over master list
  for (auto iter = list_master->begin(); iter != list_master->end();) {
    WeatherLoadCase* weathercase = *iter;

    // searches keep list for a match
    std::list<WeatherLoadCase*>::const_iterator iter_keep = list_keep->cbegin();
    while (iter_keep != list_keep->cend()) {
      WeatherLoadCase* weathercase_keep = *iter_keep;
      if (weathercase == weathercase_keep) {
        break;
      } else {
        iter_keep++;
      }
    }

    // increments master list iterator
    if (iter_keep == list_keep->cend()) {
      // the cablefile is deallocated
      iter = list_master->erase(iter);
      delete weathercase;
    } else {
      iter++;
    }
  }
}

bool WeatherLoadCaseManagerDialog::IsReferencedByAnalysisFilters(
    const std::string& name) const {
  // gets document
  const SpanAnalyzerData* data = wxGetApp().data();

  // scans all analysis filters to see if weathercase is referenced
  for (auto iter = data->groups_filters.cbegin();
       iter != data->groups_filters.cend(); iter++) {
    const AnalysisFilterGroup& group = *iter;

    for (auto it = group.filters.cbegin(); it != group.filters.cend(); it++) {
      const AnalysisFilter& filter = *it;
      if (filter.weathercase->description == name) {
        return true;
      }
    }
  }

  return false;
}

bool WeatherLoadCaseManagerDialog::IsReferencedByDocument(
    const std::string& name) const {
  // gets document
  const SpanAnalyzerDoc* doc = wxGetApp().GetDocument();
  if (doc == nullptr) {
    return false;
  }

  // scans all spans to see if weathercase is referenced
  for (auto iter = doc->spans().cbegin(); iter != doc->spans().cend(); iter++) {
    const Span& span = *iter;

    if (span.linecable.constraint().case_weather->description == name) {
      return true;
    }

    if (span.linecable.weathercase_stretch_creep()->description == name) {
      return true;
    }

    if (span.linecable.weathercase_stretch_load()->description == name) {
      return true;
    }
  }

  return false;
}

bool WeatherLoadCaseManagerDialog::IsUniqueName(const std::string& name,
                                                const int& index_ignore) const {
  // scans all weathercases to see if the name is a duplicate
  for (auto iter = weathercases_modified_.cbegin();
       iter != weathercases_modified_.cend(); iter++) {
    const WeatherLoadCase* weathercase = *iter;

    const int index = std::distance(weathercases_modified_.cbegin(), iter);
    if (index == index_ignore) {
      continue;
    } else {
      if (weathercase->description == name) {
        return false;
      }
    }
  }

  return true;
}

void WeatherLoadCaseManagerDialog::OnButtonAdd(wxCommandEvent& event) {
  // creates a weathercase
  WeatherLoadCase weathercase;

  // lets user edit weathercase
  // ensures that the weathercase name is unique
  bool is_unique = false;
  while (is_unique == false) {
    // creates a dialog to let the user edit
    WeatherLoadCaseEditorDialog dialog(this, &weathercase, *units_);
    if (dialog.ShowModal() != wxID_OK) {
      return;
    }

    // checks if the weathercase name is unique
    is_unique = IsUniqueName(weathercase.description);
    if (is_unique == false) {
      std::string message = "Weathercase name is a duplicate. Please provide "
                            "another name.";
      wxMessageBox(message);
    }
  }

  wxBusyCursor cursor;

  // converts weathercase to consistent unit style
  WeatherLoadCaseUnitConverter::ConvertUnitStyle(
      *units_,
      units::UnitStyle::kDifferent,
      units::UnitStyle::kConsistent,
      weathercase);

  // adds weathercase to set
  WeatherLoadCase* weathercase_new = new WeatherLoadCase(weathercase);

  if (index_selected_ == wxNOT_FOUND) {
    // nothing is selected so the weathercase is appended
    weathercases_->push_back(weathercase_new);
    weathercases_modified_.push_back(weathercase_new);
  } else {
    // an index is selected so the weathercase is inserted after selection
    auto iter = std::next(weathercases_->begin(), index_selected_ + 1);
    weathercases_->insert(iter, weathercase_new);

    iter = std::next(weathercases_modified_.begin(), index_selected_ + 1);
    weathercases_modified_.insert(iter, weathercase_new);
  }

  // updates weathercase listbox
  if (index_selected_ == wxNOT_FOUND) {
    listbox_weathercases_->Append(weathercase.description);
  } else {
    index_selected_++;
    listbox_weathercases_->Insert(weathercase.description, index_selected_);
    listbox_weathercases_->SetSelection(index_selected_);
  }
}

void WeatherLoadCaseManagerDialog::OnButtonDelete(wxCommandEvent& event) {
  // checks index
  if (index_selected_ == wxNOT_FOUND) {
    return;
  }

  // gets the weathercase
  auto iter = std::next(weathercases_modified_.begin(), index_selected_);
  WeatherLoadCase* weathercase = *iter;

  // checks if weathercase is referenced by analysis filters
  bool is_referenced = IsReferencedByAnalysisFilters(weathercase->description);
  if (is_referenced == true) {
    std::string message = weathercase->description + "  --  "
                          "Weathercase is currently referenced by analysis "
                          "filters.";
    wxMessageBox(message);

    return;
  }

  // checks if the weathercase is referenced by the document
  is_referenced = IsReferencedByDocument(weathercase->description);
  if (is_referenced == true) {
    std::string message = weathercase->description + "  --  "
                          "Weathercase is currently referenced by the open "
                          "document.";
    wxMessageBox(message);

    return;
  }

  wxBusyCursor cursor;

  // removes from modified weathercase set only
  weathercases_modified_.erase(iter);

  // updates weathercase listbox
  listbox_weathercases_->Delete(index_selected_);

  // reselects item in listbox
  const int kSize = listbox_weathercases_->GetCount();
  if (kSize == 0) {
    // no items left in listbox
    index_selected_ = wxNOT_FOUND;
  } else if (index_selected_ < kSize) {
    // do nothing
  } else {
    // last item in the listbox was deleted, decrements index
    index_selected_--;
  }

  listbox_weathercases_->SetSelection(index_selected_);
}

void WeatherLoadCaseManagerDialog::OnCancel(wxCommandEvent& event) {
  // deletes any extra weathercases
  DeleteExtraWeathercases(&weathercases_original_, weathercases_);

  EndModal(wxID_CANCEL);
}

void WeatherLoadCaseManagerDialog::OnClose(wxCloseEvent& event) {
  // deletes any extra weathercases
  DeleteExtraWeathercases(&weathercases_original_, weathercases_);

  EndModal(wxID_CLOSE);
}

void WeatherLoadCaseManagerDialog::OnListBoxDoubleClick(wxCommandEvent& event) {
  // checks index
  if (index_selected_ == wxNOT_FOUND) {
    return;
  }

  // gets the weathercase, copies, and converts to different unit style
  auto iter_weathercase = std::next(weathercases_modified_.begin(),
                                    index_selected_);
  WeatherLoadCase* weathercase = *iter_weathercase;
  WeatherLoadCaseUnitConverter::ConvertUnitStyle(*units_,
                                                 units::UnitStyle::kConsistent,
                                                 units::UnitStyle::kDifferent,
                                                 *weathercase);

  // lets user edit weathercase
  // ensures that the weathercase name is unique
  bool is_unique = false;
  while (is_unique == false) {
    // shows an editor
    WeatherLoadCaseEditorDialog dialog(this, weathercase, *units_);
    if (dialog.ShowModal() != wxID_OK) {
      return;
    }

    // checks if the weathercase name is unique
    is_unique = IsUniqueName(weathercase->description, index_selected_);
    if (is_unique == false) {
      std::string message = "Weathercase name is a duplicate. Please provide "
                            "another name.";
      wxMessageBox(message);
    }
  }

  wxBusyCursor cursor;

  // converts weathercase to consistent unit style
  WeatherLoadCaseUnitConverter::ConvertUnitStyle(
      *units_,
      units::UnitStyle::kDifferent,
      units::UnitStyle::kConsistent,
      *weathercase);

  // saves weathercase to modified cases
  *iter_weathercase = weathercase;

  // updates listbox of description change
  listbox_weathercases_->SetString(index_selected_,
                                   weathercase->description);
}

void WeatherLoadCaseManagerDialog::OnListBoxSelect(wxCommandEvent& event) {
  index_selected_ = listbox_weathercases_->GetSelection();
}

void WeatherLoadCaseManagerDialog::OnOk(wxCommandEvent& event) {
  wxBusyCursor cursor;

  // deletes any extra weathercases
  DeleteExtraWeathercases(&weathercases_modified_, weathercases_);

  // copies modified cable files
  *weathercases_ = weathercases_modified_;

  // ends modal by returning ok indicator
  EndModal(wxID_OK);
}

void WeatherLoadCaseManagerDialog::OnSpinButtonDown(wxSpinEvent& event) {
  // checks index
  if (index_selected_ == wxNOT_FOUND) {
    return;
  }

  // exits if the selected index is the last weathercase
  const int kCount = listbox_weathercases_->GetCount() - 1;
  if (index_selected_ == kCount) {
    return;
  }

  wxBusyCursor cursor;

  // switches weathercases in list using iterators
  auto iter_selection = std::next(weathercases_modified_.begin(),
                                  index_selected_);
  auto iter_position = std::next(iter_selection, 2);
  weathercases_modified_.splice(iter_position, weathercases_modified_,
                                iter_selection);

  // updates listbox
  const WeatherLoadCase* weathercase = *iter_selection;
  listbox_weathercases_->Delete(index_selected_);
  listbox_weathercases_->Insert(weathercase->description, index_selected_ + 1);
  index_selected_++;
  listbox_weathercases_->SetSelection(index_selected_);
}

void WeatherLoadCaseManagerDialog::OnSpinButtonUp(wxSpinEvent& event) {
  // checks index
  if (index_selected_ == wxNOT_FOUND) {
    return;
  }

  // exits if the selected index is the first weathercase
  if (index_selected_ == 0) {
    return;
  }

  wxBusyCursor cursor;

  // switches weathercases in list using iterators
  auto iter_selection = std::next(weathercases_modified_.begin(),
                                  index_selected_);
  auto iter_position = std::prev(iter_selection, 1);
  weathercases_modified_.splice(iter_position, weathercases_modified_,
                                iter_selection);

  // updates listbox
  const WeatherLoadCase* weathercase = *iter_selection;
  listbox_weathercases_->Delete(index_selected_);
  listbox_weathercases_->Insert(weathercase->description, index_selected_ - 1);
  index_selected_--;
  listbox_weathercases_->SetSelection(index_selected_);
}
