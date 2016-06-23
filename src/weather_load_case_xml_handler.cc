// This is free and unencumbered software released into the public domain.
// For more information, please refer to <http://unlicense.org/>

#include "weather_load_case_xml_handler.h"

#include "models/base/helper.h"

WeatherLoadCaseXmlHandler::WeatherLoadCaseXmlHandler() {
}

WeatherLoadCaseXmlHandler::~WeatherLoadCaseXmlHandler() {
}

wxXmlNode* WeatherLoadCaseXmlHandler::CreateNode(
    const WeatherLoadCase& weathercase,
    const std::string& name,
    const units::UnitSystem& units) {
  // variables used to create XML node
  wxXmlNode* node_root = nullptr;
  wxXmlNode* node_element = nullptr;
  std::string title;
  std::string content;
  wxXmlAttribute attribute;
  double value;

  // creates a node for the root
  node_root = new wxXmlNode(wxXML_ELEMENT_NODE, "weather_load_case");
  node_root->AddAttribute("version", "1");

  if (name != "") {
    node_root->AddAttribute("name", name);
  }

  // adds child nodes for parameters

  // creates description node
  title = "description";
  content = weathercase.description;
  node_element = CreateElementNodeWithContent(title, content);
  node_root->AddChild(node_element);

  // creates thickness-ice node
  title = "thickness_ice";
  value = weathercase.thickness_ice;
  content = helper::DoubleToFormattedString(value, 3);
  if (units == units::UnitSystem::kMetric) {
    attribute = wxXmlAttribute("units", "cm");
  } else if (units == units::UnitSystem::kImperial) {
    attribute = wxXmlAttribute("units", "in");
  }
  node_element = CreateElementNodeWithContent(title, content, &attribute);
  node_root->AddChild(node_element);

  // creates density-ice node
  title = "density_ice";
  value = weathercase.density_ice;
  content = helper::DoubleToFormattedString(value, 1);
  if (units == units::UnitSystem::kMetric) {
    attribute = wxXmlAttribute("units", "N/m^3");
  } else if (units == units::UnitSystem::kImperial) {
    attribute = wxXmlAttribute("units", "lb/ft^3");
  }
  node_element = CreateElementNodeWithContent(title, content, &attribute);
  node_root->AddChild(node_element);

  // creates pressure-wind node
  title = "pressure_wind";
  value = weathercase.pressure_wind;
  content = helper::DoubleToFormattedString(value, 1);
  if (units == units::UnitSystem::kMetric) {
    attribute = wxXmlAttribute("units", "Pa");
  } else if (units == units::UnitSystem::kImperial) {
    attribute = wxXmlAttribute("units", "lb/ft^2");
  }
  node_element = CreateElementNodeWithContent(title, content, &attribute);
  node_root->AddChild(node_element);

  // creates temperature-cable node
  title = "temperature_cable";
  value = weathercase.temperature_cable;
  content = helper::DoubleToFormattedString(value, 0);
  if (units == units::UnitSystem::kMetric) {
    attribute = wxXmlAttribute("units", "deg C");
  } else if (units == units::UnitSystem::kImperial) {
    attribute = wxXmlAttribute("units", "deg F");
  }
  node_element = CreateElementNodeWithContent(title, content, &attribute);
  node_root->AddChild(node_element);

  // returns node
  return node_root;
}

int WeatherLoadCaseXmlHandler::ParseNode(const wxXmlNode* root,
                                         WeatherLoadCase& weathercase) {
  // checks for valid root node
  if (root->GetName() != "weather_load_case") {
    return root->GetLineNumber();
  }

  // gets version attribute
  wxString version;
  if (root->GetAttribute("version", &version) == false) {
    return root->GetLineNumber();
  }

  // sends to proper parsing function
  if (version == "1") {
    return ParseNodeV1(root, weathercase);
  } else {
    return root->GetLineNumber();
  }
}

int WeatherLoadCaseXmlHandler::ParseNodeV1(const wxXmlNode* root,
                                           WeatherLoadCase& weathercase) {
  std::string message;

  // evaluates each child node
  wxXmlNode* node = root->GetChildren();
  while (node != nullptr) {
    const wxString title = node->GetName();
    const wxString content = ParseElementNodeWithContent(node);
    double value = -999999;

    if (title == "description") {
      weathercase.description = content;
    } else if (title == "thickness_ice") {
      if (content.ToDouble(&value) == true) {
        weathercase.thickness_ice = value;
      } else {
        message = "Line " + std::to_string(node->GetLineNumber()) + ". "
                  "Invalid ice thickness.";
        wxLogError(message.c_str());
        weathercase.thickness_ice = -999999;
      }
    } else if (title == "density_ice") {
      if (content.ToDouble(&value) == true) {
        weathercase.density_ice = value;
      } else {
        message = "Line " + std::to_string(node->GetLineNumber()) + ". "
                  "Invalid ice density.";
        wxLogError(message.c_str());
        weathercase.density_ice = -999999;
      }
    } else if (title == "pressure_wind") {
      if (content.ToDouble(&value) == true) {
        weathercase.pressure_wind = value;
      } else {
        message = "Line " + std::to_string(node->GetLineNumber()) + ". "
                  "Invalid wind pressure.";
        wxLogError(message.c_str());
        weathercase.pressure_wind = -999999;
      }
    } else if (title == "temperature_cable") {
      if (content.ToDouble(&value) == true) {
        weathercase.temperature_cable = value;
      } else {
        message = "Line " + std::to_string(node->GetLineNumber()) + ". "
                  "Invalid cable temperature.";
        wxLogError(message.c_str());
        weathercase.temperature_cable = -999999;
      }
    } else {
      message = "Line " + std::to_string(node->GetLineNumber()) + ". "
                "XML node isn't recognized.";
      wxLogError(message.c_str());
    }

    node = node->GetNext();
  }

  // if it gets to this point, no errors were encountered
  return 0;
}
