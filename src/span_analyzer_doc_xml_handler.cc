// This is free and unencumbered software released into the public domain.
// For more information, please refer to <http://unlicense.org/>

#include "span_analyzer_doc_xml_handler.h"

#include "span_xml_handler.h"
#include "weather_load_case_xml_handler.h"

SpanAnalyzerDocXmlHandler::SpanAnalyzerDocXmlHandler() {
}

SpanAnalyzerDocXmlHandler::~SpanAnalyzerDocXmlHandler() {
}

wxXmlNode* SpanAnalyzerDocXmlHandler::CreateNode(
    SpanAnalyzerDoc& doc,
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

  // creates weather load cases node
  title = "weather_load_cases";
  node_element = new wxXmlNode(wxXML_ELEMENT_NODE, title);
  const std::list<WeatherLoadCase>& weathercases = doc.weathercases();
  for (auto iter = weathercases.cbegin(); iter != weathercases.cend();
       iter++) {
    // gets weathercase
    const WeatherLoadCase& weathercase = *iter;

    // creates weather load case node and adds to root
    node_element->AddChild(
        WeatherLoadCaseXmlHandler::CreateNode(weathercase, "", units));
  }
  node_root->AddChild(node_element);

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
    const std::list<Cable>* cables,
    SpanAnalyzerDoc& doc,
    units::UnitSystem& units) {
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
    return ParseNodeV1(root, cables, doc, units);
  } else {
    return root->GetLineNumber();
  }
}

int SpanAnalyzerDocXmlHandler::ParseNodeV1(
    const wxXmlNode* root,
    const std::list<Cable>* cables,
    SpanAnalyzerDoc& doc,
    units::UnitSystem& units) {
  // gets units attribute
  wxString str_units = root->GetAttribute("units");
  if (str_units == "Metric") {
    units = units::UnitSystem::kMetric;
  } else if (str_units == "Imperial") {
    units = units::UnitSystem::kImperial;
  } else {
    return root->GetLineNumber();
  }

  // evaluates each child node
  const wxXmlNode* node = root->GetChildren();
  while (node != nullptr) {
    const wxString title = node->GetName();
    const wxString content = ParseElementNodeWithContent(node);

    if (title == "weather_load_cases") {
      // parses each weathercase
      const wxXmlNode* sub_node = node->GetChildren();
      while(sub_node != nullptr) {
        wxString sub_title = sub_node->GetName();
        if (sub_title != "weather_load_case") {
          return sub_node->GetLineNumber();
        } else {
          WeatherLoadCase weathercase;
          int line_number = WeatherLoadCaseXmlHandler::ParseNode(sub_node,
                                                                 weathercase);
          if (line_number == 0) {
            doc.AppendWeathercase(weathercase);
          } else {
            return line_number;
          }
        }
        sub_node = sub_node->GetNext();
      }
    } else if (title == "spans") {
      // parses each span
      const wxXmlNode* sub_node = node->GetChildren();
      while(sub_node != nullptr) {
        wxString sub_title = sub_node->GetName();
        if (sub_title != "span") {
          return sub_node->GetLineNumber();
        } else {
          Span span;
          const int line_number = SpanXmlHandler::ParseNode(
              sub_node, cables, &doc.weathercases(), span);
          if (line_number == 0) {
            doc.AppendSpan(span);
          } else {
            return line_number;
          }
        }
        sub_node = sub_node->GetNext();
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
