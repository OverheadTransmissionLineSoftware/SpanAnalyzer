// This is free and unencumbered software released into the public domain.
// For more information, please refer to <http://unlicense.org/>

#ifndef OTLS_SPANANALYZER_PLOTAXIS_H_
#define OTLS_SPANANALYZER_PLOTAXIS_H_

/// \par OVERVIEW
///
/// This class keeps track of the data values that are visible along a single
/// axis (either vertical or horizontal).
class PlotAxis {
 public:
  /// The plot axis orientation type.
  enum class OrientationType {
    kHorizontal,
    kVertical
  };

  /// \brief Default constructor.
  PlotAxis();

  /// \brief Alternate (preferred) constructor.
  PlotAxis(OrientationType orientation);

  /// \brief Destructor.
  ~PlotAxis();

  /// \brief Determines if the point is visible.
  /// \param[in] v
  ///   The value to check.
  /// \return If the point is visible on the axis.
  bool IsVisible(const double& v) const;

  /// \brief Gets the maximum rendered data value.
  /// \return The maximum rendered data value.
  double Max() const;

  /// \brief Gets the minimum rendered data value.
  /// \return The minimum rendered data value.
  double Min() const;

  /// \brief Gets the center position value.
  /// \return The center position.
  double position_center() const;

  /// \brief Gets the range (distance) between min/max values.
  /// \return The range (distance) between min/max values.
  double range() const;

  /// \brief Sets the center position.
  /// \param[in] position_center
  ///   The center position.
  void set_position_center(const double& position_center);

  /// \brief Sets the range.
  /// \param[in] range
  ///   The range.
  void set_range(const double& range);

 private:
  /// \var max_
  ///   The maximum data value to be rendered on the axis.
  double max_;

  /// \var min_
  ///   The minimum data value to be rendered on the axis.
  double min_;

  /// \var orientation_
  ///   The type of orientation.
  OrientationType orientation_;

  /// \var position_center_
  ///   The data position/value at the center of the axis.
  double position_center_;

  /// \var range_
  ///   The distance between the max/min data values, centered on the position.
  double range_;
};

#endif //  OTLS_SPANANALYZER_PLOTAXIS_H_
