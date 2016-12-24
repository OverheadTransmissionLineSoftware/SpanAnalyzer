// This is free and unencumbered software released into the public domain.
// For more information, please refer to <http://unlicense.org/>

#ifndef OTLS_SPANANALYZER_CABLEXMLHANDLER_H_
#define OTLS_SPANANALYZER_CABLEXMLHANDLER_H_

#include <string>

#include "models/base/units.h"
#include "models/transmissionline/cable.h"
#include "wx/xml/xml.h"

#include "xml_handler.h"

/// \par OVERVIEW
///
/// This class parses and generates a cable component XML node. The data is
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
class CableComponentXmlHandler : public XmlHandler {
 public:
  /// \brief Constructor.
  CableComponentXmlHandler();

  /// \brief Destructor.
  ~CableComponentXmlHandler();

  /// \brief Creates an XML node for a cable component.
  /// \param[in] component
  ///   The cable component.
  /// \param[in] name
  ///   The name of the XML node. This will be an attribute for the created
  ///   node. If empty, no attribute will be created.
  /// \param[in] units
  ///   The unit system, which is used for attributing child XML nodes.
  /// \return An XML node for the cable component.
  static wxXmlNode* CreateNode(const CableComponent& component,
                               const std::string& name,
                               const units::UnitSystem& units);

  /// \brief Parses an XML node and populates a cable component.
  /// \param[in] root
  ///   The XML root node for the cable component.
  /// \param[in] filepath
  ///   The filepath that the xml node was loaded from. This is for logging
  ///   purposes only and can be left blank.
  /// \param[out] component
  ///   The cable component that is populated.
  /// \return The status of the xml node parse. If any errors are encountered
  ///   false is returned.
  /// All errors are logged to the active application log target. Critical
  /// errors cause the parsing to abort. Non-critical errors set the object
  /// property to an invalid state (if applicable).
  static bool ParseNode(const wxXmlNode* root,
                        const std::string& filepath,
                        CableComponent& component);

 private:
  /// \brief Parses a version 1 XML node and populates a cable component.
  /// \param[in] root
  ///   The XML root node for the cable component.
  /// \param[in] filepath
  ///   The filepath that the xml node was loaded from. This is for logging
  ///   purposes only and can be left blank.
  /// \param[out] component
  ///   The cable component that is populated.
  /// \return The status of the xml node parse. If any errors are encountered
  ///   false is returned.
  /// All errors are logged to the active application log target. Critical
  /// errors cause the parsing to abort. Non-critical errors set the object
  /// property to an invalid state (if applicable).
  static bool ParseNodeV1(const wxXmlNode* root,
                          const std::string& filepath,
                          CableComponent& component);
};

/// \par OVERVIEW
///
/// This class parses and generates a cable XML node. The data is transferred
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
class CableXmlHandler : public XmlHandler {
 public:
  /// \brief Default constructor.
  CableXmlHandler();

  /// \brief Destructor.
  ~CableXmlHandler();

  /// \brief Creates an XML node for a cable struct.
  /// \param[in] cable
  ///   The cable.
  /// \param[in] name
  ///   The name of the XML node. This will be an attribute for the created
  ///   node. If empty, no attribute will be created.
  /// \param[in] units
  ///   The unit system, which is used for attributing child XML nodes.
  /// \return An XML node for the cable.
  static wxXmlNode* CreateNode(const Cable& cable,
                               const std::string& name,
                               const units::UnitSystem& units);

  /// \brief Parses an XML node and populates a cable struct.
  /// \param[in] root
  ///   The XML root node for the cable.
  /// \param[in] filepath
  ///   The filepath that the xml node was loaded from. This is for logging
  ///   purposes only and can be left blank.
  /// \param[out] cable
  ///   The cable that is populated.
  /// \return The status of the xml node parse. If any errors are encountered
  ///   false is returned.
  /// All errors are logged to the active application log target. Critical
  /// errors cause the parsing to abort. Non-critical errors set the object
  /// property to an invalid state (if applicable).
  static bool ParseNode(const wxXmlNode* root,
                        const std::string& filepath,
                        Cable& cable);

 private:
  /// \brief Parses a version 1 XML node and populates a cable.
  /// \param[in] root
  ///   The XML root node for the cable.
  /// \param[in] filepath
  ///   The filepath that the xml node was loaded from. This is for logging
  ///   purposes only and can be left blank.
  /// \param[out] cable
  ///   The cable that is populated.
  /// \return The status of the xml node parse. If any errors are encountered
  ///   false is returned.
  /// All errors are logged to the active application log target. Critical
  /// errors cause the parsing to abort. Non-critical errors set the object
  /// property to an invalid state (if applicable).
  static bool ParseNodeV1(const wxXmlNode* root,
                          const std::string& filepath,
                          Cable& cable);
};

#endif  // OTLS_SPANANALYZER_CABLEXMLHANDLER_H_
