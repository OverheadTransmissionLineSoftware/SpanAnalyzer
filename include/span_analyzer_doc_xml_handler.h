// This is free and unencumbered software released into the public domain.
// For more information, please refer to <http://unlicense.org/>

#ifndef OTLS_SPANANALYZER_SPANANALYZERDOCXMLHANDLER_H_
#define OTLS_SPANANALYZER_SPANANALYZERDOCXMLHANDLER_H_

#include <vector>

#include "models/base/units.h"
#include "xml_handler.h"

#include "span_analyzer_doc.h"

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
  static wxXmlNode* CreateNode(SpanAnalyzerDoc& doc,
                               const units::UnitSystem& units);

  /// \brief Parses an XML node and populates a document.
  /// \param[in] root
  ///   The XML root node for the document.
  /// \param[in] cables
  ///   A vector of cables that is matched against a cable description. If
  ///   found, a pointer will be set to the matching cable.
  /// \param[out] doc
  ///   The document that is populated.
  /// \param[out] units
  ///   The units for the document.
  /// \return The file line number of the node if the content could not be
  ///   converted to the expected data type. Returns 0 if no errors were
  ///   encountered.
  static int ParseNode(const wxXmlNode* root,
                       const std::vector<Cable>* cables,
                       SpanAnalyzerDoc& doc,
                       units::UnitSystem& units);

 private:
  /// \brief Parses a version 1 XML node and populates a document.
  /// \param[in] root
  ///   The XML root node for the document.
  /// \param[in] cables
  ///   A vector of cables that is matched against a cable description. If
  ///   found, a pointer will be set to the matching cable.
  /// \param[out] doc
  ///   The document that is populated.
  /// \param[out] units
  ///   The units for the document.
  /// \return The file line number of the node if the content could not be
  ///   converted to the expected data type. Returns 0 if no errors were
  ///   encountered.
  static int ParseNodeV1(const wxXmlNode* root,
                         const std::vector<Cable>* cables,
                         SpanAnalyzerDoc& doc,
                         units::UnitSystem& units);
};

#endif  // OTLS_SPANANALYZER_SPANANALYZERDOCXMLHANDLER_H_
