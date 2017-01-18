// This is free and unencumbered software released into the public domain.
// For more information, please refer to <http://unlicense.org/>

#ifndef OTLS_SPANANALYZER_PLOT2D_H_
#define OTLS_SPANANALYZER_PLOT2D_H_

#include <list>

#include "models/base/point.h"
#include "wx/wx.h"

#include "renderer_2d.h"
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

  /// \var y_max
  ///   The maximum y value.
  double y_max;

  /// \var y_min
  ///   The minimum y value.
  double y_min;
};

/// \par OVERVIEW
///
/// This class is a 2D plot. It accepts data/renderers and renders them onto a
/// device context.
///
/// There are two coordinate systems that are used in this class: data and
/// graphics. The data coordinate system is used to define most class
/// parameters. However, most of the functions for manipulating the plot accept
/// graphics coordinates and convert them to data coordinates internally as
/// needed.
///
/// \par RENDERERS
///
/// This plot uses individual renderers of varying types to render the data onto
/// the device context. Multiple renderers may be added to create successive
/// layers to be drawn.
///
/// Note that this plot copies the renderers as they are added to the class, but
/// does NOT own the datasets that are referenced by the renderers. This is done
/// by design, as this class is purely a data visualizer.
///
/// \par ASPECT RATIO
///
/// This plot allows the vertical and horizontal axes to have different scales.
/// The data will stay in the native units, but the vertical graphics
/// coordinates will be scaled by the aspect ratio factor (V/H) when rendered.
///
/// \par FIT
///
/// The plot can fit the data to a specific device context size. It will keep
/// the aspect ratio that is currently set.
///
/// \par SHIFTING
///
/// The plot can be shifted on both axes.
///
/// \par ZOOM
///
/// This plot supports zooming in/out.
///
class Plot2d {
 public:
  /// \brief Constructor.
  Plot2d();

  /// \brief Destructor.
  ~Plot2d();

  /// \brief Adds a renderer.
  /// \param[in] renderer
  ///   The renderer. The plot will take ownership of the pointer.
  void AddRenderer(const Renderer2d* renderer);

  /// \brief Clears the renderers.
  void ClearRenderers();

  /// \brief Gets the plot data limits.
  /// \return The plot data limits.
  Plot2dDataLimits LimitsData() const;

  /// \brief Gets if the plot has any renderers.
  /// \return If the plot has any renderers.
  bool HasRenderers() const;

  /// \brief Translates a data coordinate to a graphics coordinate.
  /// \param[in] point_data
  ///   The data coordinate.
  /// \return The graphics coordinate.
  wxPoint PointDataToGraphics(const Point2d& point_data) const;

  /// \brief Translates a graphics coordinate to a data coordinate.
  /// \param[in] point_graphics
  ///   The graphics coordinate.
  /// \return The data coordinate.
  Point2d PointGraphicsToData(const wxPoint& point_graphics) const;

  /// \brief Renders the plot.
  /// \param[in] dc
  ///   The device context.
  /// \param[in] rc
  ///   The rectangle of the rendering region.
  void Render(wxDC& dc, wxRect rc) const;

  /// \brief Shifts the plot.
  /// \param[in] x
  ///   The amount (in graphics units) to shift the x axis.
  /// \param[in] y
  ///   The amount (in graphics units) to shift the y axis.
  void Shift(const double& x, const double& y);

  /// \brief Zooms the plot.
  /// \param[in] factor
  ///   The zoom factor, which is used to adjust the current plot scale.
  /// \param[in] point
  ///   The point (in graphics units) to zoom to.
  /// This function does not render the graph. The function parameters are used
  /// to update the internal class members only.
  void Zoom(const double& factor, const wxPoint& point);

  /// \brief Gets the background brush.
  /// \return The background brush.
  wxBrush background() const;

  /// \brief Gets if the plot data is fitted when rendered.
  /// \return If the plot data is fitted when rendered.
  bool is_fitted() const;

  /// \brief Gets the offset.
  /// \return The offset.
  Point2d offset() const;

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

  /// \brief Sets if the plot data is fitted when rendered.
  /// \param[in] is_fitted
  ///   An indicator that determines if the plot data is fitted to the window
  ///   when rendered.
  void set_is_fitted(const bool& is_fitted);

  /// \brief Sets the offset.
  /// \param[in] offset
  ///   The offset.
  void set_offset(const Point2d& offset);

  /// \brief Sets the aspect ratio.
  /// \param[in] ratio_aspect
  ///   The aspect ratio.
  void set_ratio_aspect(const double& ratio_aspect);

  /// \brief Sets the scale.
  /// \param[in] scale
  ///   The scaling factor.
  void set_scale(const double& scale);

 private:
  /// \brief Gets a plot axis used for rendering.
  /// \param[in] position
  ///   The upper/left coordinate component of the graphics rectangle.
  /// \param[in] range
  ///   The height/width of the graphics rectangle.
  /// \param[in] is_vertical
  ///   An indicator that adjusts whether the aspect ratio is used.
  /// \return A plot axis (in data coordinates) of the area being rendered.
  ///   These axes are needed by the renderers to draw onto the graphics rect.
  PlotAxis Axis(const int& position, const int& range,
                const bool& is_vertical) const;

  /// \brief Updates the plot offset and scale to encapsulate the plot data.
  /// \param[in] rc
  ///   The graphics rectangle.
  /// This is typically done when fitting the data to the graphics rect.
  void UpdateOffsetAndScaleToFitData(const wxRect& rc) const;

  /// \brief Updates the data boundaries.
  void UpdateDataLimits() const;

  /// \var brush_background_
  ///   The background brush.
  wxBrush brush_background_;

  /// \var limits_data_
  ///   The plot data min/max values.
  mutable Plot2dDataLimits limits_data_;

  /// \var is_fitted_
  ///   An indicator that tells if the plot is fitted to the graphics rect on a
  ///   redraw.
  bool is_fitted_;

  /// \var is_updated_limits_data_
  ///   An indicator that tells if the data limits are updated.
  mutable bool is_updated_limits_data_;

  /// \var offset_
  ///   The upper left coordinate of the rendered data. This aligns with the
  ///   graphics origin coordinate, which is always (0,0). This value is used to
  ///   translate between graphics and data coordinates.
  mutable Point2d offset_;

  /// \var ratio_aspect_
  ///   The ratio of the vertical and horizontal units (V / H). When rendering
  ///   the vertical axis is scaled by this ratio, while the horizontal axis
  ///   remains unchanged.
  double ratio_aspect_;

  /// \var scale_
  ///   The multiplication factor used to scale graphics coordinates to the data
  ///   coordinates.
  mutable double scale_;

  /// \var renderers_
  ///   The list of renderers.
  std::list<const Renderer2d*> renderers_;
};

#endif //  OTLS_SPANANALYZER_PLOT2D_H_
