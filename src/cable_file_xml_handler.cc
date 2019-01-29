// This is free and unencumbered software released into the public domain.
// For more information, please refer to <http://unlicense.org/>

#include "spananalyzer/cable_file_xml_handler.h"

#include "appcommon/xml/cable_xml_handler.h"

wxXmlNode* CableFileXmlHandler::CreateNode(
    const Cable& cable,
    const std::string& name,
    const units::UnitSystem& system_units,
    const units::UnitStyle& style_units) {
  // variables used to create XML node
  wxXmlNode* node_root = nullptr;
  wxXmlNode* node_element = nullptr;

  // creates a node for the cable file root
  node_root = new wxXmlNode(wxXML_ELEMENT_NODE, "cable_file");
  node_root->AddAttribute("version", "1");

  if (system_units == units::UnitSystem::kImperial) {
    node_root->AddAttribute("units", "Imperial");
  } else if (system_units == units::UnitSystem::kMetric) {
    node_root->AddAttribute("units", "Metric");
  }

  if (name != "") {
    node_root->AddAttribute("name", name);
  }

  // creates cable node and adds to root node
  node_element = CableXmlHandler::CreateNode(
      cable, name, system_units, style_units);
  node_root->AddChild(node_element);

  // returns node
  return node_root;
}

bool CableFileXmlHandler::ParseNode(
    const wxXmlNode* root,
    const std::string& filepath,
    const units::UnitSystem& units,
    const bool& convert,
    Cable& cable) {
  wxString message;

  // checks for valid root node
  if (root->GetName() != "cable_file") {
    message = FileAndLineNumber(filepath, root) +
              " Invalid root node. Aborting node parse.";
    wxLogError(message);
    return false;
  }

  // gets version attribute
  const int kVersion = Version(root);
  if (kVersion == -1) {
    message = FileAndLineNumber(filepath, root) +
              " Version attribute is missing or invalid. Aborting node parse.";
    wxLogError(message);
    return false;
  }

  // sends to proper parsing function
  if (kVersion == 1) {
    return ParseNodeV1(root, filepath, units, convert, cable);
  } else {
    message = FileAndLineNumber(filepath, root) +
              " Invalid version number. Aborting node parse.";
    wxLogError(message);
    return false;
  }
}

bool CableFileXmlHandler::ParseNodeV1(
    const wxXmlNode* root,
    const std::string& filepath,
    const units::UnitSystem& units,
    const bool& convert,
    Cable& cable) {
  // variables used to parse XML node
  bool status = true;
  wxString title;
  wxString content;
  wxString message;

  // evaluates each child node
  wxXmlNode* node = root->GetChildren();
  while (node != nullptr) {
    title = node->GetName();
    content = ParseElementNodeWithContent(node);

    if (title == "cable") {
      const bool status_node = CableXmlHandler::ParseNode(
          node, filepath, units, convert, cable);
      if (status_node == false) {
        status = false;
      }
    } else {
      message = FileAndLineNumber(filepath, node)
                + "XML node isn't recognized.";
      wxLogError(message);
      status = false;
    }

    node = node->GetNext();
  }

  return status;
}
