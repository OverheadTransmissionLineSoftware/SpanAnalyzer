// This is free and unencumbered software released into the public domain.
// For more information, please refer to <http://unlicense.org/>

#ifndef OTLS_SPANANALYZER_CATENARYTABLEPANEL_H_
#define OTLS_SPANANALYZER_CATENARYTABLEPANEL_H_

#include "wx/listctrl.h"
#include "wx/wx.h"

#include "sag_tension_analysis_results.h"

/// \par OVERVIEW
///
/// This panel contains the Catenary panel and all controls contained within.
///
/// \par SAG-TENSION RESULTS
///
/// The sag-tension results are calculated independently of this panel. This
/// panel only provides an interface for viewing the sag-tension results.
///
/// \par CATENARY RESULTS
///
/// The catenary results are calculated within this panel. The catenary curve
/// (H/w) is defined by the sag-tension results, but the spacing is defined by
/// the controls on the panel.
class CatenaryTablePanel : public wxPanel {
 public:
  /// \brief Constructor.
  /// \param[in] parent
  ///   The parent window.
  /// \param[in] results
  ///   The sag-tension analysis results.
  CatenaryTablePanel(wxWindow* parent,
                     const SagTensionAnalysisResultSet* results);

  /// \brief Destructor.
  ~CatenaryTablePanel();

  /// \brief Updates the pane.
  /// \param[in] hint
  ///   The hint describing what kind of update is needed.
  /// This function is called as part of the view update process.
  void UpdateView(wxObject* hint = nullptr);

 private:
  /// \brief Copies table information to the clipboard.
  void CopyTableToClipboard();

  /// \brief Fills the listctrl with blank entries.
  /// This is an optimization so that as filter controls, span selection, etc.
  /// causes the data to change, the listctrl items still remain. Helps with
  /// maintaining previously selected rows.
  void FillBlank();

  /// \brief Fills the listctrl with the analysis results.
  void FillData();

  /// \brief Handles the condition choice event.
  /// \param[in] event
  ///   The event.
  void OnChoiceCondition(wxCommandEvent& event);

  /// \brief Handles the condition choice event.
  /// \param[in] event
  ///   The event.
  void OnChoiceSide(wxCommandEvent& event);

  /// \brief Handles the column right click event.
  /// \param[in] event
  ///   The event.
  void OnColumnRightClick(wxListEvent& event);

  /// \brief Handles the context menu select event.
  /// \param[in] event
  ///   The event.
  void OnContextMenuSelect(wxCommandEvent& event);

  /// \brief Handles the column right click event.
  /// \param[in] event
  ///   The event.
  void OnItemRightClick(wxListEvent& event);

  /// \var listctrl_
  ///   The listctrl that contains calculated results.
  wxListCtrl* listctrl_;

  /// \var results_
  ///   The sag-tension analysis results.
  const SagTensionAnalysisResultSet* results_;

  DECLARE_EVENT_TABLE()
};

#endif  // OTLS_SPANANALYZER_CATENARYTABLEPANEL_H_