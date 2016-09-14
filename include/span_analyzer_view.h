// This is free and unencumbered software released into the public domain.
// For more information, please refer to <http://unlicense.org/>

#ifndef OTLS_SPANANALYZER_SPANANALYZERVIEW_H_
#define OTLS_SPANANALYZER_SPANANALYZERVIEW_H_

#include <list>

#include "wx/docview.h"

#include "edit_pane.h"
#include "plot_pane.h"
#include "results_pane.h"

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

  /// \brief Handles updating of the view.
  /// \param[in] sender
  ///   The view that sent the update request. Since this is a single view
  ///   application, this will remain nullptr.
  /// \param[in] hint
  ///   The update hint that helps child windows optimize the update. This is
  ///   provided by the function/action that triggered the update.
  /// This function is called by wxWidgets.
  virtual void OnUpdate(wxView *sender, wxObject *hint = nullptr);

  /// \brief Gets the display cable condition.
  /// \return The display cable condition.
  const CableConditionType& condition() const;

  /// \brief Gets the activated weathercase group.
  /// \return The activated weathercase group. If no weathercase group is
  ///   activated, a nullptr is returned.
  const WeatherLoadCaseGroup* group_weathercases() const;

  /// \brief Gets the selected weathercase index.
  /// \return The selected weathercase index.
  const int index_weathercase() const;

  /// \brief Gets the edit pane.
  /// \return The edit pane.
  EditPane* pane_edit();

  /// \brief Gets the plot pane.
  /// \return The plot pane.
  PlotPane* pane_plot();

  /// \brief Gets the results pane.
  /// \return The results pane.
  ResultsPane* pane_results();

  /// \brief Sets the display condition.
  /// \param[in] condition
  ///   The display condition.
  void set_condition(const CableConditionType& condition);

  /// \brief Sets the weathercase group.
  /// \param[in] group
  ///   The weathercase group.
  void set_group_weathercase(const WeatherLoadCaseGroup* group);

  /// \brief Sets the selected weathercase index.
  /// \param[in] index_weathercase
  ///   The selected weathercase index.
  void set_index_weathercase(const int& index_weathercase);

 private:
  /// \var condition_
  ///   The display cable condition.
  CableConditionType condition_;

  /// \var group_weathercases_
  ///   The weathercase group that is currently activated.
  const WeatherLoadCaseGroup* group_weathercases_;

  /// \var index_weathercase_
  ///   The weathercase index that is currently selected.
  int index_weathercase_;

  /// \var pane_edit_
  ///   The edit pane.
  EditPane* pane_edit_;

  /// \var pane_plot_
  ///   The plot pane.
  PlotPane* pane_plot_;

  /// \var pane_results_
  ///   The results pane.
  ResultsPane* pane_results_;

  /// \brief This allows wxWidgets to create this class dynamically as part of
  ///   the docview framework.
  wxDECLARE_DYNAMIC_CLASS(SpanAnalyzerView);
};

#endif //  OTLS_SPANANALYZER_SPANANALYZERVIEW_H_
