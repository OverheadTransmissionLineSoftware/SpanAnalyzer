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
    attribute = wxXmlAttribute("units", "???");
  }
  node_element = Vector3dXmlHandler::CreateNode(span.spacing_catenary,
                                                "catenary_spacing",
                                                attribute,
                                                1);
  node_root->AddChild(node_element);

  // returns node
  return node_root;
}

int SpanXmlHandler::ParseNode(
    const wxXmlNode* root,
    const std::list<Cable>* cables,
    const std::list<WeatherLoadCase>* weathercases,
    Span& span) {
  // checks for valid root node
  if (root->GetName() != "span") {
    return root->GetLineNumber();
  }

  // gets version attribute
  wxString version;
  if (root->GetAttribute("version", &version) == false) {
    return root->GetLineNumber();
  }

  // sends to proper parsing function
  if (version == "1") {
    return ParseNodeV1(root, cables, weathercases, span);
  } else {
    return root->GetLineNumber();
  }
}

int SpanXmlHandler::ParseNodeV1(
    const wxXmlNode* root,
    const std::list<Cable>* cables,
    const std::list<WeatherLoadCase>* weathercases,
    Span& span) {
  // variables used to parse XML node
  wxString title;
  wxString content;

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
        return node->GetLineNumber();
      }
    } else if (title == "line_cable") {
      int line_number = LineCableXmlHandler::ParseNode(
          node, cables, weathercases, span.linecable);
      if(line_number != 0) {
        return line_number;
      }
    } else if (title == "vector_3d") {
      const int line_number = Vector3dXmlHandler::ParseNode(
          node, span.spacing_catenary);
      if(line_number != 0) {
        return line_number;
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
