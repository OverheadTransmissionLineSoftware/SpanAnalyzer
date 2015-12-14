// This is free and unencumbered software released into the public domain.
// For more information, please refer to <http://unlicense.org/>

#include "file_handler.h"

#include "wx/filename.h"
#include "wx/xml/xml.h"

#include "cable_unit_converter.h"
#include "span_analyzer_app.h"
#include "span_analyzer_config_xml_handler.h"
#include "span_analyzer_data_xml_handler.h"
#include "weather_load_case_unit_converter.h"

FileHandler::FileHandler() {
}

FileHandler::~FileHandler() {
}

int FileHandler::LoadAppData(std::string filepath,
                             units::UnitSystem& units,
                             SpanAnalyzerData& data) {
  SpanAnalyzerFrame* frame = wxGetApp().frame();

  // locates config file to fill
  wxFileName path(filepath);

  // uses an xml document to load cable files
  wxXmlDocument doc;
  if (doc.Load(path.GetFullPath()) == false) {
    wxMessageDialog dialog(frame, "Could not locate app data file");
    dialog.ShowModal();
  } else {
    // parses the XML root and loads into the cable object
    const wxXmlNode* root = doc.GetRoot();
    int line_number = SpanAnalyzerDataXmlHandler::ParseNode(root, data, units);
    if (line_number != 0) {
      wxString message = "AppData file: Error at line "
                         + std::to_string(line_number);
      wxMessageDialog dialog(frame, message);
      dialog.ShowModal();
      return line_number;
    }
  }

  // converts cables to consistent unit style
  for (auto iter = data.cables.begin(); iter != data.cables.end(); iter++) {
    Cable& cable = *iter;
    CableUnitConverter::ConvertUnitStyle(
        units,
        units::UnitStyle::kDifferent,
        units::UnitStyle::kConsistent,
        cable);
  }

  // converts weathercases to consistent unit style
  for (auto iter = data.weathercases_analysis.begin();
       iter != data.weathercases_analysis.end(); iter++) {
    std::list<WeatherLoadCase>& weathercases = *iter;
    for (auto it = weathercases.begin(); it != weathercases.end(); it++) {
      WeatherLoadCase& weathercase = *it;
      WeatherLoadCaseUnitConverter::ConvertUnitStyle(
          units,
          units::UnitStyle::kDifferent,
          units::UnitStyle::kConsistent,
          weathercase);
    }
  }

  return 0;
}

int FileHandler::LoadConfigFile(SpanAnalyzerConfig& config) {
  SpanAnalyzerFrame* frame = wxGetApp().frame();

  // locates config file to fill
  wxFileName path(wxGetApp().directory(), "appconfig", "xml");

  wxXmlDocument doc;
  if (doc.Load(path.GetFullPath()) == false) {
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
  wxFileName path(wxGetApp().directory(), "appconfig", "xml");

  wxXmlDocument doc;
  wxXmlNode* root = SpanAnalyzerConfigXmlHandler::CreateNode(config);
  doc.SetRoot(root);
  doc.Save(path.GetFullPath(), 2);
}
