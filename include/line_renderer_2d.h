// This is free and unencumbered software released into the public domain.
// For more information, please refer to <http://unlicense.org/>

#ifndef OTLS_SPANANALYZER_LINERENDERER2D_H_
#define OTLS_SPANANALYZER_LINERENDERER2D_H_

#include "wx/wx.h"

#include "line_data_set_2d.h"
#include "plot_axis.h"

/// \par OVERVIEW
///
/// This class renders a line dataset onto a plot.
class LineRenderer2d {
 public:
  /// \brief Gets the dataset.
  /// \return The dataset.
  const LineDataSet2d* dataset() const;

  /// \brief Gets the pen.
  /// \return The pen.
  const wxPen* pen() const;

  /// \brief Sets the dataset.
  /// \param[in] dataset
  ///   The dataset.
  void set_dataset(const LineDataSet2d* dataset);

  /// \brief Sets the pen.
  /// \param[in] pen
  ///   The pen.
  void set_pen(const wxPen* pen);

  /// \brief Draws the data onto the device context.
  /// \param[in] dc
  ///   The device context.
  /// \param[in] rc
  ///   The dc region to draw onto.
  /// \param[in] axis_horizontal
  ///   The horizontal plot axis.
  /// \param[in] axis_vertical
  ///   The vertical plot axis.
  void Draw(wxDC& dc, wxRect rc, const PlotAxis& axis_horizontal,
            const PlotAxis& axis_vertical) const;

 private:
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
  const LineDataSet2d* dataset_;

  /// \var pen_
  ///   The pen.
  const wxPen* pen_;
};

#endif //  OTLS_SPANANALYZER_LINERENDERER2D_H_
