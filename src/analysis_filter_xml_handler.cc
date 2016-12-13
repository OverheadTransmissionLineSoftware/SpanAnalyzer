// This is free and unencumbered software released into the public domain.
// For more information, please refer to <http://unlicense.org/>

#include "analysis_filter_xml_handler.h"

AnalysisFilterXmlHandler::AnalysisFilterXmlHandler() {
}

AnalysisFilterXmlHandler::~AnalysisFilterXmlHandler() {
}

wxXmlNode* AnalysisFilterXmlHandler::CreateNode(const AnalysisFilter& filter,
                                                const std::string& name) {
  // variables used to create XML node
  wxXmlNode* node_root = nullptr;
  wxXmlNode* node_element = nullptr;
  std::string title;
  std::string content;

  // creates a node for the root
  node_root = new wxXmlNode(wxXML_ELEMENT_NODE, "analysis_filter");
  node_root->AddAttribute("version", "1");

  if (name != "") {
    node_root->AddAttribute("name", name);
  }

  // adds child nodes for struct parameters
  
  // creates condition node
  title = "condition";
  if (filter.condition == CableConditionType::kCreep) {
    content = "Creep";
  } else if (filter.condition == CableConditionType::kInitial) {
    content = "Initial";
  } else if (filter.condition == CableConditionType::kLoad) {
    content = "Load";
  }
  node_element = CreateElementNodeWithContent(title, content);
  node_root->AddChild(node_element);

  // creates weathercase node
  title = "weather_load_case";
  content = filter.weathercase->description;
  node_element = CreateElementNodeWithContent(title, content);
  node_root->AddChild(node_element);

  // returns node
  return node_root;
}

int AnalysisFilterXmlHandler::ParseNode(
    const wxXmlNode* root,
    const std::string& filepath,
    const std::list<WeatherLoadCase*>* weathercases,
    AnalysisFilter& filter) {
  // checks for valid root node
  if (root->GetName() != "analysis_filter") {
    return root->GetLineNumber();
  }

  // gets version attribute
  wxString version;
  if (root->GetAttribute("version", &version) == false) {
    return root->GetLineNumber();
  }

  // sends to proper parsing function
  if (version == "1") {
    return ParseNodeV1(root, filepath, weathercases, filter);
  } else {
    return root->GetLineNumber();
  }
}

int AnalysisFilterXmlHandler::ParseNodeV1(
    const wxXmlNode* root,
    const std::string& filepath,
    const std::list<WeatherLoadCase*>* weathercases,
    AnalysisFilter& filter) {
  wxString message;

  // evaluates each child node
  const wxXmlNode* node = root->GetChildren();
  while (node != nullptr) {
    const wxString title = node->GetName();
    const wxString content = ParseElementNodeWithContent(node);

    if (title == "condition") {
      if (content == "Creep") {
        filter.condition = CableConditionType::kCreep;
      } else if (content == "Initial") {
        filter.condition = CableConditionType::kInitial;
      } else if (content == "Load") {
        filter.condition = CableConditionType::kLoad;
      } else {
        message = FileAndLineNumber(filepath, node)
                  + "Invalid condition.";
        wxLogError(message);
      }
    } else if (title == "weather_load_case") {
      // initializes the weathercase and attempts to find a match
      filter.weathercase = nullptr;
      for (auto iter = weathercases->cbegin(); iter != weathercases->cend();
           iter++) {
        const WeatherLoadCase* weathercase = *iter;
        if (content == weathercase->description) {
          filter.weathercase = weathercase;
          break;
        }
      }

      // checks if match was found
      if (filter.weathercase == nullptr) {
        message = FileAndLineNumber(filepath, node)
                  + "Invalid weathercase. Couldn't find " + content;
        wxLogError(message);
      }
    } else {
      message = FileAndLineNumber(filepath, node)
                + "XML node isn't recognized.";
      wxLogMessage(message);
    }

    node = node->GetNext();
  }

  // if it gets to this point, no errors were encountered
  return 0;
}
