// This is free and unencumbered software released into the public domain.
// For more information, please refer to <http://unlicense.org/>

#ifndef OTLS_SPANANALYZER_XMLHANDLER_H_
#define OTLS_SPANANALYZER_XMLHANDLER_H_

#include <string>

#include "wx/wx.h"
#include "wx/xml/xml.h"

/// \par OVERVIEW
///
/// This is the base class for XmlHandlers. This class helps create and parse
/// 'element' type XML nodes which have a 'text' type XML node as a child. An
/// attribute can optionally be assigned. The XML node be displayed as the
/// following form:
///
///   <title attribute="optional">content</title>
///
/// where the 'element' node is the title, and the 'text' node is the content,
/// and the attribute is in parenthesis.
class XmlHandler {
 public:
  /// \brief Constructor.
  XmlHandler();

  /// \brief Destructor.
  ~XmlHandler();

  /// \brief Creates an 'element' type node with a child 'text' type node.
  /// \param[in] title
  ///   The title of the element node.
  /// \param[in] content
  ///   The content to be held in the child 'text' type node.
  /// \param[in] attribute
  ///   The attribute that is assigned to the node. The attribute that is
  ///   attached to the node is allocated within the function. This parameter is
  ///   only a pointer to make it optional.
  /// \return An 'element' type XML node.
  static wxXmlNode* CreateElementNodeWithContent(
      const std::string& title,
      const std::string& content,
      const wxXmlAttribute* attribute = nullptr);

  /// \brief Parses the content stored in a child 'text' type node.
  /// \param[in] node
  ///   The 'element' node that contains the text node.
  /// \return A string containing the 'text' type node content.
  /// If no text node is present, an empty string is returned. This helps
  /// avoid runtime parsing errors.
  static wxString ParseElementNodeWithContent(const wxXmlNode* node);
};

#endif  // OTLS_SPANANALYZER_XMLHANDLER_H_
