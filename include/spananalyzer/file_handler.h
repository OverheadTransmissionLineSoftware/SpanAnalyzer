// This is free and unencumbered software released into the public domain.
// For more information, please refer to <http://unlicense.org/>

#ifndef OTLS_SPANANALYZER_FILEHANDLER_H_
#define OTLS_SPANANALYZER_FILEHANDLER_H_

#include <list>
#include <string>

#include "spananalyzer/span_analyzer_config.h"
#include "spananalyzer/span_analyzer_data.h"

/// \par OVERVIEW
///
/// This class handles reading and writing application files that are not
/// document related.
///
/// \par UNIT CONVERSION
///
/// The functions in this class convert the file unit system and style to match
/// the needs of the application. If the file unit system is different than the
/// application unit system, these functions will convert to application unit
/// system. The files will always contain information in user-friendly
/// units, but the data is converted to consistent unit style.
class FileHandler {
 public:
  /// \brief Loads the application data file.
  /// \param[in] filepath
  ///   The file path for the application data.
  /// \param[in] units
  ///   The unit system to convert to.
  /// \param[out] data
  ///   The application data that is populated.
  /// \return 0 if no errors, -1 if file related errors, or 1 if parsing
  ///   errors are encountered.
  /// All errors are logged to the active application log target.
  static int LoadAppData(const std::string& filepath,
                         const units::UnitSystem& units,
                         SpanAnalyzerData& data);

  /// \brief Loads a cable file.
  /// \param[in] filepath
  ///   The filepath.
  /// \param[in] units
  ///   The unit system to convert to.
  /// \param[out] cable
  ///   The cable that is populated.
  /// \return 0 if no errors, -1 if file related errors, or 1 if parsing
  ///   errors are encountered.
  /// All errors are logged to the active application log target.
  static int LoadCable(const std::string& filepath,
                       const units::UnitSystem& units,
                       Cable& cable);

  /// \brief Loads the config file.
  /// \param[in] filepath
  ///   The filepath.
  /// \param[out] config
  ///   The application configuration settings.
  /// \return 0 if no errors, -1 if file related errors, or 1 if parsing
  ///   errors are encountered.
  /// All errors are logged to the active application log target.
  static int LoadConfigFile(const std::string& filepath,
                            SpanAnalyzerConfig& config);

  /// \brief Saves the application data file.
  /// \param[in] filepath
  ///   The filepath.
  /// \param[in] data
  ///   The application data.
  /// \param[in] units
  ///   The current unit system, which will be labeled in the file.
  static void SaveAppData(const std::string& filepath,
                          const SpanAnalyzerData& data,
                          const units::UnitSystem& units);

  /// \brief Saves a cable file.
  /// \param[in] filepath
  ///   The filepath.
  /// \param[in] cable
  ///   The cable.
  /// \param[in] units
  ///   The current unit system, which will be labeled in the file.
  /// This function will create a copy of the cable before converting the
  /// unit style so the cable will not be modified.
  static void SaveCable(const std::string& filepath, const Cable& cable,
                        const units::UnitSystem& units);

  /// \brief Saves the application configuration file.
  /// \param[in] filepath
  ///   The filepath.
  /// \param[in] config
  ///   The application configuration settings.
  static void SaveConfigFile(const std::string& filepath,
                             const SpanAnalyzerConfig& config);
};

#endif  // OTLS_SPANANALYZER_FILEHANDLER_H_
