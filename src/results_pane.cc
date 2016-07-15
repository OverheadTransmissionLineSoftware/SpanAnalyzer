// This is free and unencumbered software released into the public domain.
// For more information, please refer to <http://unlicense.org/>

#include "results_pane.h"

#include "wx/notebook.h"
#include "wx/xrc/xmlres.h"

#include "span_analyzer_app.h"
#include "span_analyzer_view.h"

BEGIN_EVENT_TABLE(ResultsPane, wxPanel)
  EVT_CHOICE(XRCID("choice_condition"), ResultsPane::OnChoiceCondition)
  EVT_CHOICE(XRCID("choice_weathercase_set"), ResultsPane::OnChoiceWeathercaseSet)
END_EVENT_TABLE()

ResultsPane::ResultsPane(wxWindow* parent, wxView* view) {
  // loads dialog from virtual xrc file system
  wxXmlResource::Get()->LoadPanel(this, parent, "results_pane");

  // saves view reference
  view_ = view;

  // initializes weathercase set choice
  UpdateAnalysisWeathercaseSetChoice();

  // intializes condition choice
  wxChoice* choice = XRCCTRL(*this, "choice_condition", wxChoice);
  choice->Append("Initial");
  choice->Append("Load");
  choice->SetSelection(0);

  // creates child windows for displaying results and adds to notebook
  SpanAnalyzerView* view_app = (SpanAnalyzerView*)view_;
  wxNotebook* notebook = XRCCTRL(*this, "notebook_results", wxNotebook);

  panel_table_sagtension_ = new SagTensionTablePanel(notebook,
                                                     &view_app->results());
  notebook->AddPage(panel_table_sagtension_, "Sag-Tension Table");

  panel_table_catenary_ = new CatenaryTablePanel(notebook,
                                                 &view_app->results());
  notebook->AddPage(panel_table_catenary_, "Catenary Table");
}

ResultsPane::~ResultsPane() {
}

void ResultsPane::Update(wxObject* hint) {
  // interprets hint
  ViewUpdateHint* hint_update = (ViewUpdateHint*)hint;
  if (hint_update == nullptr) {
    // do nothing, this is only passed when pane is created
  } else if (hint_update->type() ==
       ViewUpdateHint::HintType::kModelAnalysisWeathercaseEdit) {
    UpdateAnalysisWeathercaseSetChoice();

    // updates managed windows
    panel_table_sagtension_->Initialize();
    panel_table_sagtension_->FillResults();

    panel_table_catenary_->Initialize();
    panel_table_catenary_->FillResults();
  } else if (hint_update->type() ==
       ViewUpdateHint::HintType::kModelPreferencesEdit) {
    // updates managed windows
    panel_table_sagtension_->FillResults();
    panel_table_catenary_->FillResults();
  } else if (hint_update->type() ==
       ViewUpdateHint::HintType::kModelSpansEdit) {
    // updates managed windows
    panel_table_sagtension_->FillResults();
    panel_table_catenary_->FillResults();
  } else if (hint_update->type() ==
       ViewUpdateHint::HintType::kModelWeathercaseEdit) {
    // updates managed windows
    panel_table_sagtension_->Initialize();
    panel_table_sagtension_->FillResults();

    panel_table_catenary_->Initialize();
    panel_table_catenary_->FillResults();
  } else if (hint_update->type() ==
      ViewUpdateHint::HintType::kViewConditionChange) {
    // updates manaaged windows
    panel_table_sagtension_->FillResults();
    panel_table_catenary_->FillResults();
  } else if (hint_update->type() ==
      ViewUpdateHint::HintType::kViewWeathercasesSetChange) {
    // updates managed windows
    panel_table_sagtension_->Initialize();
    panel_table_sagtension_->FillResults();

    panel_table_catenary_->Initialize();
    panel_table_catenary_->FillResults();
  }
}

void ResultsPane::OnChoiceCondition(wxCommandEvent& event) {
  // gets choice selection and updates view cache
  wxChoice* choice = XRCCTRL(*this, "choice_condition", wxChoice);
  wxString str = choice->GetStringSelection();

  SpanAnalyzerView* view = (SpanAnalyzerView*)view_;
  if (str == "Initial") {
    view->set_condition(CableConditionType::kInitial);
  } else if (str == "Load") {
    view->set_condition(CableConditionType::kLoad);
  } else {
    return;
  }

  // updates views
  ViewUpdateHint hint;
  hint.set_type(ViewUpdateHint::HintType::kViewConditionChange);
  view_->GetDocument()->UpdateAllViews(nullptr, &hint);
}

void ResultsPane::OnChoiceWeathercaseSet(wxCommandEvent& event) {
  // gets weathercase set from application data
  wxChoice* choice = XRCCTRL(*this, "choice_weathercase_set", wxChoice);
  wxString str_selection = choice->GetStringSelection();

  // updates the selected/cached weathercases
  UpdateSelectedWeathercases();

  // updates views
  ViewUpdateHint hint;
  hint.set_type(ViewUpdateHint::HintType::kViewWeathercasesSetChange);
  view_->GetDocument()->UpdateAllViews(nullptr, &hint);
}

void ResultsPane::UpdateAnalysisWeathercaseSetChoice() {
  // gets choice control
  wxChoice* choice = XRCCTRL(*this, "choice_weathercase_set", wxChoice);

  // saves current choice string
  std::string str_choice = choice->GetStringSelection();

  // clears choice control
  choice->Clear();

  // appends weathercase groups stored in the application data
  const std::list<WeatherLoadCaseGroup>& groups =
      wxGetApp().data()->groups_weathercase;
  for (auto iter = groups.cbegin(); iter != groups.cend(); iter ++) {
    const WeatherLoadCaseGroup& group = *iter;
    choice->Append(group.name);
  }

  // attempts to find the old weathercase set
  choice->SetSelection(choice->FindString(str_choice));

  // updates the selected/cached weathercases
  UpdateSelectedWeathercases();
}

void ResultsPane::UpdateSelectedWeathercases() {
  // initializes weathercases cached in view
  SpanAnalyzerView* view = (SpanAnalyzerView*)view_;
  view->set_weathercases(nullptr);

  // searches the choice control to see if a weathercase group is selected
  wxChoice* choice = XRCCTRL(*this, "choice_weathercase_set", wxChoice);
  std::string str_selection = choice->GetStringSelection();

  // gets weathercases from app data
  const std::list<WeatherLoadCaseGroup>& groups_weathercase =
       wxGetApp().data()->groups_weathercase;
  for (auto iter = groups_weathercase.cbegin();
       iter != groups_weathercase.cend(); iter++) {
    const WeatherLoadCaseGroup& group = *iter;
    if (group.name == str_selection) {
      view->set_weathercases(&group.weathercases);
      break;
    }
  }
}
