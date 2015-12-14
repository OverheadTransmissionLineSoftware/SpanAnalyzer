// This is free and unencumbered software released into the public domain.
// For more information, please refer to <http://unlicense.org/>

#ifndef OTLS_SPANANALYZER_FILEHANDLER_H_
#define OTLS_SPANANALYZER_FILEHANDLER_H_

#include <string>

#include "span_analyzer_config.h"
#include "span_analyzer_data.h"

/// \par OVERVIEW
///
/// This class handles reading and writing application files that are not
/// document related.
class FileHandler {
 public:
  /// \brief Constructor.
  FileHandler();

  /// \brief Destructor.
  ~FileHandler();

  /// \brief Loads the application data at the provided file path.
  /// \param[in] filepath
  ///   The file path for the application data.
  /// \param[in] units
  ///   The unit system. This is used for converting to consistent units.
  /// \param[out] data
  ///   The application data that is populated.
  /// \return The file line number of the node if the content could not be
  ///   converted to the expected data type. Returns 0 if no errors were
  ///   encountered.
  /// \todo this needs some sort of parsing feedback.
  /// \todo Does this need to be a public function? Why not embed within the
  /// parse config file. Or why not create a Load/Save config.
  static int LoadAppData(std::string filepath, units::UnitSystem& units,
                         SpanAnalyzerData& data);

  /// \brief Parses the config file.
  /// \param[out] config
  ///   The application configuration settings.
  /// \return The file line number of the node if the content could not be
  ///   converted to the expected data type. Returns 0 if no errors were
  ///   encountered.
  /// This function will parse a file in the application directory.
  static int LoadConfigFile(SpanAnalyzerConfig& config);

  /// \brief Generates an application configuration file.
  /// \param[in] config
  ///   The application configuration settings.
  /// This function will create a file in the application directory.
  static void SaveConfigFile(const SpanAnalyzerConfig& config);
};

#endif  // OTLS_SPANANALYZER_FILEHANDLER_H_
