// This is free and unencumbered software released into the public domain.
// For more information, please refer to <http://unlicense.org/>

#include "results_pane.h"

#include "models/sagtension/line_cable_reloader.h"
#include "wx/notebook.h"
#include "wx/xrc/xmlres.h"

#include "edit_pane.h"
#include "span_analyzer_app.h"
#include "span_analyzer_doc.h"
#include "span_analyzer_view.h"

BEGIN_EVENT_TABLE(ResultsPane, wxPanel)
  EVT_CHOICE(XRCID("choice_weathercase_set"), ResultsPane::OnChoiceWeathercaseSet)
END_EVENT_TABLE()

ResultsPane::ResultsPane(wxWindow* parent, wxView* view) {
  // loads dialog from virtual xrc file system
  wxXmlResource::Get()->LoadPanel(this, parent, "results_pane");

  // saves view reference
  view_ = view;

  // initializes weathercase set choice
  UpdateAnalysisWeathercaseSetChoice();

  // creates notebook and adds child notebook pages
  wxNotebook* notebook = XRCCTRL(*this, "notebook_results", wxNotebook);

  panel_table_sagtension_ = new SagTensionTablePanel(notebook, &results_);
  notebook->AddPage(panel_table_sagtension_, "Sag-Tension Table");

  panel_table_catenary_ = new CatenaryTablePanel(notebook, &results_);
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
    // updates class results
    UpdateAnalysisWeathercaseSetChoice();
    UpdateSagTensionResults();

    // updates managed windows
    panel_table_sagtension_->Initialize();
    panel_table_sagtension_->FillResults();

    panel_table_catenary_->Initialize();
    panel_table_catenary_->FillResults();
  } else if (hint_update->type() ==
       ViewUpdateHint::HintType::kModelPreferencesEdit) {
    // updates class results
     UpdateSagTensionResults();

    // updates managed windows
    panel_table_sagtension_->FillResults();
    panel_table_catenary_->FillResults();
  } else if (hint_update->type() ==
       ViewUpdateHint::HintType::kModelSpansEdit) {
    // updates class results
    UpdateSagTensionResults();

    // updates managed windows
    panel_table_sagtension_->FillResults();
    panel_table_catenary_->FillResults();
  } else if (hint_update->type() ==
       ViewUpdateHint::HintType::kModelWeathercaseEdit) {
    // updates class results
    UpdateSagTensionResults();

    // updates managed windows
    panel_table_sagtension_->Initialize();
    panel_table_sagtension_->FillResults();

    panel_table_catenary_->Initialize();
    panel_table_catenary_->FillResults();
  } else if (hint_update->type() ==
      ViewUpdateHint::HintType::kViewWeathercasesSetChange) {
    // updates class results
    UpdateSagTensionResults();

    // updates managed windows
    panel_table_sagtension_->Initialize();
    panel_table_sagtension_->FillResults();

    panel_table_catenary_->Initialize();
    panel_table_catenary_->FillResults();
  }
}

const SagTensionAnalysisResultSet& ResultsPane::results() const {
  return results_;
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

  // appends project weathercase set, and any stored in the application data
  choice->Append("Project");

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

/// \todo add a log message in here for the analysis time
void ResultsPane::UpdateSagTensionResults() {
  std::list <ErrorMessage> errors;
  std::string message;

  // clears cached results
  results_.descriptions_weathercase.clear();
  results_.results_initial.clear();
  results_.results_load.clear();

  // gets activated span
  SpanAnalyzerView* view = (SpanAnalyzerView*)view_;
  results_.span = view->pane_edit()->SpanActivated();
  if (results_.span == nullptr) {
    return;
  }

  // validates span
  if (results_.span->Validate(false, &errors) == false) {
    // logs errors
    for (auto iter = errors.cbegin(); iter != errors.cend(); iter++) {
      const ErrorMessage& error = *iter;
      message = "Span: " + results_.span->name + "  --  "
                + error.description;
      wxLogError(message.c_str());
    }

    wxLogMessage("Span validation errors are present. Aborting analysis.");
    return;
  }

  // checks if weathercase set has been selected
  if (weathercases_selected_ == nullptr) {
    return;
  }

  wxLogMessage("Running sag-tension analysis.");

  // sets up reloader
  LineCableReloader reloader;
  reloader.set_line_cable(&results_.span->linecable);
  reloader.set_length_unloaded_unstretched_adjustment(0);

  // runs analysis for each weathercase
  for (auto iter = weathercases_selected_->cbegin();
       iter != weathercases_selected_->cend(); iter++) {
    const WeatherLoadCase& weathercase = *iter;


    // validates and logs any errors
    errors.clear();
    bool is_valid_weathercase = weathercase.Validate(false, &errors);
    for (auto iter = errors.cbegin(); iter != errors.cend(); iter++) {
      const ErrorMessage& error = *iter;
      message = weathercase.description + " weathercase  --  "
                + error.description;
      wxLogError(message.c_str());
    }

    // caches description
    results_.descriptions_weathercase.push_back(weathercase.description);

    // updates reloader with weathercase
    reloader.set_weathercase_reloaded(&weathercase);

    // if valid, calculates value and adds to cache
    // if invalid, adds invalid result to cache
    Catenary3d catenary;
    SagTensionAnalysisResult result;
    if (is_valid_weathercase == true) {
      // analyzes and caches initial condition results
      // validates reloader and logs any errors
      reloader.set_condition_reloaded(CableConditionType::kInitial);
      errors.clear();
      if (reloader.Validate(false, &errors) == false) {
        message = "Reloader error for " + weathercase.description
                  + " weathercase when determining the Initial condition.";
        wxLogError(message.c_str());

        for (auto iter = errors.cbegin(); iter != errors.cend(); iter++) {
          const ErrorMessage& error = *iter;
          std::string message = error.title + "  --  "
                                + error.description;
          wxLogError(message.c_str());
        }
      } else {
        catenary = reloader.CatenaryReloaded();

        result.tension_horizontal = catenary.tension_horizontal();
        result.tension_horizontal_core = reloader.TensionHorizontalComponent(
            CableElongationModel::ComponentType::kCore);
        result.tension_horizontal_shell = reloader.TensionHorizontalComponent(
            CableElongationModel::ComponentType::kShell);
        result.weight_unit = catenary.weight_unit();

        results_.results_initial.push_back(result);
      }

      // analyzes and caches initial condition results
      // validates reloader and logs any errors
      reloader.set_condition_reloaded(CableConditionType::kLoad);
      errors.clear();
      if (reloader.Validate(false, &errors) == false) {
        message = "Reloader error for " + weathercase.description
                  + "weathercase when determining the Final-Load condition.";
        wxLogError(message.c_str());

        for (auto iter = errors.cbegin(); iter != errors.cend(); iter++) {
          const ErrorMessage& error = *iter;
          std::string message = error.title + "  --  "
                                + error.description;
          wxLogError(message.c_str());
        }
      } else {
        catenary = reloader.CatenaryReloaded();

        result.tension_horizontal = catenary.tension_horizontal();
        result.tension_horizontal_core = reloader.TensionHorizontalComponent(
            CableElongationModel::ComponentType::kCore);
        result.tension_horizontal_shell = reloader.TensionHorizontalComponent(
            CableElongationModel::ComponentType::kShell);
        result.weight_unit = catenary.weight_unit();

        results_.results_load.push_back(result);
      }
    } else {
      result.tension_horizontal = -999999;
      result.tension_horizontal_core = -999999;
      result.tension_horizontal_shell = -999999;
      result.weight_unit = Vector3d();

      results_.results_initial.push_back(result);
      results_.results_load.push_back(result);
    }
  }
}

void ResultsPane::UpdateSelectedWeathercases() {
  weathercases_selected_ = nullptr;

  wxChoice* choice = XRCCTRL(*this, "choice_weathercase_set", wxChoice);
  std::string str_selection = choice->GetStringSelection();
  if (str_selection == "Project") {
    // gets weathercases from document
    const SpanAnalyzerDoc* doc = (SpanAnalyzerDoc*)view_->GetDocument();
    weathercases_selected_ = &doc->weathercases();
  } else {
    // gets weathercases from app data
    const std::list<WeatherLoadCaseGroup>& groups_weathercase =
         wxGetApp().data()->groups_weathercase;
    for (auto iter = groups_weathercase.cbegin();
         iter != groups_weathercase.cend(); iter++) {
      const WeatherLoadCaseGroup& group = *iter;
      if (group.name == str_selection) {
        weathercases_selected_ = &group.weathercases;
        break;
      }
    }
  }
}
