// This is free and unencumbered software released into the public domain.
// For more information, please refer to <http://unlicense.org/>

#ifndef OTLS_SPANANALYZER_TEXTDATASET2D_H_
#define OTLS_SPANANALYZER_TEXTDATASET2D_H_

#include <list>
#include <string>

#include "models/base/point.h"

#include "data_set_2d.h"

/// \par OVERVIEW
///
/// This struct represents 2d text at a specific point.
struct Text2d {
  /// \par OVERVIEW
  ///
  /// This enumeration contains positions along the text boundary rectangle. The
  /// point is aligned with the position when rendering.
  enum class BoundaryPosition {
    kCenterLower,
    kCenterUpper,
    kLeftCenter,
    kLeftLower,
    kLeftUpper,
    kRightCenter,
    kRightLower,
    kRightUpper
  };

  /// \var angle
  ///   The angle (in degrees) to rotate the text. Zero is aligned horizontal.
  ///   Positive angles are rotated counter-clockwise.
  double angle;

  /// \var message
  ///   The message.
  std::string message;

  /// \var point
  ///   The text origin point, in data coordinates.
  Point2d point;

  /// \var position
  ///   The boundary position of the point.
  BoundaryPosition position;
};

/// \par OVERVIEW
///
/// This class represents a set of 2D text.
class TextDataSet2d : public DataSet2d {
 public:
  /// \brief Constructor.
  TextDataSet2d();

  /// \brief Destructor.
  virtual ~TextDataSet2d();

  /// \brief Adds text to the dataset.
  /// \param[in] text
  ///   The text.
  /// This class will take ownership of the pointer.
  void Add(const Text2d* text);

  /// \brief Clears all of the stored text.
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
  const std::list<const Text2d*>* data() const;

 private:
  /// \brief Updates the cached values.
  void Update() const;

  /// \var data_
  ///   The line data.
  std::list<const Text2d*> data_;

  /// \var is_updated_
  ///   An indicator that tells if the cached values have been updated.
  mutable bool is_updated_;
};

#endif //  OTLS_SPANANALYZER_TEXTDATASET2D_H_
