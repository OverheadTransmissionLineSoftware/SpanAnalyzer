// This is free and unencumbered software released into the public domain.
// For more information, please refer to <http://unlicense.org/>

#include "plot_axis.h"

PlotAxis::PlotAxis() {
}

PlotAxis::PlotAxis(PlotAxis::OrientationType orientation) {
  orientation_ = orientation;
  max_ = -999999;
  min_ = 999999;
  position_center_ = -999999;
  range_ = -999999;
}

PlotAxis::~PlotAxis() {
}

bool PlotAxis::IsVisible(const double& v) const {
  return (v >= min_ && v <= max_);
}

bool PlotAxis::IsVisible(const double& v0, const double& v1) const {
  return (IsVisible(v0) || IsVisible(v1));
}

double PlotAxis::Max() const {
  return max_;
}

double PlotAxis::Min() const {
  return min_;
}

double PlotAxis::position_center() const {
  return position_center_;
}

double PlotAxis::range() const {
  return range_;
}

void PlotAxis::set_position_center(const double& position_center) {
  position_center_ = position_center;

  min_ = position_center_ - (range_ / 2);
  max_ = position_center_ + (range_ / 2);
}

void PlotAxis::set_range(const double& range) {
  range_ = range;

  min_ = position_center_ - (range_ / 2);
  max_ = position_center_ + (range_ / 2);
}
