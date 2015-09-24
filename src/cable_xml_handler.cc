// This is free and unencumbered software released into the public domain.
// For more information, please refer to <http://unlicense.org/>

#include "cable_xml_handler.h"

#include "models/base/helper.h"
#include "models/base/units.h"

XmlHandler::XmlHandler() {
}

XmlHandler::~XmlHandler() {
}

wxXmlNode* XmlHandler::CreateElementNodeWithContent(
    const std::string& name,
    const std::string& content,
    wxXmlAttribute* attribute) {
  wxXmlNode* node_child = new wxXmlNode(wxXML_TEXT_NODE, wxEmptyString,
                                        content);
  wxXmlNode* node = new wxXmlNode(wxXML_ELEMENT_NODE, name);
  node->AddChild(node_child);

  if (attribute != nullptr) {
    node->AddAttribute(attribute);
  }

  return node;
}

CableComponentXmlHandler::CableComponentXmlHandler() {
}

CableComponentXmlHandler::~CableComponentXmlHandler() {
}

wxXmlNode* CableComponentXmlHandler::CreateNode(
    const CableComponent& component,
    const std::string& name_cable,
    const units::UnitSystem& units) {
  // variables used to create XML node
  double value_converted;
  std::string name;
  std::string content;
  wxXmlAttribute* attribute = nullptr;
  wxXmlNode* node_root = nullptr;
  wxXmlNode* node_element = nullptr;
  wxXmlNode* node_text = nullptr;

  // creates a node for the cable component root
  node_root = new wxXmlNode(wxXML_ELEMENT_NODE, "cable_component");
  node_root->AddAttribute("name", wxString(name_cable));
  node_root->AddAttribute("version", "1");

  // adds child nodes for struct parameters

  // creates coefficient-expansion-linear-thermal node and adds to component
  // node
  name = "coefficient_expansion_linear_thermal";
  if (units == units::UnitSystem::Metric) {
    value_converted = 0;
    content = "???";
    attribute = new wxXmlAttribute("units", "???");
  } else if (units == units::UnitSystem::Imperial) {
    value_converted = component.coefficient_expansion_linear_thermal;
    content = helper::DoubleToFormattedString(value_converted, 0);
    attribute = new wxXmlAttribute("units", "deg F");
  }
  node_element = CreateElementNodeWithContent(name, content, attribute);
  node_root->AddChild(node_element);

  // creates modulus-compression-elastic-area node and adds to component node
  name = "modulus_compression_elastic_area";
  if (units == units::UnitSystem::Metric) {
    value_converted = 0;
    content = "???";
    attribute = new wxXmlAttribute("units", "???");
  } else if (units == units::UnitSystem::Imperial) {
    value_converted = component.modulus_compression_elastic_area;
    content = helper::DoubleToFormattedString(value_converted, 0);
    attribute = new wxXmlAttribute("units", "lbs");
  }
  node_element = CreateElementNodeWithContent(name, content, attribute);
  node_root->AddChild(node_element);

  // creates modulus-tension-elastic-area node and adds to component node
  name = "modulus_tension_elastic_area";
  if (units == units::UnitSystem::Metric) {
    value_converted = 0;
    content = "???";
    attribute = new wxXmlAttribute("units", "???");
  } else if (units == units::UnitSystem::Imperial) {
    value_converted = component.modulus_tension_elastic_area;
    content = helper::DoubleToFormattedString(value_converted, 0);
    attribute = new wxXmlAttribute("units", "lbs");
  }
  node_element = CreateElementNodeWithContent(name, content, attribute);
  node_root->AddChild(node_element);

  // creates coefficients-loadstrain node and adds to component node
  name = "coefficients_loadstrain";
  node_element = new wxXmlNode(wxXML_ELEMENT_NODE, name);
  node_element->AddAttribute("type", "loadstrain");

  for (size_t iter = 0; 
       iter < component.coefficients_polynomial_loadstrain.size();
       iter++ ) {
    wxXmlNode* subnode_element = nullptr;

    if (iter == 0) {
      name = "a0";
    } else if (iter == 1) {
      name = "a1";
    } else if (iter == 2) {
      name = "a2";
    } else if (iter == 3) {
      name = "a3";
    } else if (iter == 4) {
      name = "a4";
    }

    if (units == units::UnitSystem::Metric) {
      value_converted = 0;
      content = "???";
      attribute = new wxXmlAttribute("units", "???");
    } else if (units == units::UnitSystem::Imperial) {
      value_converted = component.coefficients_polynomial_loadstrain.at(iter);
      content = helper::DoubleToFormattedString(value_converted, 1);
      attribute = new wxXmlAttribute("units", "lbs");
    }
    subnode_element = CreateElementNodeWithContent(name, content, attribute);
    node_element->AddChild(subnode_element);
  }
  node_root->AddChild(node_element);

  // creates load-limit-polynomial-loadstrain node and adds to component node
  name = "load_limit_polynomial_loadstrain";
  if (units == units::UnitSystem::Metric) {
    value_converted = component.load_limit_polynomial_loadstrain;
    content = "???";
    attribute = new wxXmlAttribute("units", "???");
  } else if (units == units::UnitSystem::Imperial) {
    value_converted = component.load_limit_polynomial_loadstrain;
    content = helper::DoubleToFormattedString(value_converted, 1);
    attribute = new wxXmlAttribute("units", "lbs");
  }
  node_element = CreateElementNodeWithContent(name, content, attribute);
  node_root->AddChild(node_element);

  // creates coefficients-creep node and adds to component node
  name = "coefficients_creep";
  node_element = new wxXmlNode(wxXML_ELEMENT_NODE, "coefficients_creep");
  node_element->AddAttribute("type", "creep");

  for (size_t iter = 0;
       iter < component.coefficients_polynomial_creep.size();
       iter++ ) {
    wxXmlNode* subnode_element = nullptr;

    if (iter == 0) {
      name = "b0";
    } else if (iter == 1) {
      name = "b1";
    } else if (iter == 2) {
      name = "b2";
    } else if (iter == 3) {
      name = "b3";
    } else if (iter == 4) {
      name = "b4";
    }

    if (units == units::UnitSystem::Metric) {
      value_converted = 0;
      content = "???";
      attribute = new wxXmlAttribute("units", "???");
    } else if (units == units::UnitSystem::Imperial) {
      value_converted = component.coefficients_polynomial_creep.at(iter);
      content = helper::DoubleToFormattedString(value_converted, 1);
      attribute = new wxXmlAttribute("units", "lbs");
    }
    subnode_element = CreateElementNodeWithContent(name, content, attribute);
    node_element->AddChild(subnode_element);
  }
  node_root->AddChild(node_element);

  // creates load-limit-polynomial-creep node and adds to component node
  name = "load_limit_polynomial_creep";
  if (units == units::UnitSystem::Metric) {
    value_converted = component.load_limit_polynomial_creep;
    content = "???";
    attribute = new wxXmlAttribute("units", "???");
  } else if (units == units::UnitSystem::Imperial) {
    value_converted = component.load_limit_polynomial_creep;
    content = helper::DoubleToFormattedString(value_converted, 1);
    attribute = new wxXmlAttribute("units", "lbs");
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
  }
  // <add more versions here as needed>
}

int CableComponentXmlHandler::ParseNodeV1(const wxXmlNode* root,
                                          const units::UnitSystem& units,
                                          CableComponent& component) {
  // variables used to parse XML node
  wxString name;
  wxString content;
  double value_unconverted = -999999;

  // evaluates each child node
  wxXmlNode* node = root->GetChildren();
  while (node != nullptr) {
    name = node->GetName();
    content = node->GetChildren()->GetContent();

    if (name == "coefficient_expansion_linear_thermal") {
      if (content.ToDouble(&value_unconverted) == false) {
        return node->GetLineNumber();
      }

      if (units == units::UnitSystem::Metric) {
        component.coefficient_expansion_linear_thermal = 0;
      } else if (units == units::UnitSystem::Imperial) {
        component.coefficient_expansion_linear_thermal = value_unconverted;
      }

    /// \todo possibly create a function to parse each coefficient
    } else if (name == "coefficients_creep") {
      wxXmlNode* subnode = node->GetChildren();
      while (subnode != nullptr) {
        name = subnode->GetName();
        content = subnode->GetChildren()->GetContent();

        if (name == "b0") {
          if (content.ToDouble(&value_unconverted) == false) {
            return node->GetLineNumber();
          }

          if (units == units::UnitSystem::Metric) {
            component.coefficients_polynomial_creep.at(0) = 0;
          } else if (units == units::UnitSystem::Imperial) {
            component.coefficients_polynomial_creep.at(0) = value_unconverted;
          }
        } else if (name == "b1") {
          if (content.ToDouble(&value_unconverted) == false) {
            return node->GetLineNumber();
          }

          if (units == units::UnitSystem::Metric) {
            component.coefficients_polynomial_creep.at(1) = 0;
          } else if (units == units::UnitSystem::Imperial) {
            component.coefficients_polynomial_creep.at(1) = value_unconverted;
          }
        } else if (name == "b2") {
          if (content.ToDouble(&value_unconverted) == false) {
            return node->GetLineNumber();
          }

          if (units == units::UnitSystem::Metric) {
            component.coefficients_polynomial_creep.at(2) = 0;
          } else if (units == units::UnitSystem::Imperial) {
            component.coefficients_polynomial_creep.at(2) = value_unconverted;
          }
        } else if (name == "b3") {
          if (content.ToDouble(&value_unconverted) == false) {
            return node->GetLineNumber();
          }

          if (units == units::UnitSystem::Metric) {
            component.coefficients_polynomial_creep.at(3) = 0;
          } else if (units == units::UnitSystem::Imperial) {
            component.coefficients_polynomial_creep.at(3) = value_unconverted;
          }
        } else if (name == "b4") {
          if (content.ToDouble(&value_unconverted) == false) {
            return node->GetLineNumber();
          }

          if (units == units::UnitSystem::Metric) {
            component.coefficients_polynomial_creep.at(4) = 0;
          } else if (units == units::UnitSystem::Imperial) {
            component.coefficients_polynomial_creep.at(4) = value_unconverted;
          }
        } else {
          // node is not recognized by the parser
          return subnode->GetLineNumber();
        }
        subnode = subnode->GetNext();
      }

    /// \todo possibly create a function to parse each coefficient
    } else if (name == "coefficients_loadstrain") {
      wxXmlNode* subnode = node->GetChildren();
      while (subnode != nullptr) {
        name = subnode->GetName();
        content = subnode->GetChildren()->GetContent();

        if (name == "a0") {
          if (content.ToDouble(&value_unconverted) == false) {
            return node->GetLineNumber();
          }

          if (units == units::UnitSystem::Metric) {
            component.coefficients_polynomial_loadstrain.at(0) = 0;
          } else if (units == units::UnitSystem::Imperial) {
            component.coefficients_polynomial_loadstrain.at(0) = value_unconverted;
          }
        } else if (name == "a1") {
          if (content.ToDouble(&value_unconverted) == false) {
            return node->GetLineNumber();
          }

          if (units == units::UnitSystem::Metric) {
            component.coefficients_polynomial_loadstrain.at(1) = 0;
          } else if (units == units::UnitSystem::Imperial) {
            component.coefficients_polynomial_loadstrain.at(1) = value_unconverted;
          }
        } else if (name == "a2") {
          if (content.ToDouble(&value_unconverted) == false) {
            return node->GetLineNumber();
          }

          if (units == units::UnitSystem::Metric) {
            component.coefficients_polynomial_loadstrain.at(2) = 0;
          } else if (units == units::UnitSystem::Imperial) {
            component.coefficients_polynomial_loadstrain.at(2) = value_unconverted;
          }
        } else if (name == "a3") {
          if (content.ToDouble(&value_unconverted) == false) {
            return node->GetLineNumber();
          }

          if (units == units::UnitSystem::Metric) {
            component.coefficients_polynomial_loadstrain.at(3) = 0;
          } else if (units == units::UnitSystem::Imperial) {
            component.coefficients_polynomial_loadstrain.at(3) = value_unconverted;
          }
        } else if (name == "a4") {
          if (content.ToDouble(&value_unconverted) == false) {
            return node->GetLineNumber();
          }

          if (units == units::UnitSystem::Metric) {
            component.coefficients_polynomial_loadstrain.at(4) = 0;
          } else if (units == units::UnitSystem::Imperial) {
            component.coefficients_polynomial_loadstrain.at(4) = value_unconverted;
          }
        } else {
          // node is not recognized by the parser
          return subnode->GetLineNumber();
        }
        subnode = subnode->GetNext();
      }

    } else if (name == "load_limit_polynomial_creep") {
      if (content.ToDouble(&value_unconverted) == false) {
        return node->GetLineNumber();
      }

      if (units == units::UnitSystem::Metric) {
        component.load_limit_polynomial_creep = 0;
      } else if (units == units::UnitSystem::Imperial) {
        component.load_limit_polynomial_creep = value_unconverted;
      }

    } else if (name == "load_limit_polynomial_loadstrain") {
      if (content.ToDouble(&value_unconverted) == false) {
        return node->GetLineNumber();
      }

      if (units == units::UnitSystem::Metric) {
        component.load_limit_polynomial_loadstrain = 0;
      } else if (units == units::UnitSystem::Imperial) {
        component.load_limit_polynomial_loadstrain = value_unconverted;
      }

    } else if (name == "modulus_compression_elastic_area") {
      if (content.ToDouble(&value_unconverted) == false) {
        return node->GetLineNumber();
      }

      if (units == units::UnitSystem::Metric) {
        component.modulus_compression_elastic_area = 0;
      } else if (units == units::UnitSystem::Imperial) {
        component.modulus_compression_elastic_area = value_unconverted;
      }

    } else if (name == "modulus_tension_elastic_area") {
      if (content.ToDouble(&value_unconverted) == false) {
        return node->GetLineNumber();
      }

      if (units == units::UnitSystem::Metric) {
        component.modulus_tension_elastic_area = 0;
      } else if (units == units::UnitSystem::Imperial) {
        component.modulus_tension_elastic_area = value_unconverted;
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
  double value_converted = -999999;
  std::string name;
  std::string content;
  wxXmlAttribute* attribute = nullptr;
  wxXmlNode* node_root = nullptr;
  wxXmlNode* node_element = nullptr;

  // creates a root node for the cable
  node_root = new wxXmlNode(wxXML_ELEMENT_NODE, "cable");
  node_root->AddAttribute("type", wxString(cable.type_construction));
  node_root->AddAttribute("name", wxString(cable.name));
  node_root->AddAttribute("version", "1");

  // creates area-electrical node and adds to cable node
  name = "area_electrical";
  if (units == units::UnitSystem::Metric) {
    value_converted = cable.area_electrical;
    content = "???";
    attribute = new wxXmlAttribute("units", "???");
  } else if (units == units::UnitSystem::Imperial) {
    value_converted = cable.area_electrical;
    content = helper::DoubleToFormattedString(value_converted, 1);
    attribute = new wxXmlAttribute("units", "kcmil");
  }
  node_element = CreateElementNodeWithContent(name, content, attribute);
  node_root->AddChild(node_element);

  // creates area-physical node and adds to cable node
  name = "area_physical";
  if (units == units::UnitSystem::Metric) {
    value_converted = cable.area_physical;
    content = "???";
    attribute = new wxXmlAttribute("units", "???");
  } else if (units == units::UnitSystem::Imperial) {
    value_converted = units::Convert(cable.area_physical,
                                     units::ConversionType::kFeetToInches,
                                     2);
    content = helper::DoubleToFormattedString(value_converted, 4);
    attribute = new wxXmlAttribute("units", "in^2");
  }
  node_element = CreateElementNodeWithContent(name, content, attribute);
  node_root->AddChild(node_element);

  // creates diameter node and adds to cable node
  name = "diameter";
  if (units == units::UnitSystem::Metric) {
    value_converted = cable.diameter;
    content = "???";
    attribute = new wxXmlAttribute("units", "???");
  } else if (units == units::UnitSystem::Imperial) {
    value_converted = units::Convert(cable.diameter,
                                     units::ConversionType::kFeetToInches);
    content = helper::DoubleToFormattedString(value_converted, 3);
    attribute = new wxXmlAttribute("units", "in");
  }
  node_element = CreateElementNodeWithContent(name, content, attribute);
  node_root->AddChild(node_element);

  // creates strength-rated node and adds to cable node
  name = "strength_rated";
  if (units == units::UnitSystem::Metric) {
    value_converted = cable.strength_rated;
    content = "???";
    attribute = new wxXmlAttribute("units", "???");
  } else if (units == units::UnitSystem::Imperial) {
    value_converted = cable.strength_rated;
    content = helper::DoubleToFormattedString(value_converted, 0);
    attribute = new wxXmlAttribute("units", "lbs");
  }
  node_element = CreateElementNodeWithContent(name, content, attribute);
  node_root->AddChild(node_element);

  // creates temperature-component-properties node and adds to cable node
  name = "temperature_properties_components";
  if (units == units::UnitSystem::Metric) {
    value_converted = cable.temperature_properties_components;
    content = "???";
    attribute = new wxXmlAttribute("units", "???");
  } else if (units == units::UnitSystem::Imperial) {
    value_converted = cable.temperature_properties_components;
    content = helper::DoubleToFormattedString(value_converted, 0);
    attribute = new wxXmlAttribute("units", "deg F");
  }
  node_element = CreateElementNodeWithContent(name, content, attribute);
  node_root->AddChild(node_element);

  // creates weight-unit node and adds to cable node
  name = "weight_unit";
  if (units == units::UnitSystem::Metric) {
    value_converted = cable.weight_unit;
    content = "???";
    attribute = new wxXmlAttribute("units", "???");
  } else if (units == units::UnitSystem::Imperial) {
    value_converted = cable.weight_unit;
    content = helper::DoubleToFormattedString(value_converted, 3);
    attribute = new wxXmlAttribute("units", "lbs/feet");
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
  }
  // <add more versions here as needed>
}

int CableXmlHandler::ParseNodeV1(const wxXmlNode* root,
                                 const units::UnitSystem& units,
                                 Cable& cable) {
  // variables used to parse XML node
  wxString name;
  wxString content;
  double value_unconverted = -999999;

  // evaluates each child node
  wxXmlNode* node = root->GetChildren();
  while (node != nullptr) {
    name = node->GetName();
    content = node->GetChildren()->GetContent();

    if (name == "area_electrical") {
      if (content.ToDouble(&value_unconverted) == false) {
        return node->GetLineNumber();
      }

      if (units == units::UnitSystem::Metric) {
        cable.area_electrical = 0;
      } else if (units == units::UnitSystem::Imperial) {
        cable.area_electrical = value_unconverted;
      }
    } else if (name == "area_physical") {
      if (content.ToDouble(&value_unconverted) == false) {
        return node->GetLineNumber();
      }

      if (units == units::UnitSystem::Metric) {
        cable.area_physical = 0;
      } else if (units == units::UnitSystem::Imperial) {
        cable.area_physical = units::Convert(
            value_unconverted, units::ConversionType::kInchesToFeet, 2);
      }
    } else if (name == "diameter") {
      if (content.ToDouble(&value_unconverted) == false) {
        return node->GetLineNumber();
      }

      if (units == units::UnitSystem::Metric) {
        cable.diameter = 0;
      } else if (units == units::UnitSystem::Imperial) {
        cable.diameter = units::Convert(
            value_unconverted, units::ConversionType::kInchesToFeet);
      }
    } else if (name == "strength_rated") {
      if (content.ToDouble(&value_unconverted) == false) {
        return node->GetLineNumber();
      }

      if (units == units::UnitSystem::Metric) {
        cable.strength_rated = 0;
      } else if (units == units::UnitSystem::Imperial) {
        cable.strength_rated = value_unconverted;
      }
    } else if (name == "temperature_properties_components") {
      if (content.ToDouble(&value_unconverted) == false) {
        return node->GetLineNumber();
      }

      if (units == units::UnitSystem::Metric) {
        cable.temperature_properties_components = 0;
      } else if (units == units::UnitSystem::Imperial) {
        cable.temperature_properties_components = value_unconverted;
      }
    } else if (name == "weight_unit") {
      if (content.ToDouble(&value_unconverted) == false) {
        return node->GetLineNumber();
      }

      if (units == units::UnitSystem::Metric) {
        cable.weight_unit = 0;
      } else if (units == units::UnitSystem::Imperial) {
        cable.weight_unit = value_unconverted;
      }
    } else if (name == "cable_component") {
      // selects cable component type and passes off to cable component parser
      wxString type_component = node->GetAttribute("name");
      if (type_component == "shell" ) {
        int line_number = CableComponentXmlHandler::ParseNode(
            node, units, cable.component_shell);
        if(line_number != 0) {
          return line_number;
        }
      } else if (type_component == "core" ) {
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
