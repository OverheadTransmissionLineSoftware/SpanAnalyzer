// This is free and unencumbered software released into the public domain.
// For more information, please refer to <http://unlicense.org/>

#include "spananalyzer/span_unit_converter.h"

#include "appcommon/units/line_cable_unit_converter.h"

void SpanUnitConverter::ConvertUnitStyle(
    const units::UnitSystem& system,
    const units::UnitStyle& style_from,
    const units::UnitStyle& style_to,
    const bool& is_recursive,
    Span& span) {
  // triggers member variable converters
  if (is_recursive == true) {
    // converts line cable
    LineCableUnitConverter::ConvertUnitStyle(system, style_from, style_to,
                                             is_recursive, span.linecable);
  }
}

void SpanUnitConverter::ConvertUnitSystem(
    const units::UnitSystem& system_from,
    const units::UnitSystem& system_to,
    const bool& is_recursive,
    Span& span) {
  if (system_to == units::UnitSystem::kMetric) {
    // converts spacing-attachments
    Vector3d& spacing_attachments =  span.spacing_attachments;
    spacing_attachments.set_x(units::ConvertLength(
        spacing_attachments.x(),
        units::LengthConversionType::kFeetToMeters));
    spacing_attachments.set_y(units::ConvertLength(
        spacing_attachments.y(),
        units::LengthConversionType::kFeetToMeters));
    spacing_attachments.set_z(units::ConvertLength(
        spacing_attachments.z(),
        units::LengthConversionType::kFeetToMeters));

  } else if (system_to == units::UnitSystem::kImperial) {
    // converts spacing-attachments
    Vector3d& spacing_attachments =  span.spacing_attachments;
    spacing_attachments.set_x(units::ConvertLength(
        spacing_attachments.x(),
        units::LengthConversionType::kMetersToFeet));
    spacing_attachments.set_y(units::ConvertLength(
        spacing_attachments.y(),
        units::LengthConversionType::kMetersToFeet));
    spacing_attachments.set_z(units::ConvertLength(
        spacing_attachments.z(),
        units::LengthConversionType::kMetersToFeet));
  }

  // triggers member variable converters
  if (is_recursive == true) {
    // converts line cable
    LineCableUnitConverter::ConvertUnitSystem(system_from, system_to,
                                              is_recursive, span.linecable);
  }
}
