// This is free and unencumbered software released into the public domain.
// For more information, please refer to <http://unlicense.org/>

#include "span_analyzer_data_xml_handler.h"

#include "wx/dir.h"
#include "wx/filename.h"

#include "cable_unit_converter.h"
#include "cable_xml_handler.h"
#include "span_analyzer_app.h"
#include "weather_load_case_xml_handler.h"

namespace {

void LoadCablesFromDirectory(const std::string& directory_cables,
                             units::UnitSystem& units,
                             SpanAnalyzerData& data) {
  SpanAnalyzerFrame* frame = wxGetApp().frame();

  // clears all cables in application data
  data.cables.clear();

  // uses an xml document to load cable files
  wxXmlDocument doc;

  // gets cable directory from model and attempts to parse all files with
  // the proper file extension
  if (wxDir::Exists(directory_cables) == true) {

    // loads files with proper extension into application
    wxDir directory(directory_cables);
    wxString str_file;
    if (directory.GetFirst(&str_file) == true) {
      // goes through all files in directory
      while (true) {
        wxFileName file(directory.GetNameWithSep() + str_file);
        if (file.GetExt() == "cable") {
          wxString str_path = directory.GetNameWithSep() + str_file;
          if (doc.Load(str_path) == true) {
            // parses the XML document and loads into a cable object
            Cable cable;
            const wxXmlNode* root = doc.GetRoot();
            int line_number = CableXmlHandler::ParseNode(root, cable);
            if (line_number == 0) {
              data.cables.push_back(cable);
            } else {
              wxString message = str_file + ": Error at line "
                                 + std::to_wstring(line_number);
              wxMessageDialog dialog(frame, message);
              dialog.ShowModal();
            }
          }
        }
        if (directory.GetNext(&str_file) == false) {
          break;
        }
      }
    }
  } else {
    wxString message = "Cable directory does not exist";
    wxMessageDialog dialog(frame, message);
    dialog.ShowModal();
  }
}

}  // namespace

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
  title = "cable_directory";
  content = data.directory_cables;
  node_element = CreateElementNodeWithContent(title, content);
  node_root->AddChild(node_element);

  // creates analysis weathercase node
  title = "analysis_weather_load_cases";
  

  // returns node
  return node_root;
}

int SpanAnalyzerDataXmlHandler::ParseNode(const wxXmlNode* root,
                                          SpanAnalyzerData& data,
                                          units::UnitSystem& units) {
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
    return ParseNodeV1(root, data, units);
  } else {
    return root->GetLineNumber();
  }
}

int SpanAnalyzerDataXmlHandler::ParseNodeV1(const wxXmlNode* root,
                                            SpanAnalyzerData& data,
                                            units::UnitSystem& units) {
  // evaluates each child node
  const wxXmlNode* node = root->GetChildren();
  while (node != nullptr) {
    const wxString title = node->GetName();
    const wxString content = ParseElementNodeWithContent(node);

    if (title == "cable_directory") {
      data.directory_cables = content;
      LoadCablesFromDirectory(data.directory_cables, units, data);
    } else if (title == "analysis_weather_load_cases") {
      // gets node for analysis weather load case set
      wxXmlNode* sub_node = node->GetChildren();

      while (sub_node != nullptr) {
        // creates a new weather load case set
        std::list<WeatherLoadCase> weathercase_set;
        const std::string str_name = sub_node->GetAttribute("name");
        
        // gets node for individual weathercases
        wxXmlNode* node_weathercase = sub_node->GetChildren();
        while (node_weathercase != nullptr) {
          // creates a weathercase, parses, and adds to set
          WeatherLoadCase weathercase;
          WeatherLoadCaseXmlHandler::ParseNode(node_weathercase, weathercase);
          weathercase_set.push_back(weathercase);

          node_weathercase = node_weathercase->GetNext();
        }

        // adds weathercase description to list
        data.descriptions_weathercases_analysis.push_back(str_name);

        // adds weathercase set to analysis weathercases list
        data.weathercases_analysis.push_back(weathercase_set);

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
