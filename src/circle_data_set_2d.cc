// This is free and unencumbered software released into the public domain.
// For more information, please refer to <http://unlicense.org/>

#include "circle_data_set_2d.h"

#include <algorithm>

CircleDataSet2d::CircleDataSet2d() {
  is_updated_ = false;
}

CircleDataSet2d::~CircleDataSet2d() {
  Clear();
}

void CircleDataSet2d::Add(const Circle2d* circle) {
  data_.push_back(circle);

  is_updated_ = false;
}

void CircleDataSet2d::Clear() {
  for (auto iter = data_.begin(); iter != data_.end(); iter++) {
    const Circle2d* circle = *iter;
    delete circle;
  }

  data_.clear();

  is_updated_ = false;
}

double CircleDataSet2d::MaxX() const {
  if (is_updated_ == false) {
    Update();
  }

  return x_max_;
}

double CircleDataSet2d::MaxY() const {
  if (is_updated_ == false) {
    Update();
  }

  return y_max_;
}

double CircleDataSet2d::MinX() const {
  if (is_updated_ == false) {
    Update();
  }

  return x_min_;
}

double CircleDataSet2d::MinY() const {
  if (is_updated_ == false) {
    Update();
  }

  return y_min_;
}

const std::list<const Circle2d*>* CircleDataSet2d::data() const {
  return &data_;
}

void CircleDataSet2d::Update() const {
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
    const Circle2d* circle = *iter;

    x_min_ = std::min(circle->center.x, x_min_);
    x_max_ = std::max(circle->center.x, x_max_);

    y_min_ = std::min(circle->center.y, y_min_);
    y_max_ = std::max(circle->center.y, y_max_);
  }

  is_updated_ = true;
}
