// This is free and unencumbered software released into the public domain.
// For more information, please refer to <http://unlicense.org/>

#include "spananalyzer/span_unit_converter.h"

#include "appcommon/units/line_cable_unit_converter.h"
#include "wx/wx.h"

bool SpanUnitConverter::ConvertUnitStyleToConsistent(
    const int& version,
    const units::UnitSystem& system,
    const bool& is_recursive,
    Span& span) {
  bool status = true;

  // sends to proper converter function
  if (version == 0) {
    // points to latest converter version
    ConvertUnitStyleToConsistentV1(system, is_recursive, span);
  } else if (version == 1) {
    ConvertUnitStyleToConsistentV1(system, is_recursive, span);
  } else {
    wxString message = " Invalid version number. Aborting conversion.";
    wxLogError(message);
    status = false;
  }

  return status;
}

void SpanUnitConverter::ConvertUnitStyleToDifferent(
    const units::UnitSystem& system,
    const bool& is_recursive,
    Span& span) {
  // triggers member variable converters
  if (is_recursive == true) {
    // converts line cable
    LineCableUnitConverter::ConvertUnitStyleToDifferent(
        system,
        is_recursive,
        span.linecable);
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

void SpanUnitConverter::ConvertUnitStyleToConsistentV1(
    const units::UnitSystem& system,
    const bool& is_recursive,
    Span& span) {
  // triggers member variable converters
  if (is_recursive == true) {
    // converts line cable
    LineCableUnitConverter::ConvertUnitStyleToConsistent(
        0,
        system,
        is_recursive,
        span.linecable);
  }
}
