// This is free and unencumbered software released into the public domain.
// For more information, please refer to <http://unlicense.org/>

#ifndef OTLS_SPANANALYZER_CABLEXMLHANDLER_H_
#define OTLS_SPANANALYZER_CABLEXMLHANDLER_H_

#include <string>

#include "models/base/units.h"
#include "models/transmissionline/cable.h"
#include "wx/wx.h"
#include "wx/xml/xml.h"

/// \par OVERVIEW
///
/// This is the base class for XmlHandlers.
class XmlHandler {
 public:
  /// \brief Constructor.
  XmlHandler();

  /// \brief Destructor.
  ~XmlHandler();

  /// \brief Creates an 'element' type node with a child 'text' type node.
  /// \param[in] name
  ///   The name of the element node.
  /// \param[in] content_child
  ///   The content to be held in the child 'text' type node.
  /// \return An 'element' type XML node.
  static wxXmlNode* CreateElementNodeWithContent(
      const std::string& name,
      const std::string& child_content,
      wxXmlAttribute* attribute = nullptr);
};

/// \par OVERVIEW
///
/// This class parses and generates a cable component XML node. The data is
/// transferred between the XML node and the data object.
///
/// \par VERSION
///
/// This class can parse all versions of the cable component XML node. However,
/// new cable component nodes will only be generated with the most recent
/// version.
///
/// \par UNITS
///
/// This class supports both Imperial and Metric unit systems. This class also
/// supports similar-system unit conversions (ex: meters->millimeters). This is
/// done so the user can input using preferred units while maintaining
/// compatibility with the modeling libraries that require consistent units.
/// The expected consistent units are:
///   - Length = [meters or feet]
///   - Load = [N or lbs]
///   - Temperature = [deg C or deg F]
class CableComponentXmlHandler : public XmlHandler {
 public:
  /// \brief Constructor.
  CableComponentXmlHandler();

  /// \brief Destructor.
  ~CableComponentXmlHandler();

  /// \brief Creates an XML node for a cable component struct.
  /// \param[in] component
  ///   The cable component.
  /// \param[in] name
  ///   The name for the cable component XML node.
  /// \param[in] units
  ///   The units to convert to/from.
  /// \return An XML node for the cable component struct.
  static wxXmlNode* CreateNode(const CableComponent& component,
                               const std::string& name,
                               const units::UnitSystem& units);

  /// \brief Parses an XML node and populates a cable component struct.
  /// \param[in] root
  ///   The XML root node for the cable component.
  /// \param[in] units
  ///   The units to convert to/from.
  /// \param[out] component
  ///   The cable component struct that is populated.
  /// \return The file line number of the node if the content could not be
  ///   converted to the expected data type. Returns 0 if no errors were
  ///   encountered.
  static int ParseNode(const wxXmlNode* root, const units::UnitSystem& units,
                       CableComponent& component);

 private:
  /// \brief Parses a version 1 XML node and populates a cable struct.
  /// \param[in] root
  ///   The XML root node for the cable component.
  /// \param[in] units
  ///   The units to convert to/from.
  /// \param[out] component
  ///   The cable component struct that is populated.
  /// \return The file line number of the node if the content could not be
  ///   converted to the expected data type. Returns 0 if no errors were
  ///   encountered.
  static int ParseNodeV1(const wxXmlNode* root, const units::UnitSystem& units,
                         CableComponent& component);
};

/// \par OVERVIEW
///
/// This class parses and generates a cable XML node. The data is transferred
/// between the XML node and the data object.
///
/// \par VERSION
///
/// This class can parse all versions of the cable XML node. However, new cable
/// nodes will only be generated with the most recent version.
///
/// \par UNITS
///
/// This class supports both Imperial and Metric unit systems. This class also
/// supports similar-system unit conversions (ex: meters->millimeters). This is
/// done so the user can input using preferred units while maintaining
/// compatibility with the modeling libraries that require consistent units.
/// The expected consistent units are:
///   - Length = [meters or feet]
///   - Load = [N or lbs]
///   - Temperature = [deg C or deg F]
class CableXmlHandler : public XmlHandler {
 public:
  /// \brief Default constructor.
  CableXmlHandler();

  /// \brief Destructor.
  ~CableXmlHandler();

  /// \brief Creates an XML node for a cable struct.
  /// \param[in] cable
  ///   The cable.
  /// \return An XML node for the cable struct.
  /// \param[in] units
  ///   The units to convert to/from.
  /// \return An XML node for the cable struct.
  static wxXmlNode* CreateNode(const Cable& cable,
                               const units::UnitSystem& units);

  /// \brief Parses an XML node and populates a cable struct.
  /// \param[in] root
  ///   The XML root node for the cable component.
  /// \param[in] units
  ///   The units to convert to/from.
  /// \param[out] cable
  ///   The cable struct that is populated.
  /// \return The file line number of the node if the content could not be
  ///   converted to the expected data type. Returns 0 if no errors were
  ///   encountered.
  static int ParseNode(const wxXmlNode* root, const units::UnitSystem& units,
                       Cable& cable);

 private:
  /// \brief Parses a version 1 XML node and populates a cable struct.
  /// \param[in] root
  ///   The XML root node for the cable component.
  /// \param[in] units
  ///   The units to convert to/from.
  /// \param[out] cable
  ///   The cable struct that is populated.
  /// \return The file line number of the node if the content could not be
  ///   converted to the expected data type. Returns 0 if no errors were
  ///   encountered.
  static int ParseNodeV1(const wxXmlNode* root, const units::UnitSystem& units,
                         Cable& cable);
};

#endif  // OTLS_SPANANALYZER_CABLEXMLHANDLER_H_
