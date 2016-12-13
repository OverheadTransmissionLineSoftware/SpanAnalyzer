// This is free and unencumbered software released into the public domain.
// For more information, please refer to <http://unlicense.org/>

#include "line_cable_xml_handler.h"

#include "cable_constraint_xml_handler.h"
#include "vector_xml_handler.h"

LineCableXmlHandler::LineCableXmlHandler() {
}

LineCableXmlHandler::~LineCableXmlHandler() {
}

wxXmlNode* LineCableXmlHandler::CreateNode(
    const LineCable& linecable,
    const std::string& name,
    const units::UnitSystem& units) {
  // variables used to create XML node
  wxXmlNode* node_root = nullptr;
  wxXmlNode* node_element = nullptr;
  std::string title;
  std::string content;
  wxXmlAttribute attribute;

  // creates a node for the cable constraint root
  node_root = new wxXmlNode(wxXML_ELEMENT_NODE, "line_cable");
  node_root->AddAttribute("version", "1");

  if (name != "") {
    node_root->AddAttribute("name", name);
  }

  // adds child nodes for struct parameters

  // creates cable node and adds to parent node
  title = "cable";
  if (linecable.cable != nullptr) {
    content = linecable.cable->name;
  } else {
    content = *wxEmptyString;
  }
  node_element = CreateElementNodeWithContent(title, content);
  node_root->AddChild(node_element);

  // creates constraint node and adds to parent node
  node_element = CableConstraintXmlHandler::CreateNode(
      linecable.constraint, "", units);
  node_root->AddChild(node_element);

  // creates spacing-attachments-ruling-span node and adds to parent node
  title = "spacing_attachments_ruling_span";
  if (units == units::UnitSystem::kImperial) {
    attribute = wxXmlAttribute("units", "ft");
  } else if (units == units::UnitSystem::kMetric) {
    attribute = wxXmlAttribute("units", "m");
  }
  node_element = Vector3dXmlHandler::CreateNode(
      linecable.spacing_attachments_ruling_span, title, attribute, 1);
  node_root->AddChild(node_element);

  // creates weathercase-stretch-creep node and adds to parent node
  title = "weather_load_case";
  attribute = wxXmlAttribute("name", "stretch_creep");
  if (linecable.weathercase_stretch_creep != nullptr) {
    content = linecable.weathercase_stretch_creep->description;
  } else {
    content = *wxEmptyString;
  }
  node_element = CreateElementNodeWithContent(title, content, &attribute);
  node_root->AddChild(node_element);

  // creates weathercase-stretch-creep node and adds to parent node
  title = "weather_load_case";
  attribute = wxXmlAttribute("name", "stretch_load");
  if (linecable.weathercase_stretch_load != nullptr) {
    content = linecable.weathercase_stretch_load->description;
  } else {
    content = *wxEmptyString;
  }
  node_element = CreateElementNodeWithContent(title, content, &attribute);
  node_root->AddChild(node_element);

  // returns node
  return node_root;
}

int LineCableXmlHandler::ParseNode(
    const wxXmlNode* root,
    const std::string& filepath,
    const std::list<const Cable*>* cables,
    const std::list<WeatherLoadCase*>* weathercases,
    LineCable& linecable) {
  // checks for valid root node
  if (root->GetName() != "line_cable") {
    return root->GetLineNumber();
  }

  // gets version attribute
  wxString version;
  if (root->GetAttribute("version", &version) == false) {
    return root->GetLineNumber();
  }

  // sends to proper parsing function
  if (version == "1") {
    return ParseNodeV1(root, filepath, cables, weathercases, linecable);
  } else {
    return root->GetLineNumber();
  }
}

int LineCableXmlHandler::ParseNodeV1(
    const wxXmlNode* root,
    const std::string& filepath,
    const std::list<const Cable*>* cables,
    const std::list<WeatherLoadCase*>* weathercases,
    LineCable& linecable) {

  wxString message;

  // evaluates each child node
  const wxXmlNode* node = root->GetChildren();
  while (node != nullptr) {
    const wxString title = node->GetName();
    const wxString content = ParseElementNodeWithContent(node);

    if (title == "cable") {
      // initializes the weathercase and attempts to find a match
      linecable.cable = nullptr;
      for (auto iter = cables->cbegin(); iter != cables->cend();
           iter++) {
        const Cable* cable = *iter;
        if (content == cable->name) {
          linecable.cable = cable;
          break;
        }
      }

      // checks if match was found
      if (linecable.cable == nullptr) {
        message = FileAndLineNumber(filepath, node)
                  + "Invalid cable. Couldn't find " + content;
        wxLogError(message);
      }
    } else if (title == "cable_constraint") {
      const int line_number = CableConstraintXmlHandler::ParseNode(
          node, filepath, weathercases, linecable.constraint);
      if (line_number != 0) {
        message = FileAndLineNumber(filepath, node)
                  + "Invalid cable constraint.";
        wxLogError(message);
        linecable.constraint = CableConstraint();
      }
    } else if (title == "vector_3d") {
      const int line_number = Vector3dXmlHandler::ParseNode(
          node, filepath, linecable.spacing_attachments_ruling_span);
      if(line_number != 0) {
        message = FileAndLineNumber(filepath, node)
                  + "Invalid attachment spacing vector.";
        wxLogError(message);
        linecable.spacing_attachments_ruling_span = Vector3d();
      }
    } else if (title == "weather_load_case") {
      const wxString name = node->GetAttribute("name");
      if (name == "stretch_creep") {
        // initializes the weathercase and attempts to find a match
        linecable.weathercase_stretch_creep = nullptr;
        for (auto iter = weathercases->cbegin(); iter != weathercases->cend();
             iter++) {
          const WeatherLoadCase* weathercase = *iter;
          if (content == weathercase->description) {
            linecable.weathercase_stretch_creep = weathercase;
            break;
          }
        }

        // checks if match was found
        if (linecable.weathercase_stretch_creep == nullptr) {
          message = FileAndLineNumber(filepath, node)
                    + "Invalid creep stretch weathercase. Couldn't find "
                    + content;
          wxLogError(message);
        }
      } else if (name == "stretch_load") {
        // initializes the weathercase and attempts to find a match
        linecable.weathercase_stretch_load = nullptr;
        for (auto iter = weathercases->cbegin(); iter != weathercases->cend();
             iter++) {
          const WeatherLoadCase* weathercase = *iter;
          if (content == weathercase->description) {
            linecable.weathercase_stretch_load = weathercase;
            break;
          }
        }

        // checks if match was found
        if (linecable.weathercase_stretch_creep == nullptr) {
          message = FileAndLineNumber(filepath, node)
                    + "Invalid load stretch weathercase. Couldn't find "
                    + content;
          wxLogError(message);
        }
      }
    } else {
      message = FileAndLineNumber(filepath, node)
                + "XML node isn't recognized.";
      wxLogError(message);
    }

    node = node->GetNext();
  }

  // if it gets to this point, no critical errors were encountered
  return 0;
}
