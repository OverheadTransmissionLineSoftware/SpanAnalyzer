// This is free and unencumbered software released into the public domain.
// For more information, please refer to <http://unlicense.org/>

#ifndef OTLS_SPANANALYZER_FILEHANDLER_H_
#define OTLS_SPANANALYZER_FILEHANDLER_H_

#include <list>
#include <string>

#include "span_analyzer_config.h"
#include "span_analyzer_data.h"

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
  /// \brief Constructor.
  FileHandler();

  /// \brief Destructor.
  ~FileHandler();

  /// \brief Loads the application data file.
  /// \param[in] filepath
  ///   The file path for the application data.
  /// \param[in] units
  ///   The unit system to convert to.
  /// \param[out] data
  ///   The application data that is populated.
  /// \return 0 if no errors were encountered, -1 if the file did not exist, or
  ///   the line number if an error was encountered in the file.
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
  /// \return The file line number of a critical error. Returns 0 if no critical
  ///   errors were encountered.
  /// Critical errors cause the loading to abort, but are not logged.
  /// Non-critical errors are directed to the active application log target.
  static int LoadCable(const std::string& filepath,
                       const units::UnitSystem& units,
                       Cable& cable);

  /// \brief Loads the config file.
  /// \param[in] filepath
  ///   The filepath.
  /// \param[out] config
  ///   The application configuration settings.
  /// \return 0 if no errors were encountered, -1 if the file did not exist, or
  ///   the line number if an error was encountered in the file.
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
  static void SaveCable(const std::string& filepath, const Cable& cable,
                        const units::UnitSystem& units);

  /// \brief Saves the application configuration file.
  /// \param[in] filepath
  ///   The filepath.
  /// \param[in] config
  ///   The application configuration settings.
  /// This function will create a file in the application directory.
  static void SaveConfigFile(const std::string& filepath,
                             const SpanAnalyzerConfig& config);
};

#endif  // OTLS_SPANANALYZER_FILEHANDLER_H_
