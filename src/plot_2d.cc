// This is free and unencumbered software released into the public domain.
// For more information, please refer to <http://unlicense.org/>

#include "plot_2d.h"

#include <algorithm>

Plot2d::Plot2d() {
  axis_horizontal_ = PlotAxis(PlotAxis::OrientationType::kHorizontal);
  axis_vertical_ = PlotAxis(PlotAxis::OrientationType::kVertical);
}

Plot2d::~Plot2d() {
}

void Plot2d::AddRenderer(const LineRenderer2d& renderer) {
  renderers_.push_back(renderer);

  is_updated_axes_ = false;
}

void Plot2d::ClearRenderers() {
  renderers_.clear();

  is_updated_axes_ = false;
}

void Plot2d::Redraw(wxDC& dc, wxRect rc) const {
  // sets background color and clears
  dc.SetBackgroundMode(wxSOLID);
  dc.SetBackground(brush_background_);
  dc.Clear();

  // updates axes
  if (is_updated_axes_ == false) {
    UpdatePloxAxes();
  }

  // triggers all renderers
  for (auto iter = renderers_.cbegin(); iter != renderers_.cend(); iter++) {
    const LineRenderer2d& renderer = *iter;
    renderer.Draw(dc, rc, axis_horizontal_, axis_vertical_);
  }
}

wxBrush Plot2d::background() const {
  return brush_background_;
}

void Plot2d::set_background(const wxBrush& brush) {
  brush_background_ = brush;
}

void Plot2d::UpdatePloxAxes() const {
  double x_min = 999999;
  double x_max = -999999;
  double y_min = 999999;
  double y_max = -999999;

  for (auto iter = renderers_.cbegin(); iter != renderers_.cend(); iter++) {
    const LineDataSet2d* dataset = (*iter).dataset();
    x_min = std::min(x_min, dataset->MinX());
    x_max = std::max(x_max, dataset->MaxX());

    y_min = std::min(y_min, dataset->MinY());
    y_max = std::max(y_max, dataset->MaxY());
  }

  axis_horizontal_.set_min(x_min);
  axis_horizontal_.set_max(x_max);

  axis_vertical_.set_min(y_min);
  axis_vertical_.set_max(y_max);

  is_updated_axes_ = true;
}
