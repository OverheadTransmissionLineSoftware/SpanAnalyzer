// This is free and unencumbered software released into the public domain.
// For more information, please refer to <http://unlicense.org/>

#ifndef OTLS_SPANANALYZER_LINEDATASET2D_H_
#define OTLS_SPANANALYZER_LINEDATASET2D_H_

#include <list>

#include "models/base/point.h"

/// \par OVERVIEW
///
/// This struct represents a 2D line.
struct Line2d {
  /// \var p0
  ///   The start point.
  Point2d p0;

  /// \var p1
  ///   The end point.
  Point2d p1;
};

/// \par OVERVIEW
///
/// This class represents a set of 2D lines.
class LineDataSet2d {
 public:
  /// \brief Constructor.
  LineDataSet2d();

  /// \brief Destructor.
  ~LineDataSet2d();

  /// \brief Gets the maximum x value.
  /// \return The maximum x value.
  double MaxX() const;

  /// \brief Gets the maximum y value.
  /// \return The maximum y value.
  double MaxY() const;

  /// \brief Gets the minimum x value.
  /// \return The minimum x value.
  double MinX() const;

  /// \brief Gets the minimum y value.
  /// \return The minimum y value.
  double MinY() const;

  /// \brief Gets the data.
  /// \return The data.
  const std::list<Line2d>& data() const;

  /// \brief Sets the data.
  /// \param[in] data
  ///   The data.
  void set_data(const std::list<Line2d>& data);

 private:
  /// \brief Updates the cached values.
  void Update() const;

  /// \var data_
  ///   The line data.
  std::list<Line2d> data_;

  /// \var is_updated_
  ///   An indicator that tells if the cached values have been updated.
  mutable bool is_updated_;

  /// \var x_max_
  ///   The maximum x value.
  mutable double x_max_;

  /// \var x_min_
  ///   The minimum x value.
  mutable double x_min_;

  /// \var y_max_
  ///   The maximum y value.
  mutable double y_max_;

  /// \var y_min_
  ///   The minimum value.
  mutable double y_min_;
};

#endif //  OTLS_SPANANALYZER_LINEDATASET2D_H_
