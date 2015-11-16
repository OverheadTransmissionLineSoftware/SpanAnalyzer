// This is free and unencumbered software released into the public domain.
// For more information, please refer to <http://unlicense.org/>

#ifndef OTLS_SPANANALYZER_FILEHANDLER_H_
#define OTLS_SPANANALYZER_FILEHANDLER_H_

#include <string>

#include "span_analyzer_config.h"
#include "span_analyzer_data.h"

/// \par OVERVIEW
///
class FileHandler {
 public:
  /// \brief Constructor.
  FileHandler();

  /// \brief Destructor.
  ~FileHandler();

  /// \brief Parses the cable files in the specified directory.
  /// \todo this needs some sort of parsing feedback.
  /// \todo Does this need to be a public function? Why not embed within the
  /// parse config file. Or why not create a Load/Save config.
  static void LoadAppData(const std::string& directory_cables,
                          units::UnitSystem& units,
                          SpanAnalyzerData& data);

  /// \brief Parses the config file.
  /// \param[in] config
  ///   The application configuration settings.
  /// This function will parse a file in the application directory.
  static int LoadConfigFile(SpanAnalyzerConfig& config);

  /// \brief Generates an application configuration file.
  /// This function will create a file in the application directory.
  static void SaveConfigFile(const SpanAnalyzerConfig& config);
};

#endif  // OTLS_SPANANALYZER_FILEHANDLER_H_
