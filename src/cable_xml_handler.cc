// This is free and unencumbered software released into the public domain.
// For more information, please refer to <http://unlicense.org/>

#include "cable_xml_handler.h"

CableComponentXmlHandler::CableComponentXmlHandler() {
}

CableComponentXmlHandler::~CableComponentXmlHandler() {
}

wxXmlNode* CableComponentXmlHandler::CreateNode(
    const CableComponent& component,
    const std::string& name) {
  // creates a node for the cable component
  wxXmlNode* node_component = new wxXmlNode(wxXML_ELEMENT_NODE,
                                            "cable_component");
  node_component->AddAttribute("name", wxString(name));
  node_component->AddAttribute("version", "1");

  // adds child nodes for struct parameters

  // creates coefficient-expansion-linear-thermal node and adds to component
  // node
  wxXmlNode* node_thermal_coefficient =
      new wxXmlNode(wxXML_ELEMENT_NODE, "coefficient_expansion_linear_thermal");
  wxXmlNode* data_thermal_coefficient = 
      new wxXmlNode(wxXML_TEXT_NODE, wxEmptyString, std::to_string(
                    component.coefficient_expansion_linear_thermal));
  node_thermal_coefficient->AddChild(data_thermal_coefficient);
  node_thermal_coefficient->AddAttribute("units", "/deg");
  node_component->AddChild(node_thermal_coefficient);

  // creates modulus-compression-elastic-area node and adds to component node
  wxXmlNode* node_modulus_compression_elastic_area = 
      new wxXmlNode(wxXML_ELEMENT_NODE, "modulus_compression_elastic_area");
  wxXmlNode* data_modulus_compression_elastic_area =
      new wxXmlNode(wxXML_TEXT_NODE, wxEmptyString, std::to_string(
                    component.modulus_tension_elastic_area));
  node_modulus_compression_elastic_area->AddChild(
      data_modulus_compression_elastic_area);
  node_modulus_compression_elastic_area->AddAttribute("units", "lbs");
  node_component->AddChild(node_modulus_compression_elastic_area);

  // creates modulus-tension-elastic-area node and adds to component node
  wxXmlNode* node_modulus_tension_elastic_area = 
      new wxXmlNode(wxXML_ELEMENT_NODE, "modulus_tension_elastic_area");
  wxXmlNode* data_modulus_tension_elastic_area =
      new wxXmlNode(wxXML_TEXT_NODE, wxEmptyString, std::to_string(
                    component.modulus_tension_elastic_area));
  node_modulus_tension_elastic_area->AddChild(
      data_modulus_tension_elastic_area);
  node_modulus_tension_elastic_area->AddAttribute("units", "lbs");
  node_component->AddChild(node_modulus_tension_elastic_area);

  // creates coefficients-loadstrain node and adds to component node
  wxXmlNode* node_coefficients_loadstrain =
      new wxXmlNode(wxXML_ELEMENT_NODE, "coefficients_loadstrain");
  node_coefficients_loadstrain->AddAttribute("type", "loadstrain");

  for (size_t iter = 0; 
       iter < component.coefficients_polynomial_loadstrain.size();
       iter++ ) {

    wxXmlNode* node_coefficient = new wxXmlNode();
    node_coefficient->SetType(wxXML_ELEMENT_NODE);

    if (iter == 0) {
        node_coefficient->SetName("a0");
    } else if (iter == 1) {
        node_coefficient->SetName("a1");
    } else if (iter == 2) {
        node_coefficient->SetName("a2");
    } else if (iter == 3) {
        node_coefficient->SetName("a3");
    } else if (iter == 4) {
        node_coefficient->SetName("a4");
    }

    wxXmlNode* data_coefficient =
        new wxXmlNode(wxXML_TEXT_NODE, wxEmptyString, std::to_string(
                      component.coefficients_polynomial_loadstrain.at(iter)));
    node_coefficient->AddChild(data_coefficient);
    node_coefficient->AddAttribute("units", "lbs");

    node_coefficients_loadstrain->AddChild(node_coefficient);
  }
  node_component->AddChild(node_coefficients_loadstrain);

  // creates load-limit-polynomial-loadstrain node and adds to component node
  wxXmlNode* node_load_limit_polynomial_loadstrain = 
      new wxXmlNode(wxXML_ELEMENT_NODE, "load_limit_polynomial_loadstrain");
  wxXmlNode* data_load_limit_polynomial_loadstrain =
      new wxXmlNode(wxXML_TEXT_NODE, wxEmptyString, std::to_string(
                    component.modulus_tension_elastic_area));
  node_load_limit_polynomial_loadstrain->AddChild(
      data_load_limit_polynomial_loadstrain);
  node_load_limit_polynomial_loadstrain->AddAttribute("units", "lbs");
  node_component->AddChild(node_load_limit_polynomial_loadstrain);

  // creates coefficients-creep node and adds to component node
  wxXmlNode* node_coefficients_creep =
      new wxXmlNode(wxXML_ELEMENT_NODE, "coefficients_creep");
  node_coefficients_creep->AddAttribute("type", "creep");

  for (size_t iter = 0;
       iter < component.coefficients_polynomial_creep.size();
       iter++ ) {

    wxXmlNode* node_coefficient = new wxXmlNode();
    node_coefficient->SetType(wxXML_ELEMENT_NODE);

    if (iter == 0) {
        node_coefficient->SetName("b0");
    } else if (iter == 1) {
        node_coefficient->SetName("b1");
    } else if (iter == 2) {
        node_coefficient->SetName("b2");
    } else if (iter == 3) {
        node_coefficient->SetName("b3");
    } else if (iter == 4) {
        node_coefficient->SetName("b4");
    }

    wxXmlNode* data_coefficient =
        new wxXmlNode(wxXML_TEXT_NODE, wxEmptyString, std::to_string(
                      component.coefficients_polynomial_creep.at(iter)));
    node_coefficient->AddChild(data_coefficient);
    node_coefficient->AddAttribute("units", "lbs");

    node_coefficients_creep->AddChild(node_coefficient);
  }
  node_component->AddChild(node_coefficients_creep);

  // creates load-limit-polynomial-creep node and adds to component node
  wxXmlNode* node_load_limit_polynomial_creep = 
      new wxXmlNode(wxXML_ELEMENT_NODE, "load_limit_polynomial_creep");
  wxXmlNode* data_load_limit_polynomial_creep =
      new wxXmlNode(wxXML_TEXT_NODE, wxEmptyString, std::to_string(
                    component.modulus_tension_elastic_area));
  node_load_limit_polynomial_creep->AddChild(
      data_load_limit_polynomial_creep);
  node_load_limit_polynomial_creep->AddAttribute("units", "lbs");
  node_component->AddChild(node_load_limit_polynomial_creep);

  // returns node
  return node_component;
}

bool CableComponentXmlHandler::ParseNode(const wxXmlNode* node, 
                                         CableComponent& component) {
  // checks for valid node
  if (node->GetName() != "cable_component") {
    return false;
  }

  // gets version attribute
  wxString version;
  if (node->GetAttribute("version", &version) == false) {
    return false;
  }

  // sends to proper parsing function
  bool success = false;

  if ( version == "1" ) {
    success = ParseNodeV1(node, component);
  }
  // <add more versions here as needed>

  return success;
}

bool CableComponentXmlHandler::ParseNodeV1(const wxXmlNode* node,
                                           CableComponent& component) {
  // declares string versions of struct members
  wxString coefficient_expansion_linear_thermal;
  std::vector<wxString> coefficients_polynomial_creep (5);
  std::vector<wxString> coefficients_polynomial_loadstrain (5);
  wxString load_limit_polynomial_creep;
  wxString load_limit_polynomial_loadstrain;
  wxString modulus_compression_elastic_area;
  wxString modulus_tension_elastic_area;

  // gets strings from XML node
  wxXmlNode* member = node->GetChildren();
  while (member) {
    if (member->GetName() == "coefficient_expansion_linear_thermal") {
      coefficient_expansion_linear_thermal = 
          member->GetChildren()->GetContent();
    } else if (member->GetName() == "coefficients_creep") {
      wxXmlNode* sub_member = member->GetChildren();
      while (sub_member) {
        if (sub_member->GetName() == "b0") {
          coefficients_polynomial_creep.at(0) =
              sub_member->GetChildren()->GetContent();
        } else if (sub_member->GetName() == "b1") {
          coefficients_polynomial_creep.at(1) =
              sub_member->GetChildren()->GetContent();
        } else if (sub_member->GetName() == "b2") {
          coefficients_polynomial_creep.at(2) =
              sub_member->GetChildren()->GetContent();
        } else if (sub_member->GetName() == "b3") {
          coefficients_polynomial_creep.at(3) =
              sub_member->GetChildren()->GetContent();
        } else if (sub_member->GetName() == "b4") {
          coefficients_polynomial_creep.at(4) =
              sub_member->GetChildren()->GetContent();
        }

        sub_member = sub_member->GetNext();
      }
    } else if (member->GetName() == "coefficients_loadstrain") {
      wxXmlNode* sub_member = member->GetChildren();
      while (sub_member) {
        if (sub_member->GetName() == "a0") {
          coefficients_polynomial_loadstrain.at(0) =
              sub_member->GetChildren()->GetContent();
        } else if (sub_member->GetName() == "a1") {
          coefficients_polynomial_loadstrain.at(1) =
              sub_member->GetChildren()->GetContent();
        } else if (sub_member->GetName() == "a2") {
          coefficients_polynomial_loadstrain.at(2) =
              sub_member->GetChildren()->GetContent();
        } else if (sub_member->GetName() == "a3") {
          coefficients_polynomial_loadstrain.at(3) =
              sub_member->GetChildren()->GetContent();
        } else if (sub_member->GetName() == "a4") {
          coefficients_polynomial_loadstrain.at(4) =
              sub_member->GetChildren()->GetContent();
        }

        sub_member = sub_member->GetNext();
      }
    } else if (member->GetName() == "load_limit_polynomial_creep") {
      load_limit_polynomial_creep = member->GetChildren()->GetContent();
    } else if (member->GetName() == "load_limit_polynomial_loadstrain") {
      load_limit_polynomial_loadstrain = member->GetChildren()->GetContent();
    } else if (member->GetName() == "modulus_compression_elastic_area") {
      modulus_compression_elastic_area = member->GetChildren()->GetContent();
    } else if (member->GetName() == "modulus_tension_elastic_area") {
      modulus_tension_elastic_area = member->GetChildren()->GetContent();
    }

    // gets next member of node
    member = member->GetNext();
  }

  // converts strings to appropriate data type and populates struct
  if (coefficient_expansion_linear_thermal.ToDouble(
      &component.coefficient_expansion_linear_thermal) == false) {
    return false;
  }

  for (auto iter = 0; iter <= 4; iter++) {
    wxString coefficient = coefficients_polynomial_creep.at(iter);
    if (coefficient.ToDouble(
        &component.coefficients_polynomial_creep.at(iter)) == false) {
      return false;
    }
  }

  for (auto iter = 0; iter <= 4; iter++) {
    wxString coefficient = coefficients_polynomial_loadstrain.at(iter);
    if (coefficient.ToDouble(
        &component.coefficients_polynomial_loadstrain.at(iter)) == false) {
      return false;
    }
  }

  if (load_limit_polynomial_creep.ToDouble(
      &component.load_limit_polynomial_creep) == false) {
    return false;
  }

  if (load_limit_polynomial_loadstrain.ToDouble(
      &component.load_limit_polynomial_loadstrain) == false) {
    return false;
  }

  if (modulus_compression_elastic_area.ToDouble(
      &component.modulus_compression_elastic_area) == false) {
    return false;
  }

  if (modulus_tension_elastic_area.ToDouble(
      &component.modulus_tension_elastic_area) == false) {
    return false;
  }

  // no errors
  return true;
}

CableXmlHandler::CableXmlHandler() {
}

CableXmlHandler::~CableXmlHandler() {
}

wxXmlNode* CableXmlHandler::CreateNode(const Cable& cable) {
  // creates a node for the cable
  wxXmlNode* node_cable = new wxXmlNode(wxXML_ELEMENT_NODE, "cable");
  node_cable->AddAttribute("type", wxString(cable.type_construction));
  node_cable->AddAttribute("name", wxString(cable.name));
  node_cable->AddAttribute("version", "1");

  // creates area-electrical node and adds to cable node
  wxXmlNode* node_area_electrical =
      new wxXmlNode(wxXML_ELEMENT_NODE, "area_electrical");
  wxXmlNode* data_area_electrical =
      new wxXmlNode(wxXML_TEXT_NODE, wxEmptyString,
                    std::to_string(cable.area_electrical));
  node_area_electrical->AddChild(data_area_electrical);
  node_area_electrical->AddAttribute("units", "kcmil");
  node_cable->AddChild(node_area_electrical);

  // creates area-physical node and adds to cable node
  wxXmlNode* node_area_physical =
      new wxXmlNode(wxXML_ELEMENT_NODE, "area_physical");
  wxXmlNode* data_area_physical =
      new wxXmlNode(wxXML_TEXT_NODE, wxEmptyString,
                    std::to_string(cable.area_physical));
  node_area_physical->AddChild(data_area_physical);
  node_area_physical->AddAttribute("units", "in^2");
  node_cable->AddChild(node_area_physical);

  // creates diameter node and adds to cable node
  wxXmlNode* node_diameter = new wxXmlNode(wxXML_ELEMENT_NODE, "diameter");
  wxXmlNode* data_diameter = new wxXmlNode(wxXML_TEXT_NODE, wxEmptyString,
                                           std::to_string(cable.diameter));
  node_diameter->AddChild(data_diameter);
  node_diameter->AddAttribute("units", "inches");
  node_cable->AddChild(node_diameter);

  // creates strength-rated node and adds to cable node
  wxXmlNode* node_strength_rated =
      new wxXmlNode(wxXML_ELEMENT_NODE, "strength_rated");
  wxXmlNode* data_strength_rated =
      new wxXmlNode(wxXML_TEXT_NODE, wxEmptyString,
                    std::to_string(cable.strength_rated));
  node_strength_rated->AddChild(data_strength_rated);
  node_strength_rated->AddAttribute("units", "lbs");
  node_cable->AddChild(node_strength_rated);

  // creates temperature-component-properties node and adds to cable node
  wxXmlNode* node_temperature_component_properties =
      new wxXmlNode(wxXML_ELEMENT_NODE, "temperature_properties_components");
  wxXmlNode* data_temperature_component_properties =
      new wxXmlNode(wxXML_TEXT_NODE, wxEmptyString,
                    std::to_string(cable.strength_rated));
  node_temperature_component_properties->AddChild(
      data_temperature_component_properties);
  node_temperature_component_properties->AddAttribute("units", "deg F");
  node_cable->AddChild(node_temperature_component_properties);

  // creates weight-unit node and adds to cable node
  wxXmlNode* node_weight_unit = new wxXmlNode(wxXML_ELEMENT_NODE,
                                              "weight_unit");
  wxXmlNode* data_weight_unit =
      new wxXmlNode(wxXML_TEXT_NODE, wxEmptyString,
                    std::to_string(cable.weight_unit));
  node_weight_unit->AddChild(data_weight_unit);
  node_weight_unit->AddAttribute("units", "lbs/ft");
  node_cable->AddChild(node_weight_unit);

  // creates component-shell node and adds to cable node
  node_cable->AddChild(
      CableComponentXmlHandler::CreateNode(cable.component_shell, "shell"));

  // creates component-core node and adds to cable node
  node_cable->AddChild(
      CableComponentXmlHandler::CreateNode(cable.component_core, "core"));

  return node_cable;
}

bool CableXmlHandler::ParseNode(const wxXmlNode* node, Cable& cable) {
  // checks for valid node
  if (node->GetName() != "cable") {
    return false;
  }

  // gets version attribute
  wxString version;
  if (node->GetAttribute("version", &version) == false) {
    return false;
  }

  // sends to proper parsing function
  bool success = false;

  if ( version == "1" ) {
    success = CableXmlHandler::ParseNodeV1(node, cable);
  }
  // <add more versions here as needed>

  return success;
}

bool CableXmlHandler::ParseNodeV1(const wxXmlNode* node, Cable& cable) {
  // declares string versions of struct members
  wxString type = node->GetAttribute("type");
  wxString name = node->GetAttribute("name");
  wxString area_electrical;
  wxString area_physical;
  wxString diameter;
  wxString strength_rated;
  wxString temperature_properties_components;
  wxString weight_unit;

  // gets strings from XML node
  wxXmlNode* member = node->GetChildren();
  while (member) {
    if (member->GetName() == "area_electrical") {
      area_electrical = member->GetChildren()->GetContent();
    } else if (member->GetName() == "area_physical") {
      area_physical = member->GetChildren()->GetContent();
    } else if (member->GetName() == "diameter") {
      diameter = member->GetChildren()->GetContent();
    } else if (member->GetName() == "strength_rated") {
      strength_rated = member->GetChildren()->GetContent();
    } else if (member->GetName() == "temperature_properties_components") {
      temperature_properties_components = member->GetChildren()->GetContent();
    } else if (member->GetName() == "weight_unit") {
      weight_unit = member->GetChildren()->GetContent();
    } else if (member->GetName() == "cable_component") {
      // selects cable component type and passes off to cable component parser
      wxString type_component = member->GetAttribute("name");
      if (type_component == "shell" ) {
        if(CableComponentXmlHandler::ParseNode(
            member, cable.component_shell) == false) {
          return false;
        }
      } else if (type_component == "core" ) {
        if(CableComponentXmlHandler::ParseNode(
            member, cable.component_core) == false) {
          return false;
        }
      }
    }

    // gets next member of node
    member = member->GetNext();
  }

  // converts strings to appropriate data type and populates struct
  cable.type_construction = type.ToStdString();
  cable.name = name.ToStdString();
  
  if(area_electrical.ToDouble(&cable.area_electrical) == false) {
    return false;
  }

  if(area_physical.ToDouble(&cable.area_physical) == false) {
    return false;
  }

  if(diameter.ToDouble(&cable.diameter) == false) {
    return false;
  }

  if (strength_rated.ToDouble(&cable.strength_rated) == false) {
    return false;
  }

  if(temperature_properties_components.ToDouble(
      &cable.temperature_properties_components) == false) {
    return false;
  }

  if (weight_unit.ToDouble(&cable.weight_unit) == false) {
    return false;
  }

  return true;
}