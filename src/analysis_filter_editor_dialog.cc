// This is free and unencumbered software released into the public domain.
// For more information, please refer to <http://unlicense.org/>

#include "spananalyzer/analysis_filter_editor_dialog.h"

#include "wx/xrc/xmlres.h"

BEGIN_EVENT_TABLE(AnalysisFilterEditorDialog, wxDialog)
  EVT_BUTTON(wxID_CANCEL, AnalysisFilterEditorDialog::OnCancel)
  EVT_BUTTON(wxID_OK, AnalysisFilterEditorDialog::OnOk)
  EVT_CLOSE(AnalysisFilterEditorDialog::OnClose)
END_EVENT_TABLE()

AnalysisFilterEditorDialog::AnalysisFilterEditorDialog(
    wxWindow* parent,
    const std::list<WeatherLoadCase*>* weathercases,
    AnalysisFilter* filter) {
  // loads dialog from virtual xrc file system
  wxXmlResource::Get()->LoadDialog(this, parent,
                                   "analysis_filter_editor_dialog");

  // saves unmodified reference, and copies
  filter_ = filter;
  filter_modified_ = *filter;

  // captures weathercases
  weathercases_ = weathercases;

  // populates the weathercase choice control
  wxChoice* choice = nullptr;
  choice = XRCCTRL(*this, "choice_weathercase", wxChoice);
  for (auto iter = weathercases_->cbegin(); iter != weathercases_->cend();
       iter++) {
    const WeatherLoadCase* weathercase = *iter;
    choice->Append(weathercase->description);
  }

  // populates the condition choice control
  choice = XRCCTRL(*this, "choice_condition", wxChoice);
  choice->Append("Initial");
  choice->Append("Load");
  choice->Append("Creep");

  // transfers data to gui controls
  TransferCustomDataToWindow();

  // fits the dialog around the sizers
  this->Fit();
}

AnalysisFilterEditorDialog::~AnalysisFilterEditorDialog() {
}

void AnalysisFilterEditorDialog::OnCancel(wxCommandEvent &event) {
  EndModal(wxID_CANCEL);
}

void AnalysisFilterEditorDialog::OnClose(wxCloseEvent &event) {
  EndModal(wxID_CLOSE);
}

void AnalysisFilterEditorDialog::OnOk(wxCommandEvent &event) {
  // nothing to validate
  wxBusyCursor cursor;

  // transfers data from dialog controls to modified object
  TransferCustomDataFromWindow();

  // transfers modified object to reference object
  *filter_ = filter_modified_;

  EndModal(wxID_OK);
}

void AnalysisFilterEditorDialog::TransferCustomDataFromWindow() {
  wxChoice* choice = nullptr;
  wxString str;
  int index = -9999;

  // transfers weathercase
  choice = XRCCTRL(*this, "choice_weathercase", wxChoice);
  index = choice->GetSelection();
  if (index == wxNOT_FOUND) {
    filter_modified_.weathercase = nullptr;
  } else {
    auto iter = std::next(weathercases_->cbegin(), index);
    const WeatherLoadCase* weathercase = *iter;
    filter_modified_.weathercase = weathercase;
  }

  // transfers condition
  choice = XRCCTRL(*this, "choice_condition", wxChoice);
  index = choice->GetSelection();
  if (index == wxNOT_FOUND) {
    filter_modified_.condition = CableConditionType::kNull;
  } else {
    str = choice->GetString(index);

    if (str == "Initial") {
      filter_modified_.condition = CableConditionType::kInitial;
    } else if (str == "Load") {
      filter_modified_.condition = CableConditionType::kLoad;
    } else if (str == "Creep") {
      filter_modified_.condition = CableConditionType::kCreep;
    }
  }
}

void AnalysisFilterEditorDialog::TransferCustomDataToWindow() {
  wxChoice* choice = nullptr;
  int index = -9999;

  // transfers weathercase
  if (filter_modified_.weathercase != nullptr) {
    choice = XRCCTRL(*this, "choice_weathercase", wxChoice);
    index = choice->FindString(filter_modified_.weathercase->description,
                               true);
    choice->SetSelection(index);
  }

  // transfers condition
  choice = XRCCTRL(*this, "choice_condition", wxChoice);
  CableConditionType type_condition = filter_modified_.condition;
  if (type_condition == CableConditionType::kInitial) {
    choice->SetSelection(0);
  } else if (type_condition == CableConditionType::kLoad) {
    choice->SetSelection(1);
  } else if (type_condition == CableConditionType::kCreep) {
    choice->SetSelection(2);
  } else {
    choice->SetSelection(wxNOT_FOUND);
  }
}
