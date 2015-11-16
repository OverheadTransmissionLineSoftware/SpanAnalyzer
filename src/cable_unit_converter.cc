// This is free and unencumbered software released into the public domain.
// For more information, please refer to <http://unlicense.org/>

#include "cable_unit_converter.h"

CableComponentUnitConverter::CableComponentUnitConverter() {
}

CableComponentUnitConverter::~CableComponentUnitConverter() {
}

void CableComponentUnitConverter::ConvertLoadToStress(
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

void CableComponentUnitConverter::ConvertStressToLoad(
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

void CableComponentUnitConverter::ConvertUnitStyle(
    const units::UnitSystem& system,
    const units::UnitStyle& style_from,
    const units::UnitStyle& style_to,
    CableComponent& component) {
  if (system == units::UnitSystem::kMetric) {
    /// \todo need to convert metric units
  } else if (system == units::UnitSystem::kImperial) {
    // nothing to do, consistent unit style = different unit style 
  }
}

void CableComponentUnitConverter::ConvertUnitSystem(
    const units::UnitSystem& system_from,
    const units::UnitSystem& system_to,
    Cable& cable) {
  /// \todo convert between unit systems
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
  
  // selects based on unit system
  if (system == units::UnitSystem::kMetric) {
    /// \todo need to convert metric units
  } else if (system == units::UnitSystem::kImperial) {
    
    if (style_to == units::UnitStyle::kConsistent) {
      // converts between load/stress
      // this is unique to cables, and due to industry standard polynomials
      CableComponentUnitConverter::ConvertStressToLoad(cable.area_physical,
                                                       cable.component_core);
      CableComponentUnitConverter::ConvertStressToLoad(cable.area_physical,
                                                       cable.component_shell);
      
      cable.area_electrical = cable.area_electrical; /// \todo add conv here
      cable.area_physical = units::Convert(
          cable.area_physical,
          units::ConversionType::kInchesToFeet,
          2);
      cable.diameter = units::Convert(
          cable.diameter,
          units::ConversionType::kInchesToFeet);
    } else if (style_to == units::UnitStyle::kDifferent) {
      cable.area_electrical = cable.area_electrical; /// \todo add conv here
      cable.area_physical = units::Convert(
          cable.area_physical,
          units::ConversionType::kFeetToInches,
          2);
      cable.diameter = units::Convert(
          cable.diameter,
          units::ConversionType::kFeetToInches);
      CableComponentUnitConverter::ConvertUnitStyle(
          system,
          style_from,
          style_to,
          cable.component_core);
      CableComponentUnitConverter::ConvertUnitStyle(
          system,
          style_from,
          style_to,
          cable.component_shell);

      // converts between load/stress
      // this is unique to cables, and due to industry standard polynomials
      CableComponentUnitConverter::ConvertLoadToStress(cable.area_physical,
                                                       cable.component_core);
      CableComponentUnitConverter::ConvertLoadToStress(cable.area_physical,
                                                       cable.component_shell);
    }
  }
}

void CableUnitConverter::ConvertUnitSystem(const units::UnitSystem& system_from,
                                           const units::UnitSystem& system_to,
                                           Cable& cable) {
  /// \todo convert between unit systems
}
