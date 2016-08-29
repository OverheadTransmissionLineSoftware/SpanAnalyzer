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

void LineRenderer2d::ClipHorizontal(const PlotAxis& axis,
                                    const double& x_vis, const double& y_vis,
                                    double& x, double& y) {
  // determines axis boundary
  double b;
  if (x < axis.Min()) {
    b = axis.Min();
  } else {
    b = axis.Max();
  }

  // solves for coordinates along line that intersect the boundary
  y = (b - x_vis) * (y - y_vis) / (x - x_vis) + y_vis;
  x = b;
}

void LineRenderer2d::ClipVertical(const PlotAxis& axis,
                                  const double& x_vis, const double& y_vis,
                                  double& x, double& y) {
  // determines axis boundary
  double b;
  if (y < axis.Min()) {
    b = axis.Min();
  } else {
    b = axis.Max();
  }

  // solves for coordinates along line that intersect the boundary
  x = (b - y_vis) * (x - x_vis) / (y - y_vis) + x_vis;
  y = b;
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
