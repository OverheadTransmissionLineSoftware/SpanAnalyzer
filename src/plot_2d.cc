// This is free and unencumbered software released into the public domain.
// For more information, please refer to <http://unlicense.org/>

#include "plot_2d.h"

#include <algorithm>

Plot2d::Plot2d() {
  axis_horizontal_ = PlotAxis(PlotAxis::OrientationType::kHorizontal);
  axis_vertical_ = PlotAxis(PlotAxis::OrientationType::kVertical);

  ratio_aspect_ = 1;

  limits_data_.x_max = -999999;
  limits_data_.x_min = 999999;
  limits_data_.y_max = -999999;
  limits_data_.y_min = 999999;

  is_updated_limits_data_ = false;
}

Plot2d::~Plot2d() {
}

void Plot2d::AddRenderer(const LineRenderer2d& renderer) {
  renderers_.push_back(renderer);

  is_updated_limits_data_ = false;
}

void Plot2d::ClearRenderers() {
  renderers_.clear();

  is_updated_limits_data_ = false;
}

void Plot2d::Redraw(wxDC& dc, wxRect rc) const {
  // sets background color and clears
  dc.SetBackgroundMode(wxSOLID);
  dc.SetBackground(brush_background_);
  dc.Clear();

  // updates plot data limits
  if (is_updated_limits_data_ == false) {
    UpdatePlotDataBoundaries();
  }

  // updates axes to fit plot data
  FitRenderAxesToPlotData(rc);

  // triggers all renderers
  for (auto iter = renderers_.cbegin(); iter != renderers_.cend(); iter++) {
    const LineRenderer2d& renderer = *iter;
    renderer.Draw(dc, rc, axis_horizontal_, axis_vertical_);
  }
}

wxBrush Plot2d::background() const {
  return brush_background_;
}

double Plot2d::ratio_aspect() const {
  return ratio_aspect_;
}

void Plot2d::set_background(const wxBrush& brush) {
  brush_background_ = brush;
}

void Plot2d::set_ratio_aspect(const double& ratio_aspect) {
  ratio_aspect_ = ratio_aspect;
}

void Plot2d::FitRenderAxesToPlotData(const wxRect& rc) const {
  // compares the aspect ratios of the render and data rectangles to determine
  // which axis is most space limited
  if (rc.GetHeight() == 0 || rc.GetWidth() == 0) {
    return;
  }

  const double kRatioAspectRect = (double)rc.GetHeight() / (double)rc.GetWidth();

  const double kWidthRender = limits_data_.x_max - limits_data_.x_min;
  const double kHeightRender = (limits_data_.y_max - limits_data_.y_min)
                               * ratio_aspect_;
  const double kRatioAspectData = kHeightRender / kWidthRender;

  double range_x;
  double range_y;
  if (kRatioAspectRect <= kRatioAspectData) {
    // vertical axis is the most limited, so horizontal range needs adjusted out
    range_x = (limits_data_.x_max - limits_data_.x_min)
              * (kRatioAspectRect / kRatioAspectData);
    range_y = limits_data_.y_max - limits_data_.y_min;
  } else {
    // horizontal axis is the most limited, so vertical range needs adjusted out
    range_x = limits_data_.x_max - limits_data_.x_min;
    range_y = (limits_data_.y_max - limits_data_.y_min)
              * (kRatioAspectRect / kRatioAspectData);
  }

  // updates axis ranges
  axis_horizontal_.set_range(range_x);
  axis_vertical_.set_range(range_y);

  // updates axis center positions
  axis_horizontal_.set_position_center(
      (limits_data_.x_min + limits_data_.x_max) / 2);
  axis_vertical_.set_position_center(
      (limits_data_.y_min + limits_data_.y_max) / 2);
}

void Plot2d::UpdatePlotDataBoundaries() const {
  // searches all the datasets and finds the min/max values for each axis
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

  limits_data_.x_min = x_min;
  limits_data_.x_max = x_max;
  limits_data_.y_min = y_min;
  limits_data_.y_max = y_max;

  is_updated_limits_data_ = true;
}
