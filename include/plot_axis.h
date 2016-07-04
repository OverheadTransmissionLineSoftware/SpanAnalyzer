// This is free and unencumbered software released into the public domain.
// For more information, please refer to <http://unlicense.org/>

#ifndef OTLS_SPANANALYZER_PLOTAXIS_H_
#define OTLS_SPANANALYZER_PLOTAXIS_H_

/// \par OVERVIEW
///
/// This is a helper class that keeps track of the data range being rendered.
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

  /// \brief Determines whether the either of points are visible.
  /// \param[in] v0
  ///   The starting value.
  /// \param[in] v1
  ///   The ending value.
  /// \return If any of the values are visible on the axis.
  bool IsVisible(const double& v0, const double& v1) const;

  /// \brief Gets the range (distance) between min/max values.
  /// \return The range (distance) between min/max values.
  double Range() const;

  /// \brief Gets the maximum rendered value on the axis.
  /// \return The maximum rendered value.
  double max() const;

  /// \brief Gets the minimum rendered value on the axis.
  /// \return The minimum rendered value on the axis.
  double min() const;

  /// \brief Sets the maximum rendered value on the axis.
  /// \param[in] max
  ///   The maximum value.
  void set_max(const double& max);

  /// \brief Sets the minimum rendered value on the axis.
  /// \param[in] min
  ///   The minimum value.
  void set_min(const double& min);

 private:
  /// \var orientation_
  ///   The type of orientation.
  OrientationType orientation_;

  /// \var max_
  ///   The maximum value to be rendered on the axis.
  double max_;

  /// \var min_
  ///   The minimum value to be rendered on the axis.
  double min_;
};

#endif //  OTLS_SPANANALYZER_PLOTAXIS_H_
