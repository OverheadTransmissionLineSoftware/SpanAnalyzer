// This is free and unencumbered software released into the public domain.
// For more information, please refer to <http://unlicense.org/>

#ifndef OTLS_SPANANALYZER_RESULTSPANE_H_
#define OTLS_SPANANALYZER_RESULTSPANE_H_

#include "models/transmissionline/weather_load_case.h"
#include "wx/docview.h"
#include "wx/wx.h"

#include "catenary_table_panel.h"
#include "sag_tension_analysis_results.h"
#include "sag_tension_table_panel.h"
#include "span_panel.h"

/// \par OVERVIEW
///
/// This class is a wxAUI pane that holds all of the results display/controls.
class ResultsPane : public wxPanel {
 public:
  /// \brief Constructor.
  /// \param[in] parent
  ///   The parent window.
  /// \param[in] view
  ///   The view.
  ResultsPane(wxWindow* parent, wxView* view);

  /// \brief Destructor.
  ~ResultsPane();

  /// \brief Updates the pane.
  /// \param[in] hint
  ///   The hint describing what kind of update is needed.
  /// This function is called as part of the view update process.
  void Update(wxObject* hint = nullptr);

  /// \brief Gets the sag-tension analysis results.
  /// \return The sag-tension analysis results.
  const SagTensionAnalysisResultSet& results() const;

 private:
  /// \brief Handles the weathercase set choice event.
  void OnChoiceWeathercaseSet(wxCommandEvent& event);

  /// \brief Updates the analysis weathercase set choice control.
  void UpdateAnalysisWeathercaseSetChoice();

  /// \brief Updates the sag-tension results.
  void UpdateSagTensionResults();

  /// \brief Updates the selected weathercases.
  void UpdateSelectedWeathercases();

  /// \var panel_span_
  ///   The span panel, which is a notebook page.
  SpanPanel* panel_span_;

  /// \var panel_table_catenary_
  ///   The catenary table panel, which is a notebook page.
  CatenaryTablePanel* panel_table_catenary_;

  /// \var panel_table_sagtension_
  ///   The sag-tension table panel, which is a notebook page.
  SagTensionTablePanel* panel_table_sagtension_;

  /// \var results_
  ///   The sag-tension analysis results.
  SagTensionAnalysisResultSet results_;

  /// \var view_
  ///   The view.
  wxView* view_;

  /// \var weathercases_selected_
  ///   The selected weathercases to generate results for.
  const std::list<WeatherLoadCase>* weathercases_selected_;

  DECLARE_EVENT_TABLE()
};

# endif //  OTLS_SPANANALYZER_RESULTSPANE_H_
