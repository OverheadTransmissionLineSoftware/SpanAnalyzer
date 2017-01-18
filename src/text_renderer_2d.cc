// This is free and unencumbered software released into the public domain.
// For more information, please refer to <http://unlicense.org/>

#include "text_renderer_2d.h"

TextRenderer2d::TextRenderer2d() {
  color_ = nullptr;
}

void TextRenderer2d::Draw(wxDC& dc, wxRect rc, const PlotAxis& axis_horizontal,
                          const PlotAxis& axis_vertical) const {
  // sets text colors for dc
  dc.SetBackgroundMode(wxPENSTYLE_TRANSPARENT);
  dc.SetTextForeground(*color_);

  // casts to text dataset
  const TextDataSet2d* dataset = dynamic_cast<const TextDataSet2d*>(dataset_);

  // draws each text in dataset
  const std::list<const Text2d*>* data = dataset->data();
  for (auto iter = data->cbegin(); iter != data->cend(); iter++) {
    // gets text
    const Text2d* text = *iter;

    // gets points to draw
    double x = text->point.x;
    double y = text->point.y;

    // skips points that are out of range
    if (x < axis_horizontal.Min()) {
      continue;
    } else if (axis_horizontal.Max() < x) {
      continue;
    } else if (y < axis_vertical.Min()) {
      continue;
    } else if (axis_vertical.Max() < y) {
      continue;
    }

    // translates to graphics coordinates
    wxCoord xg, yg;

    xg = DataToGraphics(x, axis_horizontal.Min(), axis_horizontal.Max(),
                        rc.GetWidth(), false);
    yg = DataToGraphics(y, axis_vertical.Min(), axis_vertical.Max(),
                        rc.GetHeight(), true);

    // adjusts the screen coordinates based on the text point alignment
    wxSize size = dc.GetTextExtent(text->message);
    if (text->position == Text2d::BoundaryPosition::kCenterLower) {
      xg -= (size.GetX() / 2);
      yg -= size.GetY();
    } else if (text->position == Text2d::BoundaryPosition::kCenterUpper) {
      xg -= (size.GetX() / 2);
    } else if (text->position == Text2d::BoundaryPosition::kLeftCenter) {
      yg -= (size.GetY() / 2);
    } else if (text->position == Text2d::BoundaryPosition::kLeftLower) {
      yg -= size.GetY();
    } else if (text->position == Text2d::BoundaryPosition::kLeftUpper) {
      // do nothing, this is the default
    } else if (text->position == Text2d::BoundaryPosition::kRightCenter) {
      xg -= size.GetX();
      yg -= (size.GetY() / 2);
    } else if (text->position == Text2d::BoundaryPosition::kRightLower) {
      xg -= size.GetX();
      yg -= size.GetY();
    } else if (text->position == Text2d::BoundaryPosition::kRightUpper) {
      xg -= size.GetX();
    }

    // draws onto DC
    dc.DrawRotatedText(text->message, xg, yg, text->angle);
  }
}

const wxColour* TextRenderer2d::color() const {
  return color_;
}

void TextRenderer2d::set_color(const wxColour* color) {
  color_ = color;
}

