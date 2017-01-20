// This is free and unencumbered software released into the public domain.
// For more information, please refer to <http://unlicense.org/>

#ifndef OTLS_SPANANALYZER_TEXTRENDERER2D_H_
#define OTLS_SPANANALYZER_TEXTRENDERER2D_H_

#include "wx/wx.h"

#include "text_data_set_2d.h"
#include "renderer_2d.h"

/// \par OVERVIEW
///
/// This class renders a text dataset onto a plot.
class TextRenderer2d : public Renderer2d {
 public:
  /// \brief Constructor.
  TextRenderer2d();

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

  /// \brief Gets the color of the text.
  /// \return The color of the text.
  const wxColour* color() const;

  /// \brief Sets the color of the text.
  /// \param[in] color
  ///   The color.
  void set_color(const wxColour* color);

 private:
  /// \var color_
  ///   The color of the text.
  const wxColour* color_;
};

#endif //  OTLS_SPANANALYZER_TEXTRENDERER2D_H_
