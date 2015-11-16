// This is free and unencumbered software released into the public domain.
// For more information, please refer to <http://unlicense.org/>

#ifndef OTLS_SPANANALYZER_CABLECONSTRAINTXMLHANDLER_H_
#define OTLS_SPANANALYZER_CABLECONSTRAINTXMLHANDLER_H_

#include <string>
#include <vector>

#include "models/base/units.h"
#include "models/transmissionline/cable_constraint.h"
#include "xml_handler.h"

/// \par OVERVIEW
///
/// This class parses and generates a cable constraint XML node. The data is
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
class CableConstraintXmlHandler : public XmlHandler {
 public:
  /// \brief Constructor.
  CableConstraintXmlHandler();

  /// \brief Destructor.
  ~CableConstraintXmlHandler();

  /// \brief Creates an XML node for a cable constraint.
  /// \param[in] constraint
  ///   The cable constraint.
  /// \param[in] name
  ///   The name of the XML node. This will be an attribute for the created
  ///   node. If empty, no attribute will be created.
  /// \param[in] units
  ///   The unit system, which is used for attributing child XML nodes.
  /// \return An XML node for the cable constraint.
  static wxXmlNode* CreateNode(const CableConstraint& constraint,
                               const std::string name,
                               const units::UnitSystem& units);

  /// \brief Parses an XML node and populates a cable constraint.
  /// \param[in] root
  ///   The XML root node for the cable constraint.
  /// \param[in] weathercases
  ///   A vector of weathercases that is matched against a weathercase
  ///   description. If found, a pointer will be set to a matching
  ///   weathercase.
  /// \param[out] constraint
  ///   The cable constraint that is populated.
  /// \return The file line number of the node if the content could not be
  ///   converted to the expected data type. Returns 0 if no errors were
  ///   encountered.
  static int ParseNode(const wxXmlNode* root,
                       const std::vector<WeatherLoadCase>* weathercases,
                       CableConstraint& constraint);

 private:
  /// \brief Parses a version 1 XML node and populates a cable constraint.
  /// \param[in] root
  ///   The XML root node for the cable constraint.
  /// \param[in] weathercases
  ///   A vector of weathercases that is matched against a weathercase
  ///   description. If found, a pointer will be set to a matching
  ///   weathercase.
  /// \param[out] constraint
  ///   The cable constraint that is populated.
  /// \return The file line number of the node if the content could not be
  ///   converted to the expected data type. Returns 0 if no errors were
  ///   encountered.
  static int ParseNodeV1(const wxXmlNode* root,
                         const std::vector<WeatherLoadCase>* weathercases,
                         CableConstraint& constraint);
};

#endif  // OTLS_SPANANALYZER_CABLECONSTRAINTXMLHANDLER_H_
