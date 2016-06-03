// This is free and unencumbered software released into the public domain.
// For more information, please refer to <http://unlicense.org/>

#include "cable_constraint_xml_handler.h"

#include "models/base/helper.h"

#include "weather_load_case_xml_handler.h"

CableConstraintXmlHandler::CableConstraintXmlHandler() {
}

CableConstraintXmlHandler::~CableConstraintXmlHandler() {
}

wxXmlNode* CableConstraintXmlHandler::CreateNode(
    const CableConstraint& constraint,
    const std::string name,
    const units::UnitSystem& units) {
  // variables used to create XML node
  wxXmlNode* node_root = nullptr;
  wxXmlNode* node_element = nullptr;
  std::string title;
  std::string content;
  wxXmlAttribute attribute;
  double value;

  // creates a node for the cable constraint root
  node_root = new wxXmlNode(wxXML_ELEMENT_NODE, "cable_constraint");
  node_root->AddAttribute("version", "1");

  if (name != "") {
    node_root->AddAttribute("name", name);
  }

  // adds child nodes for struct parameters

  // creates limit node and adds to parent node
  title = "limit";
  value = constraint.limit;
  content = helper::DoubleToFormattedString(value, 1);
  node_element = CreateElementNodeWithContent(title, content);
  node_root->AddChild(node_element);

  if (constraint.type_limit ==
      CableConstraint::LimitType::kCatenaryConstant) {
    attribute = wxXmlAttribute("type", "CatenaryConstant");
      node_element->AddAttribute(attribute.GetName(), attribute.GetValue());

    if (units == units::UnitSystem::kMetric) {
      attribute = wxXmlAttribute("units", "N/m");
      node_element->AddAttribute(&attribute);
    } else if (units == units::UnitSystem::kImperial) {
      attribute = wxXmlAttribute("units", "lbs/ft");
      node_element->AddAttribute(attribute.GetName(), attribute.GetValue());
    }
  } else if (constraint.type_limit ==
      CableConstraint::LimitType::kHorizontalTension) {
    attribute = wxXmlAttribute("type", "HorizontalTension");
      node_element->AddAttribute(attribute.GetName(), attribute.GetValue());

    if (units == units::UnitSystem::kMetric) {
      attribute = wxXmlAttribute("units", "N");
      node_element->AddAttribute(&attribute);
    } else if (units == units::UnitSystem::kImperial) {
      attribute = wxXmlAttribute("units", "lbs");
      node_element->AddAttribute(attribute.GetName(), attribute.GetValue());
    }
  } else if (constraint.type_limit ==
      CableConstraint::LimitType::kSupportTension) {
    attribute = wxXmlAttribute("type", "SupportTension");
      node_element->AddAttribute(attribute.GetName(), attribute.GetValue());

    if (units == units::UnitSystem::kMetric) {
      attribute = wxXmlAttribute("units", "N");
      node_element->AddAttribute(attribute.GetName(), attribute.GetValue());
    } else if (units == units::UnitSystem::kImperial) {
      attribute = wxXmlAttribute("units", "lbs");
      node_element->AddAttribute(attribute.GetName(), attribute.GetValue());
    }
  }

  // creates weathercase node and adds to parent node
  title = "weather_load_case";
  if (constraint.case_weather != nullptr) {
    content = constraint.case_weather->description;
  } else {
    content = *wxEmptyString;
  }
  node_element = CreateElementNodeWithContent(title, content);
  node_root->AddChild(node_element);

  // creates condition node and adds to parent node
  title = "condition";
  if (constraint.condition == CableConditionType::kCreep) {
    content = "Creep";
  } else if (constraint.condition == CableConditionType::kInitial) {
    content = "Initial";
  } else if (constraint.condition == CableConditionType::kLoad) {
    content = "Load";
  } else {
    content = *wxEmptyString;
  }
  node_element = CreateElementNodeWithContent(title, content);
  node_root->AddChild(node_element);

  // returns node
  return node_root;
}

int CableConstraintXmlHandler::ParseNode(
    const wxXmlNode* root,
    const std::list<WeatherLoadCase>* weathercases,
    CableConstraint& constraint) {
  // checks for valid root node
  if (root->GetName() != "cable_constraint") {
    return root->GetLineNumber();
  }

  // gets version attribute
  wxString version;
  if (root->GetAttribute("version", &version) == false) {
    return root->GetLineNumber();
  }

  // sends to proper parsing function
  if (version == "1") {
    return ParseNodeV1(root, weathercases, constraint);
  } else {
    return root->GetLineNumber();
  }
}

int CableConstraintXmlHandler::ParseNodeV1(
    const wxXmlNode* root,
    const std::list<WeatherLoadCase>* weathercases,
    CableConstraint& constraint) {
  // evaluates each child node
  const wxXmlNode* node = root->GetChildren();
  while (node != nullptr) {
    const wxString title = node->GetName();
    const wxString content = ParseElementNodeWithContent(node);
    double value = -999999;

    if (title == "limit") {
      if (content.ToDouble(&value) == true) {
        constraint.limit = value;
      } else {
        return node->GetLineNumber();
      }

      const wxString content_attribute = node->GetAttribute("type");
      if (content_attribute == "CatenaryConstant") {
        constraint.type_limit = CableConstraint::LimitType::kCatenaryConstant;
      } else if (content_attribute == "HorizontalTension") {
        constraint.type_limit = CableConstraint::LimitType::kHorizontalTension;
      } else if (content_attribute == "SupportTension") {
        constraint.type_limit = CableConstraint::LimitType::kSupportTension;
      } else {
        // content was not recognized by parser
        return node->GetLineNumber();
      }
    } else if (title == "weather_load_case") {
      // initializes the weathercase and attempts to find a match
      constraint.case_weather = nullptr;
      for (auto iter = weathercases->cbegin(); iter != weathercases->cend();
           iter++) {
        const WeatherLoadCase& weathercase = *iter;
        if (content == weathercase.description) {
          constraint.case_weather = &weathercase;
          break;
        }
      }

      // checks if match was found
      if (constraint.case_weather == nullptr) {
        return node->GetLineNumber();
      }
    } else if (title == "condition") {
      if (content == "Creep") {
        constraint.condition = CableConditionType::kCreep;
      } else if (content == "Initial") {
        constraint.condition = CableConditionType::kInitial;
      } else if (content == "Load") {
        constraint.condition = CableConditionType::kLoad;
      } else {
        // node was not recognized by parser
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
