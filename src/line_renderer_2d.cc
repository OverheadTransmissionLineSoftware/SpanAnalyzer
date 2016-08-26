// This is free and unencumbered software released into the public domain.
// For more information, please refer to <http://unlicense.org/>

#include "line_renderer_2d.h"

const LineDataSet2d* LineRenderer2d::dataset() const {
  return dataset_;
}

const wxPen* LineRenderer2d::pen() const {
  return pen_;
}

void LineRenderer2d::set_dataset(const LineDataSet2d* dataset) {
  dataset_ = dataset;
}

void LineRenderer2d::set_pen(const wxPen* pen) {
  pen_ = pen;
}

void LineRenderer2d::Draw(wxDC& dc, wxRect rc, const PlotAxis& axis_horizontal,
                          const PlotAxis& axis_vertical) const {
  // sets drawing pen
  dc.SetPen(*pen_);

  // draws each line in dataset
  const std::list<Line2d>& data = dataset_->data();
  for (auto iter = data.cbegin(); iter != data.cend(); iter++) {
    // gets line
    const Line2d& line = *iter;

    // gets points to draw
    double x0 = line.p0.x;
    double y0 = line.p0.y;
    double x1 = line.p1.x;
    double y1 = line.p1.y;

    // checks whether line end poinits are visible on axis
    if ((axis_horizontal.IsVisible(x0, x1) == false)
         || (axis_vertical.IsVisible(y0, y1) == false)) {
      continue;
    }

    /// \todo Is clipping necessary?
//    // clips points to edge of axis if necessary
//    ClipHoriz(horizAxis, x0, y0, x1, y1);
//    ClipHoriz(horizAxis, x1, y1, x0, y0);
//    ClipVert(vertAxis, x0, y0, x1, y1);
//    ClipVert(vertAxis, x1, y1, x0, y0);

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

wxCoord LineRenderer2d::DataToGraphics(const double& value,
                                       const double& value_min,
                                       const double& value_max,
                                       const int& range_graphics,
                                       const bool& is_vertical) {
  // calcs the value range
  const double range_values = value_max - value_min;

  // calculates ratio of point along axis
  double k = 0;
  if (is_vertical == true) {
    k = (value_max - value) / range_values;
  } else {
    k = (value - value_min) / range_values;
  }

  // scales to graphics range
  return k * range_graphics;
}
