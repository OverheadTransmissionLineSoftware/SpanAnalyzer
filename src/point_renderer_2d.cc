// This is free and unencumbered software released into the public domain.
// For more information, please refer to <http://unlicense.org/>

#include "point_renderer_2d.h"

PointRenderer2d::PointRenderer2d() {
  pen_ = nullptr;
}

void PointRenderer2d::Draw(wxDC& dc, wxRect rc, const PlotAxis& axis_horizontal,
                           const PlotAxis& axis_vertical) const {
  // sets drawing pen
  dc.SetPen(*pen_);

  // casts to point dataset
  const PointDataSet2d* dataset = dynamic_cast<const PointDataSet2d*>(dataset_);

  // draws each point in dataset
  const std::list<const Point2d*>* data = dataset->data();
  for (auto iter = data->cbegin(); iter != data->cend(); iter++) {
    // gets point
    const Point2d* line = *iter;

    // gets points to draw
    double x = line->x;
    double y = line->y;

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

    // draws onto DC
    dc.DrawPoint(xg, yg);
  }
}

const wxPen* PointRenderer2d::pen() const {
  return pen_;
}

void PointRenderer2d::set_pen(const wxPen* pen) {
  pen_ = pen;
}
