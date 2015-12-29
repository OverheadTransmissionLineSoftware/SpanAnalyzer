// This is free and unencumbered software released into the public domain.
// For more information, please refer to <http://unlicense.org/>

#include "file_handler.h"

#include "wx/dir.h"
#include "wx/filename.h"
#include "wx/xml/xml.h"

#include "cable_unit_converter.h"
#include "cable_xml_handler.h"
#include "error_message_dialog.h"
#include "span_analyzer_app.h"
#include "span_analyzer_config_xml_handler.h"
#include "span_analyzer_data_xml_handler.h"
#include "weather_load_case_unit_converter.h"

FileHandler::FileHandler() {
}

FileHandler::~FileHandler() {
}

int FileHandler::LoadAppData(const std::string& filepath,
                             const units::UnitSystem& units,
                             SpanAnalyzerData& data) {
  // initializes file units
  /// \todo fix unit system declaration once units are attributed in file
  units::UnitSystem units_file = units::UnitSystem::kImperial;

  // uses an xml document to load application data
  wxXmlDocument doc;
  if (doc.Load(filepath) == false) {
    wxMessageBox("Could not locate app data file");
  } else {
    // parses the XML root and loads into the application data
    const wxXmlNode* root = doc.GetRoot();
    int line_number = SpanAnalyzerDataXmlHandler::ParseNode(root, data,
                                                            units_file);
    if (line_number != 0) {
      wxString message = "AppData file: Error at line "
                         + std::to_string(line_number);
      wxMessageBox(message);
      return line_number;
    }
  }

  // cables are already converted to consistent unit style

  // converts analysis weathercases to consistent unit style
  for (auto iter = data.weathercases_analysis.begin();
       iter != data.weathercases_analysis.end(); iter++) {
    std::list<WeatherLoadCase>& weathercases = *iter;
    for (auto it = weathercases.begin(); it != weathercases.end(); it++) {
      WeatherLoadCase& weathercase = *it;
      WeatherLoadCaseUnitConverter::ConvertUnitStyle(
          units_file,
          units::UnitStyle::kDifferent,
          units::UnitStyle::kConsistent,
          weathercase);
    }
  }

  // converts unit systems if the file doesn't match applicaton config
  if (units_file != wxGetApp().config()->units) {

  }

  return 0;
}

int FileHandler::LoadCable(const std::string& filepath, Cable& cable) {
  // initializes file units
  /// \todo fix unit system declaration when units are attributed in cable file
  units::UnitSystem units_file = units::UnitSystem::kImperial;

  // uses an xml document to load cable files
  wxXmlDocument doc;
  int line_number = -1;
  if (doc.Load(filepath) == true) {
    /// \todo fix CableComponent constructor to have no default
    ///   coefficients
    cable.component_core.coefficients_polynomial_creep.clear();
    cable.component_core.coefficients_polynomial_loadstrain.clear();
    cable.component_shell.coefficients_polynomial_creep.clear();
    cable.component_shell.coefficients_polynomial_loadstrain.clear();

    // uses cable xml parser to populate cable object
    const wxXmlNode* root = doc.GetRoot();
    line_number = CableXmlHandler::ParseNode(root, cable);
  } else {
    // invalid xml file
    return -1;
  }

  // converts units to consistent style
  CableUnitConverter::ConvertUnitStyle(
      units_file,
      units::UnitStyle::kDifferent,
      units::UnitStyle::kConsistent,
      cable);

  // converts unit systems if the file doesn't match applicaton config
  if (units_file != wxGetApp().config()->units) {

  }

  return line_number;
}

std::list<Cable> FileHandler::LoadCablesFromDirectory(
    const std::string& directory) {
  // initializes list of cables
  std::list<Cable> cables;

  // gets application frame
  SpanAnalyzerFrame* frame = wxGetApp().frame();

  // checks if directory exists
  if (wxDir::Exists(directory) == false) {
    wxString message = "Cable directory does not exist";
    wxMessageBox(message);

    return cables;
  }

  // creates a list of error messages when parsing cable files
  std::list<ErrorMessage> messages;

  // initializes variables
  wxDir dir(directory);
  wxString str_file;

  // gets first file, and goes through all files in the directory
  if (dir.GetFirst(&str_file) == true) {
    while (true) {
      wxFileName file(dir.GetNameWithSep() + str_file);
      if (file.GetExt() == "cable") {
        // creates cable, sends to xml parser
        std::string filepath = file.GetFullPath();
        Cable cable;
        int line_number = FileHandler::LoadCable(filepath, cable);

        // if error is encountered, adds to list to display to user at the end
        if (line_number == 0) {
          cables.push_back(cable);
        } else {
          ErrorMessage message;
          message.title = file.GetFullName();
          message.description = "Parsing error on line "
                                + std::to_string(line_number)
                                + ". File skipped.";
          messages.push_back(message);
        }
      }

      // gets next file in directory
      if (dir.GetNext(&str_file) == false) {
        break;
      }
    }

    // displays errors to user
    if (messages.empty() == false) {
      ErrorMessageDialog dialog(frame, &messages);
      dialog.ShowModal();
    }
  }

  return cables;
}

int FileHandler::LoadConfigFile(SpanAnalyzerConfig& config) {
  // locates config file to fill
  wxFileName path(wxGetApp().directory(), "appconfig", "xml");

  wxXmlDocument doc;
  if (doc.Load(path.GetFullPath()) == false) {
    wxMessageBox("Could not locate config file");
  } else {
    // parses the XML root and loads into the cable object
    const wxXmlNode* root = doc.GetRoot();
    int line_number = SpanAnalyzerConfigXmlHandler::ParseNode(root, config);
    if (line_number != 0) {
      wxString message = "Config file: Error at line "
                         + std::to_string(line_number);
      wxMessageBox(message);
      return line_number;
    }

    if (config.units == units::UnitSystem::kMetric) {
      wxMessageBox("Metric units are not yet supported");
    }
  }

  return 0;
}

void FileHandler::SaveAppData(const std::string& filepath,
                              const SpanAnalyzerData& data) {
  // creates a copy of the data
  SpanAnalyzerData data_converted = data;

  // gets the units
  units::UnitSystem units = wxGetApp().config()->units;

  // converts cables to consistent unit style
  for (auto iter = data_converted.cables.begin();
       iter != data_converted.cables.end(); iter++) {
    Cable& cable = *iter;
    CableUnitConverter::ConvertUnitStyle(
        units,
        units::UnitStyle::kConsistent,
        units::UnitStyle::kDifferent,
        cable);
  }

  // converts analysis weathercases to different unit style
  for (auto iter = data_converted.weathercases_analysis.begin();
       iter != data_converted.weathercases_analysis.end(); iter++) {
    std::list<WeatherLoadCase>& weathercases = *iter;
    for (auto it = weathercases.begin(); it != weathercases.end(); it++) {
      WeatherLoadCase& weathercase = *it;
      WeatherLoadCaseUnitConverter::ConvertUnitStyle(
          units,
          units::UnitStyle::kConsistent,
          units::UnitStyle::kDifferent,
          weathercase);
    }
  }

  // generates a virtual XML document and saves
  wxFileName path(filepath);

  wxXmlDocument doc;
  wxXmlNode* root = SpanAnalyzerDataXmlHandler::CreateNode(data_converted,
                                                           units);
  doc.SetRoot(root);
  doc.Save(path.GetFullPath(), 2);
}

void FileHandler::SaveCable(const std::string& filepath, const Cable& cable,
                            const units::UnitSystem& units) {
  // creates a copy of the cable and converts to different unit style
  Cable cable_converted = cable;
  CableUnitConverter::ConvertUnitStyle(units,
                                       units::UnitStyle::kConsistent,
                                       units::UnitStyle::kDifferent,
                                       cable_converted);

  // generates an xml document and saves
  wxXmlDocument doc;
  wxXmlNode* root = CableXmlHandler::CreateNode(cable_converted, filepath,
                                                units);
  doc.SetRoot(root);
  doc.Save(filepath, 2);
}

void FileHandler::SaveConfigFile(const SpanAnalyzerConfig& config) {
  // generates a virtual XML document and saves
  wxFileName path(wxGetApp().directory(), "appconfig", "xml");

  wxXmlDocument doc;
  wxXmlNode* root = SpanAnalyzerConfigXmlHandler::CreateNode(config);
  doc.SetRoot(root);
  doc.Save(path.GetFullPath(), 2);
}
