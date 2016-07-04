// This is free and unencumbered software released into the public domain.
// For more information, please refer to <http://unlicense.org/>

#include "plot_axis.h"

PlotAxis::PlotAxis() {
}

PlotAxis::PlotAxis(PlotAxis::OrientationType orientation) {
  orientation_ = orientation;
}

PlotAxis::~PlotAxis() {
}

bool PlotAxis::IsVisible(const double& v) const {
  return (v >= min_ && v <= max_);
}

bool PlotAxis::IsVisible(const double& v0, const double& v1) const {
  return (IsVisible(v0) || IsVisible(v1));
}

double PlotAxis::Range() const {
  return max_ - min_;
}

double PlotAxis::max() const {
  return max_;
}

double PlotAxis::min() const {
  return min_;
}

void PlotAxis::set_max(const double& max) {
  max_ = max;
}

void PlotAxis::set_min(const double& min) {
  min_ = min;
}
