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
  /// \param[out] data
  ///   The data.
  /// \param[out] units
  ///   The units for the data.
  /// \return The file line number of the node if the content could not be
  ///   converted to the expected data type. Returns 0 if no errors were
  ///   encountered.
  static int ParseNode(const wxXmlNode* root,
                       SpanAnalyzerData& data,
                       units::UnitSystem& units);

 private:
  /// \brief Parses a version 1 XML node and populates a data object.
  /// \param[in] root
  ///   The XML root node for the document.
  /// \param[out] data
  ///   The data.
  /// \param[out] units
  ///   The units for the data.
  /// \return The file line number of the node if the content could not be
  ///   converted to the expected data type. Returns 0 if no errors were
  ///   encountered.
  static int ParseNodeV1(const wxXmlNode* root,
                         SpanAnalyzerData& data,
                         units::UnitSystem& units);
};

#endif  // OTLS_SPANANALYZER_SPANANALYZERDATAXMLHANDLER_H_