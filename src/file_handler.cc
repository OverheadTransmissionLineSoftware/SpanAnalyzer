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
  std::string message = "Loading application data file: " + filepath;
  wxLogMessage(message.c_str());

  // checks if the file exists
  if (wxFileName::Exists(filepath) == false) {
    wxLogError("Application data file does not exist. Aborting.");
    return -1;
  }

  // uses an xml document to load app data file
  wxXmlDocument doc;
  if (doc.Load(filepath) == false) {
    wxLogError("Application data file contains an invalid xml structure. "
               "Aborting.");
    return -1;
  }

  // checks for valid xml root
  const wxXmlNode* root = doc.GetRoot();
  if (root->GetName() != "span_analyzer_data") {
    wxLogError("Application data file contains an invalid xml root. Aborting.");
    return root->GetLineNumber();
  }

  // gets unit system attribute from file
  wxString str_units;
  units::UnitSystem units_file;
  if (root->GetAttribute("units", &str_units) == true) {
    if (str_units == "Imperial") {
      units_file = units::UnitSystem::kImperial;
    } else if (str_units == "Metric") {
      units_file = units::UnitSystem::kMetric;
    } else {
      wxLogError("Application data file contains an invalid units attribute. "
                 "Aborting.");
      return root->GetLineNumber();
    }
  } else {
    wxLogError("Application data file is missing units attribute. Aborting.");
    return root->GetLineNumber();
  }

  // parses the xml node to populate data object
  int line_number = SpanAnalyzerDataXmlHandler::ParseNode(root, filepath,
                                                          units_file, data);
  if (line_number != 0) {
    std::string message = "Applicaton data file contains a critical error on "
                          "line " + std::to_string(line_number) + "  --  "
                          "Aborting.";
    return line_number;
  }

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
    for (auto iter = data.weathercases_analysis.begin();
         iter != data.weathercases_analysis.end(); iter++) {
      std::list<WeatherLoadCase>& weathercases = *iter;
      for (auto it = weathercases.begin(); it != weathercases.end(); it++) {
        WeatherLoadCase& weathercase = *it;
        WeatherLoadCaseUnitConverter::ConvertUnitSystem(
            units_file,
            units,
            weathercase);
      }
    }
  }

  return 0;
}

int FileHandler::LoadCable(const std::string& filepath,
                           const units::UnitSystem& units,
                           Cable& cable) {
  std::string message = "Loading cable file: " + filepath;
  wxLogMessage(message.c_str());

  // checks if the file exists
  if (wxFileName::Exists(filepath) == false) {
    wxLogError("Cable file does not exist. Aborting.");
    return -1;
  }

  // uses an xml document to load cable file
  wxXmlDocument doc;
  if (doc.Load(filepath) == false) {
    wxLogError("Cable file contains an invalid xml structure. Aborting.");
    return -1;
  }

  // checks for valid xml root
  const wxXmlNode* root = doc.GetRoot();
  if (root->GetName() != "cable") {
    wxLogError("Cable file contains an invalid xml root. Aborting.");
    return root->GetLineNumber();
  }

  // gets unit system attribute from file
  wxString str_units;
  units::UnitSystem units_file;
  if (root->GetAttribute("units", &str_units) == true) {
    if (str_units == "Imperial") {
      units_file = units::UnitSystem::kImperial;
    } else if (str_units == "Metric") {
      units_file = units::UnitSystem::kMetric;
    } else {
      wxLogError("Cable file contains an invalid units attribute. Aborting.");
      return root->GetLineNumber();
    }
  } else {
    wxLogError("Cable file is missing units attribute. Aborting.");
    return root->GetLineNumber();
  }

  // parses the xml node to populate cable object
  /// \todo fix CableComponent constructor to have no default
  ///   coefficients
  cable.component_core.coefficients_polynomial_creep.clear();
  cable.component_core.coefficients_polynomial_loadstrain.clear();
  cable.component_shell.coefficients_polynomial_creep.clear();
  cable.component_shell.coefficients_polynomial_loadstrain.clear();

  int line_number = CableXmlHandler::ParseNode(root, filepath, cable);
  if (line_number != 0) {
    std::string message = "Cable file contains a critical error on line "
                          + std::to_string(line_number)
                          + ". Aborting.";
    wxLogError(message.c_str());
    return line_number;
  }

  // converts units to consistent style
  CableUnitConverter::ConvertUnitStyle(
      units_file,
      units::UnitStyle::kDifferent,
      units::UnitStyle::kConsistent,
      cable);

  // converts unit systems if the file doesn't match applicaton config
  units::UnitSystem units_config = wxGetApp().config()->units;
  if (units_file != units_config) {
    CableUnitConverter::ConvertUnitSystem(units_file, units_config, cable);
  }

  return line_number;
}

int FileHandler::LoadConfigFile(const std::string& filepath,
                                SpanAnalyzerConfig& config) {
  // checks if the file exists
  if (wxFileName::Exists(filepath) == false) {
    return -1;
  }

  // uses an xml document to load config file
  wxXmlDocument doc;
  if (doc.Load(filepath) == false) {
    return -1;
  }

  // checks for valid xml root
  const wxXmlNode* root = doc.GetRoot();
  if (root->GetName() != "span_analyzer_config") {
    return root->GetLineNumber();
  }

  // parses the XML node and loads into the config struct
  int line_number = SpanAnalyzerConfigXmlHandler::ParseNode(root, config);
  if (line_number != 0) {
    return line_number;
  }

  return 0;
}

void FileHandler::SaveAppData(const std::string& filepath,
                              const SpanAnalyzerData& data,
                              const units::UnitSystem& units) {
  // creates a copy of the data
  SpanAnalyzerData data_converted = data;

  // cables are stored in individual files, and are not included in the app
  // data file

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

  // generates an xml node
  wxXmlNode* root = SpanAnalyzerDataXmlHandler::CreateNode(data_converted,
                                                           units);

  // gets the units
  if (units == units::UnitSystem::kImperial) {
    root->AddAttribute("units", "Imperial");
  } else if (units == units::UnitSystem::kMetric) {
    root->AddAttribute("units", "Metric");
  }

  // creates an xml document and saves
  wxXmlDocument doc;
  doc.SetRoot(root);
  doc.Save(filepath, 2);
}

void FileHandler::SaveCable(const std::string& filepath, const Cable& cable,
                            const units::UnitSystem& units) {
  // creates a copy of the cable and converts to different unit style
  Cable cable_converted = cable;
  CableUnitConverter::ConvertUnitStyle(units,
                                       units::UnitStyle::kConsistent,
                                       units::UnitStyle::kDifferent,
                                       cable_converted);

  // generates an xml node
  wxXmlNode* root = CableXmlHandler::CreateNode(cable_converted, filepath,
                                                units);
  // gets the units
  if (units == units::UnitSystem::kImperial) {
    root->AddAttribute("units", "Imperial");
  } else if (units == units::UnitSystem::kMetric) {
    root->AddAttribute("units", "Metric");
  }

  // creates an xml document and saves
  wxXmlDocument doc;
  doc.SetRoot(root);
  doc.Save(filepath, 2);
}

void FileHandler::SaveConfigFile(const std::string& filepath,
                                 const SpanAnalyzerConfig& config) {
  // generates an xml node
  wxXmlNode* root = SpanAnalyzerConfigXmlHandler::CreateNode(config);

  // creates an xml document and saves
  wxXmlDocument doc;
  doc.SetRoot(root);
  doc.Save(filepath, 2);
}
