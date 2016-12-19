// This is free and unencumbered software released into the public domain.
// For more information, please refer to <http://unlicense.org/>

#include "span_analyzer_data_xml_handler.h"

#include "wx/filename.h"

#include "analysis_filter_xml_handler.h"
#include "file_handler.h"
#include "weather_load_case_xml_handler.h"

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

int SpanAnalyzerDataXmlHandler::ParseNode(const wxXmlNode* root,
                                          const std::string& filepath,
                                          const units::UnitSystem& units,
                                          SpanAnalyzerData& data) {
  // checks for valid root node
  if (root->GetName() != "span_analyzer_data") {
    return root->GetLineNumber();
  }

  // gets version attribute
  wxString version;
  if (root->GetAttribute("version", &version) == false) {
    return root->GetLineNumber();
  }

  // sends to proper parsing function
  if (version == "1") {
    return ParseNodeV1(root, filepath, units, data);
  } else {
    return root->GetLineNumber();
  }
}

int SpanAnalyzerDataXmlHandler::ParseNodeV1(const wxXmlNode* root,
                                            const std::string& filepath,
                                            const units::UnitSystem& units,
                                            SpanAnalyzerData& data) {
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
        int status =  FileHandler::LoadCable(cablefile->filepath, units,
                                             cablefile->cable);

        if (status == 0) {
          // adds to data lists
          data.cablefiles.push_back(cablefile);
        } else {
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
        int status = WeatherLoadCaseXmlHandler::ParseNode(
            sub_node,
            filepath,
            *weathercase);

        // adds to container if parsing was successful
        if (status == 0) {
          data.weathercases.push_back(weathercase);
        } else {
          message = FileAndLineNumber(filepath, sub_node)
                    + "Invalid weathercase. Skipping.";
          wxLogError(message);

          delete weathercase;
        }

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
          int status = AnalysisFilterXmlHandler::ParseNode(
              node_filter,
              filepath,
              &data.weathercases,
              filter);

          // adds to container if parsing was successful
          if (status == 0) {
            group.filters.push_back(filter);
          } else {
            message = FileAndLineNumber(filepath, node_filter)
                      + "Invalid filter. Skipping.";
            wxLogError(message);
          }
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
    }

    node = node->GetNext();
  }

  // if it gets to this point, no errors were encountered
  return 0;
}
