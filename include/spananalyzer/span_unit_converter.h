// This is free and unencumbered software released into the public domain.
// For more information, please refer to <http://unlicense.org/>

#ifndef SPANANALYZER_SPAN_UNIT_CONVERTER_H_
#define SPANANALYZER_SPAN_UNIT_CONVERTER_H_

#include "models/base/units.h"

#include "spananalyzer/span.h"

/// \par OVERVIEW
///
/// This class converts a span between unit systems as well as unit styles.
///
/// \par RECURSION
///
/// This class supports optionally invoking member variable converters,
/// depending on whether the entire set of data needs converted or just a
/// portion of it.
class SpanUnitConverter {
 public:
  /// \brief Converts the span between unit styles.
  /// \param[in] system
  ///   The unit system.
  /// \param[in] style_from
  ///   The unit style to convert from.
  /// \param[in] style_to
  ///   The unit style to convert to.
  /// \param[in] is_recursive
  ///   An indicator that determines if member variable converters are invoked.
  /// \param[in,out] span
  ///   The span to be converted.
  /// The 'different' style units are as follows:
  ///  - linecable.constraint_limit = [N or lbs]
  ///  - linecable.spacing_attachments_ruling_span = [m or ft]
  ///  - spacing_catenary = [m or ft]
  static void ConvertUnitStyle(const units::UnitSystem& system,
                               const units::UnitStyle& style_from,
                               const units::UnitStyle& style_to,
                               const bool& is_recursive,
                               Span& span);

  /// \brief Converts the span between unit systems.
  /// \param[in] system_from
  ///   The unit system to convert from.
  /// \param[in] system_to
  ///   The unit system to convert to.
  /// \param[in] is_recursive
  ///   An indicator that determines if member variable converters are invoked.
  /// \param[in,out] span
  ///   The span to be converted.
  /// This function requires that the span is in a 'consistent' unit
  /// style.
  static void ConvertUnitSystem(const units::UnitSystem& system_from,
                                const units::UnitSystem& system_to,
                                const bool& is_recursive,
                                Span& span);
};

#endif  // SPANANALYZER_SPAN_UNIT_CONVERTER_H_
