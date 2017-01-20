// This is free and unencumbered software released into the public domain.
// For more information, please refer to <http://unlicense.org/>

#ifndef OTLS_SPANANALYZER_CIRCLEDATASET2D_H_
#define OTLS_SPANANALYZER_CIRCLEDATASET2D_H_

#include <list>

#include "models/base/point.h"

#include "data_set_2d.h"

/// \par OVERVIEW
///
/// This struct represents a 2D circle.
struct Circle2d {
  /// \var center
  ///   The center coordinate.
  Point2d center;

  /// \var radius
  ///   The radius of the circle in graphics coordinates. This value is in
  ///   graphics coordinates so the plot scale and aspect ratio do not distort
  ///   the shape.
  unsigned int radius;
};

/// \par OVERVIEW
///
/// This class represents a set of 2D lines.
class CircleDataSet2d : public DataSet2d {
 public:
  /// \brief Constructor.
  CircleDataSet2d();

  /// \brief Destructor.
  virtual ~CircleDataSet2d();

  /// \brief Adds a circle to the dataset.
  /// \param[in] circle
  ///   The circle.
  /// This class will take ownership of the pointer.
  void Add(const Circle2d* circle);

  /// \brief Clears all of the stored circles.
  void Clear();

  /// \brief Gets the maximum x value.
  /// \return The maximum x value.
  virtual double MaxX() const override;

  /// \brief Gets the maximum y value.
  /// \return The maximum y value.
  virtual double MaxY() const override;

  /// \brief Gets the minimum x value.
  /// \return The minimum x value.
  virtual double MinX() const override;

  /// \brief Gets the minimum y value.
  /// \return The minimum y value.
  virtual double MinY() const override;

  /// \brief Gets the data.
  /// \return The data.
  const std::list<const Circle2d*>* data() const;

 private:
  /// \brief Updates the cached values.
  void Update() const;

  /// \var data_
  ///   The line data.
  std::list<const Circle2d*> data_;

  /// \var is_updated_
  ///   An indicator that tells if the cached values have been updated.
  mutable bool is_updated_;
};

#endif //  OTLS_SPANANALYZER_CIRCLEDATASET2D_H_
