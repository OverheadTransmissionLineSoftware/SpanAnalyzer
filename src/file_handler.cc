// This is free and unencumbered software released into the public domain.
// For more information, please refer to <http://unlicense.org/>

#include "spananalyzer/file_handler.h"

#include "appcommon/units/cable_unit_converter.h"
#include "appcommon/units/weather_load_case_unit_converter.h"
#include "appcommon/widgets/error_message_dialog.h"
#include "appcommon/widgets/status_bar_log.h"
#include "appcommon/xml/cable_xml_handler.h"
#include "wx/dir.h"
#include "wx/filename.h"
#include "wx/xml/xml.h"

#include "spananalyzer/span_analyzer_app.h"
#include "spananalyzer/span_analyzer_config_xml_handler.h"
#include "spananalyzer/span_analyzer_data_xml_handler.h"

int FileHandler::LoadAppData(const std::string& filepath,
                             const units::UnitSystem& units,
                             SpanAnalyzerData& data) {
  std::string message = "Loading application data file: " + filepath;
  wxLogVerbose(message.c_str());
  status_bar_log::PushText(message, 0);

  // checks if the file exists
  if (wxFileName::Exists(filepath) == false) {
    message = filepath + "  --  "
              "File does not exist. Aborting.";
    wxLogError(message.c_str());
    return -1;
  }

  // uses an xml document to load app data file
  wxXmlDocument doc;
  if (doc.Load(filepath) == false) {
    message = filepath + "  --  "
              "Application data file contains an invalid xml structure. "
              "Aborting.";
    wxLogError(message.c_str());
    return -1;
  }

  // checks for valid xml root
  const wxXmlNode* root = doc.GetRoot();
  if (root->GetName() != "span_analyzer_data") {
    message = filepath + "  --  "
              "Application data file contains an invalid xml root. Aborting.";
    wxLogError(message.c_str());
    return 1;
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
      message = filepath + "  --  "
                "Application data file contains an invalid units attribute. "
                "Aborting.";
      wxLogError(message.c_str());
      return root->GetLineNumber();
    }
  } else {
    message = filepath + "  --  "
              "Application data file is missing units attribute. Aborting.";
    wxLogError(message.c_str());
    return root->GetLineNumber();
  }

  // parses the xml node to populate data object
  const bool status_node = SpanAnalyzerDataXmlHandler::ParseNode(
      root, filepath, units_file, data);

  // converts weathercases to consistent unit style
  for (auto iter = data.weathercases.begin();
       iter != data.weathercases.end(); iter++) {
    WeatherLoadCase* weathercase = *iter;
    WeatherLoadCaseUnitConverter::ConvertUnitStyle(
        units_file,
        units::UnitStyle::kDifferent,
        units::UnitStyle::kConsistent,
        *weathercase);
  }

  // converts unit systems if the file doesn't match applicaton config
  if (units_file != wxGetApp().config()->units) {
    for (auto iter = data.weathercases.begin();
          iter != data.weathercases.end(); iter++) {
      WeatherLoadCase* weathercase = *iter;
      WeatherLoadCaseUnitConverter::ConvertUnitSystem(
          units_file,
          units,
          *weathercase);
    }
  }

  // resets statusbar
  status_bar_log::PopText(0);

  // selects return based on parsing status
  if (status_node == true) {
    return 0;
  } else {
    return 1;
  }
}

int FileHandler::LoadCable(const std::string& filepath,
                           const units::UnitSystem& units,
                           Cable& cable) {
  std::string message = "Loading cable file: " + filepath;
  wxLogVerbose(message.c_str());
  status_bar_log::PushText(message, 0);

  // checks if the file exists
  if (wxFileName::Exists(filepath) == false) {
    message = "Cable file (" + filepath + ") does not exist. Aborting.";
    wxLogError(message.c_str());
    return -1;
  }

  // uses an xml document to load cable file
  wxXmlDocument doc;
  if (doc.Load(filepath) == false) {
    message = filepath + "  --  "
              "Cable file contains an invalid xml structure. Aborting.";
    wxLogError(message.c_str());
    return -1;
  }

  // checks for valid xml root
  const wxXmlNode* root = doc.GetRoot();
  if (root->GetName() != "cable") {
    message = filepath + "  --  "
              "Cable file contains an invalid xml root. Aborting.";
    wxLogError(message.c_str());
    return 1;
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
      message = filepath + "  --  "
                "Cable file contains an invalid units attribute. Aborting.";
      wxLogError(message.c_str());
      return 1;
    }
  } else {
    message = filepath + "  --  "
              "Cable file is missing units attribute. Aborting.";
    wxLogError(message.c_str());
    return 1;
  }

  // parses the xml node to populate cable object
  const bool status_node = CableXmlHandler::ParseNode(root, filepath, cable);

  // converts units to consistent style
  CableUnitConverter::ConvertUnitStyle(
      units_file,
      units::UnitStyle::kDifferent,
      units::UnitStyle::kConsistent,
      true,
      cable);

  // converts unit systems if the file doesn't match applicaton config
  units::UnitSystem units_config = wxGetApp().config()->units;
  if (units_file != units_config) {
    CableUnitConverter::ConvertUnitSystem(units_file, units_config,
                                          true, cable);
  }

  // adds any missing polynomial coefficients
  // at least 5 coefficients per polynomial
  std::vector<double>* coefficients = nullptr;
  const unsigned int kSizeRequired = 5;
  int num_needed = 0;

  coefficients = &cable.component_core.coefficients_polynomial_creep;
  num_needed = kSizeRequired - coefficients->size();
  for (int i = 0; i < num_needed; i++) {
    coefficients->push_back(0);
  }

  coefficients = &cable.component_core.coefficients_polynomial_loadstrain;
  num_needed = kSizeRequired - coefficients->size();
  for (int i = 0; i < num_needed; i++) {
    coefficients->push_back(0);
  }

  coefficients = &cable.component_shell.coefficients_polynomial_creep;
  num_needed = kSizeRequired - coefficients->size();
  for (int i = 0; i < num_needed; i++) {
    coefficients->push_back(0);
  }

  coefficients = &cable.component_shell.coefficients_polynomial_loadstrain;
  num_needed = kSizeRequired - coefficients->size();
  for (int i = 0; i < num_needed; i++) {
    coefficients->push_back(0);
  }

  // sets polynomial scaling factors
  cable.component_core.scale_polynomial_x = 0.01;
  cable.component_core.scale_polynomial_y = 1;

  cable.component_shell.scale_polynomial_x = 0.01;
  cable.component_shell.scale_polynomial_y = 1;

  // resets statusbar
  status_bar_log::PopText(0);

  // selects return based on parsing status
  if (status_node == true) {
    return 0;
  } else {
    return 1;
  }
}

int FileHandler::LoadConfigFile(const std::string& filepath,
                                SpanAnalyzerConfig& config) {
  std::string message = "Loading config file: " + filepath;
  wxLogVerbose(message.c_str());
  status_bar_log::PushText(message, 0);

  // checks if the file exists
  if (wxFileName::Exists(filepath) == false) {
    message = "Config file (" + filepath + ") does not exist. Keeping "
              "application defaults.";
    wxLogError(message.c_str());
    return -1;
  }

  // uses an xml document to load config file
  wxXmlDocument doc;
  if (doc.Load(filepath) == false) {
    message = filepath + "  --  "
              "Config file contains an invalid xml structure. Keeping "
              "application defaults.";
    wxLogError(message.c_str());
    return -1;
  }

  // checks for valid xml root
  const wxXmlNode* root = doc.GetRoot();
  if (root->GetName() != "span_analyzer_config") {
    message = filepath + "  --  "
              "Config file contains an invalid xml root. Keeping "
              "application defaults.";
    wxLogError(message.c_str());
    return 1;
  }

  // parses the XML node and loads into the config struct
  const bool status_node = SpanAnalyzerConfigXmlHandler::ParseNode(
      root, filepath, config);

  // resets statusbar
  status_bar_log::PopText(0);

  // selects return based on parsing status
  if (status_node == true) {
    return 0;
  } else {
    return 1;
  }
}

/// To avoid re-allocating all of the app data for a different unit style,
/// this function will convert the data to the different unit style and
/// then back to the consistent unit style.
void FileHandler::SaveAppData(const std::string& filepath,
                              const SpanAnalyzerData& data,
                              const units::UnitSystem& units) {
  // logs
  std::string message = "Saving application data file: " + filepath;
  wxLogVerbose(message.c_str());
  status_bar_log::PushText(message, 0);

  // cables are stored in individual files, and are not included in the app
  // data file

  // converts weathercases to different unit style
  for (auto iter = data.weathercases.begin();
        iter != data.weathercases.end(); iter++) {
    WeatherLoadCase* weathercase = *iter;
    WeatherLoadCaseUnitConverter::ConvertUnitStyle(
        units,
        units::UnitStyle::kConsistent,
        units::UnitStyle::kDifferent,
        *weathercase);
  }

  // generates an xml node
  wxXmlNode* root = SpanAnalyzerDataXmlHandler::CreateNode(data, units);

  // gets the units
  if (units == units::UnitSystem::kImperial) {
    root->AddAttribute("units", "Imperial");
  } else if (units == units::UnitSystem::kMetric) {
    root->AddAttribute("units", "Metric");
  }

  // creates any directories that are needed
  wxFileName filename(filepath);
  if (filename.DirExists() == false) {
    filename.Mkdir(wxS_DIR_DEFAULT, wxPATH_MKDIR_FULL);
  }

  // creates an xml document and saves
  wxXmlDocument doc;
  doc.SetRoot(root);
  bool status = doc.Save(filepath, 2);
  if (status == false) {
    wxLogError("File didn't save");
  }

  // converts weathercases to different unit style
  for (auto iter = data.weathercases.begin();
        iter != data.weathercases.end(); iter++) {
    WeatherLoadCase* weathercase = *iter;
    WeatherLoadCaseUnitConverter::ConvertUnitStyle(
        units,
        units::UnitStyle::kDifferent,
        units::UnitStyle::kConsistent,
        *weathercase);
  }

  // resets statusbar
  status_bar_log::PopText(0);
}

void FileHandler::SaveCable(const std::string& filepath, const Cable& cable,
                            const units::UnitSystem& units) {
  // logs
  std::string message = "Saving cable file: " + filepath;
  wxLogVerbose(message.c_str());
  status_bar_log::PushText(message, 0);

  // creates a copy of the cable and converts to different unit style
  Cable cable_converted = cable;
  CableUnitConverter::ConvertUnitStyle(units,
                                       units::UnitStyle::kConsistent,
                                       units::UnitStyle::kDifferent,
                                       true,
                                       cable_converted);

  // generates an xml node
  wxXmlNode* root = CableXmlHandler::CreateNode(cable_converted, "", units);

  // gets the units and adds attribute
  if (units == units::UnitSystem::kImperial) {
    root->AddAttribute("units", "Imperial");
  } else if (units == units::UnitSystem::kMetric) {
    root->AddAttribute("units", "Metric");
  }

  // creates any directories that are needed
  wxFileName filename(filepath);
  if (filename.DirExists() == false) {
    filename.Mkdir(wxS_DIR_DEFAULT, wxPATH_MKDIR_FULL);
  }

  // creates an xml document and saves
  wxXmlDocument doc;
  doc.SetRoot(root);
  bool status = doc.Save(filepath, 2);
  if (status == false) {
    wxLogError("File didn't save");
  }

  // resets statusbar
  status_bar_log::PopText(0);
}

void FileHandler::SaveConfigFile(const std::string& filepath,
                                 const SpanAnalyzerConfig& config) {
  // logs
  std::string message = "Saving config file: " + filepath;
  wxLogVerbose(message.c_str());
  status_bar_log::PushText(message, 0);

  // generates an xml node
  wxXmlNode* root = SpanAnalyzerConfigXmlHandler::CreateNode(config);

  // creates any directories that are needed
  wxFileName filename(filepath);
  if (filename.DirExists() == false) {
    filename.Mkdir(wxS_DIR_DEFAULT, wxPATH_MKDIR_FULL);
  }

  // creates an xml document and saves
  wxXmlDocument doc;
  doc.SetRoot(root);
  bool status = doc.Save(filepath, 2);
  if (status == false) {
    wxLogError("File didn't save");
  }

  // resets statusbar
  status_bar_log::PopText(0);
}
