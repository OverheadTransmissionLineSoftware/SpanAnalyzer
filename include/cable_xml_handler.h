// This is free and unencumbered software released into the public domain.
// For more information, please refer to <http://unlicense.org/>

#ifndef TLS_SPANANALYZER_CABLEXMLHANDLER_H_
#define TLS_SPANANALYZER_CABLEXMLHANDLER_H_

#include <string>

#include "transmissionline/cable.h"
#include "wx/wx.h"
#include "wx/xml/xml.h"

class CableComponentXmlHandler {
 public:
  /// \brief Default constructor.
  CableComponentXmlHandler();

  /// \brief Destructor.
  ~CableComponentXmlHandler();

  /// \brief Creates an XML node for a cable component struct.
  /// \param[in] component
  ///   The cable component.
  /// \return An XML node for the cable component struct.
  static wxXmlNode* CreateNode(const CableComponent& component,
                               const std::string& name);

  /// \brief Parses an XML node and populates a cable component struct.
  /// \param[in] node
  ///   The XML node.
  /// \param[out] component
  ///   The cable component struct that is populated.
  /// \return The success status of parsing the node.
  static bool ParseNode(const wxXmlNode* node, CableComponent& component);

 private:
  /// \brief Parses a version 1 XML node and populates a cable struct.
  /// \param[in] node
  ///   The XML node.
  /// \param[out] component
  ///   The cable component struct that is populated.
  /// \return The success status of parsing the node.
  static bool ParseNodeV1(const wxXmlNode* node, CableComponent& component);
};

class CableXmlHandler {
 public:
  /// \brief Default constructor.
  CableXmlHandler();

  /// \brief Destructor.
  ~CableXmlHandler();

  /// \brief Creates an XML node for a cable struct.
  /// \param[in] cable
  ///   The cable.
  /// \return An XML node for the cable struct.
  static wxXmlNode* CreateNode(const Cable& cable);

  /// \brief Parses an XML node and populates a cable struct.
  /// \param[in] node
  ///   The XML node.
  /// \param[out] cable
  ///   The cable struct that is populated.
  /// \return The success status of parsing the node.
  static bool ParseNode(const wxXmlNode* node, Cable& cable);

 private:
  /// \brief Parses a version 1 XML node and populates a cable struct.
  /// \param[in] node
  ///   The XML node.
  /// \param[out] cable
  ///   The cable struct that is populated.
  /// \return The success status of parsing the node.
  static bool ParseNodeV1(const wxXmlNode* node, Cable& cable);
};

#endif  // TLS_SPANANALYZER_CABLEXMLHANDLER_H_