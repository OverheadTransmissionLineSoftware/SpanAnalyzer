// This is free and unencumbered software released into the public domain.
// For more information, please refer to <http://unlicense.org/>

#ifndef SPANANALYZER_SPAN_ANALYZER_DATA_XML_HANDLER_H_
#define SPANANALYZER_SPAN_ANALYZER_DATA_XML_HANDLER_H_

#include <string>

#include "appcommon/xml/xml_handler.h"
#include "models/base/units.h"
#include "wx/xml/xml.h"

#include "spananalyzer/span_analyzer_data.h"

/// \par OVERVIEW
///
/// This class parses and generates a span analyzer data XML node. The data is
/// transferred between the XML node and the data object.
///
/// \par VERSION
///
/// This class can parse all versions of the XML node. However, new nodes will
/// only be generated with the most recent version.
///
/// \par UNIT ATTRIBUTES
///
/// This class supports attributing the child XML nodes for various unit
/// systems and styles.
class SpanAnalyzerDataXmlHandler : public XmlHandler {
 public:
  /// \brief Creates an XML node for a span analyzer data object.
  /// \param[in] data
  ///   The data.
  /// \param[in] system_units
  ///   The unit system, which is used for attributing child XML nodes.
  /// \param[in] style_units
  ///   The unit style, which is used for attributing child XML nodes.
  /// \return An XML node for the document.
  static wxXmlNode* CreateNode(const SpanAnalyzerData& data,
                               const units::UnitSystem& system_units,
                               const units::UnitStyle& style_units);

  /// \brief Parses an XML node and populates a data object.
  /// \param[in] root
  ///   The XML root node for the data.
  /// \param[in] filepath
  ///   The filepath that the xml node was loaded from. This is for logging
  ///   purposes only and can be left blank.
  /// \param[in] units
  ///   The unit system, which is used by the filehandler to convert external
  ///   files (ex: cable files) as they are loaded into the data. Data that is
  ///   stored internally in the data file is NOT converted.
  /// \param[out] data
  ///   The data.
  /// \return The status of the xml node parse. If any errors are encountered
  ///   false is returned.
  /// All errors are logged to the active application log target. Critical
  /// errors cause the parsing to abort. Non-critical errors set the object
  /// property to an invalid state (if applicable).
  static bool ParseNode(const wxXmlNode* root,
                        const std::string& filepath,
                        const units::UnitSystem& units,
                        SpanAnalyzerData& data);

 private:
  /// \brief Parses a version 1 XML node and populates a data object.
  /// \param[in] root
  ///   The XML root node for the document.
  /// \param[in] filepath
  ///   The filepath that the xml node was loaded from. This is for logging
  ///   purposes only and can be left blank.
  /// \param[in] units
  ///   The unit system, which is used by the filehandler to convert external
  ///   files (ex: cable files) as they are loaded into the data. Data that is
  ///   stored internally in the data file is NOT converted.
  /// \param[out] data
  ///   The data.
  /// \return The status of the xml node parse. If any errors are encountered
  ///   false is returned.
  /// All errors are logged to the active application log target. Critical
  /// errors cause the parsing to abort. Non-critical errors set the object
  /// property to an invalid state (if applicable).
  static bool ParseNodeV1(const wxXmlNode* root,
                          const std::string& filepath,
                          const units::UnitSystem& units,
                          SpanAnalyzerData& data);

  /// \brief Parses a version 2 XML node and populates a data object.
  /// \param[in] root
  ///   The XML root node for the document.
  /// \param[in] filepath
  ///   The filepath that the xml node was loaded from. This is for logging
  ///   purposes only and can be left blank.
  /// \param[in] units
  ///   The unit system, which is used by the filehandler to convert external
  ///   files (ex: cable files) as they are loaded into the data. Data that is
  ///   stored internally in the data file is NOT converted.
  /// \param[out] data
  ///   The data.
  /// \return The status of the xml node parse. If any errors are encountered
  ///   false is returned.
  /// All errors are logged to the active application log target. Critical
  /// errors cause the parsing to abort. Non-critical errors set the object
  /// property to an invalid state (if applicable).
  static bool ParseNodeV2(const wxXmlNode* root,
                          const std::string& filepath,
                          const units::UnitSystem& units,
                          SpanAnalyzerData& data);
};

#endif  // SPANANALYZER_SPAN_ANALYZER_DATA_XML_HANDLER_H_
