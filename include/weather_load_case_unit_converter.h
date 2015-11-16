// This is free and unencumbered software released into the public domain.
// For more information, please refer to <http://unlicense.org/>

#ifndef OTLS_SPANANALYZER_WEATHERLOADCASEUNITCONVERTER_H_
#define OTLS_SPANANALYZER_WEATHERLOADCASEUNITCONVERTER_H_

#include "models/base/units.h"
#include "models/transmissionline/weather_load_case.h"

/// \par OVERVIEW
///
/// This class converts a weather case between unit systems as well as unit
/// styles.
class WeatherLoadCaseUnitConverter {
 public:
  /// \brief Constructor.
  WeatherLoadCaseUnitConverter();
  
  /// \brief Destructor.
  ~WeatherLoadCaseUnitConverter();

  /// \brief Changes the weather case between unit styles.
  /// \param[in] system
  ///   The unit system.
  /// \param[in] style_from
  ///   The unit style to convert from.
  /// \param[in] style_to
  ///   The unit style to convert to.
  /// \param[in,out] weathercase
  ///   The weather case to be converted.
  /// The 'different' style units are as follows:
  ///  - density_ice = [??? or feet]
  ///  - pressure_wind = [??? or lbs/ft^2]
  ///  - temperature_cable = [??? or deg F]
  ///  - thickness_ice = [??? or in]
  static void ConvertUnitStyle(const units::UnitSystem& system,
                               const units::UnitStyle& style_from,
                               const units::UnitStyle& style_to,
                               WeatherLoadCase& weathercase);

  /// \brief Changes the weather case between unit systems.
  /// \param[in] system_from
  ///   The unit system to convert from. These must be consistent style units.
  /// \param[in] system_to
  ///   The unit system to convert to. These will also be in consistent style
  ///   units.
  /// \param[in,out] weathercase
  ///   The weather case to be converted.
  static void ConvertUnitSystem(const units::UnitSystem& system_from,
                                const units::UnitSystem& system_to,
                                WeatherLoadCase& weathercase);
};

#endif  // OTLS_SPANANALYZER_WEATHERLOADCASEUNITCONVERTER_H_