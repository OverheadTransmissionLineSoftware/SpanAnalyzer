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
/// This class is a 2D plot. There are two coordinate systems that are used in
/// this class: data and graphics. The data coordinate system is used to define
/// most class parameters. When the graphics coordinate system is needed, is
/// is converted to as needed.
///
/// \par RENDERERS
///
/// This plot does not manage datasets, but instead stores renderers (which have
/// the references to datasets). Multiple renderers may be added to create
/// successive layers to be drawn.
///
/// \par AXES
///
/// The plot uses axes to manage the boundaries of the rendered data. The axes
/// are updated whenever the window is resized or shifted.
///
/// \par FIT
///
/// The plot can fit the plot axes and zoom to encapsulate all of the data. It
/// will keep the aspect ratio that is currently set.
///
/// \var SHIFTING
///
/// The plot supports shifting the render axes.
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

  /// \brief Shifts the plot.
  /// \param[in] x
  ///   The amount to shift the x axis.
  /// \param[in] y
  ///   The amount to shift the y axis.
  /// This will adjust the center position of the plot axes.
  void Shift(const double& x, const double& y);

  /// \brief Gets the horizontal axis.
  /// \return The horizontal axis.
  const PlotAxis& axis_horizontal() const;

  /// \brief Gets the vertical axis.
  /// \return The vertical axis.
  const PlotAxis& axis_vertical() const;

  /// \brief Gets the background brush.
  /// \return The background brush.
  wxBrush background() const;

  /// \brief Gets if the plot render axes are fitted to the window on a redraw.
  /// \return If the plot render axes are fitted to the window on a redraw.
  bool is_fitted() const;

  /// \brief Gets the aspect ratio.
  /// \return The aspect ratio.
  double ratio_aspect() const;

  /// \brief Gets the scaling factor.
  /// \return The scaling factor.
  double scale() const;

  /// \brief Sets the background brush.
  /// \param[in] brush
  ///   The background brush.
  void set_background(const wxBrush& brush);

  /// \brief Sets if the plot data is fitted to the window on a redraw.
  /// \param[in] is_fitted
  ///   An indicator that determines if the plot is fitted to the window on a
  ///   redraw.
  void set_is_fitted(const bool& is_fitted);

  /// \brief Sets the aspect ratio.
  /// \param[in] ratio_aspect
  ///   The aspect ratio.
  void set_ratio_aspect(const double& ratio_aspect);

  /// \brief Sets the scale.
  /// \param[in] scale
  ///   The scaling factor.
  void set_scale(const double& scale);

 private:
  /// \brief Updates the plot axes and scaling to encapsulate the plot data.
  /// \param[in] rc
  ///   The rectangle of the rendering region.
  /// The aspect ratio is used when fitting the window.
  void FitPlotDataToRect(const wxRect& rc) const;

  /// \brief Updates the plot axes to match the plot window rect size.
  /// \param[in] rc
  ///   The rectangle fo the rendering region.
  void UpdatePlotAxesToRect(const wxRect& rc) const;

  /// \brief Updates the plot data boundaries.
  void UpdatePlotDataLimits() const;

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

  /// \var is_fitted_
  ///   An indicator that tells if the plot is fitted to the window on a redraw.
  bool is_fitted_;

  /// \var is_updated_limits_data_
  ///   An indicator that tells if the data limits are updated.
  mutable bool is_updated_limits_data_;

  /// \var ratio_aspect_
  ///   The ratio of the vertical and horizontal units (V / H). When rendering
  ///   the vertical axis is scaled by this ratio, while the horizontal axis
  ///   remains unchanged.
  double ratio_aspect_;

  /// \var scale_
  ///   The factor used to scale the plot axes coordinates to the graphics axis
  ///   coordinates.
  mutable double scale_;

  /// \var renderers_
  ///   The list of renderers.
  std::list<LineRenderer2d> renderers_;
};

#endif //  OTLS_SPANANALYZER_PLOT2D_H_
