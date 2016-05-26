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
    if (style_to == units::UnitStyle::kConsistent) {
      weathercase.thickness_ice = units::ConvertLength(
          weathercase.thickness_ice,
          units::LengthConversionType::kCentimetersToMeters);
    } else if (style_to == units::UnitStyle::kDifferent) {
      weathercase.thickness_ice = units::ConvertLength(
          weathercase.thickness_ice,
          units::LengthConversionType::kMetersToCentimeters);
    }
  } else if (system == units::UnitSystem::kImperial) {
    if (style_to == units::UnitStyle::kConsistent) {
      weathercase.thickness_ice = units::ConvertLength(
          weathercase.thickness_ice,
          units::LengthConversionType::kInchesToFeet);
    } else if (style_to == units::UnitStyle::kDifferent) {
      weathercase.thickness_ice = units::ConvertLength(
          weathercase.thickness_ice,
          units::LengthConversionType::kFeetToInches);
    }
  }
}

void WeatherLoadCaseUnitConverter::ConvertUnitSystem(
    const units::UnitSystem& system_from,
    const units::UnitSystem& system_to,
    WeatherLoadCase& weathercase) {
  if (system_to == units::UnitSystem::kMetric) {
    weathercase.density_ice = units::ConvertForce(
        weathercase.density_ice,
        units::ForceConversionType::kPoundsToNewtons);
    weathercase.density_ice = units::ConvertLength(
        weathercase.density_ice,
        units::LengthConversionType::kFeetToMeters);

    weathercase.pressure_wind = units::ConvertStress(
        weathercase.pressure_wind,
        units::StressConversionType::kPsfToPascal);
    weathercase.temperature_cable = units::ConvertTemperature(
        weathercase.temperature_cable,
        units::TemperatureConversionType::kFahrenheitToCelsius);
    weathercase.thickness_ice = units::ConvertLength(
        weathercase.thickness_ice,
        units::LengthConversionType::kFeetToMeters);
  } else if (system_to == units::UnitSystem::kImperial) {
    weathercase.density_ice = units::ConvertForce(
        weathercase.density_ice,
        units::ForceConversionType::kNewtonsToPounds);
    weathercase.density_ice = units::ConvertLength(
        weathercase.density_ice,
        units::LengthConversionType::kMetersToFeet);

    weathercase.pressure_wind = units::ConvertStress(
        weathercase.pressure_wind,
        units::StressConversionType::kPascalToPsf);
    weathercase.temperature_cable = units::ConvertTemperature(
        weathercase.temperature_cable,
        units::TemperatureConversionType::kCelsiusToFahrenheit);
    weathercase.thickness_ice = units::ConvertLength(
        weathercase.thickness_ice,
        units::LengthConversionType::kMetersToFeet);
  }
}
