// This is free and unencumbered software released into the public domain.
// For more information, please refer to <http://unlicense.org/>

#ifndef OTLS_SPANANALYZER_PLOT2D_H_
#define OTLS_SPANANALYZER_PLOT2D_H_

#include "wx/wx.h"

#include "line_renderer_2d.h"
#include "plot_axis.h"

/// \par OVERVIEW
///
/// This struct contains mix/max values for the x and y axis.
struct Plot2dDataLimits {
  /// \var x_max
  ///   The maximum x value.
  double x_max;

  /// \var x_min
  ///   The minimum x value.
  double x_min;

  /// \var x_max
  ///   The maximum y value.
  double y_max;

  /// \var x_max
  ///   The minimum y value.
  double y_min;
};

/// \par OVERVIEW
///
/// This class is a 2D plot.
///
/// \par RENDERERS
///
/// This plot supports multiple renderers, so the data may be added in
/// successive layers.
///
/// \par AXES
///
/// The plot uses axes to manage the boundaries of the rendered data. The axes
/// are updated whenever the window is resized.
///
/// \par FUTURE IMPROVEMENTS
///
/// This plot should eventually be capable of the following:
/// \todo Make this plot accept multiple renderer types via inheritance.
/// \todo Add scrolling.
class Plot2d {
 public:
  /// \brief Constructor.
  Plot2d();

  /// \brief Destructor.
  ~Plot2d();

  /// \brief Adds a renderer.
  /// \param[in] renderer
  ///   The renderer.
  void AddRenderer(const LineRenderer2d& renderer);

  /// \brief Clears the renderers.
  void ClearRenderers();

  /// \brief Redraws the plot.
  /// \param[in] dc
  ///   The device context.
  /// \param[in] rc
  ///   The rectangle of the rendering region.
  void Redraw(wxDC& dc, wxRect rc) const;

  /// \brief Gets the background brush.
  /// \return The background brush.
  wxBrush background() const;

  /// \brief Gets the aspect ratio.
  /// \return The aspect ratio.
  double ratio_aspect() const;

  /// \brief Sets the background brush.
  /// \param[in] brush
  ///   The background brush.
  void set_background(const wxBrush& brush);

  /// \brief Sets the aspect ratio.
  /// \param[in] ratio_aspect
  ///   The aspect ratio.
  void set_ratio_aspect(const double& ratio_aspect);

 private:
  /// \brief Updates the plot axes to encompass the plot data.
  /// \param[in] rc
  ///   The rectangle of the rendering region.
  /// The aspect ratio is used when fitting the window.
  void FitRenderAxesToPlotData(const wxRect& rc) const;

  /// \brief Updates the plot data boundaries.
  void UpdatePlotDataBoundaries() const;

  /// \var axis_horizontal_
  ///   The horizontal axis that defines the visible render boundary.
  mutable PlotAxis axis_horizontal_;

  /// \var axis_vertical_
  ///   The vertical axis that defines the visible render boundary.
  mutable PlotAxis axis_vertical_;

  /// \var brush_background_
  ///   The background brush.
  wxBrush brush_background_;

  /// \var limits_data_
  ///   The plot data min/max values.
  mutable Plot2dDataLimits limits_data_;

  /// \var is_updated_limits_data_
  ///   An indicator that tells if the data limits are updated.
  mutable bool is_updated_limits_data_;

  /// \var ratio_aspect_
  ///   The ratio of the vertical and horizontal units (V / H).
  double ratio_aspect_;

  /// \var renderers_
  ///   The list of renderers.
  std::list<LineRenderer2d> renderers_;
};

#endif //  OTLS_SPANANALYZER_PLOT2D_H_
