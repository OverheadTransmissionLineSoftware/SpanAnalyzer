// This is free and unencumbered software released into the public domain.
// For more information, please refer to <http://unlicense.org/>

#include "span.h"

Span::Span() {
  name = "";
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

  // returns validation status
  return is_valid;
}
