// This is free and unencumbered software released into the public domain.
// For more information, please refer to <http://unlicense.org/>

#include "point_data_set_2d.h"

#include <algorithm>

PointDataSet2d::PointDataSet2d() {
  is_updated_ = false;
}

PointDataSet2d::~PointDataSet2d() {
  Clear();
}

void PointDataSet2d::Add(const Point2d* point) {
  data_.push_back(point);

  is_updated_ = false;
}

void PointDataSet2d::Clear() {
  for (auto iter = data_.begin(); iter != data_.end(); iter++) {
    const Point2d* point = *iter;
    delete point;
  }

  data_.clear();

  is_updated_ = false;
}

double PointDataSet2d::MaxX() const {
  if (is_updated_ == false) {
    Update();
  }

  return x_max_;
}

double PointDataSet2d::MaxY() const {
  if (is_updated_ == false) {
    Update();
  }

  return y_max_;
}

double PointDataSet2d::MinX() const {
  if (is_updated_ == false) {
    Update();
  }

  return x_min_;
}

double PointDataSet2d::MinY() const {
  if (is_updated_ == false) {
    Update();
  }

  return y_min_;
}

const std::list<const Point2d*>* PointDataSet2d::data() const {
  return &data_;
}

void PointDataSet2d::Update() const {
  // initializes
  x_min_ = 999999;
  x_max_ = -999999;

  y_min_ = 999999;
  y_max_ = -999999;

  // checks if data exists, exits if not
  if (data_.empty() == true) {
    return;
  }

  for (auto iter = data_.cbegin(); iter != data_.cend(); iter++) {
    const Point2d* point = *iter;

    x_min_ = std::min(point->x, x_min_);
    x_max_ = std::max(point->x, x_max_);

    y_min_ = std::min(point->y, y_min_);
    y_max_ = std::max(point->y, y_max_);
  }

  is_updated_ = true;
}
