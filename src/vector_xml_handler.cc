// This is free and unencumbered software released into the public domain.
// For more information, please refer to <http://unlicense.org/>

#include "vector_xml_handler.h"

#include "models/base/helper.h"

Vector2dXmlHandler::Vector2dXmlHandler() {
}

Vector2dXmlHandler::~Vector2dXmlHandler() {
}

wxXmlNode* Vector2dXmlHandler::CreateNode(
    const Vector2d& vector,
    const std::string& name,
    const wxXmlAttribute& attribute_components,
    const int& precision) {
  // variables used to create XML node
  wxXmlNode* node_root = nullptr;
  wxXmlNode* node_element = nullptr;
  std::string title;
  std::string content;
  double value;

  // creates a node for the cable constraint root
  node_root = new wxXmlNode(wxXML_ELEMENT_NODE, "vector_2d");
  node_root->AddAttribute("version", "1");
  node_root->AddAttribute("name", name);

  // adds child nodes

  // creates x node and adds to parent node
  title = "x";
  value = vector.x();
  content = helper::DoubleToFormattedString(value, precision);
  node_element = CreateElementNodeWithContent(title, content,
                                              &attribute_components);
  node_root->AddChild(node_element);

  // creates y node and adds to parent node
  title = "y";
  value = vector.y();
  content = helper::DoubleToFormattedString(value, precision);
  node_element = CreateElementNodeWithContent(title, content,
                                              &attribute_components);
  node_root->AddChild(node_element);

  // returns node
  return node_root;
}

int Vector2dXmlHandler::ParseNode(const wxXmlNode* root,
                                  const std::string& filepath,
                                  Vector2d& vector) {
  // checks for valid root node
  if (root->GetName() != "vector_2d") {
    return root->GetLineNumber();
  }

  // gets version attribute
  wxString version;
  if (root->GetAttribute("version", &version) == false) {
    return root->GetLineNumber();
  }

  // sends to proper parsing function
  if (version == "1") {
    return ParseNodeV1(root, filepath, vector);
  } else {
    return root->GetLineNumber();
  }
}

int Vector2dXmlHandler::ParseNodeV1(const wxXmlNode* root,
                                    const std::string& filepath,
                                    Vector2d& vector) {
  wxString message;

  // evaluates each child node
  wxXmlNode* node = root->GetChildren();
  while (node != nullptr) {
    const wxString title = node->GetName();
    const wxString content = ParseElementNodeWithContent(node);
    double value = -999999;

    if (title == "x") {
      if (content.ToDouble(&value) == true) {
        vector.set_x(value);
      } else {
        message = FileAndLineNumber(filepath, node)
                  + "Invalid x component.";
        wxLogError(message);
        vector.set_x(-999999);
      }
    } else if (title == "y") {
      if (content.ToDouble(&value) == true) {
        vector.set_y(value);
      } else {
        message = FileAndLineNumber(filepath, node)
                  + "Invalid y component.";
        wxLogError(message);
        vector.set_y(-999999);
      }
    } else {
      message = FileAndLineNumber(filepath, node)
                + "XML node isn't recognized.";
      wxLogError(message);
    }

    node = node->GetNext();
  }

  // if it gets to this point, no critical errors were encountered
  return 0;
}

Vector3dXmlHandler::Vector3dXmlHandler() {
}

Vector3dXmlHandler::~Vector3dXmlHandler() {
}

wxXmlNode* Vector3dXmlHandler::CreateNode(
    const Vector3d& vector,
    const std::string& name,
    const wxXmlAttribute& attribute_components,
    const int& precision) {
  // variables used to create XML node
  wxXmlNode* node_root = nullptr;
  wxXmlNode* node_element = nullptr;
  std::string title;
  std::string content;
  double value;

  // creates a node for the cable constraint root
  node_root = new wxXmlNode(wxXML_ELEMENT_NODE, "vector_3d");
  node_root->AddAttribute("version", "1");
  node_root->AddAttribute("name", name);

  // adds child nodes

  // creates x node and adds to parent node
  title = "x";
  value = vector.x();
  content = helper::DoubleToFormattedString(value, precision);
  node_element = CreateElementNodeWithContent(title, content,
                                              &attribute_components);
  node_root->AddChild(node_element);

  // creates y node and adds to parent node
  title = "y";
  value = vector.y();
  content = helper::DoubleToFormattedString(value, precision);
  node_element = CreateElementNodeWithContent(title, content,
                                              &attribute_components);
  node_root->AddChild(node_element);

  // creates z node and adds to parent node
  title = "z";
  value = vector.z();
  content = helper::DoubleToFormattedString(value, precision);
  node_element = CreateElementNodeWithContent(title, content,
                                              &attribute_components);
  node_root->AddChild(node_element);

  // returns node
  return node_root;
}

int Vector3dXmlHandler::ParseNode(const wxXmlNode* root,
                                  const std::string& filepath,
                                  Vector3d& vector) {
  // checks for valid root node
  if (root->GetName() != "vector_3d") {
    return root->GetLineNumber();
  }

  // gets version attribute
  wxString version;
  if (root->GetAttribute("version", &version) == false) {
    return root->GetLineNumber();
  }

  // sends to proper parsing function
  if (version == "1") {
    return ParseNodeV1(root, filepath, vector);
  } else {
    return root->GetLineNumber();
  }
}

int Vector3dXmlHandler::ParseNodeV1(const wxXmlNode* root,
                                    const std::string& filepath,
                                    Vector3d& vector) {
  wxString message;

  // evaluates each child node
  wxXmlNode* node = root->GetChildren();
  while (node != nullptr) {
    const wxString title = node->GetName();
    const wxString content = ParseElementNodeWithContent(node);
    double value = -999999;

    if (title == "x") {
      if (content.ToDouble(&value) == true) {
        vector.set_x(value);
      } else {
        message = FileAndLineNumber(filepath, node)
                  + "Invalid x component.";
        wxLogError(message);
        vector.set_x(-999999);
      }
    } else if (title == "y") {
      if (content.ToDouble(&value) == true) {
        vector.set_y(value);
      } else {
        message = FileAndLineNumber(filepath, node)
                  + "Invalid y component.";
        wxLogError(message);
        vector.set_y(-999999);
      }
    } else if (title == "z") {
      if (content.ToDouble(&value) == true) {
        vector.set_z(value);
      } else {
        message = FileAndLineNumber(filepath, node)
                  + "Invalid z component.";
        wxLogError(message);
        vector.set_z(-999999);
      }
    } else {
      message = FileAndLineNumber(filepath, node)
                + "XML node isn't recognized.";
      wxLogError(message);
    }

    node = node->GetNext();
  }

  // if it gets to this point, no critical errors were encountered
  return 0;
}
