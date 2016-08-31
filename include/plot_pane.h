// This is free and unencumbered software released into the public domain.
// For more information, please refer to <http://unlicense.org/>

#ifndef OTLS_SPANANALYZER_PLOTPANE_H_
#define OTLS_SPANANALYZER_PLOTPANE_H_

#include "wx/docview.h"
#include "wx/wx.h"

#include "line_data_set_2d.h"
#include "plot_2d.h"

/// \par OVERVIEW
///
/// This class is a wxAUI pane that displays a plot of the activate span.
class PlotPane : public wxPanel {
 public:
  /// \brief Constructor.
  /// \param[in] parent
  ///   The parent window.
  /// \param[in] view
  ///   The view.
  PlotPane(wxWindow* parent, wxView* view);

  /// \brief Destructor.
  ~PlotPane();

  /// \brief Updates the pane.
  /// \param[in] hint
  ///   The hint describing what kind of update is needed.
  /// This function is called as part of the view update process.
  void Update(wxObject* hint = nullptr);

 private:
  /// \brief Clears the plot.
  /// \param[in] dc
  ///   The device context that is drawn onto.
  void ClearPlot(wxDC& dc);

  /// \brief Handles the context menu select event.
  /// \param[in] event
  ///   The event.
  void OnContextMenuSelect(wxCommandEvent& event);

  /// \brief Handles the mouse events.
  /// \param[in] event
  ///   The event.
  void OnMouse(wxMouseEvent& event);

  /// \brief Handles the mouse wheel events.
  /// \param[in] event
  ///   The event.
  void OnMouseWheel(wxMouseEvent& event);

  /// \brief Handles the resize window event.
  /// \param[in] event
  ///   The event.
  void OnPaint(wxPaintEvent& event);

  /// \brief Renders the plot within the pane.
  /// \param[in] dc
  ///   The device context.
  void RenderPlot(wxDC& dc);

  /// \brief Updates the plot.
  /// This includes re-calculating the points and recreating the renderers.
  void UpdatePlot();

  /// \var coord_mouse_
  ///   The mouse coordinate. This is only kept up-to-date when the mouse is
  ///   being dragged.
  wxPoint coord_mouse_;

  /// \var dataset_catenary_
  ///   The catenary plot dataset.
  LineDataSet2d dataset_catenary_;

  /// \var plot_
  ///   The plot.
  Plot2d plot_;

  /// \var view_
  ///   The view.
  wxView* view_;

  DECLARE_EVENT_TABLE()
};

# endif //  OTLS_SPANANALYZER_PLOTPANE_H_
