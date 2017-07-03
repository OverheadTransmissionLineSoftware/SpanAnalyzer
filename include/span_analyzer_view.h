// This is free and unencumbered software released into the public domain.
// For more information, please refer to <http://unlicense.org/>

#ifndef OTLS_SPANANALYZER_SPANANALYZERVIEW_H_
#define OTLS_SPANANALYZER_SPANANALYZERVIEW_H_

#include <list>

#include "wx/docview.h"
#include "wx/notebook.h"

#include "cable_elongation_model_plot_pane.h"
#include "edit_pane.h"
#include "profile_plot_pane.h"
#include "results_pane.h"
#include "span_analyzer_data.h"

/// \par OVERVIEW
///
/// This is the SpanAnalyzer application view, which is responsible for
/// displaying information and results from the SpanAnalyzerDoc.
class SpanAnalyzerView : public wxView {
 public:
  /// \brief Constructor.
  SpanAnalyzerView();

  /// \brief Destructor.
  ~SpanAnalyzerView();

  /// \brief Gets the analysis filter for the active filter index.
  /// \return The analysis filter. If an active analysis filter is not valid
  ///   a nullptr is returned.
  const AnalysisFilter* AnalysisFilterActive() const;

  /// \brief Gets the weathercase index for the filter.
  /// \param[in] filter
  ///   The analysis filter.
  /// \return The weathercase index.
  int IndexWeathercase(const AnalysisFilter& filter) const;

  /// \brief Handles closing the view.
  /// \param[in] deleteWindow
  ///   An indicator that determines if the child windows should be deleted.
  /// \return true
  /// This function is called by wxWidgets.
  virtual bool OnClose(bool deleteWindow = true);

  /// \brief Creates the view and initializes.
  /// \param[in] doc
  ///   The document.
  /// \param[in] flags
  ///   The parameters used by wxWidgets.
  /// \return Always true.
  /// This function is called by wxWidgets.
  virtual bool OnCreate(wxDocument *doc, long flags);

  /// \brief Handles drawing/rendering the view.
  /// \param[in] dc
  ///   The device context.
  /// This function is called by wxWidgets.
  virtual void OnDraw(wxDC *dc);

  /// \brief Handles the notebook page change event.
  /// \param[in] event
  ///   The event.
  void OnNotebookPageChange(wxBookCtrlEvent& event);

  /// \brief Handles updating of the view.
  /// \param[in] sender
  ///   The view that sent the update request. Since this is a single view
  ///   application, this will remain nullptr.
  /// \param[in] hint
  ///   The update hint that helps child windows optimize the update. This is
  ///   provided by the function/action that triggered the update.
  /// This function is called by wxWidgets.
  virtual void OnUpdate(wxView *sender, wxObject *hint = nullptr);

  /// \brief Gets the activated analysis filter group.
  /// \return The activated analysis filter group. If no analysis filter group
  ///   is activated, a nullptr is returned.
  const AnalysisFilterGroup* group_filters() const;

  /// \brief Gets the selected filter index.
  /// \return The selected filter index.
  const int index_filter() const;

  /// \brief Gets the cable model pane.
  /// \return The cable model pane.
  CableElongationModelPlotPane* pane_cable();

  /// \brief Gets the edit pane.
  /// \return The edit pane.
  EditPane* pane_edit();

  /// \brief Gets the plot pane.
  /// \return The plot pane.
  ProfilePlotPane* pane_profile();

  /// \brief Gets the results pane.
  /// \return The results pane.
  ResultsPane* pane_results();

  /// \brief Sets the analysis filter group.
  /// \param[in] group
  ///   The analysis filter group.
  void set_group_filters(const AnalysisFilterGroup* group);

  /// \brief Sets the selected index of the analysis filter.
  /// \param[in] index_filter
  ///   The selected filter index.
  void set_index_filter(const int& index_filter);

 private:
  /// \var group_filters_
  ///   The group of filters that is currently activated.
  const AnalysisFilterGroup* group_filters_;

  /// \var index_filter_
  ///   The filter index that is currently selected.
  int index_filter_;

  /// \var notebook_plot_
  ///   The notebook that contains the plots.
  wxNotebook* notebook_plot_;

  /// \var pane_cable_
  ///   The cable elongation model plot pane.
  CableElongationModelPlotPane* pane_cable_;

  /// \var pane_edit_
  ///   The edit pane.
  EditPane* pane_edit_;

  /// \var pane_profile_
  ///   The profile plot pane.
  ProfilePlotPane* pane_profile_;

  /// \var pane_results_
  ///   The results pane.
  ResultsPane* pane_results_;

  /// \brief This allows wxWidgets to create this class dynamically as part of
  ///   the docview framework.
  wxDECLARE_DYNAMIC_CLASS(SpanAnalyzerView);

  DECLARE_EVENT_TABLE()
};

#endif //  OTLS_SPANANALYZER_SPANANALYZERVIEW_H_
