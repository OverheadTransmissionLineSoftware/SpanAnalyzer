// This is free and unencumbered software released into the public domain.
// For more information, please refer to <http://unlicense.org/>

#include "spananalyzer/span_analyzer_data_xml_handler.h"

#include "appcommon/xml/weather_load_case_xml_handler.h"
#include "wx/filename.h"

#include "spananalyzer/analysis_filter_xml_handler.h"
#include "spananalyzer/file_handler.h"

wxXmlNode* SpanAnalyzerDataXmlHandler::CreateNode(
    const SpanAnalyzerData& data,
    const units::UnitSystem& units) {
  // variables used to create XML node
  wxXmlNode* node_root = nullptr;
  wxXmlNode* node_element = nullptr;
  std::string title;
  std::string content;

  // creates a node for the root
  node_root = new wxXmlNode(wxXML_ELEMENT_NODE, "span_analyzer_data");
  node_root->AddAttribute("version", "1");

  // adds child nodes for struct parameters

  // creates cable directory node
  title = "cables";
  node_element = new wxXmlNode(wxXML_ELEMENT_NODE, title);

  for (auto iter = data.cablefiles.cbegin(); iter != data.cablefiles.cend();
       iter++) {
    const CableFile* cablefile = *iter;
    wxXmlNode* sub_node = CreateElementNodeWithContent("file",
                                                       cablefile->filepath);
    node_element->AddChild(sub_node);
  }

  node_root->AddChild(node_element);

  // creates weathercase node
  title = "weather_load_cases";
  node_element = new wxXmlNode(wxXML_ELEMENT_NODE, title);

  for (auto iter = data.weathercases.cbegin();
       iter != data.weathercases.cend(); iter++) {
    const WeatherLoadCase* weathercase = *iter;
    wxXmlNode* sub_node =
        WeatherLoadCaseXmlHandler::CreateNode(*weathercase, "", units);
    node_element->AddChild(sub_node);
  }

  node_root->AddChild(node_element);

  // creates analysis filter groups node
  title = "analysis_filter_groups";
  node_element = new wxXmlNode(wxXML_ELEMENT_NODE, title);

  for (auto iter = data.groups_filters.cbegin();
       iter != data.groups_filters.cend(); iter++ ) {
    // gets filter group
    const AnalysisFilterGroup& group = *iter;

    // creates an analysis filter group node
    wxXmlNode* sub_node = new wxXmlNode(wxXML_ELEMENT_NODE,
                                        "analysis_filter_group");
    sub_node->AddAttribute("name", group.name);

    // adds analysis filter nodes to the group node
    for (auto it = group.filters.cbegin();
         it != group.filters.cend(); it++) {
      const AnalysisFilter& filter = *it;
      wxXmlNode* node_filter =
          AnalysisFilterXmlHandler::CreateNode(filter, "");
      sub_node->AddChild(node_filter);
    }
    node_element->AddChild(sub_node);
  }
  node_root->AddChild(node_element);

  // returns node
  return node_root;
}

bool SpanAnalyzerDataXmlHandler::ParseNode(const wxXmlNode* root,
                                           const std::string& filepath,
                                           const units::UnitSystem& units,
                                           SpanAnalyzerData& data) {
  wxString message;

  // checks for valid root node
  if (root->GetName() != "span_analyzer_data") {
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
    return ParseNodeV1(root, filepath, units, data);
  } else {
    message = FileAndLineNumber(filepath, root) +
              " Invalid version number. Aborting node parse.";
    wxLogError(message);
    return false;
  }
}

bool SpanAnalyzerDataXmlHandler::ParseNodeV1(const wxXmlNode* root,
                                             const std::string& filepath,
                                             const units::UnitSystem& units,
                                             SpanAnalyzerData& data) {
  bool status = true;
  wxString message;

  // evaluates each child node
  const wxXmlNode* node = root->GetChildren();
  while (node != nullptr) {
    const wxString title = node->GetName();
    const wxString content = ParseElementNodeWithContent(node);

    if (title == "cables") {
      // gets node for cable file
      wxXmlNode* sub_node = node->GetChildren();
      while (sub_node != nullptr) {
        CableFile* cablefile = new CableFile();

        // gets filepath
        cablefile->filepath = ParseElementNodeWithContent(sub_node);

        // loads cable file
        // filehandler function handles all logging
        const int status_node =  FileHandler::LoadCable(cablefile->filepath,
                                                         units,
                                                         cablefile->cable);

        // adds to container if no file errors were encountered
        if ((status_node == 0) || (status_node == 1)) {
          data.cablefiles.push_back(cablefile);
        } else {
          message = FileAndLineNumber(filepath, sub_node)
                    + "Invalid cable file. Skipping.";
          wxLogError(message);
          status = false;

          delete cablefile;
        }

        sub_node = sub_node->GetNext();
      }
    } else if (title == "weather_load_cases") {
      // gets node for weather load case
      wxXmlNode* sub_node = node->GetChildren();
      while (sub_node != nullptr) {
        // creates a weathercase and parses
        WeatherLoadCase* weathercase = new WeatherLoadCase();
        const bool status_node = WeatherLoadCaseXmlHandler::ParseNode(
            sub_node,
            filepath,
            *weathercase);
        if (status_node == false) {
          status = false;
        }

        // adds to container
        data.weathercases.push_back(weathercase);

        sub_node = sub_node->GetNext();
      }
    } else if (title == "analysis_filter_groups") {
      // gets node for weather load case
      wxXmlNode* sub_node = node->GetChildren();
      while (sub_node != nullptr) {
        // creates a new analysis filter group
        AnalysisFilterGroup group;
        group.name = sub_node->GetAttribute("name");

        // gets node for individual analysis filters
        wxXmlNode* node_filter = sub_node->GetChildren();
        while (node_filter != nullptr) {
          // creates an analysis filter and parses
          AnalysisFilter filter;
          const bool status_node = AnalysisFilterXmlHandler::ParseNode(
              node_filter,
              filepath,
              &data.weathercases,
              filter);
          if (status_node == false) {
            status = false;
          }

          // adds to container
          group.filters.push_back(filter);

          node_filter = node_filter->GetNext();
        }

        // adds filter group
        data.groups_filters.push_back(group);

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
