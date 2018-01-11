// This is free and unencumbered software released into the public domain.
// For more information, please refer to <http://unlicense.org/>

#include "span.h"

#include <cmath>

Span::Span() {
  name = "";
  type = Span::Type::kNull;
}

Span::~Span() {
}

bool Span::Validate(const bool& is_included_warnings,
                    std::list<ErrorMessage>* messages) const {
  // initializes
  bool is_valid = true;
  ErrorMessage message;
  message.title = "SPAN";

  // validates linecable
  if (linecable.Validate(is_included_warnings, messages) == false) {
    is_valid = false;
  }

  // validates name
  if (name == "") {
    is_valid = false;
    if (messages != nullptr) {
      message.description = "Invalid name";
      messages->push_back(message);
    }
  }

  // validates spacing-attachments
  if (spacing_attachments.x() < 0) {
    is_valid = false;
    if (messages != nullptr) {
      message.description = "Invalid attachment spacing - x";
      messages->push_back(message);
    }
  }

  if (spacing_attachments.y() != 0) {
    is_valid = false;
    if (messages != nullptr) {
      message.description = "Invalid attachment spacing - y";
      messages->push_back(message);
    }
  }

  if (1000 < std::abs(spacing_attachments.z())) {
    is_valid = false;
    if (messages != nullptr) {
      message.description = "Invalid attachment spacing - z";
      messages->push_back(message);
    }
  }

  // validates type
  if (type == Span::Type::kNull) {
    is_valid = false;
    if (messages != nullptr) {
      message.description = "Invalid span type";
      messages->push_back(message);
    }
  }

  // returns validation status
  return is_valid;
}
