// This is free and unencumbered software released into the public domain.
// For more information, please refer to <http://unlicense.org/>

#include "data_set_2d.h"

DataSet2d::DataSet2d() {
  x_max_ = -999999;
  x_min_ = 999999;

  y_max_ = -999999;
  y_min_ = 999999;
}

DataSet2d::~DataSet2d() {
}