// This is free and unencumbered software released into the public domain.
// For more information, please refer to <http://unlicense.org/>

#include "span_xml_handler.h"

#include "line_cable_xml_handler.h"
#include "vector_xml_handler.h"

SpanXmlHandler::SpanXmlHandler() {
}

SpanXmlHandler::~SpanXmlHandler() {
}

wxXmlNode* SpanXmlHandler::CreateNode(
    const Span& span,
    const std::string& name,
    const units::UnitSystem& units) {
  // variables used to create XML node
  wxXmlNode* node_root = nullptr;
  wxXmlNode* node_element = nullptr;
  std::string title;
  std::string content;
  wxXmlAttribute attribute;

  // creates a node for the span root
  node_root = new wxXmlNode(wxXML_ELEMENT_NODE, "span");
  node_root->AddAttribute("version", "1");
  node_root->AddAttribute("name", name);

  // adds child nodes for parameters

  // creates type node and adds to parent node
  title = "type";
  if (span.type == Span::Type::kDeadendSpan) {
    content = "Deadend";
  } else if (span.type == Span::Type::kRulingSpan) {
    content = "RulingSpan";
  }
  node_element = CreateElementNodeWithContent(title, content);
  node_root->AddChild(node_element);

  // creates linecable node and adds to parent node
  node_element = LineCableXmlHandler::CreateNode(span.linecable, "", units);
  node_root->AddChild(node_element);

  // creates catenary geometry node and adds to parent node
  if (units == units::UnitSystem::kImperial) {
    attribute = wxXmlAttribute("units", "ft");
  } else if (units == units::UnitSystem::kMetric) {
    attribute = wxXmlAttribute("units", "m");
  }
  node_element = Vector3dXmlHandler::CreateNode(span.spacing_catenary,
                                                "spacing_catenary",
                                                attribute,
                                                1);
  node_root->AddChild(node_element);

  // returns node
  return node_root;
}

bool SpanXmlHandler::ParseNode(
    const wxXmlNode* root,
    const std::string& filepath,
    const std::list<CableFile*>* cablefiles,
    const std::list<WeatherLoadCase*>* weathercases,
    Span& span) {
  wxString message;

  // checks for valid root node
  if (root->GetName() != "span") {
    message = FileAndLineNumber(filepath, root) +
              " Invalid root node. Aborting node parse.";
    wxLogError(message);
    return false;
  }

  // gets version attribute
  wxString version;
  if (root->GetAttribute("version", &version) == false) {
    message = FileAndLineNumber(filepath, root) +
              " Version attribute is missing. Aborting node parse.";
    wxLogError(message);
    return false;
  }

  // sends to proper parsing function
  if (version == "1") {
    return ParseNodeV1(root, filepath, cablefiles, weathercases, span);
  } else {
    message = FileAndLineNumber(filepath, root) +
              " Invalid version number. Aborting node parse.";
    wxLogError(message);
    return false;
  }
}

bool SpanXmlHandler::ParseNodeV1(
    const wxXmlNode* root,
    const std::string& filepath,
    const std::list<CableFile*>* cablefiles,
    const std::list<WeatherLoadCase*>* weathercases,
    Span& span) {
  // variables used to parse XML node
  bool status = true;
  wxString title;
  wxString content;
  wxString message;

  // gets name attribute
  wxString name;
  root->GetAttribute("name", &name);
  span.name = name;

  // evaluates each child node
  wxXmlNode* node = root->GetChildren();
  while (node != nullptr) {
    title = node->GetName();
    content = ParseElementNodeWithContent(node);

    if (title == "type") {
      if (content == "Deadend") {
        span.type = Span::Type::kDeadendSpan;
      } else if (content == "RulingSpan") {
        span.type = Span::Type::kRulingSpan;
      } else {
        message = FileAndLineNumber(filepath, node)
                  + "Invalid span type.";
        wxLogError(message);
        status = false;
      }
    } else if (title == "line_cable") {
      // line cable is not intended to be application-specific
      // creates a list of cable pointers from the cable files
      std::list<const Cable*> cables;

      for (auto iter = cablefiles->begin(); iter != cablefiles->end(); iter++) {
        const CableFile* cablefile = *iter;
        const Cable* cable = &cablefile->cable;
        cables.push_back(cable);
      }

      const bool status_node = LineCableXmlHandler::ParseNode(
          node, filepath, &cables, weathercases, span.linecable);
      if (status_node == false) {
        status = false;
      }
    } else if (title == "vector_3d") {
      const bool status_node = Vector3dXmlHandler::ParseNode(
          node, filepath, span.spacing_catenary);
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
