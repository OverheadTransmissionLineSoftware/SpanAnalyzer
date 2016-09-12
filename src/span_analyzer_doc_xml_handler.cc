// This is free and unencumbered software released into the public domain.
// For more information, please refer to <http://unlicense.org/>

#include "span_analyzer_doc_xml_handler.h"

#include "span_xml_handler.h"

SpanAnalyzerDocXmlHandler::SpanAnalyzerDocXmlHandler() {
}

SpanAnalyzerDocXmlHandler::~SpanAnalyzerDocXmlHandler() {
}

wxXmlNode* SpanAnalyzerDocXmlHandler::CreateNode(
    const SpanAnalyzerDoc& doc,
    const units::UnitSystem& units) {
  // initializes variables used to create XML node
  wxXmlNode* node_root = nullptr;
  wxXmlNode* node_element = nullptr;
  std::string title;
  std::string content;

  // creates a node for the root
  node_root = new wxXmlNode(wxXML_ELEMENT_NODE, "span_analyzer_doc");
  node_root->AddAttribute("version", "1");

  // adds child nodes for parameters

  // creates spans node
  title = "spans";
  node_element = new wxXmlNode(wxXML_ELEMENT_NODE, title);
  const std::list<Span>& spans = doc.spans();
  for (auto iter = spans.cbegin(); iter != spans.cend(); iter++) {
    const Span& span = *iter;

    // creates span node and adds to root
    node_element->AddChild(
        SpanXmlHandler::CreateNode(span, span.name, units));
  }
  node_root->AddChild(node_element);

  // returns node
  return node_root;
}

int SpanAnalyzerDocXmlHandler::ParseNode(
    const wxXmlNode* root,
    const std::string& filepath,
    const std::list<CableFile>* cablefiles,
    const std::list<WeatherLoadCase>* weathercases,
    SpanAnalyzerDoc& doc) {
  // checks for valid root node
  if (root->GetName() != "span_analyzer_doc") {
    return root->GetLineNumber();
  }

  // gets version attribute
  wxString version;
  if (root->GetAttribute("version", &version) == false) {
    return root->GetLineNumber();
  }

  // sends to proper parsing function
  if (version == "1") {
    return ParseNodeV1(root, filepath, cablefiles, weathercases, doc);
  } else {
    return root->GetLineNumber();
  }
}

int SpanAnalyzerDocXmlHandler::ParseNodeV1(
    const wxXmlNode* root,
    const std::string& filepath,
    const std::list<CableFile>* cablefiles,
    const std::list<WeatherLoadCase>* weathercases,
    SpanAnalyzerDoc& doc) {

  wxString message;

  // evaluates each child node
  const wxXmlNode* node = root->GetChildren();
  while (node != nullptr) {
    const wxString title = node->GetName();
    const wxString content = ParseElementNodeWithContent(node);

    if (title == "spans") {
      // parses each span xml node
      const wxXmlNode* sub_node = node->GetChildren();
      while(sub_node != nullptr) {
        wxString sub_title = sub_node->GetName();
        if (sub_title == "span") {
          // creates new weathercase and parses node
          Span span;
          const int line_number = SpanXmlHandler::ParseNode(
              sub_node, filepath, cablefiles, weathercases, span);

          // adds if no critical errors were encountered during parsing
          if (line_number == 0) {
            doc.AppendSpan(span);
          } else {
            message = FileAndLineNumber(filepath, sub_node)
                      + "Invalid span. Skipping.";
            wxLogError(message);
          }
        } else {
          message = FileAndLineNumber(filepath, sub_node)
                    + "XML node isn't recognized. Skipping.";
          wxLogError(message);
        }
        sub_node = sub_node->GetNext();
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
