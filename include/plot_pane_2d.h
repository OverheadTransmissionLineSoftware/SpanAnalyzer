// This is free and unencumbered software released into the public domain.
// For more information, please refer to <http://unlicense.org/>

#ifndef OTLS_SPANANALYZER_PLOTPANE2D_H_
#define OTLS_SPANANALYZER_PLOTPANE2D_H_

#include "wx/wx.h"

#include "line_data_set_2d.h"
#include "plot_2d.h"

/// \par OVERVIEW
///
/// This class is an abstract wxAUI pane used for 2d plotting.
class PlotPane2d : public wxPanel {
 public:
  /// \brief Constructor.
  /// \param[in] parent
  ///   The parent window.
  PlotPane2d(wxWindow* parent);

  /// \brief Destructor.
  ~PlotPane2d();

 protected:
  /// \brief Handles the erase background event.
  /// \param[in] event
  ///   The event.
  void OnEraseBackground(wxEraseEvent& event);

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

  /// \brief Updates the plot datasets.
  virtual void UpdatePlotDatasets() = 0;

  /// \brief Updates the plot renderers.
  virtual void UpdatePlotRenderers() = 0;

  /// \var bitmap_buffer_
  ///   The bitmap that is used as a device context buffer. Keeping this cached
  ///   helps speed up redraws.
  wxBitmap bitmap_buffer_;

  /// \var coord_mouse_
  ///   The mouse coordinate. This is only kept up-to-date when the mouse is
  ///   being dragged.
  wxPoint coord_mouse_;

  /// \var plot_
  ///   The plot.
  Plot2d plot_;

  DECLARE_EVENT_TABLE()
};

# endif //  OTLS_SPANANALYZER_PLOTPANE2D_H_
