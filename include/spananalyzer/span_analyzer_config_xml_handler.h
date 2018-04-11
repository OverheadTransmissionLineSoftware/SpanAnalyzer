// This is free and unencumbered software released into the public domain.
// For more information, please refer to <http://unlicense.org/>

#ifndef OTLS_SPANANALYZER_SPANANALYZERCONFIGXMLHANDLER_H_
#define OTLS_SPANANALYZER_SPANANALYZERCONFIGXMLHANDLER_H_

#include "appcommon/xml/xml_handler.h"
#include "wx/xml/xml.h"

#include "spananalyzer/span_analyzer_config.h"

/// \par OVERVIEW
///
/// This class parses and generates a SpanAnalyzerConfig XML node. The data is
/// transferred between the XML node and the data object.
///
/// \par VERSION
///
/// This class can parse all versions of the XML node. However, new nodes will
/// only be generated with the most recent version.
class SpanAnalyzerConfigXmlHandler : public XmlHandler {
 public:
  /// \brief Creates an XML node for the config settings.
  /// \param[in] config
  ///   The config settings.
  /// \return An XML node for the config settings.
  static wxXmlNode* CreateNode(const SpanAnalyzerConfig& config);

  /// \brief Parses an XML node and populates the config settings.
  /// \param[in] root
  ///   The XML root node for the config settings.
  /// \param[in] filepath
  ///   The filepath that the xml node was loaded from. This is for logging
  ///   purposes only and can be left blank.
  /// \param[out] config
  ///   The config settings that are populated.
  /// \return The status of the xml node parse. If any errors are encountered
  ///   false is returned.
  /// All errors are logged to the active application log target. Critical
  /// errors cause the parsing to abort. Non-critical errors set the object
  /// property to an invalid state (if applicable).
  static bool ParseNode(const wxXmlNode* root,
                        const std::string& filepath,
                        SpanAnalyzerConfig& config);

 private:
  /// \brief Parses a version 1 XML node and populates the config settings.
  /// \param[in] root
  ///   The XML root node for the config settings.
  /// \param[in] filepath
  ///   The filepath that the xml node was loaded from. This is for logging
  ///   purposes only and can be left blank.
  /// \param[out] config
  ///   The config settings that are populated.
  /// \return The status of the xml node parse. If any errors are encountered
  ///   false is returned.
  /// All errors are logged to the active application log target. Critical
  /// errors cause the parsing to abort. Non-critical errors set the object
  /// property to an invalid state (if applicable).
  static bool ParseNodeV1(const wxXmlNode* root,
                          const std::string& filepath,
                          SpanAnalyzerConfig& config);
};

#endif  // OTLS_SPANANALYZER_SPANANALYZERCONFIGXMLHANDLER_H_
