// This is free and unencumbered software released into the public domain.
// For more information, please refer to <http://unlicense.org/>

#ifndef OTLS_SPANANALYZER_ANALYSISFILTERXMLHANDLER_H_
#define OTLS_SPANANALYZER_ANALYSISFILTERXMLHANDLER_H_

#include "wx/xml/xml.h"

#include "span_analyzer_data.h"
#include "xml_handler.h"

/// \par OVERVIEW
///
/// This class parses and generates an analysis filter XML node. The data is
/// transferred between the XML node and the data object.
///
/// \par VERSION
///
/// This class can parse all versions of the XML node. However, new nodes will
/// only be generated with the most recent version.
class AnalysisFilterXmlHandler : public XmlHandler {
 public:
  /// \brief Default constructor.
  AnalysisFilterXmlHandler();

  /// \brief Destructor.
  ~AnalysisFilterXmlHandler();

  /// \brief Creates an XML node for an analysis filter object.
  /// \param[in] filter
  ///   The filter.
  /// \param[in] name
  ///   The name of the XML node. This will be an attribute for the created
  ///   node. If empty, no attribute will be created.
  /// \return An XML node for the analysis filter.
  static wxXmlNode* CreateNode(const AnalysisFilter& filter,
                               const std::string& name);

  /// \brief Parses an XML node and populates an analysis filter object.
  /// \param[in] root
  ///   The XML root node for the analysis filter.
  /// \param[in] filepath
  ///   The filepath that the xml node was loaded from. This is for logging
  ///   purposes only and can be left blank.
  /// \param[in] weathercases
  ///   A list of weathercases that is matched against a weathercase
  ///   description. If found, a pointer will be set to a matching
  ///   weathercase.
  /// \param[out] filter
  ///   The filter.
  /// \return The status of the xml node parse. If any errors are encountered
  ///   false is returned.
  /// All errors are logged to the active application log target. Critical
  /// errors cause the parsing to abort. Non-critical errors set the object
  /// property to an invalid state (if applicable).
  static bool ParseNode(const wxXmlNode* root,
                        const std::string& filepath,
                        const std::list<WeatherLoadCase*>* weathercases,
                        AnalysisFilter& filter);

 private:
  /// \brief Parses a version 1 XML node and populates an analysis filter
  ///   object.
  /// \param[in] root
  ///   The XML root node for the analysis filter.
  /// \param[in] filepath
  ///   The filepath that the xml node was loaded from. This is for logging
  ///   purposes only and can be left blank.
  /// \param[in] weathercases
  ///   A list of weathercases that is matched against a weathercase
  ///   description. If found, a pointer will be set to a matching
  ///   weathercase.
  /// \param[out] filter
  ///   The filter.
  /// \return The status of the xml node parse. If any errors are encountered
  ///   false is returned.
  /// All errors are logged to the active application log target. Critical
  /// errors cause the parsing to abort. Non-critical errors set the object
  /// property to an invalid state (if applicable).
  static bool ParseNodeV1(const wxXmlNode* root,
                          const std::string& filepath,
                          const std::list<WeatherLoadCase*>* weathercases,
                          AnalysisFilter& filter);
};

#endif  // OTLS_SPANANALYZER_ANALYSISFILTERXMLHANDLER_H_