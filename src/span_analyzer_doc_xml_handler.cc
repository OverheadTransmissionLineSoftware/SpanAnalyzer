// This is free and unencumbered software released into the public domain.
// For more information, please refer to <http://unlicense.org/>

#include "spananalyzer/span_analyzer_doc_xml_handler.h"

#include "spananalyzer/span_xml_handler.h"

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
    node_element->AddChild(SpanXmlHandler::CreateNode(span, "", units));
  }
  node_root->AddChild(node_element);

  // returns node
  return node_root;
}

bool SpanAnalyzerDocXmlHandler::ParseNode(
    const wxXmlNode* root,
    const std::string& filepath,
    const units::UnitSystem& units,
    const bool& convert,
    const std::list<CableFile*>* cablefiles,
    const std::list<WeatherLoadCase*>* weathercases,
    SpanAnalyzerDoc& doc) {
  wxString message;

  // checks for valid root node
  if (root->GetName() != "span_analyzer_doc") {
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
    return ParseNodeV1(root, filepath, units, convert, cablefiles, weathercases,
                       doc);
  } else {
    message = FileAndLineNumber(filepath, root) +
              " Invalid version number. Aborting node parse.";
    wxLogError(message);
    return false;
  }
}

bool SpanAnalyzerDocXmlHandler::ParseNodeV1(
    const wxXmlNode* root,
    const std::string& filepath,
    const units::UnitSystem& units,
    const bool& convert,
    const std::list<CableFile*>* cablefiles,
    const std::list<WeatherLoadCase*>* weathercases,
    SpanAnalyzerDoc& doc) {
  bool status = true;
  wxString message;

  // evaluates each child node
  const wxXmlNode* node = root->GetChildren();
  while (node != nullptr) {
    const wxString title = node->GetName();
    const wxString content = ParseElementNodeWithContent(node);

    if (title == "spans") {
      // parses each span xml node
      const wxXmlNode* sub_node = node->GetChildren();
      while (sub_node != nullptr) {
        wxString sub_title = sub_node->GetName();
        if (sub_title == "span") {
          // creates new span and parses node
          Span span;
          const bool status_node = SpanXmlHandler::ParseNode(
              sub_node, filepath, units, convert, cablefiles, weathercases,
              span);
          if (status_node == false) {
            status = false;
          }

          // adds to container
          doc.AppendSpan(span);
        } else {
          message = FileAndLineNumber(filepath, sub_node)
                    + "XML node isn't recognized. Skipping.";
          wxLogError(message);
          status = false;
        }
        sub_node = sub_node->GetNext();
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
