// This is free and unencumbered software released into the public domain.
// For more information, please refer to <http://unlicense.org/>

#include "span_unit_converter.h"

SpanUnitConverter::SpanUnitConverter() {
}

SpanUnitConverter::~SpanUnitConverter() {
}

void SpanUnitConverter::ConvertUnitStyle(
    const units::UnitSystem& system,
    const units::UnitStyle& style_from,
    const units::UnitStyle& style_to,
    Span& span) {
  // nothing to do!
  // this function is kept for consistency with other unit converters, and
  // to show that nothing was overlooked
}

void SpanUnitConverter::ConvertUnitSystem(
    const units::UnitSystem& system_from,
    const units::UnitSystem& system_to,
    Span& span) {
  if (system_to == units::UnitSystem::kMetric) {
    span.linecable.constraint.limit = units::ConvertForce(
        span.linecable.constraint.limit,
        units::ForceConversionType::kPoundsToNewtons);

    Vector3d& spacing_ruling_span =  span.linecable.spacing_attachments_ruling_span;
    spacing_ruling_span.set_x(units::ConvertLength(
        spacing_ruling_span.x(),
        units::LengthConversionType::kFeetToMeters));
    spacing_ruling_span.set_y(units::ConvertLength(
        spacing_ruling_span.y(),
        units::LengthConversionType::kFeetToMeters));
    spacing_ruling_span.set_z(units::ConvertLength(
        spacing_ruling_span.z(),
        units::LengthConversionType::kFeetToMeters));

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
    span.linecable.constraint.limit = units::ConvertForce(
        span.linecable.constraint.limit,
        units::ForceConversionType::kNewtonsToPounds);

    Vector3d& spacing_ruling_span =  span.linecable.spacing_attachments_ruling_span;
    spacing_ruling_span.set_x(units::ConvertLength(
        spacing_ruling_span.x(),
        units::LengthConversionType::kMetersToFeet));
    spacing_ruling_span.set_y(units::ConvertLength(
        spacing_ruling_span.y(),
        units::LengthConversionType::kMetersToFeet));
    spacing_ruling_span.set_z(units::ConvertLength(
        spacing_ruling_span.z(),
        units::LengthConversionType::kMetersToFeet));

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
}
