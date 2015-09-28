// This is free and unencumbered software released into the public domain.
// For more information, please refer to <http://unlicense.org/>

#ifndef OTLS_SPANANALYZER_CABLEUNITCONVERTER_H_
#define OTLS_SPANANALYZER_CABLEUNITCONVERTER_H_

#include "models/base/units.h"
#include "models/transmissionline/cable.h"

/// \par OVERVIEW
///
/// Unit styles can either be consistent or different.  When consistent, all
/// values use one common unit for each measurement type. Ex: length/distance
/// is always represented in meters. When different, values can be represented
/// in the unit that is most measurable. Ex: length/distance can be measured
/// in meters or millimeters, based on the size of the object.
/// 
/// This is needed because calculation libraries require consistent units, but
/// user interface controls and generated XML files need to be in convenient,
/// understandable units.
///
/// The consistent style units are as follows:
///   - Length = [meters or feet]
///   - Load = [N or lbs]
///   - Temperature = [deg C or deg F]
///
/// The different style units vary for every struct, depending on how the user
/// typically prefers to see the information represented.
enum class UnitStyle {
  kConsistent,
  kDifferent
};

/// \par OVERVIEW
///
/// This class converts a cable component between unit systems as well as unit
/// styles.
class CableComponentUnitConverter {
 public:
  /// \brief Constructor.
  CableComponentUnitConverter();

  /// \brief Destructor.
  ~CableComponentUnitConverter();

  /// \brief Converts the component loads to virtual stresses. This is needed
  ///   for compatibility with industry-standard polynomials.
  /// \param[in] area_virtual
  ///   The virtual area of the component when converting to stress.
  /// \param[in,out] component
  ///   The component to be converted.
  /// This function requires a consistent unit style.
  static void ConvertLoadToStress(const double& area_virtual,
                                  CableComponent& component);

  /// \brief Converts the component virtual stresses to loads. This is needed
  ///   for compatibility with industry-standard polynomials.
  /// \param[in] area_virtual
  ///   The virtual area of the component when converting to stress.
  /// \param[in,out] component
  ///   The component to be converted.
  static void ConvertStressToLoad(const double& area_virtual,
                                  CableComponent& component);

  /// \brief Changes the cable between unit styles.
  /// \param[in] system
  ///   The unit system.
  /// \param[in] style_from
  ///   The unit style to convert from.
  /// \param[in] style_to
  ///   The unit style to convert to.
  /// \param[in,out] component
  ///   The component to be converted.
  /// The 'different' style units are as follows:
  ///  - coefficient_expansion_linear_thermal = [??? or /degF]
  ///  - coefficients_polynomial_creep = [??? or lbs]
  ///  - coefficients_polynomial_loadstrain = [??? or lbs]
  ///  - load_limit_polynomial_creep = [??? or lbs]
  ///  - load_limit_polynomial_loadstrain = [??? or lbs]
  ///  - modulus_compression_elastic_area = [??? or lbs]
  ///  - modulus_tension_elastic_area = [??? or lbs]
  /// This function requires that the component be represented by loads, not
  /// stresses.
  static void ConvertUnitStyle(const units::UnitSystem& system,
                               const UnitStyle& style_from,
                               const UnitStyle& style_to,
                               CableComponent& component);

  /// \brief Changes the cable between unit systems.
  /// \param[in] system_from
  ///   The unit system to convert from. These must be consistent style units.
  /// \param[in] system_to
  ///   The unit system to convert to. These will also be in consistent style
  ///   units.
  /// \param[in,out] cable
  ///   The cable to be converted.
  static void ConvertUnitSystem(const units::UnitSystem& system_from,
                                const units::UnitSystem& system_to,
                                Cable& cable);
};

/// \par OVERVIEW
///
/// This class converts a cable between unit systems as well as unit styles.
class CableUnitConverter {
 public:
  /// \brief Constructor.
  CableUnitConverter();

  /// \brief Destructor.
  ~CableUnitConverter();

  /// \brief Changes the cable between unit styles.
  /// \param[in] system
  ///   The unit system.
  /// \param[in] style_from
  ///   The unit style to convert from.
  /// \param[in] style_to
  ///   The unit style to convert to.
  /// \param[in,out] cable
  ///   The cable to be converted.
  /// The 'different' style units are as follows:
  ///  - area_electrical = [??? or kcmil]
  ///  - area_physical = [??? or in^2]
  ///  - diameter = [??? or in]
  ///  - strength_rated = [??? or lbs]
  ///  - temperature_properties_components = [??? or degF]
  ///  - weight_unit = [/// or lbs/ft]
  static void ConvertUnitStyle(const units::UnitSystem& system,
                               const UnitStyle& style_from,
                               const UnitStyle& style_to,
                               Cable& cable);

  /// \brief Changes the cable between unit systems.
  /// \param[in] system_from
  ///   The unit system to convert from. These must be consistent style units.
  /// \param[in] system_to
  ///   The unit system to convert to. These will also be in consistent style
  ///   units.
  /// \param[in,out] cable
  ///   The cable to be converted.
  static void ConvertUnitSystem(const units::UnitSystem& system_from,
                                const units::UnitSystem& system_to,
                                Cable& cable);
};

#endif  // OTLS_SPANANALYZER_CABLEUNITCONVERTER_H_