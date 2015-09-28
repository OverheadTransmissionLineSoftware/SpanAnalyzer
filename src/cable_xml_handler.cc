// This is free and unencumbered software released into the public domain.
// For more information, please refer to <http://unlicense.org/>

#include "cable_xml_handler.h"

#include "models/base/helper.h"

#include "polynomial_xml_handler.h"

CableComponentXmlHandler::CableComponentXmlHandler() {
}

CableComponentXmlHandler::~CableComponentXmlHandler() {
}

wxXmlNode* CableComponentXmlHandler::CreateNode(
    const CableComponent& component,
    const std::string& name_cable,
    const units::UnitSystem& units) {
  // variables used to create XML node
  double value;
  std::string name;
  std::string content;
  wxXmlAttribute* attribute = nullptr;
  wxXmlAttribute attribute_polynomial;
  wxXmlNode* node_root = nullptr;
  wxXmlNode* node_element = nullptr;
  Polynomial polynomial;
  // std::vector<double> coefficients;

  // creates a node for the cable component root
  node_root = new wxXmlNode(wxXML_ELEMENT_NODE, "cable_component");
  node_root->AddAttribute("name", wxString(name_cable));
  node_root->AddAttribute("version", "1");

  // adds child nodes for struct parameters

  // creates coefficient-expansion-linear-thermal node and adds to component
  // node
  name = "coefficient_expansion_linear_thermal";
  value = component.coefficient_expansion_linear_thermal;
  content = helper::DoubleToFormattedString(value, 7);
  if (units == units::UnitSystem::Metric) {
    attribute = new wxXmlAttribute("units", "???");
  } else if (units == units::UnitSystem::Imperial) {
    attribute = new wxXmlAttribute("units", "deg F");
  }
  node_element = CreateElementNodeWithContent(name, content, attribute);
  node_root->AddChild(node_element);

  // creates modulus-tension-elastic-area node and adds to component node
  name = "modulus_tension_elastic";
  value = component.modulus_tension_elastic_area;
  content = helper::DoubleToFormattedString(value, 0);
  if (units == units::UnitSystem::Metric) {
    attribute = new wxXmlAttribute("units", "???");
  } else if (units == units::UnitSystem::Imperial) {
    attribute = new wxXmlAttribute("units", "lbs/in^2");
  }
  node_element = CreateElementNodeWithContent(name, content, attribute);
  node_root->AddChild(node_element);

  // creates modulus-compression-elastic-area node and adds to component node
  name = "modulus_compression_elastic";
  value = component.modulus_compression_elastic_area;
  content = helper::DoubleToFormattedString(value, 0);
  if (units == units::UnitSystem::Metric) {
    attribute = new wxXmlAttribute("units", "???");
  } else if (units == units::UnitSystem::Imperial) {
    attribute = new wxXmlAttribute("units", "lbs/in^2");
  }
  node_element = CreateElementNodeWithContent(name, content, attribute);
  node_root->AddChild(node_element);

  // creates polynomial stress-strain node and adds to component node
  name = "polynomial";
  polynomial.set_coefficients(&component.coefficients_polynomial_loadstrain);
  if (units == units::UnitSystem::Metric) {
    attribute_polynomial = wxXmlAttribute("units", "???");
  } else if (units == units::UnitSystem::Imperial) {
    attribute_polynomial = wxXmlAttribute("units", "lbs/in^2");
  } else {
    attribute = nullptr;
  }
  node_element = PolynomialXmlHandler::CreateNode(polynomial,
                                                  "stress-strain",
                                                  attribute_polynomial);
  node_root->AddChild(node_element);

  // creates load-limit-polynomial-loadstrain node and adds to component node
  name = "limit_polynomial_stress-strain";
  value = component.load_limit_polynomial_loadstrain;
  content = helper::DoubleToFormattedString(value, 1);
  if (units == units::UnitSystem::Metric) {
    attribute = new wxXmlAttribute("units", "???");
  } else if (units == units::UnitSystem::Imperial) {
    attribute = new wxXmlAttribute("units", "lbs/in^2");
  }
  node_element = CreateElementNodeWithContent(name, content, attribute);
  node_root->AddChild(node_element);

  // creates coefficients-creep node and adds to component node
  name = "polynomial";
  polynomial.set_coefficients(&component.coefficients_polynomial_creep);
  if (units == units::UnitSystem::Metric) {
    attribute_polynomial = wxXmlAttribute("units", "???");
  } else if (units == units::UnitSystem::Imperial) {
    attribute_polynomial = wxXmlAttribute("units", "lbs/in^2");
  } else {
    attribute = nullptr;
  }
  node_element = PolynomialXmlHandler::CreateNode(polynomial,
                                                  "creep",
                                                  attribute_polynomial);
  node_root->AddChild(node_element);

  // creates load-limit-polynomial-creep node and adds to component node
  name = "limit_polynomial_creep";
  value = component.load_limit_polynomial_creep;
  content = helper::DoubleToFormattedString(value, 1);
  if (units == units::UnitSystem::Metric) {
    attribute = new wxXmlAttribute("units", "???");
  } else if (units == units::UnitSystem::Imperial) {
    attribute = new wxXmlAttribute("units", "lbs/in^2");
  }
  node_element = CreateElementNodeWithContent(name, content, attribute);
  node_root->AddChild(node_element);

  // returns node
  return node_root;
}

int CableComponentXmlHandler::ParseNode(const wxXmlNode* root,
                                         const units::UnitSystem& units,
                                         CableComponent& component) {
  // checks for valid root node
  if (root->GetName() != "cable_component") {
    return root->GetLineNumber();
  }

  // gets version attribute
  wxString version;
  if (root->GetAttribute("version", &version) == false) {
    return root->GetLineNumber();
  }

  // sends to proper parsing function
  if (version == "1") {
    return ParseNodeV1(root, units, component);
  } else {
    return root->GetLineNumber();
  }
}

int CableComponentXmlHandler::ParseNodeV1(const wxXmlNode* root,
                                          const units::UnitSystem& units,
                                          CableComponent& component) {
  // variables used to parse XML node
  wxString name;
  wxString content;
  double value = -999999;

  // evaluates each child node
  wxXmlNode* node = root->GetChildren();
  while (node != nullptr) {
    name = node->GetName();
    content = node->GetChildren()->GetContent();

    if (name == "coefficient_expansion_linear_thermal") {
      if (content.ToDouble(&value) == true) {
        component.coefficient_expansion_linear_thermal = value;
      } else {
        return node->GetLineNumber();
      }
    } else if (name == "polynomial") {
      wxString name_polynomial;
      node->GetAttribute("name", &name_polynomial);
      Polynomial polynomial;

      if (name_polynomial == "stress-strain") {
        int line_number = PolynomialXmlHandler::ParseNode(node, polynomial);
        if(line_number != 0) {
          return line_number;
        }

        std::vector<double> coefficients = *polynomial.coefficients();
        component.coefficients_polynomial_loadstrain = coefficients;

      } else if (name_polynomial == "creep") {
        int line_number = PolynomialXmlHandler::ParseNode(node, polynomial);
        if(line_number != 0) {
          return line_number;
        }

        std::vector<double> coefficients = *polynomial.coefficients();
        component.coefficients_polynomial_creep = coefficients;

      } else {
        // node was not recognized by parser
        return node->GetLineNumber();
      }

    } else if (name == "limit_polynomial_creep") {
      if (content.ToDouble(&value) == true) {
        component.load_limit_polynomial_creep = value;
      } else {
        return node->GetLineNumber();
      }
    } else if (name == "limit_polynomial_stress-strain") {
      if (content.ToDouble(&value) == true) {
        component.load_limit_polynomial_loadstrain = value;
      } else {
        return node->GetLineNumber();
      }
    } else if (name == "modulus_compression_elastic") {
      if (content.ToDouble(&value) == true) {
        component.modulus_compression_elastic_area = value;
      } else {
        return node->GetLineNumber();
      }
    } else if (name == "modulus_tension_elastic") {
      if (content.ToDouble(&value) == true) {
        component.modulus_tension_elastic_area = value;
      } else {
        return node->GetLineNumber();
      }
    } else {
      // node is not recognized by ther parser
      return node->GetLineNumber();
    }

    node = node->GetNext();
  }

  // if it gets to this point, no errors were encountered
  return 0;
}

CableXmlHandler::CableXmlHandler() {
}

CableXmlHandler::~CableXmlHandler() {
}

wxXmlNode* CableXmlHandler::CreateNode(const Cable& cable,
                                       const units::UnitSystem& units) {
  // variables used to create XML node
  double value = -999999;
  std::string name;
  std::string content;
  wxXmlAttribute* attribute = nullptr;
  wxXmlNode* node_root = nullptr;
  wxXmlNode* node_element = nullptr;

  // creates a root node for the cable
  node_root = new wxXmlNode(wxXML_ELEMENT_NODE, "cable");
  node_root->AddAttribute("name", wxString(cable.name));
  node_root->AddAttribute("version", "1");

  // creates area-electrical node and adds to cable node
  name = "area_electrical";
  value = cable.area_electrical;
  content = helper::DoubleToFormattedString(value, 1);
  if (units == units::UnitSystem::Metric) {
    attribute = new wxXmlAttribute("units", "???");
  } else if (units == units::UnitSystem::Imperial) {
    attribute = new wxXmlAttribute("units", "kcmil");
  }
  node_element = CreateElementNodeWithContent(name, content, attribute);
  node_root->AddChild(node_element);

  // creates area-physical node and adds to cable node
  name = "area_physical";
  value = cable.area_physical;
  content = helper::DoubleToFormattedString(value, 4);
  if (units == units::UnitSystem::Metric) {
    attribute = new wxXmlAttribute("units", "???");
  } else if (units == units::UnitSystem::Imperial) {
    attribute = new wxXmlAttribute("units", "in^2");
  }
  node_element = CreateElementNodeWithContent(name, content, attribute);
  node_root->AddChild(node_element);

  // creates diameter node and adds to cable node
  name = "diameter";
  value = cable.diameter;
  content = helper::DoubleToFormattedString(value, 3);
  if (units == units::UnitSystem::Metric) {
    attribute = new wxXmlAttribute("units", "???");
  } else if (units == units::UnitSystem::Imperial) {
    attribute = new wxXmlAttribute("units", "in");
  }
  node_element = CreateElementNodeWithContent(name, content, attribute);
  node_root->AddChild(node_element);

  // creates weight-unit node and adds to cable node
  name = "weight_unit";
  value = cable.weight_unit;
  content = helper::DoubleToFormattedString(value, 3);
  if (units == units::UnitSystem::Metric) {
    attribute = new wxXmlAttribute("units", "???");
  } else if (units == units::UnitSystem::Imperial) {
    attribute = new wxXmlAttribute("units", "lbs/feet");
  }
  node_element = CreateElementNodeWithContent(name, content, attribute);
  node_root->AddChild(node_element);

  // creates strength-rated node and adds to cable node
  name = "strength_rated";
  value = cable.strength_rated;
  content = helper::DoubleToFormattedString(value, 0);
  if (units == units::UnitSystem::Metric) {
    attribute = new wxXmlAttribute("units", "???");
  } else if (units == units::UnitSystem::Imperial) {
    attribute = new wxXmlAttribute("units", "lbs");
  }
  node_element = CreateElementNodeWithContent(name, content, attribute);
  node_root->AddChild(node_element);

  // creates temperature-component-properties node and adds to cable node
  name = "temperature_properties_components";
  value = cable.temperature_properties_components;
  content = helper::DoubleToFormattedString(value, 0);
  if (units == units::UnitSystem::Metric) {
    attribute = new wxXmlAttribute("units", "???");
  } else if (units == units::UnitSystem::Imperial) {
    attribute = new wxXmlAttribute("units", "deg F");
  }
  node_element = CreateElementNodeWithContent(name, content, attribute);
  node_root->AddChild(node_element);

  // creates component-shell node and adds to cable node
  node_root->AddChild(
      CableComponentXmlHandler::CreateNode(cable.component_shell, "shell",
                                           units));

  // creates component-core node and adds to cable node
  node_root->AddChild(
      CableComponentXmlHandler::CreateNode(cable.component_core, "core",
                                           units));

  return node_root;
}

int CableXmlHandler::ParseNode(const wxXmlNode* root,
                                const units::UnitSystem& units,
                                Cable& cable) {
  // checks for valid node
  if (root->GetName() != "cable") {
    return root->GetLineNumber();
  }

  // gets version attribute
  wxString version;
  if (root->GetAttribute("version", &version) == false) {
    return root->GetLineNumber();
  }

  // sends to proper parsing function
  if (version == "1") {
    return CableXmlHandler::ParseNodeV1(root, units, cable);
  } else {
    return root->GetLineNumber();
  }
}

int CableXmlHandler::ParseNodeV1(const wxXmlNode* root,
                                 const units::UnitSystem& units,
                                 Cable& cable) {
  // variables used to parse XML node
  wxString name;
  wxString content;
  double value = -999999;

  // gets cable name from root attribute
  root->GetAttribute("name", &name);
  cable.name = name;

  // evaluates each child node
  wxXmlNode* node = root->GetChildren();
  while (node != nullptr) {
    name = node->GetName();
    content = node->GetChildren()->GetContent();

    if (name == "area_electrical") {
      if (content.ToDouble(&value) == true) {
        cable.area_electrical = value;
      } else {
        return node->GetLineNumber();
      }
    } else if (name == "area_physical") {
      if (content.ToDouble(&value) == true) {
        cable.area_physical = value;
      } else {
        return node->GetLineNumber();
      }
    } else if (name == "diameter") {
      if (content.ToDouble(&value) == true) {
        cable.diameter = value;
      } else {
        return node->GetLineNumber();
      }
    } else if (name == "strength_rated") {
      if (content.ToDouble(&value) == true) {
        cable.strength_rated = value;
      } else {
        return node->GetLineNumber();
      }
    } else if (name == "temperature_properties_components") {
      if (content.ToDouble(&value) == true) {
        cable.temperature_properties_components = value;
      } else {
        return node->GetLineNumber();
      }
    } else if (name == "weight_unit") {
      if (content.ToDouble(&value) == true) {
        cable.weight_unit = value;
      } else {
        return node->GetLineNumber();
      }
    } else if (name == "cable_component") {
      // selects cable component type and passes off to cable component parser
      wxString name_component = node->GetAttribute("name");
      if (name_component == "shell" ) {
        int line_number = CableComponentXmlHandler::ParseNode(
            node, units, cable.component_shell);
        if(line_number != 0) {
          return line_number;
        }
      } else if (name_component == "core" ) {
        int line_number = CableComponentXmlHandler::ParseNode(
            node, units, cable.component_core);
        if(line_number != 0) {
          return line_number;
        }
      }
    }

    node = node->GetNext();
  }

  // if it gets to this point, no errors were encountered
  return 0;
}
