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

    if (span.linecable.constraint.case_weather->description == name) {
      return true;
    }

    if (span.linecable.weathercase_stretch_creep->description == name) {
      return true;
    }

    if (span.linecable.weathercase_stretch_load->description == name) {
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
  weathercases_->push_back(weathercase_new);
  weathercases_modified_.push_back(weathercase_new);

  // updates weathercase listbox
  listbox_weathercases_->Append(weathercase.description);
}

void WeatherLoadCaseManagerDialog::OnButtonDelete(wxCommandEvent& event) {
  // gets weathercase listbox index
  const int index = listbox_weathercases_->GetSelection();

  // gets the weathercase
  auto iter = std::next(weathercases_modified_.begin(), index);
  WeatherLoadCase* weathercase = *iter;

  // checks if the weathercase is referenced by the document
  bool is_referenced = IsReferencedByDocument(weathercase->description);
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
  listbox_weathercases_->Delete(index);
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
  // gets the weathercase listbox index
  const int index_weathercases = listbox_weathercases_->GetSelection();
  if (index_weathercases == wxNOT_FOUND) {
    return;
  }

  // gets the weathercase, copies, and converts to different unit style
  auto iter_weathercase = std::next(weathercases_modified_.begin(),
                                    index_weathercases);
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
    is_unique = IsUniqueName(weathercase->description, index_weathercases);
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
  listbox_weathercases_->SetString(index_weathercases,
                                   weathercase->description);
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
  // gets weathercase index
  const int index = listbox_weathercases_->GetSelection();

  // exits if nothing is selected
  if (index == wxNOT_FOUND) {
    return;
  }

  // exits if the selected index is the last weathercase
  const int kCount = listbox_weathercases_->GetCount() - 1;
  if (index == kCount) {
    return;
  }

  wxBusyCursor cursor;

  // switches weathercases in list using iterators
  auto iter_selection = std::next(weathercases_modified_.begin(), index);
  auto iter_position = std::next(iter_selection, 2);
  weathercases_modified_.splice(iter_position, weathercases_modified_,
                                iter_selection);

  // updates listbox
  const WeatherLoadCase* weathercase = *iter_selection;
  listbox_weathercases_->Delete(index);
  listbox_weathercases_->Insert(weathercase->description, index + 1);
  listbox_weathercases_->SetSelection(index + 1);
}

void WeatherLoadCaseManagerDialog::OnSpinButtonUp(wxSpinEvent& event) {
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

  wxBusyCursor cursor;

  // switches weathercases in list using iterators
  auto iter_selection = std::next(weathercases_modified_.begin(), index);
  auto iter_position = std::prev(iter_selection, 1);
  weathercases_modified_.splice(iter_position, weathercases_modified_,
                                iter_selection);

  // updates listbox
  const WeatherLoadCase* weathercase = *iter_selection;
  listbox_weathercases_->Delete(index);
  listbox_weathercases_->Insert(weathercase->description, index - 1);
  listbox_weathercases_->SetSelection(index - 1);
}
