// This is free and unencumbered software released into the public domain.
// For more information, please refer to <http://unlicense.org/>

#ifndef OTLS_SPANANALYZER_PLOT2D_H_
#define OTLS_SPANANALYZER_PLOT2D_H_

#include "wx/wx.h"

#include "line_renderer_2d.h"
#include "plot_axis.h"

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
/// The plot does not currently render any axes. Internally it does use a
/// plot axis to manage the boundaries of the data to be plotted.
///
/// \par FUTURE IMPROVEMENTS
///
/// This plot should eventually be capable of the following:
/// \todo Make this plot accept multiple renderer types via inheritance.
/// \todo Add fixed H:V scale.
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
  ///   The dc region to draw onto.
  void Redraw(wxDC& dc, wxRect rc) const;

  /// \brief Gets the background brush.
  /// \return The background brush.
  wxBrush background() const;

  /// \brief Sets the background brush.
  /// \param[in] brush
  ///   The background brush.
  void set_background(const wxBrush& brush);

 private:
  /// \brief Updates the plot axes.
  void UpdatePloxAxes() const;

  /// \var axis_horizontal_
  ///   The horizontal axis.
  mutable PlotAxis axis_horizontal_;

  /// \var axis_vertical_
  ///   The vertical axis.
  mutable PlotAxis axis_vertical_;

  /// \var brush_background_
  ///   The background brush.
  wxBrush brush_background_;

  /// \var is_updated_axes_
  ///   An indicator that tells if the plot is updated.
  mutable bool is_updated_axes_;

  /// \var renderers_
  ///   The list of renderers.
  std::list<LineRenderer2d> renderers_;
};

#endif //  OTLS_SPANANALYZER_PLOT2D_H_
