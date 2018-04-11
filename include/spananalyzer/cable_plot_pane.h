// This is free and unencumbered software released into the public domain.
// For more information, please refer to <http://unlicense.org/>

#ifndef OTLS_SPANANALYZER_CABLEPLOTPANE_H_
#define OTLS_SPANANALYZER_CABLEPLOTPANE_H_

#include "wx/docview.h"

#include "appcommon/graphics/circle_data_set_2d.h"
#include "appcommon/graphics/line_data_set_2d.h"
#include "appcommon/graphics/plot_pane_2d.h"
#include "appcommon/graphics/text_data_set_2d.h"
#include "models/sagtension/cable_elongation_model.h"

#include "spananalyzer/sag_tension_analysis_result.h"
#include "spananalyzer/span_analyzer_config.h"

/// \par OVERVIEW
///
/// This class is a wxAUI pane that displays the cable elongation model of the
/// activated span.
class CablePlotPane : public PlotPane2d {
 public:
  /// \brief Constructor.
  /// \param[in] parent
  ///   The parent window.
  /// \param[in] view
  ///   The view.
  CablePlotPane(wxWindow* parent, wxView* view);

  /// \brief Destructor.
  ~CablePlotPane();

  /// \brief Updates the pane.
  /// \param[in] hint
  ///   The hint describing what kind of update is needed.
  /// This function is called as part of the view update process.
  void Update(wxObject* hint = nullptr);

 private:
  /// \brief Clears the datasets.
  void ClearDataSets();

  /// \brief Handles the context menu select event.
  /// \param[in] event
  ///   The event.
  void OnContextMenuSelect(wxCommandEvent& event);

  /// \brief Handles the mouse events.
  /// \param[in] event
  ///   The event.
  void OnMouse(wxMouseEvent& event);

  /// \brief Updates the chart axes datasets (lines and text).
  /// \param[in] x_min
  ///   The minimum x value.
  /// \param[in] x_max
  ///   The maximum x value.
  /// \param[in] y_min
  ///   The minimum y value.
  /// \param[in] y_max
  ///   The maximum y value.
  void UpdateDataSetAxes(const double& x_min, const double& x_max,
                         const double& y_min, const double& y_max);

  /// \brief Updates a cable dataset.
  /// \param[in] model
  ///   The cable elongation model used to calculate the dataset points.
  /// \param[in] type_component
  ///   The type of component to calculate data for.
  /// \param[out] dataset
  ///   The dataset to populate.
  void UpdateDataSetCable(
      const CableElongationModel& model,
      const CableElongationModel::ComponentType& type_component,
      LineDataSet2d& dataset);

  /// \brief Updates the circle marker dataset.
  /// \param[in] model
  ///   The cable elongation model.
  /// \param[in] result
  ///   The sag-tension result.
  void UpdateDataSetMarker(const CableElongationModel& model,
                           const SagTensionAnalysisResult* result);

  /// \brief Updates the plot datasets.
  void UpdatePlotDatasets() override;

  /// \brief Updates the plot renderers.
  /// This includes re-calculating the dataset.
  void UpdatePlotRenderers() override;

  /// \brief Updates the plot scaling.
  void UpdatePlotScaling();

  /// \brief Updates the strain values.
  /// \param[in] model
  ///   The cable elongation model.
  void UpdateStrains(const CableElongationModel& model);

  /// \var dataset_axis_lines_
  ///   The dataset for the axis lines.
  LineDataSet2d dataset_axis_lines_;

  /// \var dataset_axis_text_
  ///   The dataset for the axis text.
  TextDataSet2d dataset_axis_text_;

  /// \var dataset_core_
  ///   The dataset for the core component.
  LineDataSet2d dataset_core_;

  /// \var dataset_markers_
  ///   The dataset for the tension markers.
  CircleDataSet2d dataset_markers_;

  /// \var dataset_shell_
  ///   The dataset for the shell component.
  LineDataSet2d dataset_shell_;

  /// \var dataset_total_
  ///   The dataset for the entire cable.
  LineDataSet2d dataset_total_;

  /// \var options_
  ///   The plot options, which are stored in the application config.
  CablePlotOptions* options_;

  /// \var strains_
  ///   The strain values to calculate points for.
  std::list<double> strains_;

  /// \var view_
  ///   The view.
  wxView* view_;

  DECLARE_EVENT_TABLE()
};

# endif  // OTLS_SPANANALYZER_CABLEPLOTPANE_H_
