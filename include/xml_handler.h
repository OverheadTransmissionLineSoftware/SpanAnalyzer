// This is free and unencumbered software released into the public domain.
// For more information, please refer to <http://unlicense.org/>

#ifndef OTLS_SPANANALYZER_XMLHANDLER_H_
#define OTLS_SPANANALYZER_XMLHANDLER_H_

#include <string>

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

#endif  // OTLS_SPANANALYZER_XMLHANDLER_H_