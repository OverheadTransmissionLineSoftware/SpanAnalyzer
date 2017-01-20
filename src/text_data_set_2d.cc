// This is free and unencumbered software released into the public domain.
// For more information, please refer to <http://unlicense.org/>

#include "text_data_set_2d.h"

#include <algorithm>

TextDataSet2d::TextDataSet2d() {
  is_updated_ = false;
}

TextDataSet2d::~TextDataSet2d() {
  Clear();
}

void TextDataSet2d::Add(const Text2d* text) {
  data_.push_back(text);

  is_updated_ = false;
}

void TextDataSet2d::Clear() {
  for (auto iter = data_.begin(); iter != data_.end(); iter++) {
    const Text2d* text = *iter;
    delete text;
  }

  data_.clear();

  is_updated_ = false;
}

double TextDataSet2d::MaxX() const {
  if (is_updated_ == false) {
    Update();
  }

  return x_max_;
}

double TextDataSet2d::MaxY() const {
  if (is_updated_ == false) {
    Update();
  }

  return y_max_;
}

double TextDataSet2d::MinX() const {
  if (is_updated_ == false) {
    Update();
  }

  return x_min_;
}

double TextDataSet2d::MinY() const {
  if (is_updated_ == false) {
    Update();
  }

  return y_min_;
}

const std::list<const Text2d*>* TextDataSet2d::data() const {
  return &data_;
}

void TextDataSet2d::Update() const {
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
    const Text2d* text = *iter;

    x_min_ = std::min(text->point.x, x_min_);
    x_max_ = std::max(text->point.x, x_max_);

    y_min_ = std::min(text->point.y, y_min_);
    y_max_ = std::max(text->point.y, y_max_);
  }

  is_updated_ = true;
}
