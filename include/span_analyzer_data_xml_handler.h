// This is free and unencumbered software released into the public domain.
// For more information, please refer to <http://unlicense.org/>

#ifndef OTLS_SPANANALYZER_SPANANALYZERDATAXMLHANDLER_H_
#define OTLS_SPANANALYZER_SPANANALYZERDATAXMLHANDLER_H_

#include "models/base/units.h"
#include "wx/xml/xml.h"

#include "span_analyzer_data.h"
#include "xml_handler.h"

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
/// systems.
class SpanAnalyzerDataXmlHandler : public XmlHandler {
 public:
  /// \brief Creates an XML node for a span analyzer data object.
  /// \param[in] data
  ///   The data.
  /// \param[in] units
  ///   The unit system, which is used for attributing child XML nodes.
  /// \return An XML node for the document.
  static wxXmlNode* CreateNode(const SpanAnalyzerData& data,
                               const units::UnitSystem& units);

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
  /// \return The file line number of a critical error. Returns 0 if no critical
  ///   errors were encountered.
  /// Critical errors cause the parsing to abort, but are not logged.
  /// Non-critical errors are directed to the active application log target. If
  /// the property is recognized, it is set to an invalid state.
  static int ParseNode(const wxXmlNode* root,
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
  /// \return The file line number of a critical error. Returns 0 if no critical
  ///   errors were encountered.
  /// Critical errors cause the parsing to abort, but are not logged.
  /// Non-critical errors are directed to the active application log target. If
  /// the property is recognized, it is set to an invalid state.
  static int ParseNodeV1(const wxXmlNode* root,
                         const std::string& filepath,
                         const units::UnitSystem& units,
                         SpanAnalyzerData& data);
};

#endif  // OTLS_SPANANALYZER_SPANANALYZERDATAXMLHANDLER_H_
