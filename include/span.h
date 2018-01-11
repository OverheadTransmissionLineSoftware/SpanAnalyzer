// This is free and unencumbered software released into the public domain.
// For more information, please refer to <http://unlicense.org/>

#ifndef OTLS_SPANANALYZER_SPAN_H_
#define OTLS_SPANANALYZER_SPAN_H_

#include <string>

#include "models/base/error_message.h"
#include "models/transmissionline/line_cable.h"

/// \par OVERVIEW
///
/// This struct contains information for a cable in a single span. This class
/// acts as an interface so that the SpanAnalyzer program can manipulate and use
/// the LineCable class.
struct Span {
 public:
  /// \par OVERVIEW
  ///
  ///  This enum contains types of spans.
  enum class Type {
    kDeadendSpan,
    kRulingSpan
  };

  /// \brief Constructor.
  Span();

  /// \brief Destructor.
  ~Span();

  /// \brief Validates member variables.
  /// \param[in] is_included_warnings
  ///   A flag that tightens the acceptable value range.
  /// \param[in,out] messages
  ///   A list of detailed error messages. If this is provided, any validation
  ///   errors will be appended to the list.
  /// \return A boolean value indicating status of member variables.
  bool Validate(const bool& is_included_warnings = true,
                std::list<ErrorMessage>* messages = nullptr) const;

  /// \var linecable
  ///   The line cable, which holds most of the cable data for the span.
  LineCable linecable;

  /// \var name
  ///   The name.
  std::string name;

  /// \var spacing_attachments
  ///   The attachment spacing for the span. The line cable spacing contains the
  ///   attachment spacing for the entire line section. When the span type is
  ///   set to Dead-End, this spacing should match the line cable spacing.
  Vector3d spacing_attachments;

  /// \var type
  ///   The span type.
  Type type;
};

#endif  // OTLS_SPANANALYZER_SPAN_H_
