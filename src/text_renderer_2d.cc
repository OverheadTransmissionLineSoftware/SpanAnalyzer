// This is free and unencumbered software released into the public domain.
// For more information, please refer to <http://unlicense.org/>

#include "text_renderer_2d.h"

#include "models/base/vector.h"

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

    // translates point to graphics coordinates
    wxCoord xg, yg;
    xg = DataToGraphics(x, axis_horizontal.Min(), axis_horizontal.Max(),
                        rc.GetWidth(), false);
    yg = DataToGraphics(y, axis_vertical.Min(), axis_vertical.Max(),
                        rc.GetHeight(), true);

    // calculates the graphics offset due to boundary position and angle
    // offset is vector from upper left position to specified boundary position
    Vector2d offset;
    wxSize size = dc.GetTextExtent(text->message);
    if (text->position == Text2d::BoundaryPosition::kCenterLower) {
      offset.set_x(size.GetX() / 2);
      offset.set_y(-size.GetY());
    } else if (text->position == Text2d::BoundaryPosition::kCenterUpper) {
      offset.set_x(size.GetX() / 2);
      offset.set_y(0);
    } else if (text->position == Text2d::BoundaryPosition::kLeftCenter) {
      offset.set_x(0);
      offset.set_y(-size.GetY() / 2);
    } else if (text->position == Text2d::BoundaryPosition::kLeftLower) {
      offset.set_x(0);
      offset.set_y(-size.GetY());
    } else if (text->position == Text2d::BoundaryPosition::kLeftUpper) {
      offset.set_x(0);
      offset.set_y(0);
    } else if (text->position == Text2d::BoundaryPosition::kRightCenter) {
      offset.set_x(size.GetX());
      offset.set_y(-size.GetY() / 2);
    } else if (text->position == Text2d::BoundaryPosition::kRightLower) {
      offset.set_x(size.GetX());
      offset.set_y(-size.GetY());
    } else if (text->position == Text2d::BoundaryPosition::kRightUpper) {
      offset.set_x(size.GetX());
      offset.set_y(0);
    } else {
      offset.set_x(0);
      offset.set_y(0);
    }

    offset.Rotate(text->angle);

    // adjusts the graphics coordinates using the offset
    // y-axis is inverted to match dc coordinate system
    xg -= static_cast<int>(offset.x());
    yg += static_cast<int>(offset.y());

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

