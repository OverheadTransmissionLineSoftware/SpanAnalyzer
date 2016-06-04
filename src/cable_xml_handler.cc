// This is free and unencumbered software released into the public domain.
// For more information, please refer to <http://unlicense.org/>

#include "cable_xml_handler.h"

#include "models/base/helper.h"

CableComponentXmlHandler::CableComponentXmlHandler() {
}

CableComponentXmlHandler::~CableComponentXmlHandler() {
}

wxXmlNode* CableComponentXmlHandler::CreateNode(
    const CableComponent& component,
    const std::string& name,
    const units::UnitSystem& units) {
  // variables used to create XML node
  wxXmlNode* node_root = nullptr;
  wxXmlNode* node_element = nullptr;
  std::string title;
  std::string content;
  wxXmlAttribute attribute;
  double value;
  const std::vector<double>* coefficients = nullptr;

  // creates a node for the cable component root
  node_root = new wxXmlNode(wxXML_ELEMENT_NODE, "cable_component");
  node_root->AddAttribute("name", wxString(name));
  node_root->AddAttribute("version", "1");

  // adds child nodes for struct parameters

  // creates coefficient-expansion-linear-thermal node and adds to component
  // node
  title = "coefficient_expansion_linear_thermal";
  value = component.coefficient_expansion_linear_thermal;
  content = helper::DoubleToFormattedString(value, 7);
  if (units == units::UnitSystem::kMetric) {
    attribute = wxXmlAttribute("units", "deg C");
  } else if (units == units::UnitSystem::kImperial) {
    attribute = wxXmlAttribute("units", "deg F");
  }
  node_element = CreateElementNodeWithContent(title, content, &attribute);
  node_root->AddChild(node_element);

  // creates modulus-tension-elastic-area node and adds to component node
  title = "modulus_tension_elastic";
  value = component.modulus_tension_elastic_area;
  content = helper::DoubleToFormattedString(value, 0);
  if (units == units::UnitSystem::kMetric) {
    attribute = wxXmlAttribute("units", "MPa");
  } else if (units == units::UnitSystem::kImperial) {
    attribute = wxXmlAttribute("units", "lbs/in^2");
  }
  node_element = CreateElementNodeWithContent(title, content, &attribute);
  node_root->AddChild(node_element);

  // creates modulus-compression-elastic-area node and adds to component node
  title = "modulus_compression_elastic";
  value = component.modulus_compression_elastic_area;
  content = helper::DoubleToFormattedString(value, 0);
  if (units == units::UnitSystem::kMetric) {
    attribute = wxXmlAttribute("units", "MPa");
  } else if (units == units::UnitSystem::kImperial) {
    attribute = wxXmlAttribute("units", "lbs/in^2");
  }
  node_element = CreateElementNodeWithContent(title, content, &attribute);
  node_root->AddChild(node_element);

  // creates coefficients stress-strain node and adds to component node
  title = "coefficients";
  node_element = new wxXmlNode(wxXML_ELEMENT_NODE, title);
  node_element->AddAttribute("name", "stress-strain");

  if (units == units::UnitSystem::kMetric) {
    attribute = wxXmlAttribute("units", "MPa");
  } else if (units == units::UnitSystem::kImperial) {
    attribute = wxXmlAttribute("units", "lbs/in^2");
  } else {
    attribute = wxXmlAttribute();
  }

  coefficients = &component.coefficients_polynomial_loadstrain;
  for (auto iter = coefficients->cbegin(); iter != coefficients->cend();
       iter++) {
    const double& coefficient = *iter;
    content = helper::DoubleToFormattedString(coefficient, 1);
    wxXmlNode* sub_node = XmlHandler::CreateElementNodeWithContent(
        "coefficient",
        content,
        &attribute);
    node_element->AddChild(sub_node);
  }
  node_root->AddChild(node_element);

  // creates load-limit-polynomial-loadstrain node and adds to component node
  title = "limit_polynomial_stress-strain";
  value = component.load_limit_polynomial_loadstrain;
  content = helper::DoubleToFormattedString(value, 1);
  if (units == units::UnitSystem::kMetric) {
    attribute = wxXmlAttribute("units", "MPa");
  } else if (units == units::UnitSystem::kImperial) {
    attribute = wxXmlAttribute("units", "lbs/in^2");
  }
  node_element = CreateElementNodeWithContent(title, content, &attribute);
  node_root->AddChild(node_element);

  // creates coefficients-creep node and adds to component node
  title = "coefficients";
  node_element = new wxXmlNode(wxXML_ELEMENT_NODE, title);
  node_element->AddAttribute("name", "creep");

  if (units == units::UnitSystem::kMetric) {
    attribute = wxXmlAttribute("units", "MPa");
  } else if (units == units::UnitSystem::kImperial) {
    attribute = wxXmlAttribute("units", "lbs/in^2");
  } else {
    attribute = wxXmlAttribute();
  }

  coefficients = &component.coefficients_polynomial_creep;
  for (auto iter = coefficients->cbegin(); iter != coefficients->cend();
       iter++) {
    const double& coefficient = *iter;
    content = helper::DoubleToFormattedString(coefficient, 1);
    wxXmlNode* sub_node = XmlHandler::CreateElementNodeWithContent(
        "coefficient",
        content,
        &attribute);
    node_element->AddChild(sub_node);
  }
  node_root->AddChild(node_element);

  // creates load-limit-polynomial-creep node and adds to component node
  title = "limit_polynomial_creep";
  value = component.load_limit_polynomial_creep;
  content = helper::DoubleToFormattedString(value, 1);
  if (units == units::UnitSystem::kMetric) {
    attribute = wxXmlAttribute("units", "MPa");
  } else if (units == units::UnitSystem::kImperial) {
    attribute = wxXmlAttribute("units", "lbs/in^2");
  }
  node_element = CreateElementNodeWithContent(title, content, &attribute);
  node_root->AddChild(node_element);

  // returns node
  return node_root;
}

int CableComponentXmlHandler::ParseNode(const wxXmlNode* root,
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
    return ParseNodeV1(root, component);
  } else {
    return root->GetLineNumber();
  }
}

int CableComponentXmlHandler::ParseNodeV1(const wxXmlNode* root,
                                          CableComponent& component) {
  // evaluates each child node
  const wxXmlNode* node = root->GetChildren();
  while (node != nullptr) {
    const wxString title = node->GetName();
    const wxString content = ParseElementNodeWithContent(node);
    double value = -999999;

    if (title == "coefficient_expansion_linear_thermal") {
      if (content.ToDouble(&value) == true) {
        component.coefficient_expansion_linear_thermal = value;
      } else {
        return node->GetLineNumber();
      }
    } else if (title == "coefficients") {
      wxString name_coefficients;
      node->GetAttribute("name", &name_coefficients);

      // gets coefficient sub-nodes
      wxXmlNode* sub_node = node->GetChildren();
      while (sub_node != nullptr) {
        // creates a new coefficient
        double coefficient = -999999;
        const wxString sub_content = ParseElementNodeWithContent(sub_node);
        sub_content.ToDouble(&coefficient);

        // adds coefficient to container
        if (name_coefficients == "stress-strain") {
          component.coefficients_polynomial_loadstrain.push_back(coefficient);
        } else if (name_coefficients == "creep") {
          component.coefficients_polynomial_creep.push_back(coefficient);
        } else {
          // node was not recognized by parser
          return node->GetLineNumber();
        }
        sub_node = sub_node->GetNext();
      }
    } else if (title == "limit_polynomial_creep") {
      if (content.ToDouble(&value) == true) {
        component.load_limit_polynomial_creep = value;
      } else {
        return node->GetLineNumber();
      }
    } else if (title == "limit_polynomial_stress-strain") {
      if (content.ToDouble(&value) == true) {
        component.load_limit_polynomial_loadstrain = value;
      } else {
        return node->GetLineNumber();
      }
    } else if (title == "modulus_compression_elastic") {
      if (content.ToDouble(&value) == true) {
        component.modulus_compression_elastic_area = value;
      } else {
        return node->GetLineNumber();
      }
    } else if (title == "modulus_tension_elastic") {
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
                                       const std::string& name,
                                       const units::UnitSystem& units) {
  // variables used to create XML node
  double value = -999999;
  std::string title;
  std::string content;
  wxXmlAttribute* attribute = nullptr;
  wxXmlNode* node_root = nullptr;
  wxXmlNode* node_element = nullptr;

  // creates a root node for the cable
  node_root = new wxXmlNode(wxXML_ELEMENT_NODE, "cable");
  node_root->AddAttribute("version", "1");

  // creates name node and adds to cable node
  title = "name";
  content = cable.name;
  node_element = CreateElementNodeWithContent(title, content);
  node_root->AddChild(node_element);

  // creates area-physical node and adds to cable node
  title = "area_physical";
  value = cable.area_physical;
  content = helper::DoubleToFormattedString(value, 4);
  if (units == units::UnitSystem::kMetric) {
    attribute = new wxXmlAttribute("units", "mm^2");
  } else if (units == units::UnitSystem::kImperial) {
    attribute = new wxXmlAttribute("units", "in^2");
  }
  node_element = CreateElementNodeWithContent(title, content, attribute);
  node_root->AddChild(node_element);

  // creates diameter node and adds to cable node
  title = "diameter";
  value = cable.diameter;
  content = helper::DoubleToFormattedString(value, 3);
  if (units == units::UnitSystem::kMetric) {
    attribute = new wxXmlAttribute("units", "mm");
  } else if (units == units::UnitSystem::kImperial) {
    attribute = new wxXmlAttribute("units", "in");
  }
  node_element = CreateElementNodeWithContent(title, content, attribute);
  node_root->AddChild(node_element);

  // creates weight-unit node and adds to cable node
  title = "weight_unit";
  value = cable.weight_unit;
  content = helper::DoubleToFormattedString(value, 3);
  if (units == units::UnitSystem::kMetric) {
    attribute = new wxXmlAttribute("units", "N/m");
  } else if (units == units::UnitSystem::kImperial) {
    attribute = new wxXmlAttribute("units", "lbs/feet");
  }
  node_element = CreateElementNodeWithContent(title, content, attribute);
  node_root->AddChild(node_element);

  // creates strength-rated node and adds to cable node
  title = "strength_rated";
  value = cable.strength_rated;
  content = helper::DoubleToFormattedString(value, 0);
  if (units == units::UnitSystem::kMetric) {
    attribute = new wxXmlAttribute("units", "N");
  } else if (units == units::UnitSystem::kImperial) {
    attribute = new wxXmlAttribute("units", "lbs");
  }
  node_element = CreateElementNodeWithContent(title, content, attribute);
  node_root->AddChild(node_element);

  // creates temperature-component-properties node and adds to cable node
  title = "temperature_properties_components";
  value = cable.temperature_properties_components;
  content = helper::DoubleToFormattedString(value, 0);
  if (units == units::UnitSystem::kMetric) {
    attribute = new wxXmlAttribute("units", "deg C");
  } else if (units == units::UnitSystem::kImperial) {
    attribute = new wxXmlAttribute("units", "deg F");
  }
  node_element = CreateElementNodeWithContent(title, content, attribute);
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
    return CableXmlHandler::ParseNodeV1(root, cable);
  } else {
    return root->GetLineNumber();
  }
}

int CableXmlHandler::ParseNodeV1(const wxXmlNode* root,
                                 Cable& cable) {
  // variables used to parse XML node
  wxString title;
  wxString content;
  double value = -999999;

  // gets cable name from root attribute
  root->GetAttribute("name", &title);
  cable.name = title;

  // evaluates each child node
  const wxXmlNode* node = root->GetChildren();
  while (node != nullptr) {
    title = node->GetName();
    content = ParseElementNodeWithContent(node);

    if (title == "name") {
      cable.name = content;
    } else if (title == "area_physical") {
      if (content.ToDouble(&value) == true) {
        cable.area_physical = value;
      } else {
        return node->GetLineNumber();
      }
    } else if (title == "diameter") {
      if (content.ToDouble(&value) == true) {
        cable.diameter = value;
      } else {
        return node->GetLineNumber();
      }
    } else if (title == "strength_rated") {
      if (content.ToDouble(&value) == true) {
        cable.strength_rated = value;
      } else {
        return node->GetLineNumber();
      }
    } else if (title == "temperature_properties_components") {
      if (content.ToDouble(&value) == true) {
        cable.temperature_properties_components = value;
      } else {
        return node->GetLineNumber();
      }
    } else if (title == "weight_unit") {
      if (content.ToDouble(&value) == true) {
        cable.weight_unit = value;
      } else {
        return node->GetLineNumber();
      }
    } else if (title == "cable_component") {
      // selects cable component type and passes off to cable component parser
      wxString name_component = node->GetAttribute("name");
      if (name_component == "shell" ) {
        int line_number = CableComponentXmlHandler::ParseNode(
            node, cable.component_shell);
        if(line_number != 0) {
          return line_number;
        }
      } else if (name_component == "core" ) {
        int line_number = CableComponentXmlHandler::ParseNode(
            node, cable.component_core);
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
