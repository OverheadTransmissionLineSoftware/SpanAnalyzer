// This is free and unencumbered software released into the public domain.
// For more information, please refer to <http://unlicense.org/>

#ifndef OTLS_SPANANALYZER_VECTORXMLHANDLER_H_
#define OTLS_SPANANALYZER_VECTORXMLHANDLER_H_

#include <string>

#include "models/base/vector.h"

#include "xml_handler.h"

/// \todo
///   Add a Vector2dXmlHandler class here.

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
  /// \param[out] vector
  ///   The vector that is populated.
  /// \return The file line number of the node if the content could not be
  ///   converted to the expected data type. Returns 0 if no errors were
  ///   encountered.
  static int ParseNode(const wxXmlNode* root,
                       Vector3d& vector);

 private:
  /// \brief Parses a version 1 XML node and populates a vector.
  /// \param[in] root
  ///   The XML root node for the vector.
  /// \param[out] vector
  ///   The vector that is populated.
  /// \return The file line number of the node if the content could not be
  ///   converted to the expected data type. Returns 0 if no errors were
  ///   encountered.
  static int ParseNodeV1(const wxXmlNode* root,
                         Vector3d& constraint);
};

#endif  // OTLS_SPANANALYZER_VECTORXMLHANDLER_H_