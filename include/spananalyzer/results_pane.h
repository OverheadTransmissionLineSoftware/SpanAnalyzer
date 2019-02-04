// This is free and unencumbered software released into the public domain.
// For more information, please refer to <http://unlicense.org/>

#ifndef SPANANALYZER_RESULTS_PANE_H_
#define SPANANALYZER_RESULTS_PANE_H_

#include <list>

#include "appcommon/widgets/report_table.h"
#include "wx/docview.h"
#include "wx/wx.h"

#include "spananalyzer/sag_tension_analysis_result.h"
#include "spananalyzer/span_analyzer_data.h"

/// \par OVERVIEW
///
/// This class is a wxAUI pane that holds all of the results display/controls.
class ResultsPane : public wxPanel {
 public:
  /// \brief These are the types of reports that can be displayed.
  enum class ReportType {
    kCatenary,
    kCatenaryEndpoints,
    kConstraint,
    kLength,
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
  /// \brief Handles the filter group choice event.
  /// \param[in] event
  ///   The event.
  void OnChoiceFilterGroup(wxCommandEvent& event);

  /// \brief Handles the report choice event.
  /// \param[in] event
  ///   The event.
  void OnChoiceReport(wxCommandEvent& event);

  /// \brief Handles the listctrl select event.
  /// \param[in] event
  ///   The event.
  void OnListCtrlSelect(wxListEvent& event);

  /// \brief Gets a list of filtered results.
  /// \return The filtered results.
  std::list<const SagTensionAnalysisResult*> Results();

  /// \brief Toggles the filter group controls.
  void ToggleFilterGroupControls();

  /// \brief Updates the filter group choice control.
  void UpdateFilterGroupChoice();

  /// \brief Updates the selected filter group.
  void UpdateFilterGroupSelected();

  /// \brief Updates the report data.
  void UpdateReportData();

  /// \brief Updates the report data with constraint results.
  void UpdateReportDataConstraint();

  /// \brief Updates the report data with catenary results.
  void UpdateReportDataCatenaryCurve();

  /// \brief Updates the report data with catenary endpoint results.
  void UpdateReportDataCatenaryEndpoints();

  /// \brief Updates the report data with length results.
  void UpdateReportDataLength();

  /// \brief Updates the report data with sag-tension results.
  void UpdateReportDataSagTension();

  /// \brief Updates the report data with tension distribution results.
  /// \param[in] results
  ///   The filtered results from the document.
  void UpdateReportDataTensionDistribution();

  /// \var data_
  ///   The report data.
  ReportData data_;

  /// \var table_
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

#endif  // SPANANALYZER_RESULTS_PANE_H_
