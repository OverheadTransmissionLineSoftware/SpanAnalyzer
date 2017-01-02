// This is free and unencumbered software released into the public domain.
// For more information, please refer to <http://unlicense.org/>

#include "renderer_2d.h"

Renderer2d::Renderer2d() {
  dataset_ = nullptr;
}

Renderer2d::~Renderer2d() {
}

const DataSet2d* Renderer2d::dataset() const {
  return dataset_;
}

void Renderer2d::set_dataset(const DataSet2d* dataset) {
  dataset_ = dataset;
}

void Renderer2d::ClipHorizontal(const PlotAxis& axis,
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

void Renderer2d::ClipVertical(const PlotAxis& axis,
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

wxCoord Renderer2d::DataToGraphics(const double& value,
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
