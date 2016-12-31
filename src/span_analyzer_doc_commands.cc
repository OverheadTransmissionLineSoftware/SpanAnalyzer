// This is free and unencumbered software released into the public domain.
// For more information, please refer to <http://unlicense.org/>

#include "span_analyzer_doc_commands.h"

#include "wx/wx.h"

#include "span_analyzer_app.h"
#include "span_analyzer_doc.h"

// These are the string definitions for the various command types.
const std::string SpanCommand::kNameDelete = "Delete Span";
const std::string SpanCommand::kNameInsert = "Insert Span";
const std::string SpanCommand::kNameModify = "Modify Span";
const std::string SpanCommand::kNameMoveDown = "Move Span Down";
const std::string SpanCommand::kNameMoveUp = "Move Span Up";

namespace {

SpanAnalyzerDoc* GetDocument() {
  return (SpanAnalyzerDoc*)wxGetApp().manager_doc()->GetCurrentDocument();
}

}

SpanCommand::SpanCommand(const std::string& name)
    : wxCommand(true, name) {
  index_ = -1;
}

SpanCommand::~SpanCommand() {
}

bool SpanCommand::Do() {
  const std::string name = GetName();

  // gets spans list from document
  SpanAnalyzerDoc* doc = GetDocument();

  // checks index to see if a valid iterator is possible
  const int kSize = doc->spans().size();
  if ((index_ < 0) || (kSize < index_)) {
    wxLogError("Invalid span index. Aborting command.");
    return false;
  }

  // gets iterator
  std::list<Span>::const_iterator iter;
  iter = std::next(doc->spans().cbegin(), index_);

  // selects based on command name
  if (name == kNameDelete) {
    // checks that index isn't end of list
    if (index_ == kSize) {
      wxLogError("Invalid span index. Aborting delete command.");
      return false;
    }

    DoDelete(iter);
  } else if (name == kNameInsert) {
    DoInsert(iter);
  } else if (name == kNameModify) {
    // checks that index isn't end of list
    if (index_ == kSize) {
      wxLogError("Invalid span index. Aborting modify command.");
      return false;
    }

    DoModify(iter);
  } else if (name == kNameMoveDown) {
    // checks that index isn't the last span
    if (index_ == kSize - 1) {
      wxLogError("Invalid span index. Aborting move down command.");
      return false;
    }

    DoMoveDown(iter);
  } else if (name == kNameMoveUp) {
    // checks that index isn't the first span
    if (index_ == 0) {
      wxLogError("Invalid span index. Aborting move up command.");
      return false;
    }

    DoMoveUp(iter);
  } else {
    wxLogError("Invalid command. Aborting.");
    return false;
  }

  // posts a view update
  UpdateHint hint(HintType::kSpansEdit);
  doc->UpdateAllViews(nullptr, &hint);

  return true;
}

bool SpanCommand::Undo() {
  const std::string name = GetName();

  // gets spans list from document
  SpanAnalyzerDoc* doc = GetDocument();

  // checks index to see if a valid iterator is possible
  const int kSize = doc->spans().size();
  if ((index_ < 0) || (kSize < index_)) {
    wxLogError("Invalid span index. Aborting command.");
    return false;
  }

  // gets iterator
  std::list<Span>::const_iterator iter;
  iter = std::next(doc->spans().cbegin(), index_);

  // selects based on command name
  if (name == kNameDelete) {
    DoInsert(iter);
  } else if (name == kNameInsert) {
    DoDelete(iter);
  } else if (name == kNameModify) {
    DoModify(iter);
  } else if (name == kNameMoveDown) {
    DoMoveUp(iter);
  } else if (name == kNameMoveUp) {
    DoMoveDown(iter);
  } else {
    wxLogError("Invalid command. Aborting.");
    return false;
  }

  // posts a view update
  UpdateHint hint(HintType::kSpansEdit);
  doc->UpdateAllViews(nullptr, &hint);

  return true;
}

int SpanCommand::index() const {
  return index_;
}

void SpanCommand::set_index(const int& index) {
  index_ = index;
}

void SpanCommand::set_span(const Span& span) {
  span_ = span;
}

Span SpanCommand::span() const {
  return span_;
}

bool SpanCommand::DoDelete(const std::list<Span>::const_iterator& iter) {
  SpanAnalyzerDoc* doc = GetDocument();

  // copies span into command to store
  span_ = *iter;

  // deletes from document
  doc->DeleteSpan(iter);

  return true;
}

bool SpanCommand::DoInsert(const std::list<Span>::const_iterator& iter) {
  SpanAnalyzerDoc* doc = GetDocument();

  // inserts span to document
  doc->InsertSpan(iter, span_);

  // invalidates the command span
  span_ = Span();

  return true;
}

bool SpanCommand::DoModify(const std::list<Span>::const_iterator& iter) {
  SpanAnalyzerDoc* doc = GetDocument();

  // swaps the span from the document and the command
  Span span_tmp = *iter;
  doc->ReplaceSpan(iter, span_);
  span_ = span_tmp;

  return true;
}

bool SpanCommand::DoMoveDown(const std::list<Span>::const_iterator& iter) {
  SpanAnalyzerDoc* doc = GetDocument();

  // swaps the spans in the document
  auto iter_to = std::next(iter, 2);
  doc->MoveSpan(iter, iter_to);

  // increments the command index
  index_ = index_ + 1;

  return true;
}

bool SpanCommand::DoMoveUp(const std::list<Span>::const_iterator& iter) {
  SpanAnalyzerDoc* doc = GetDocument();

  // swaps the spans in the document
  auto iter_to = std::prev(iter, 1);
  doc->MoveSpan(iter, iter_to);

  // decrements the command index
  index_ = index_ - 1;

  return true;
}
