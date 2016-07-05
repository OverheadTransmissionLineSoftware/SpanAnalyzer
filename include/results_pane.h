// This is free and unencumbered software released into the public domain.
// For more information, please refer to <http://unlicense.org/>

#ifndef OTLS_SPANANALYZER_RESULTSPANE_H_
#define OTLS_SPANANALYZER_RESULTSPANE_H_

#include "wx/docview.h"
#include "wx/wx.h"

#include "catenary_table_panel.h"
#include "sag_tension_table_panel.h"

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

 private:
  /// \brief Handles the condition choice event.
  /// \param[in] event
  ///   The event.
  void OnChoiceCondition(wxCommandEvent& event);

  /// \brief Handles the weathercase set choice event.
  void OnChoiceWeathercaseSet(wxCommandEvent& event);

  /// \brief Updates the analysis weathercase set choice control.
  void UpdateAnalysisWeathercaseSetChoice();

  /// \brief Updates the selected weathercases.
  void UpdateSelectedWeathercases();

  /// \var panel_table_catenary_
  ///   The catenary table panel, which is a notebook page.
  CatenaryTablePanel* panel_table_catenary_;

  /// \var panel_table_sagtension_
  ///   The sag-tension table panel, which is a notebook page.
  SagTensionTablePanel* panel_table_sagtension_;

  /// \var view_
  ///   The view.
  wxView* view_;

  DECLARE_EVENT_TABLE()
};

# endif //  OTLS_SPANANALYZER_RESULTSPANE_H_
