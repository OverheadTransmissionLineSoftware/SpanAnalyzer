// This is free and unencumbered software released into the public domain.
// For more information, please refer to <http://unlicense.org/>

#include "xml_handler.h"

XmlHandler::XmlHandler() {
}

XmlHandler::~XmlHandler() {
}

wxXmlNode* XmlHandler::CreateElementNodeWithContent(
    const std::string& title,
    const std::string& content,
    const wxXmlAttribute* attribute) {
  // creates element node
  wxXmlNode* node = new wxXmlNode(wxXML_ELEMENT_NODE, title);
  
  // creates child text node
  if (content != *wxEmptyString) {
    wxXmlNode* node_child = new wxXmlNode(wxXML_TEXT_NODE,
                                          wxEmptyString, content);
    node->AddChild(node_child);
  }

  // adds attribute
  if (attribute != nullptr) {
    wxXmlAttribute* attr = new wxXmlAttribute(*attribute);
    node->AddAttribute(attr);
  }

  return node;
}

wxString XmlHandler::ParseElementNodeWithContent(const wxXmlNode* node) {
  wxString content;
  wxXmlNode* node_child = node->GetChildren();

  if (node_child != nullptr) {
    content = node_child->GetContent();
  }

  return content;
}
