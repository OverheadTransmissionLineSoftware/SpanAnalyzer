// This is free and unencumbered software released into the public domain.
// For more information, please refer to <http://unlicense.org/>

#ifndef OTLS_SPANANALYZER_RENDERER2D_H_
#define OTLS_SPANANALYZER_RENDERER2D_H_

#include "wx/wx.h"

#include "data_set_2d.h"
#include "plot_axis.h"

/// \par OVERVIEW
///
/// This class is an abstract renderer which must be derived from.
class Renderer2d {
 public:
  /// \brief Constructor.
  Renderer2d();

  /// \brief Destructor.
  virtual ~Renderer2d();

  /// \brief Draws the data onto the device context.
  /// \param[in] dc
  ///   The device context.
  /// \param[in] rc
  ///   The dc region to draw onto.
  /// \param[in] axis_horizontal
  ///   The horizontal plot axis.
  /// \param[in] axis_vertical
  ///   The vertical plot axis.
  virtual void Draw(wxDC& dc, wxRect rc, const PlotAxis& axis_horizontal,
                    const PlotAxis& axis_vertical) const = 0;

  /// \brief Gets the dataset.
  /// \return The dataset.
  const DataSet2d* dataset() const;

  /// \brief Sets the dataset.
  /// \param[in] dataset
  ///   The dataset.
  void set_dataset(const DataSet2d* dataset);

 protected:
  /// \brief Clips the points to fit the horizontal axis.
  /// \param[in] axis
  ///   The axis, which contains the bounds/limits.
  /// \param[in] x_vis
  ///   The visible x coordinate.
  /// \param[in] y_vis
  ///   The visible y coordinate.
  /// \param[in,out] x
  ///   The x coordinate that needs clipped. At the start of the function
  ///   this coordinate is not visible.
  /// \param[in,out] y
  ///   The x coordinate that needs clipped. At the start of the function
  ///   this coordinate is not visible.
  static void ClipHorizontal(const PlotAxis& axis,
                             const double& x_vis, const double& y_vis,
                             double& x, double& y);

  /// \brief Clips the points to fit the horizontal axis.
  /// \param[in] axis
  ///   The axis, which contains the bounds/limits.
  /// \param[in] x_vis
  ///   The visible x coordinate.
  /// \param[in] y_vis
  ///   The visible y coordinate.
  /// \param[in,out] x
  ///   The x coordinate that needs clipped. At the start of the function
  ///   this coordinate is not visible.
  /// \param[in,out] y
  ///   The x coordinate that needs clipped. At the start of the function
  ///   this coordinate is not visible.
  static void ClipVertical(const PlotAxis& axis,
                           const double& x_vis, const double& y_vis,
                           double& x, double& y);

  /// \brief Translates from data to graphics coordinates.
  /// \param[in] value
  ///   The data value to convert.
  /// \param[in] value_min
  ///   The minimum data value (at the edge of the graphics drawing region).
  /// \param[in] value_max
  ///   The maximum data value (at the edge of the graphics drawing region).
  /// \param[in] range_graphics
  ///   The range (width/height) of the graphics coordinates.
  /// \param[in] is_vertical
  ///   An indicator that tells which type of conversion to do. Data with a
  ///   vertical orientations has to be inverted to match the graphics
  ///   coordinate system.
  /// \return The graphics coordinate.
  static wxCoord DataToGraphics(const double& value,
                                const double& value_min,
                                const double& value_max,
                                const int& range_graphics,
                                const bool& is_vertical);

  /// \var dataset_
  ///   The dataset.
  const DataSet2d* dataset_;
};

#endif //  OTLS_SPANANALYZER_RENDERER2D_H_
