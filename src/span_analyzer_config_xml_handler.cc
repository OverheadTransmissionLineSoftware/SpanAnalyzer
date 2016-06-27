// This is free and unencumbered software released into the public domain.
// For more information, please refer to <http://unlicense.org/>

#include "span_analyzer_config_xml_handler.h"

#include "wx/filename.h"

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

  // creates log level node
  title = "level_log";
  if (config.level_log == wxLOG_Error) {
    content = "Error";
  } else if (config.level_log == wxLOG_Message) {
    content = "Message";
  } else {
    content = "";
  }
  node_element = CreateElementNodeWithContent(title, content);
  node_root->AddChild(node_element);

  // creates filepath-data node
  title = "filepath_data";
  content = config.filepath_data;
  node_element = CreateElementNodeWithContent(title, content);
  node_root->AddChild(node_element);

  // creates size-frame node
  title = "size_frame";
  content = "";
  node_element = CreateElementNodeWithContent(title, content);

  wxString str;
  str = std::to_string(config.size_frame.GetWidth());
  node_element->AddAttribute("x", str);
  str = std::to_string(config.size_frame.GetHeight());
  node_element->AddAttribute("y", str);
  node_root->AddChild(node_element);

  // creates perspective node
  title = "perspective";
  content = config.perspective;
  node_element = CreateElementNodeWithContent(title, content);
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

  // returns node
  return node_root;
}

int SpanAnalyzerConfigXmlHandler::ParseNode(const wxXmlNode* root,
                                            const std::string& filepath,
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
    return ParseNodeV1(root, filepath, config);
  } else {
    return root->GetLineNumber();
  }
}

int SpanAnalyzerConfigXmlHandler::ParseNodeV1(const wxXmlNode* root,
                                              const std::string& filepath,
                                              SpanAnalyzerConfig& config) {
  std::string message;

  // evaluates each child node
  const wxXmlNode* node = root->GetChildren();
  while (node != nullptr) {
    const wxString title = node->GetName();
    const wxString content = ParseElementNodeWithContent(node);

    if (title == "filepath_data") {
      config.filepath_data = content;
      if (config.filepath_data.empty() == true) {
        message = FileAndLineNumber(filepath, node)
                  + "Application data file isn't defined. Keeping default "
                  "setting.";
        wxLogError(message.c_str());
      }
    } else if (title == "level_log") {
      if (content == "Error") {
        config.level_log = wxLOG_Error;
      } else if (content == "Message") {
        config.level_log = wxLOG_Message;
      } else {
        message = FileAndLineNumber(filepath, node)
                  + "Logging level isn't recognized. Keeping default setting.";
        wxLogError(message.c_str());
      }
    } else if (title == "size_frame") {
      std::string str;
      str = node->GetAttribute("x");
      config.size_frame.SetWidth(std::stoi(str));
      str = node->GetAttribute("y");
      config.size_frame.SetHeight(std::stoi(str));
    } else if (title == "perspective") {
      config.perspective = content;
    } else if (title == "units") {
      if (content == "Metric") {
        config.units = units::UnitSystem::kMetric;
      } else if (content == "Imperial") {
        config.units = units::UnitSystem::kImperial;
      } else {
        message = FileAndLineNumber(filepath, node)
                  + "Unit system isn't recognized. Keeping default setting.";
        wxLogError(message.c_str());
      }
    } else {
      message = FileAndLineNumber(filepath, node)
                + "XML node isn't recognized. Skipping.";
      wxLogError(message.c_str());
    }

    node = node->GetNext();
  }

  // if it gets to this point, no errors were encountered
  return 0;
}
