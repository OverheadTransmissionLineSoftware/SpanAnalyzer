// This is free and unencumbered software released into the public domain.
// For more information, please refer to <http://unlicense.org/>

#ifndef OTLS_SPANANALYZER_WEATHERLOADCASEXMLHANDLER_H_
#define OTLS_SPANANALYZER_WEATHERLOADCASEXMLHANDLER_H_

#include <string>

#include "models/base/units.h"
#include "models/transmissionline/weather_load_case.h"
#include "wx/xml/xml.h"

#include "xml_handler.h"

/// \par OVERVIEW
///
/// This class parses and generates a weathercase XML node. The data is
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
class WeatherLoadCaseXmlHandler : public XmlHandler {
 public:
  /// \brief Constructor.
  WeatherLoadCaseXmlHandler();

  /// \brief Destructor.
  ~WeatherLoadCaseXmlHandler();

  /// \brief Creates an XML node for a weathercase.
  /// \param[in] weathercase
  ///   The weathercase.
  /// \param[in] name
  ///   The name of the XML node. This will be an attribute for the created
  ///   node. If empty, no attribute will be created.
  /// \param[in] units
  ///   The unit system, which is used for attributing child XML nodes.
  /// \return An XML node for the weathercase.
  static wxXmlNode* CreateNode(const WeatherLoadCase& weathercase,
                               const std::string& name,
                               const units::UnitSystem& units);

  /// \brief Parses an XML node and populates a weathercase.
  /// \param[in] root
  ///   The XML root node for the weathercase.
  /// \param[in] filepath
  ///   The filepath that the xml node was loaded from. This is for logging
  ///   purposes only and can be left blank.
  /// \param[out] weathercase
  ///   The weathercase that is populated.
  /// \return The file line number of a critical error. Returns 0 if no critical
  ///   errors were encountered.
  /// Critical errors cause the parsing to abort, but are not logged.
  /// Non-critical errors are directed to the active application log target. If
  /// the property is recognized, it is set to an invalid state.
  static int ParseNode(const wxXmlNode* root,
                       const std::string& filepath,
                       WeatherLoadCase& weathercase);

 private:
  /// \brief Parses a version 1 XML node and populates a weather load case.
  /// \param[in] root
  ///   The XML root node for the weathercase.
  /// \param[in] filepath
  ///   The filepath that the xml node was loaded from. This is for logging
  ///   purposes only and can be left blank.
  /// \param[out] weathercase
  ///   The weathercase that is populated.
  /// \return The file line number of a critical error. Returns 0 if no critical
  ///   errors were encountered.
  /// Critical errors cause the parsing to abort, but are not logged.
  /// Non-critical errors are directed to the active application log target. If
  /// the property is recognized, it is set to an invalid state.
  static int ParseNodeV1(const wxXmlNode* root,
                         const std::string& filepath,
                         WeatherLoadCase& weathercase);
};

#endif  // OTLS_SPANANALYZER_WEATHERLOADCASEXMLHANDLER_H_
