// This is free and unencumbered software released into the public domain.
// For more information, please refer to <http://unlicense.org/>

#ifndef SPANANALYZER_SPAN_ANALYZER_DOC_XML_HANDLER_H_
#define SPANANALYZER_SPAN_ANALYZER_DOC_XML_HANDLER_H_

#include <list>
#include <string>

#include "appcommon/xml/xml_handler.h"
#include "models/base/units.h"

#include "spananalyzer/span_analyzer_data.h"
#include "spananalyzer/span_analyzer_doc.h"

/// \par OVERVIEW
///
/// This class parses and generates a span XML node. The data is transferred
/// between the XML node and the data object.
///
/// \par VERSION
///
/// This class can parse all versions of the XML node. However, new nodes will
/// only be generated with the most recent version.
///
/// \par UNIT CONVERSIONS
///
/// This class can optionally convert the unit style to 'consistent' when
/// parsing. The conversion will occur after the xml node has been parsed. The
/// respective unit converter class will perform the conversion using the
/// converter version that matches the xml node version.
///
/// \par UNIT ATTRIBUTES
///
/// This class supports attributing the child XML nodes for various unit
/// systems.
class SpanAnalyzerDocXmlHandler : public XmlHandler {
 public:
  /// \brief Creates an XML node for a span analyzer document.
  /// \param[in] doc
  ///   The document.
  /// \param[in] units
  ///   The unit system, which is used for attributing child XML nodes.
  /// \return An XML node for the document.
  static wxXmlNode* CreateNode(const SpanAnalyzerDoc& doc,
                               const units::UnitSystem& units);

  /// \brief Parses an XML node and populates a document.
  /// \param[in] root
  ///   The XML root node for the document.
  /// \param[in] filepath
  ///   The filepath that the xml node was loaded from. This is for logging
  ///   purposes only and can be left blank.
  /// \param[in] units
  ///   The unit system. If no conversion is being done this will be ignored.
  /// \param[in] convert
  ///   A flag that determines if the unit style is converted to 'consistent'.
  /// \param[in] cablefiles
  ///   A list of cables files that is matched against a cable description. If
  ///   found, a pointer will be set to the matching cable.
  /// \param[in] weathercases
  ///   A list of weathercases that is matched against a weathercase
  ///   description. If found, a pointer will be set to the matching
  ///   weathercase.
  /// \param[out] doc
  ///   The document that is populated.
  /// \return The status of the xml node parse. If any errors are encountered
  ///   false is returned.
  /// All errors are logged to the active application log target. Critical
  /// errors cause the parsing to abort. Non-critical errors set the object
  /// property to an invalid state (if applicable).
  static bool ParseNode(const wxXmlNode* root,
                        const std::string& filepath,
                        const units::UnitSystem& units,
                        const bool& convert,
                        const std::list<CableFile*>* cablefiles,
                        const std::list<WeatherLoadCase*>* weathercases,
                        SpanAnalyzerDoc& doc);

 private:
  /// \brief Parses a version 1 XML node and populates a document.
  /// \param[in] root
  ///   The XML root node for the document.
  /// \param[in] filepath
  ///   The filepath that the xml node was loaded from. This is for logging
  ///   purposes only and can be left blank.
  /// \param[in] units
  ///   The unit system. If no conversion is being done this will be ignored.
  /// \param[in] convert
  ///   A flag that determines if the unit style is converted to 'consistent'.
  /// \param[in] cablefiles
  ///   A list of cable files that is matched against a cable description. If
  ///   found, a pointer will be set to the matching cable.
  /// \param[in] weathercases
  ///   A list of weathercases that is matched against a weathercase
  ///   description. If found, a pointer will be set to the matching
  ///   weathercase.
  /// \param[out] doc
  ///   The document that is populated.
  /// \return The status of the xml node parse. If any errors are encountered
  ///   false is returned.
  /// All errors are logged to the active application log target. Critical
  /// errors cause the parsing to abort. Non-critical errors set the object
  /// property to an invalid state (if applicable).
  static bool ParseNodeV1(const wxXmlNode* root,
                          const std::string& filepath,
                          const units::UnitSystem& units,
                          const bool& convert,
                          const std::list<CableFile*>* cablefiles,
                          const std::list<WeatherLoadCase*>* weathercases,
                          SpanAnalyzerDoc& doc);

  /// \brief Parses a version 2 XML node and populates a document.
  /// \param[in] root
  ///   The XML root node for the document.
  /// \param[in] filepath
  ///   The filepath that the xml node was loaded from. This is for logging
  ///   purposes only and can be left blank.
  /// \param[in] units
  ///   The unit system. If no conversion is being done this will be ignored.
  /// \param[in] convert
  ///   A flag that determines if the unit style is converted to 'consistent'.
  /// \param[in] cablefiles
  ///   A list of cable files that is matched against a cable description. If
  ///   found, a pointer will be set to the matching cable.
  /// \param[in] weathercases
  ///   A list of weathercases that is matched against a weathercase
  ///   description. If found, a pointer will be set to the matching
  ///   weathercase.
  /// \param[out] doc
  ///   The document that is populated.
  /// \return The status of the xml node parse. If any errors are encountered
  ///   false is returned.
  /// All errors are logged to the active application log target. Critical
  /// errors cause the parsing to abort. Non-critical errors set the object
  /// property to an invalid state (if applicable).
  static bool ParseNodeV2(const wxXmlNode* root,
                          const std::string& filepath,
                          const units::UnitSystem& units,
                          const bool& convert,
                          const std::list<CableFile*>* cablefiles,
                          const std::list<WeatherLoadCase*>* weathercases,
                          SpanAnalyzerDoc& doc);
};

#endif  // SPANANALYZER_SPAN_ANALYZER_DOC_XML_HANDLER_H_
