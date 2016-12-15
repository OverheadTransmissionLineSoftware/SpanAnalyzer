// This is free and unencumbered software released into the public domain.
// For more information, please refer to <http://unlicense.org/>

#ifndef OTLS_SPANANALYZER_VECTORXMLHANDLER_H_
#define OTLS_SPANANALYZER_VECTORXMLHANDLER_H_

#include <string>

#include "models/base/vector.h"

#include "xml_handler.h"

/// \par OVERVIEW
///
/// This class parses and generates a Vector2d XML node. The data is
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
class Vector2dXmlHandler : public XmlHandler {
 public:
  /// \brief Constructor.
  Vector2dXmlHandler();

  /// \brief Destructor.
  ~Vector2dXmlHandler();

  /// \brief Creates an XML node for a Vector2d.
  /// \param[in] vector
  ///   The vector.
  /// \param[in] name
  ///   The name of the XML node. This will be an attribute for the created
  ///   node. If empty, no attribute will be created.
  /// \param[in] attribute_components
  ///   The attribute that will be created for all vector components.
  /// \param[in] precision
  ///   The rounding precision for the vector components.
  /// \return An XML node for the vector.
  static wxXmlNode* CreateNode(const Vector2d& vector,
                               const std::string& name,
                               const wxXmlAttribute& attribute_components,
                               const int& precision);

  /// \brief Parses an XML node and populates a vector.
  /// \param[in] root
  ///   The XML root node for the vector.
  /// \param[in] filepath
  ///   The filepath that the xml node was loaded from. This is for logging
  ///   purposes only and can be left blank.
  /// \param[out] vector
  ///   The vector that is populated.
  /// \return The file line number of a critical error. Returns 0 if no critical
  ///   errors were encountered.
  /// Critical errors cause the parsing to abort, but are not logged.
  /// Non-critical errors are directed to the active application log target. If
  /// the property is recognized, it is set to an invalid state.
  static int ParseNode(const wxXmlNode* root,
                       const std::string& filepath,
                       Vector2d& vector);

 private:
  /// \brief Parses a version 1 XML node and populates a vector.
  /// \param[in] root
  ///   The XML root node for the vector.
  /// \param[in] filepath
  ///   The filepath that the xml node was loaded from. This is for logging
  ///   purposes only and can be left blank.
  /// \param[out] vector
  ///   The vector that is populated.
  /// \return The file line number of a critical error. Returns 0 if no critical
  ///   errors were encountered.
  /// Critical errors cause the parsing to abort, but are not logged.
  /// Non-critical errors are directed to the active application log target. If
  /// the property is recognized, it is set to an invalid state.
  static int ParseNodeV1(const wxXmlNode* root,
                         const std::string& filepath,
                         Vector2d& vector);
};

/// \par OVERVIEW
///
/// This class parses and generates a Vector3d XML node. The data is
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
class Vector3dXmlHandler : public XmlHandler {
 public:
  /// \brief Constructor.
  Vector3dXmlHandler();

  /// \brief Destructor.
  ~Vector3dXmlHandler();

  /// \brief Creates an XML node for a Vector3d.
  /// \param[in] vector
  ///   The vector.
  /// \param[in] name
  ///   The name of the XML node. This will be an attribute for the created
  ///   node. If empty, no attribute will be created.
  /// \param[in] attribute_components
  ///   The attribute that will be created for all vector components.
  /// \param[in] precision
  ///   The rounding precision for the vector components.
  /// \return An XML node for the vector.
  static wxXmlNode* CreateNode(const Vector3d& vector,
                               const std::string& name,
                               const wxXmlAttribute& attribute_components,
                               const int& precision);

  /// \brief Parses an XML node and populates a vector.
  /// \param[in] root
  ///   The XML root node for the vector.
  /// \param[in] filepath
  ///   The filepath that the xml node was loaded from. This is for logging
  ///   purposes only and can be left blank.
  /// \param[out] vector
  ///   The vector that is populated.
  /// \return The file line number of a critical error. Returns 0 if no critical
  ///   errors were encountered.
  /// Critical errors cause the parsing to abort, but are not logged.
  /// Non-critical errors are directed to the active application log target. If
  /// the property is recognized, it is set to an invalid state.
  static int ParseNode(const wxXmlNode* root,
                       const std::string& filepath,
                       Vector3d& vector);

 private:
  /// \brief Parses a version 1 XML node and populates a vector.
  /// \param[in] root
  ///   The XML root node for the vector.
  /// \param[in] filepath
  ///   The filepath that the xml node was loaded from. This is for logging
  ///   purposes only and can be left blank.
  /// \param[out] vector
  ///   The vector that is populated.
  /// \return The file line number of a critical error. Returns 0 if no critical
  ///   errors were encountered.
  /// Critical errors cause the parsing to abort, but are not logged.
  /// Non-critical errors are directed to the active application log target. If
  /// the property is recognized, it is set to an invalid state.
  static int ParseNodeV1(const wxXmlNode* root,
                         const std::string& filepath,
                         Vector3d& vector);
};

#endif  // OTLS_SPANANALYZER_VECTORXMLHANDLER_H_
