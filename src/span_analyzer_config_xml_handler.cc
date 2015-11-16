// This is free and unencumbered software released into the public domain.
// For more information, please refer to <http://unlicense.org/>

#include "span_analyzer_config_xml_handler.h"

#include "wx/dir.h"

#include "span_analyzer_app.h"

SpanAnalyzerConfigXmlHandler::SpanAnalyzerConfigXmlHandler() {
}

SpanAnalyzerConfigXmlHandler::~SpanAnalyzerConfigXmlHandler() {
}

wxXmlNode* SpanAnalyzerConfigXmlHandler::CreateNode(
    const SpanAnalyzerConfig& config) {
  // variables used to create XML node
  wxXmlNode* node_root = nullptr;
  wxXmlNode* node_element = nullptr;
  std::string title;
  std::string content;

  // creates a node for the root
  node_root = new wxXmlNode(wxXML_ELEMENT_NODE, "span_analyzer_config");
  node_root->AddAttribute("version", "1");

  // adds child nodes for struct parameters

  // creates size-frame node
  title = "size_frame";
  const bool is_maximized = wxGetApp().frame()->IsMaximized();
  const wxSize size = wxGetApp().frame()->GetSize();
  if (is_maximized == true) {
    content = "Automatic";
  } else {
    content = "Manual";
  }
  node_element = CreateElementNodeWithContent(title, content);
  if (is_maximized == false) {
    wxString content_attr;

    content_attr = std::to_string(size.GetWidth());
    node_element->AddAttribute("x", content_attr);

    content_attr = std::to_string(size.GetHeight());
    node_element->AddAttribute("y", content_attr);
  }
  node_root->AddChild(node_element);

  // creates units node
  title = "units";
  if (config.units == units::UnitSystem::kMetric) {
    content = wxString("Metric");
  } else if (config.units == units::UnitSystem::kImperial) {
    content = wxString("Imperial");
  }
  node_element = CreateElementNodeWithContent(title, content);
  node_root->AddChild(node_element);

  // creates cable directory
  title = "cable_directory";
  content = config.cable_directory;
  node_element = CreateElementNodeWithContent(title, content);
  node_root->AddChild(node_element);

  // returns node
  return node_root;
}

int SpanAnalyzerConfigXmlHandler::ParseNode(const wxXmlNode* root,
                                            SpanAnalyzerConfig& config) {
  // checks for valid root node
  if (root->GetName() != "span_analyzer_config") {
    return root->GetLineNumber();
  }

  // gets version attribute
  wxString version;
  if (root->GetAttribute("version", &version) == false) {
    return root->GetLineNumber();
  }

  // sends to proper parsing function
  if (version == "1") {
    return ParseNodeV1(root, config);
  } else {
    return root->GetLineNumber();
  }
}

int SpanAnalyzerConfigXmlHandler::ParseNodeV1(const wxXmlNode* root,
                                              SpanAnalyzerConfig& config) {
  // evaluates each child node
  const wxXmlNode* node = root->GetChildren();
  while (node != nullptr) {
    const wxString title = node->GetName();
    const wxString content = ParseElementNodeWithContent(node);

    if (title == "size_frame") {
      if (content == "Automatic") {
        config.size_frame.SetWidth(0);
        config.size_frame.SetHeight(0);
      } else if (content == "Manual") {
        std::string content_attr;

        content_attr = node->GetAttribute("x");
        config.size_frame.SetWidth(std::stoi(content_attr));

        content_attr = node->GetAttribute("y");
        config.size_frame.SetHeight(std::stoi(content_attr));
      }
    } else if (title == "units") {
      if (content == "Metric") {
        config.units = units::UnitSystem::kMetric;
      } else if (content == "Imperial") {
        config.units = units::UnitSystem::kImperial;
      } else {
        return node->GetLineNumber();
      }
    } else if (title == "cable_directory") {
      if (wxDir::Exists(content) == true) {
        config.cable_directory = content;
      } else {
        config.cable_directory = *wxEmptyString;
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
