// This is free and unencumbered software released into the public domain.
// For more information, please refer to <http://unlicense.org/>

#include "weather_load_case_unit_converter.h"

WeatherLoadCaseUnitConverter::WeatherLoadCaseUnitConverter() {
}

WeatherLoadCaseUnitConverter::~WeatherLoadCaseUnitConverter() {
}

void WeatherLoadCaseUnitConverter::ConvertUnitStyle(
    const units::UnitSystem& system,
    const units::UnitStyle& style_from,
    const units::UnitStyle& style_to,
    WeatherLoadCase& weathercase) {
  if (system == units::UnitSystem::kMetric) {
    /// \todo need to convert metric units
  } else if (system == units::UnitSystem::kImperial) {
    if (style_to == units::UnitStyle::kConsistent) {
      weathercase.thickness_ice = units::Convert(
          weathercase.thickness_ice,
          units::ConversionType::kInchesToFeet);
    } else if (style_to == units::UnitStyle::kDifferent) {
      weathercase.thickness_ice = units::Convert(
          weathercase.thickness_ice,
          units::ConversionType::kFeetToInches);
    }
  }
}

void WeatherLoadCaseUnitConverter::ConvertUnitSystem(
    const units::UnitSystem& system_from,
    const units::UnitSystem& system_to,
    WeatherLoadCase& weathercase) {
  /// \todo convert between unit systems
}

