// This is free and unencumbered software released into the public domain.
// For more information, please refer to <http://unlicense.org/>

#include "xml_handler.h"

XmlHandler::XmlHandler() {
}

XmlHandler::~XmlHandler() {
}

wxXmlNode* XmlHandler::CreateElementNodeWithContent(
    const std::string& name,
    const std::string& content,
    wxXmlAttribute* attribute) {
  wxXmlNode* node_child = new wxXmlNode(wxXML_TEXT_NODE, wxEmptyString,
                                        content);
  wxXmlNode* node = new wxXmlNode(wxXML_ELEMENT_NODE, name);
  node->AddChild(node_child);

  if (attribute != nullptr) {
    node->AddAttribute(attribute);
  }

  return node;
}
