// This is free and unencumbered software released into the public domain.
// For more information, please refer to <http://unlicense.org/>

#ifndef OTLS_SPANANALYZER_RESULTSPANE_H_
#define OTLS_SPANANALYZER_RESULTSPANE_H_

#include "wx/docview.h"
#include "wx/wx.h"

#include "report_table.h"

/// \par OVERVIEW
///
/// This class is a wxAUI pane that holds all of the results display/controls.
class ResultsPane : public wxPanel {
 public:
  /// \brief These are the types of reports that can be displayed.
  enum class ReportType {
    kCatenary,
    kCatenaryEndpoints,
    kSagTension,
    kTensionDistribution,
  };

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

  /// \brief Handles the report choice event.
  /// \param[in] event
  ///   The event.
  void OnChoiceReport(wxCommandEvent& event);

  /// \brief Handles the weathercase set choice event.
  /// \param[in] event
  ///   The event.
  void OnChoiceWeathercaseGroup(wxCommandEvent& event);

  /// \brief Updates the report data.
  void UpdateReportData();

  /// \brief Updates the report data with catenary results.
  void UpdateReportDataCatenary();

  /// \brief Updates the report data with catenary endpoint results.
  void UpdateReportDataCatenaryEndpoints();

  /// \brief Updates the report data with sag-tension results.
  void UpdateReportDataSagTension();

  /// \brief Updates the report data with tension distribution results.
  void UpdateReportDataTensionDistribution();

  /// \brief Updates the weathercase group choice control.
  void UpdateWeathercaseGroupChoice();

  /// \brief Updates the selected weathercases.
  void UpdateWeathercaseGroupSelected();

  /// \var data_
  ///   The report data.
  ReportData data_;

  /// \var listctrl_
  ///   The report table used to display the results.
  ReportTable* table_;

  /// \var type_report_
  ///   The report type to display.
  ReportType type_report_;

  /// \var view_
  ///   The view.
  wxView* view_;

  DECLARE_EVENT_TABLE()
};

# endif //  OTLS_SPANANALYZER_RESULTSPANE_H_
