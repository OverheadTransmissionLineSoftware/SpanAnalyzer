// This is free and unencumbered software released into the public domain.
// For more information, please refer to <http://unlicense.org/>

#include "line_renderer_2d.h"

LineRenderer2d::LineRenderer2d() {
  pen_ = nullptr;
}

void LineRenderer2d::Draw(wxDC& dc, wxRect rc, const PlotAxis& axis_horizontal,
                          const PlotAxis& axis_vertical) const {
  // sets drawing pen
  dc.SetPen(*pen_);

  // casts to line dataset
  const LineDataSet2d* dataset = (LineDataSet2d*)dataset_;

  // draws each line in dataset
  const std::list<const Line2d*>* data = dataset->data();
  for (auto iter = data->cbegin(); iter != data->cend(); iter++) {
    // gets line
    const Line2d* line = *iter;

    // gets points to draw
    double x0 = line->p0.x;
    double y0 = line->p0.y;
    double x1 = line->p1.x;
    double y1 = line->p1.y;

    // skips lines that are out of range
    if ((x0 < axis_horizontal.Min()) && (x1 < axis_horizontal.Min())) {
      continue;
    } else if ((axis_horizontal.Max() < x0) && (axis_horizontal.Max() < x1)) {
      continue;
    } else if ((y0 < axis_vertical.Min()) && (y1 < axis_vertical.Min())) {
      continue;
    } else if ((axis_vertical.Max() < y0) && (axis_vertical.Max() < y1)) {
      continue;
    }

    // clips points to edge of axis if necessary
    if (axis_horizontal.IsVisible(x0) == false) {
      ClipHorizontal(axis_horizontal, x1, y1, x0, y0);
    }

    if (axis_vertical.IsVisible(y0) == false) {
      ClipVertical(axis_vertical, x1, y1, x0, y0);
    }

    if (axis_horizontal.IsVisible(x1) == false) {
      ClipHorizontal(axis_horizontal, x0, y0, x1, y1);
    }

    if (axis_vertical.IsVisible(y1) == false) {
      ClipVertical(axis_vertical, x0, y0, x1, y1);
    }

    // translates to graphics coordinates
    wxCoord xg0, yg0;
    wxCoord xg1, yg1;

    xg0 = DataToGraphics(x0, axis_horizontal.Min(), axis_horizontal.Max(),
                         rc.GetWidth(), false);
    yg0 = DataToGraphics(y0, axis_vertical.Min(), axis_vertical.Max(),
                         rc.GetHeight(), true);
    xg1 = DataToGraphics(x1, axis_horizontal.Min(), axis_horizontal.Max(),
                         rc.GetWidth(), false);
    yg1 = DataToGraphics(y1, axis_vertical.Min(), axis_vertical.Max(),
                         rc.GetHeight(), true);

    // draws onto DC
    dc.DrawLine(xg0, yg0, xg1, yg1);
  }
}

const wxPen* LineRenderer2d::pen() const {
  return pen_;
}

void LineRenderer2d::set_pen(const wxPen* pen) {
  pen_ = pen;
}
