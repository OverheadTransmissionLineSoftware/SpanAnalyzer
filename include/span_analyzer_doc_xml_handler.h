// This is free and unencumbered software released into the public domain.
// For more information, please refer to <http://unlicense.org/>

#ifndef OTLS_SPANANALYZER_SPANANALYZERDOCXMLHANDLER_H_
#define OTLS_SPANANALYZER_SPANANALYZERDOCXMLHANDLER_H_

#include <list>

#include "models/base/units.h"

#include "span_analyzer_data.h"
#include "span_analyzer_doc.h"
#include "xml_handler.h"

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
/// \par UNIT ATTRIBUTES
///
/// This class supports attributing the child XML nodes for various unit
/// systems.
class SpanAnalyzerDocXmlHandler : public XmlHandler {
 public:
  /// \brief Constructor.
  SpanAnalyzerDocXmlHandler();

  /// \brief Destructor.
  ~SpanAnalyzerDocXmlHandler();

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
  /// \param[in] cablefiles
  ///   A list of cables files that is matched against a cable description. If
  ///   found, a pointer will be set to the matching cable.
  /// \param[in] weathercases
  ///   A list of weathercases that is matched against a weathercase
  ///   description. If found, a pointer will be set to the matching
  ///   weathercase.
  /// \param[out] doc
  ///   The document that is populated.
  /// \return The file line number of a critical error. Returns 0 if no critical
  ///   errors were encountered.
  /// Critical errors cause the parsing to abort, but are not logged.
  /// Non-critical errors are directed to the active application log target. If
  /// the property is recognized, it is set to an invalid state.
  static int ParseNode(const wxXmlNode* root,
                       const std::string& filepath,
                       const std::list<CableFile>* cablefiles,
                       const std::list<WeatherLoadCase>* weathercases,
                       SpanAnalyzerDoc& doc);

 private:
  /// \brief Parses a version 1 XML node and populates a document.
  /// \param[in] root
  ///   The XML root node for the document.
  /// \param[in] filepath
  ///   The filepath that the xml node was loaded from. This is for logging
  ///   purposes only and can be left blank.
  /// \param[in] cablefiles
  ///   A list of cable files that is matched against a cable description. If
  ///   found, a pointer will be set to the matching cable.
  /// \param[in] weathercases
  ///   A list of weathercases that is matched against a weathercase
  ///   description. If found, a pointer will be set to the matching
  ///   weathercase.
  /// \param[out] doc
  ///   The document that is populated.
  /// \return The file line number of a critical error. Returns 0 if no critical
  ///   errors were encountered.
  /// Critical errors cause the parsing to abort, but are not logged.
  /// Non-critical errors are directed to the active application log target. If
  /// the property is recognized, it is set to an invalid state.
  static int ParseNodeV1(const wxXmlNode* root,
                         const std::string& filepath,
                         const std::list<CableFile>* cablefiles,
                         const std::list<WeatherLoadCase>* weathercases,
                         SpanAnalyzerDoc& doc);
};

#endif  // OTLS_SPANANALYZER_SPANANALYZERDOCXMLHANDLER_H_
