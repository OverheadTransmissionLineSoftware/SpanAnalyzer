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
  // checks if the file exists
  if (wxFileName::Exists(filepath) == false) {
    return -1;
  }

  // uses an xml document to load app data file
  wxXmlDocument doc;
  if (doc.Load(filepath) == false) {
    // invalid xml file
    return -1;
  }

  // checks for valid xml root
  const wxXmlNode* root = doc.GetRoot();
  if (root->GetName() != "span_analyzer_data") {
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
      return root->GetLineNumber();
    }
  } else {
    return root->GetLineNumber();
  }

  // parses the xml node to populate data object
  int line_number = SpanAnalyzerDataXmlHandler::ParseNode(root, data,
                                                          units_file);
  if (line_number != 0) {
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
  // checks if the file exists
  if (wxFileName::Exists(filepath) == false) {
    return -1;
  }

  // uses an xml document to load cable file
  wxXmlDocument doc;
  if (doc.Load(filepath) == false) {
    // invalid xml file
    return -1;
  }

  // checks for valid xml root
  const wxXmlNode* root = doc.GetRoot();
  if (root->GetName() != "cable") {
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
      return root->GetLineNumber();
    }
  } else {
    return root->GetLineNumber();
  }

  // parses the xml node to populate cable object
  /// \todo fix CableComponent constructor to have no default
  ///   coefficients
  cable.component_core.coefficients_polynomial_creep.clear();
  cable.component_core.coefficients_polynomial_loadstrain.clear();
  cable.component_shell.coefficients_polynomial_creep.clear();
  cable.component_shell.coefficients_polynomial_loadstrain.clear();

  int line_number = CableXmlHandler::ParseNode(root, cable);
  if (line_number != 0) {
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

std::list<Cable> FileHandler::LoadCablesFromDirectory(
    const std::string& directory,
    const units::UnitSystem& units) {
  // initializes list of cables
  std::list<Cable> cables;

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
        int line_number = FileHandler::LoadCable(filepath, units, cable);

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
      SpanAnalyzerFrame* frame = wxGetApp().frame();
      ErrorMessageDialog dialog(frame, &messages);
      dialog.ShowModal();
    }
  }

  return cables;
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
  // generates a virtual XML document and saves
  wxXmlDocument doc;
  wxXmlNode* root = SpanAnalyzerConfigXmlHandler::CreateNode(config);
  doc.SetRoot(root);
  doc.Save(filepath, 2);
}
