// This is free and unencumbered software released into the public domain.
// For more information, please refer to <http://unlicense.org/>

#ifndef OTLS_SPANANALYZER_SPANUNITCONVERTER_H_
#define OTLS_SPANANALYZER_SPANUNITCONVERTER_H_

#include "models/base/units.h"

#include "span.h"

/// \par OVERVIEW
///
/// This class converts a span between unit systems as well as unit styles.
///
/// All child objects that this class owns/stores will be converted. The objects
/// that are referenced will not be converted, such as:
///  - Cable
///  - WeatherLoadCase
class SpanUnitConverter {
 public:
  /// \brief Constructor.
  SpanUnitConverter();

  /// \brief Destructor.
  ~SpanUnitConverter();

  /// \brief Converts the span between unit styles.
  /// \param[in] system
  ///   The unit system.
  /// \param[in] style_from
  ///   The unit style to convert from.
  /// \param[in] style_to
  ///   The unit style to convert to.
  /// \param[in,out] span
  ///   The span to be converted.
  /// The 'different' style units are as follows:
  ///  - linecable.constraint_limit = [N or lbs]
  ///  - linecable.spacing_attachments_ruling_span = [m or ft]
  ///  - spacing_catenary = [m or ft]
  static void ConvertUnitStyle(const units::UnitSystem& system,
                               const units::UnitStyle& style_from,
                               const units::UnitStyle& style_to,
                               Span& span);

  /// \brief Converts the span between unit systems.
  /// \param[in] system_from
  ///   The unit system to convert from.
  /// \param[in] system_to
  ///   The unit system to convert to.
  /// \param[in,out] span
  ///   The span to be converted.
  /// This function requires that the span is in a 'consistent' unit
  /// style.
  static void ConvertUnitSystem(const units::UnitSystem& system_from,
                                const units::UnitSystem& system_to,
                                Span& span);
};

#endif  // OTLS_SPANANALYZER_SPANUNITCONVERTER_H_
