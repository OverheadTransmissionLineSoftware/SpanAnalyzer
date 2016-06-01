// This is free and unencumbered software released into the public domain.
// For more information, please refer to <http://unlicense.org/>

#include "cable_unit_converter.h"

namespace {
/// \brief Converts the component coefficients load.
/// \param[in] type
///   The type of force conversion.
/// \param[in,out] coefficients
///   The coefficients to be converted.
void ConvertComponentCoefficientsLoad(
    const units::ForceConversionType& type,
    std::vector<double>& coefficients) {
  // converts stress coefficients
  for (auto iter = coefficients.begin();
       iter != coefficients.end(); iter ++) {
    double& coefficient = *iter;
    coefficient = units::ConvertForce(coefficient, type);
  }
}

/// \brief Converts the component coefficients stress.
/// \param[in] type
///   The type of stress conversion.
/// \param[in,out] coefficients
///   The coefficients to be converted.
void ConvertComponentCoefficientsStress(
    const units::StressConversionType& type,
    std::vector<double>& coefficients) {
  // converts stress coefficients
  for (auto iter = coefficients.begin();
       iter != coefficients.end(); iter ++) {
    double& coefficient = *iter;
    coefficient = units::ConvertStress(coefficient, type);
  }
}

/// \brief Converts the component loads to virtual stresses. This is needed for
///   compatibility with industry-standard polynomials.
/// \param[in] area_virtual
///   The virtual area of the component when converting to stress.
/// \param[in,out] component
///   The component to be converted.
void ConvertComponentLoadToStress(
    const double& area_virtual,
    CableComponent& component) {
  for (auto iter = component.coefficients_polynomial_creep.begin();
       iter != component.coefficients_polynomial_creep.end(); iter++) {
    double& coefficient = *iter;
    coefficient = coefficient / area_virtual;
  }

  for (auto iter = component.coefficients_polynomial_loadstrain.begin();
        iter != component.coefficients_polynomial_loadstrain.end(); iter++) {
    double& coefficient = *iter;
    coefficient = coefficient / area_virtual;
  }

  component.load_limit_polynomial_creep =
      component.load_limit_polynomial_creep / area_virtual;
  component.load_limit_polynomial_loadstrain =
      component.load_limit_polynomial_loadstrain / area_virtual;
  component.modulus_compression_elastic_area =
      component.modulus_compression_elastic_area / area_virtual;
  component.modulus_tension_elastic_area =
      component.modulus_tension_elastic_area / area_virtual;
}

/// \brief Converts the component virtual stresses to loads. This is needed
///   for compatibility with industry-standard polynomials.
/// \param[in] area_virtual
///   The virtual area of the component when converting to stress.
/// \param[in,out] component
///   The component to be converted.
void ConvertComponentStressToLoad(
    const double& area_virtual,
    CableComponent& component) {
  for (auto iter = component.coefficients_polynomial_creep.begin();
       iter != component.coefficients_polynomial_creep.end(); iter++) {
    double& coefficient = *iter;
    coefficient = coefficient * area_virtual;
  }

  for (auto iter = component.coefficients_polynomial_loadstrain.begin();
        iter != component.coefficients_polynomial_loadstrain.end(); iter++) {
    double& coefficient = *iter;
    coefficient = coefficient * area_virtual;
  }

  component.load_limit_polynomial_creep =
      component.load_limit_polynomial_creep * area_virtual;
  component.load_limit_polynomial_loadstrain =
      component.load_limit_polynomial_loadstrain * area_virtual;
  component.modulus_compression_elastic_area =
      component.modulus_compression_elastic_area * area_virtual;
  component.modulus_tension_elastic_area =
      component.modulus_tension_elastic_area * area_virtual;
}
}


CableComponentUnitConverter::CableComponentUnitConverter() {
}

CableComponentUnitConverter::~CableComponentUnitConverter() {
}

void CableComponentUnitConverter::ConvertUnitStyle(
    const units::UnitSystem& system,
    const units::UnitStyle& style_from,
    const units::UnitStyle& style_to,
    CableComponent& component) {
  if (style_from == style_to) {
    return;
  }

  // converts unit style for cable components
  if (system == units::UnitSystem::kMetric) {
    // converts cable units to consistent unit style
    if (style_to == units::UnitStyle::kConsistent) {
      ConvertComponentCoefficientsStress(
          units::StressConversionType::kMegaPascalToPascal,
          component.coefficients_polynomial_creep);
      ConvertComponentCoefficientsStress(
          units::StressConversionType::kMegaPascalToPascal,
          component.coefficients_polynomial_loadstrain);

      component.modulus_compression_elastic_area = units::ConvertStress(
          component.modulus_compression_elastic_area,
          units::StressConversionType::kMegaPascalToPascal);
      component.modulus_tension_elastic_area = units::ConvertStress(
          component.modulus_tension_elastic_area,
          units::StressConversionType::kMegaPascalToPascal);

    } else if (style_to == units::UnitStyle::kDifferent) {
    // converts cable units to different unit style
    if (style_to == units::UnitStyle::kConsistent) {
      ConvertComponentCoefficientsStress(
          units::StressConversionType::kPascalToMegaPascal,
          component.coefficients_polynomial_creep);
      ConvertComponentCoefficientsStress(
          units::StressConversionType::kPascalToMegaPascal,
          component.coefficients_polynomial_loadstrain);

      component.modulus_compression_elastic_area = units::ConvertStress(
          component.modulus_compression_elastic_area,
          units::StressConversionType::kPascalToMegaPascal);
      component.modulus_tension_elastic_area = units::ConvertStress(
          component.modulus_tension_elastic_area,
          units::StressConversionType::kPascalToMegaPascal);
      }
    }

  } else if (system == units::UnitSystem::kImperial) {
    // converts cable units to consistent unit style
    if (style_to == units::UnitStyle::kConsistent) {
      ConvertComponentCoefficientsStress(
          units::StressConversionType::kPsiToPsf,
          component.coefficients_polynomial_creep);
      ConvertComponentCoefficientsStress(
          units::StressConversionType::kPsiToPsf,
          component.coefficients_polynomial_loadstrain);

      component.load_limit_polynomial_creep = units::ConvertStress(
          component.load_limit_polynomial_creep,
          units::StressConversionType::kPsiToPsf);
      component.load_limit_polynomial_loadstrain = units::ConvertStress(
          component.load_limit_polynomial_loadstrain,
          units::StressConversionType::kPsiToPsf);
      component.modulus_compression_elastic_area = units::ConvertStress(
          component.modulus_compression_elastic_area,
          units::StressConversionType::kPsiToPsf);
      component.modulus_tension_elastic_area = units::ConvertStress(
          component.modulus_tension_elastic_area,
          units::StressConversionType::kPsiToPsf);

    } else if (style_to == units::UnitStyle::kDifferent) {
      // converts cable units to different unit style
      if (style_to == units::UnitStyle::kConsistent) {
        ConvertComponentCoefficientsStress(
            units::StressConversionType::kPsfToPsi,
            component.coefficients_polynomial_creep);
        ConvertComponentCoefficientsStress(
            units::StressConversionType::kPsfToPsi,
            component.coefficients_polynomial_loadstrain);

      component.load_limit_polynomial_creep = units::ConvertStress(
          component.load_limit_polynomial_creep,
          units::StressConversionType::kPsfToPsi);
      component.load_limit_polynomial_loadstrain = units::ConvertStress(
          component.load_limit_polynomial_loadstrain,
          units::StressConversionType::kPsfToPsi);
        component.modulus_compression_elastic_area = units::ConvertStress(
            component.modulus_compression_elastic_area,
            units::StressConversionType::kPsfToPsi);
        component.modulus_tension_elastic_area = units::ConvertStress(
            component.modulus_tension_elastic_area,
            units::StressConversionType::kPsfToPsi);
      }
    }
  }
}

void CableComponentUnitConverter::ConvertUnitSystem(
    const units::UnitSystem& system_from,
    const units::UnitSystem& system_to,
    CableComponent& component) {
  if (system_from == system_to) {
    return;
  }

  if (system_to == units::UnitSystem::kMetric) {
    // manually converting this parameter because only scaling is needed to
    // convert to the new unit system
    component.coefficient_expansion_linear_thermal =
        component.coefficient_expansion_linear_thermal * 1.8;

    ConvertComponentCoefficientsLoad(
        units::ForceConversionType::kPoundsToNewtons,
        component.coefficients_polynomial_creep);
    ConvertComponentCoefficientsLoad(
        units::ForceConversionType::kPoundsToNewtons,
        component.coefficients_polynomial_loadstrain);

    component.load_limit_polynomial_creep = units::ConvertForce(
        component.load_limit_polynomial_creep,
        units::ForceConversionType::kPoundsToNewtons);
    component.load_limit_polynomial_loadstrain = units::ConvertForce(
        component.load_limit_polynomial_loadstrain,
        units::ForceConversionType::kPoundsToNewtons);
    component.modulus_compression_elastic_area = units::ConvertForce(
        component.modulus_compression_elastic_area,
        units::ForceConversionType::kPoundsToNewtons);
    component.modulus_tension_elastic_area = units::ConvertForce(
        component.modulus_tension_elastic_area,
        units::ForceConversionType::kPoundsToNewtons);
  } else if (system_to == units::UnitSystem::kImperial) {
    // manually converting this parameter because only scaling is needed to
    // convert to the new unit system
    component.coefficient_expansion_linear_thermal =
        component.coefficient_expansion_linear_thermal / 1.8;

    ConvertComponentCoefficientsLoad(
        units::ForceConversionType::kNewtonsToPounds,
        component.coefficients_polynomial_creep);
    ConvertComponentCoefficientsLoad(
        units::ForceConversionType::kNewtonsToPounds,
        component.coefficients_polynomial_loadstrain);

    component.load_limit_polynomial_creep = units::ConvertForce(
        component.load_limit_polynomial_creep,
        units::ForceConversionType::kNewtonsToPounds);
    component.load_limit_polynomial_loadstrain = units::ConvertForce(
        component.load_limit_polynomial_loadstrain,
        units::ForceConversionType::kNewtonsToPounds);
    component.modulus_compression_elastic_area = units::ConvertForce(
        component.modulus_compression_elastic_area,
        units::ForceConversionType::kNewtonsToPounds);
    component.modulus_tension_elastic_area = units::ConvertForce(
        component.modulus_tension_elastic_area,
        units::ForceConversionType::kNewtonsToPounds);
  }
}


CableUnitConverter::CableUnitConverter() {
}

CableUnitConverter::~CableUnitConverter() {
}

void CableUnitConverter::ConvertUnitStyle(const units::UnitSystem& system,
                                          const units::UnitStyle& style_from,
                                          const units::UnitStyle& style_to,
                                          Cable& cable) {
  if (style_from == style_to) {
    return;
  }

  // converts unit style for cable
  if (system == units::UnitSystem::kMetric) {
    // converts cable units to consistent unit style
    if (style_to == units::UnitStyle::kConsistent) {
      cable.area_physical = units::ConvertLength(
          cable.area_physical,
          units::LengthConversionType::kMillimetersToMeters,
          2);
      cable.diameter = units::ConvertLength(
          cable.diameter,
          units::LengthConversionType::kMillimetersToMeters);

    // converts cable units to different unit style
    } else if (style_to == units::UnitStyle::kDifferent) {
      cable.area_physical = units::ConvertLength(
          cable.area_physical,
          units::LengthConversionType::kMetersToMillimeters,
          2);
      cable.diameter = units::ConvertLength(
          cable.diameter,
          units::LengthConversionType::kMetersToMillimeters);
    }
  } else if (system == units::UnitSystem::kImperial) {
    // converts cable units to consistent unit style
    if (style_to == units::UnitStyle::kConsistent) {
      cable.area_physical = units::ConvertLength(
          cable.area_physical,
          units::LengthConversionType::kInchesToFeet,
          2);
      cable.diameter = units::ConvertLength(
          cable.diameter,
          units::LengthConversionType::kInchesToFeet);

    // converts cable units to different unit style
    } else if (style_to == units::UnitStyle::kDifferent) {
      cable.area_physical = units::ConvertLength(
          cable.area_physical,
          units::LengthConversionType::kFeetToInches,
          2);
      cable.diameter = units::ConvertLength(
          cable.diameter,
          units::LengthConversionType::kFeetToInches);
    }
  }

  // converts unit style for cable components
  CableComponentUnitConverter::ConvertUnitStyle(system,
                                                style_from,
                                                style_to,
                                                cable.component_core);

  CableComponentUnitConverter::ConvertUnitStyle(system,
                                                style_from,
                                                style_to,
                                                cable.component_shell);

  // converts component coefficients, limits, and modulus between load/stress
  // this is unique to cables, and due to industry standard polynomials
  if (style_to == units::UnitStyle::kConsistent) {
    ConvertComponentStressToLoad(cable.area_physical,
                                 cable.component_core);
    ConvertComponentStressToLoad(cable.area_physical,
                                 cable.component_shell);
  } else if (style_to == units::UnitStyle::kDifferent) {
    ConvertComponentLoadToStress(cable.area_physical,
                                 cable.component_core);
    ConvertComponentLoadToStress(cable.area_physical,
                                 cable.component_shell);
  }
}

void CableUnitConverter::ConvertUnitSystem(const units::UnitSystem& system_from,
                                           const units::UnitSystem& system_to,
                                           Cable& cable) {
  if (system_from == system_to) {
    return;
  }

  // converts unit system for cable
  if (system_to == units::UnitSystem::kMetric) {
    cable.area_physical = units::ConvertLength(
        cable.area_physical,
        units::LengthConversionType::kFeetToMeters, 2);
    cable.diameter = units::ConvertLength(
        cable.diameter,
        units::LengthConversionType::kFeetToMeters);
    cable.strength_rated = units::ConvertForce(
        cable.strength_rated,
        units::ForceConversionType::kPoundsToNewtons);
    cable.temperature_properties_components = units::ConvertTemperature(
        cable.temperature_properties_components,
        units::TemperatureConversionType::kFahrenheitToCelsius);

    cable.weight_unit = units::ConvertForce(
        cable.weight_unit,
        units::ForceConversionType::kPoundsToNewtons);
    cable.weight_unit = units::ConvertLength(
        cable.weight_unit,
        units::LengthConversionType::kFeetToMeters, 1, false);
  } else if (system_to == units::UnitSystem::kImperial) {
    cable.area_physical = units::ConvertLength(
        cable.area_physical,
        units::LengthConversionType::kMetersToFeet, 2);
    cable.diameter = units::ConvertLength(
        cable.diameter,
        units::LengthConversionType::kMetersToFeet);
    cable.strength_rated = units::ConvertForce(
        cable.strength_rated,
        units::ForceConversionType::kNewtonsToPounds);
    cable.temperature_properties_components = units::ConvertTemperature(
        cable.temperature_properties_components,
        units::TemperatureConversionType::kCelsiusToFahrenheit);

    cable.weight_unit = units::ConvertForce(
        cable.weight_unit,
        units::ForceConversionType::kNewtonsToPounds);
    cable.weight_unit = units::ConvertLength(
        cable.weight_unit,
        units::LengthConversionType::kMetersToFeet, 1, false);
  }

  // converts unit system for cable components
  CableComponentUnitConverter::ConvertUnitSystem(system_from, system_to,
                                                 cable.component_core);
  CableComponentUnitConverter::ConvertUnitSystem(system_from, system_to,
                                                 cable.component_shell);
}
