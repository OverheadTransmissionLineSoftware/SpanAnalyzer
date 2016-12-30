// This is free and unencumbered software released into the public domain.
// For more information, please refer to <http://unlicense.org/>

#ifndef OTLS_SPANANALYZER_SPANANALYZERDOCCOMMANDS_H_
#define OTLS_SPANANALYZER_SPANANALYZERDOCCOMMANDS_H_

#include <string>

#include "wx/cmdproc.h"

#include "span.h"

/// \par OVERVIEW
///
/// This class is a command that modifies spans in the SpanAnalyzerDoc. The
/// command must be initialized with one of the following declared strings.
class SpanCommand : public wxCommand {
 public:
  /// \var kNameDelete
  ///   The command string to use for deleting a span.
  static const std::string kNameDelete;

  /// \var kNameInsert
  ///   The command name to use for inserting a span.
  static const std::string kNameInsert;

  /// \var kNameModify
  ///   The command string to use for modifying a span.
  static const std::string kNameModify;

  /// \var kNameMoveDown
  ///   The command string to use for moving a span down.
  static const std::string kNameMoveDown;

  /// \var kNameMoveUp
  ///   The command string to use for moving a span up.
  static const std::string kNameMoveUp;

  /// \brief Constructor.
  /// \param[in] name
  ///   The command name, which should match one of the defined strings.
  SpanCommand(const std::string& name);

  /// \brief Destructor.
  virtual ~SpanCommand();

  /// \brief Executes the command.
  /// \return True if the action has taken place, false otherwise.
  virtual bool Do();

  /// \brief Undoes the command.
  /// \return True if the action has taken place, false otherwise.
  virtual bool Undo();

  /// \brief Gets the index.
  /// \return The index.
  int index() const;

  /// \brief Sets the index.
  /// \param[in] index
  ///   The index.
  void set_index(const int& index);

  /// \brief Sets the span.
  /// \param[in] span
  ///   The span.
  void set_span(const Span& span);

  /// \brief Gets the span.
  /// \return The span.
  Span span() const;

 private:
  
  /// \brief Does the delete span command.
  /// \param[in] iter
  ///   The iterator position to the spans list.
  /// \return The success status.
  /// This function will copy the document span to the command before it is
  /// deleted.
  bool DoDelete(const std::list<Span>::const_iterator& iter);

  /// \brief Does the insert span command.
  /// \param[in] iter
  ///   The iterator position to the spans list.
  /// \return The success status.
  /// This function will also set the span to an invalid state after it has
  /// been inserted into the document.
  bool DoInsert(const std::list<Span>::const_iterator& iter);

  /// \brief Does the modify span command.
  /// \param[in] iter
  ///   The iterator position to the spans list.
  /// \return The success status.
  /// This function will switch the document span with the command span.
  bool DoModify(const std::list<Span>::const_iterator& iter);

  /// \brief Does the move span down command.
  /// \param[in] iter
  ///   The iterator position to the spans list.
  /// \return The success status.
  bool DoMoveDown(const std::list<Span>::const_iterator& iter);

  /// \brief Does the move span up command.
  /// \param[in] iter
  ///   The iterator position to the spans list.
  /// \return The success status.
  bool DoMoveUp(const std::list<Span>::const_iterator& iter);

  /// \var index_
  ///   The index to the spans list in the document. The command will be applied
  ///   at this index.
  int index_;

  /// \var span_
  ///   The span that is stored.
  Span span_; 
};

#endif  // OTLS_SPANANALYZER_SPANANALYZERDOCCOMMANDS_H_
