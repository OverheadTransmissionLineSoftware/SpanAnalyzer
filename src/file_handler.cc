// This is free and unencumbered software released into the public domain.
// For more information, please refer to <http://unlicense.org/>

#include "file_handler.h"

#include "wx/dir.h"
#include "wx/filename.h"
#include "wx/xml/xml.h"

#include "cable_unit_converter.h"
#include "cable_xml_handler.h"
#include "span_analyzer_app.h"
#include "span_analyzer_config_xml_handler.h"

FileHandler::FileHandler() {
}

FileHandler::~FileHandler() {
}

void FileHandler::LoadAppData(const std::string& directory_cables,
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

    // converts cable files to consistent unit style
    for (auto iter = data.cables.begin(); iter != data.cables.end(); iter++) {
      Cable& cable = *iter;
      CableUnitConverter::ConvertUnitStyle(
          units,
          units::UnitStyle::kDifferent,
          units::UnitStyle::kConsistent,
          cable);
    }
  } else {
    wxString message = "Cable directory does not exist";
    wxMessageDialog dialog(frame, message);
    dialog.ShowModal();
  }
}

int FileHandler::LoadConfigFile(SpanAnalyzerConfig& config) {
  SpanAnalyzerFrame* frame = wxGetApp().frame();

  // locates config file to fill
  wxXmlDocument doc;
  if (doc.Load("spananalyzer.config") == false) {
    wxMessageDialog dialog(frame, "Could not locate config file");
    dialog.ShowModal();
  } else {
    // parses the XML root and loads into the cable object
    const wxXmlNode* root = doc.GetRoot();
    int line_number = SpanAnalyzerConfigXmlHandler::ParseNode(root, config);
    if (line_number != 0) {
      wxString message = "Config file: Error at line "
                         + std::to_string(line_number);
      wxMessageDialog dialog(frame, message);
      dialog.ShowModal();
      return line_number;
    }

    if (config.units == units::UnitSystem::kMetric) {
      wxMessageDialog dialog(frame, "Metric units are not yet supported");
      dialog.ShowModal();
    }
  }

  return 0;
}

void FileHandler::SaveConfigFile(const SpanAnalyzerConfig& config) {
  // generates a virtual XML document
  wxXmlDocument doc;
  wxXmlNode* root = SpanAnalyzerConfigXmlHandler::CreateNode(config);
  doc.SetRoot(root);
  doc.Save("spananalyzer.config", 2);
}
