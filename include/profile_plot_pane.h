// This is free and unencumbered software released into the public domain.
// For more information, please refer to <http://unlicense.org/>

#ifndef OTLS_SPANANALYZER_PROFILEPLOTPANE_H_
#define OTLS_SPANANALYZER_PROFILEPLOTPANE_H_

#include "wx/docview.h"

#include "line_data_set_2d.h"
#include "plot_pane_2d.h"

/// \par OVERVIEW
///
/// This class is a wxAUI pane that displays a profile plot of the activated
/// span.
class ProfilePlotPane : public PlotPane2d {
 public:
  /// \brief Constructor.
  /// \param[in] parent
  ///   The parent window.
  /// \param[in] view
  ///   The view.
  ProfilePlotPane(wxWindow* parent, wxView* view);

  /// \brief Destructor.
  ~ProfilePlotPane();

  /// \brief Updates the pane.
  /// \param[in] hint
  ///   The hint describing what kind of update is needed.
  /// This function is called as part of the view update process.
  void Update(wxObject* hint = nullptr);

 protected:
  /// \brief Handles the context menu select event.
  /// \param[in] event
  ///   The event.
  void OnContextMenuSelect(wxCommandEvent& event);

  /// \brief Handles the mouse events.
  /// \param[in] event
  ///   The event.
  void OnMouse(wxMouseEvent& event);

  /// \brief Updates the plot datasets.
  void UpdatePlotDatasets() override;

  /// \brief Updates the plot renderers.
  void UpdatePlotRenderers() override;

  /// \var dataset_catenary_
  ///   The catenary plot dataset.
  LineDataSet2d dataset_catenary_;

  /// \var view_
  ///   The view.
  wxView* view_;

  DECLARE_EVENT_TABLE()
};

# endif //  OTLS_SPANANALYZER_PROFILEPLOTPANE_H_
