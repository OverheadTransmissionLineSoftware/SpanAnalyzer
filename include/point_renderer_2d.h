// This is free and unencumbered software released into the public domain.
// For more information, please refer to <http://unlicense.org/>

#ifndef OTLS_SPANANALYZER_POINTRENDERER2D_H_
#define OTLS_SPANANALYZER_POINTRENDERER2D_H_

#include "wx/wx.h"

#include "point_data_set_2d.h"
#include "renderer_2d.h"

/// \par OVERVIEW
///
/// This class renders a line dataset onto a plot.
class PointRenderer2d : public Renderer2d {
 public:
  /// \brief Constructor.
  PointRenderer2d();

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

  /// \brief Gets the pen.
  /// \return The pen.
  const wxPen* pen() const;

  /// \brief Sets the pen.
  /// \param[in] pen
  ///   The pen.
  void set_pen(const wxPen* pen);

 private:
  /// \var pen_
  ///   The pen.
  const wxPen* pen_;
};

#endif //  OTLS_SPANANALYZER_POINTRENDERER2D_H_
