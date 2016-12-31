// This is free and unencumbered software released into the public domain.
// For more information, please refer to <http://unlicense.org/>

#ifndef OTLS_SPANANALYZER_DATASET2D_H_
#define OTLS_SPANANALYZER_DATASET2D_H_

/// \par OVERVIEW
///
/// This class represents a set of 2D lines.
class DataSet2d {
 public:
  /// \brief Constructor.
  DataSet2d();

  /// \brief Destructor.
  virtual ~DataSet2d();

  /// \brief Gets the maximum x value.
  /// \return The maximum x value.
  virtual double MaxX() const = 0;

  /// \brief Gets the maximum y value.
  /// \return The maximum y value.
  virtual double MaxY() const = 0;

  /// \brief Gets the minimum x value.
  /// \return The minimum x value.
  virtual double MinX() const = 0;

  /// \brief Gets the minimum y value.
  /// \return The minimum y value.
  virtual double MinY() const = 0;

 protected:
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

#endif //  OTLS_SPANANALYZER_DATASET2D_H_
