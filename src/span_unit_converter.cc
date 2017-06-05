// This is free and unencumbered software released into the public domain.
// For more information, please refer to <http://unlicense.org/>

#include "span_unit_converter.h"

#include "appcommon/units/line_cable_unit_converter.h"

SpanUnitConverter::SpanUnitConverter() {
}

SpanUnitConverter::~SpanUnitConverter() {
}

void SpanUnitConverter::ConvertUnitStyle(
    const units::UnitSystem& system,
    const units::UnitStyle& style_from,
    const units::UnitStyle& style_to,
    Span& span) {
  // converts line cable
  LineCableUnitConverter::ConvertUnitStyle(system, style_from, style_to,
                                           span.linecable);
}

void SpanUnitConverter::ConvertUnitSystem(
    const units::UnitSystem& system_from,
    const units::UnitSystem& system_to,
    Span& span) {
  if (system_to == units::UnitSystem::kMetric) {
    // converts catenary spacing
    Vector3d& spacing_catenary =  span.spacing_catenary;
    spacing_catenary.set_x(units::ConvertLength(
        spacing_catenary.x(),
        units::LengthConversionType::kFeetToMeters));
    spacing_catenary.set_y(units::ConvertLength(
        spacing_catenary.y(),
        units::LengthConversionType::kFeetToMeters));
    spacing_catenary.set_z(units::ConvertLength(
        spacing_catenary.z(),
        units::LengthConversionType::kFeetToMeters));

  } else if (system_to == units::UnitSystem::kImperial) {
    // converts catenary spacing
    Vector3d& spacing_catenary =  span.spacing_catenary;
    spacing_catenary.set_x(units::ConvertLength(
        spacing_catenary.x(),
        units::LengthConversionType::kMetersToFeet));
    spacing_catenary.set_y(units::ConvertLength(
        spacing_catenary.y(),
        units::LengthConversionType::kMetersToFeet));
    spacing_catenary.set_z(units::ConvertLength(
        spacing_catenary.z(),
        units::LengthConversionType::kMetersToFeet));
  }

  // converts line cable
  LineCableUnitConverter::ConvertUnitSystem(system_from, system_to,
                                            span.linecable);
}
