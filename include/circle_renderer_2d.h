// This is free and unencumbered software released into the public domain.
// For more information, please refer to <http://unlicense.org/>

#ifndef OTLS_SPANANALYZER_POINTRENDERER2D_H_
#define OTLS_SPANANALYZER_CIRCLERENDERER2D_H_

#include "wx/wx.h"

#include "circle_data_set_2d.h"
#include "renderer_2d.h"

/// \par OVERVIEW
///
/// This class renders a line dataset onto a plot.
class CircleRenderer2d : public Renderer2d {
 public:
  /// \brief Constructor.
  CircleRenderer2d();

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
                    const PlotAxis& axis_vertical) const override;

  /// \brief Gets the brush.
  /// \return The brush.
  const wxBrush* brush() const;

  /// \brief Gets the pen.
  /// \return The pen.
  const wxPen* pen() const;

  /// \brief Sets the brush.
  /// \param[in] brush
  ///   The brush.
  void set_brush(const wxBrush* brush);

  /// \brief Sets the pen.
  /// \param[in] pen
  ///   The pen.
  void set_pen(const wxPen* pen);

 private:
  /// \var brush_
  ///   The brush.
  /// The brush sets the inside color of the circle.
  const wxBrush* brush_;

  /// \var pen_
  ///   The pen.
  /// The pen sets the color for the outside of the circle.
  const wxPen* pen_;
};

#endif //  OTLS_SPANANALYZER_CIRCLERENDERER2D_H_
