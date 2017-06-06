// This is free and unencumbered software released into the public domain.
// For more information, please refer to <http://unlicense.org/>

#include "span_analyzer_doc_commands.h"

#include "wx/wx.h"

#include "span_analyzer_app.h"
#include "span_analyzer_doc.h"
#include "span_xml_handler.h"

// These are the string definitions for the various command types.
const std::string SpanCommand::kNameDelete = "Delete Span";
const std::string SpanCommand::kNameInsert = "Insert Span";
const std::string SpanCommand::kNameModify = "Modify Span";
const std::string SpanCommand::kNameMoveDown = "Move Span Down";
const std::string SpanCommand::kNameMoveUp = "Move Span Up";

SpanCommand::SpanCommand(const std::string& name)
    : wxCommand(true, name) {
  data_ = wxGetApp().data();
  doc_ = wxGetApp().GetDocument();
  index_ = -1;
  node_do_ = nullptr;
  node_undo_ = nullptr;
}

SpanCommand::~SpanCommand() {
  delete node_do_;
  delete node_undo_;
}

bool SpanCommand::CreateSpanFromXml(const wxXmlNode* node, Span& span) {
  // clears span
  span = Span();

  // gets application data
  const SpanAnalyzerData* data = wxGetApp().data();

  // parses node into span
  const bool status_node = SpanXmlHandler::ParseNode(node, "",
                                                     &data->cablefiles,
                                                     &data->weathercases,
                                                     span);
  if (status_node == false) {
    wxString message =
        SpanXmlHandler::FileAndLineNumber("", node)
        + "Invalid span. Skipping.";
    wxLogError(message);
  }

  return status_node;
}

bool SpanCommand::Do() {
  bool status = false;

  // checks index to see if a valid iterator is possible
  const int kSize = doc_->spans().size();
  if ((index_ < 0) || (kSize < index_)) {
    wxLogError("Invalid span index. Aborting command.");
    return false;
  }

  // gets iterator
  std::list<Span>::const_iterator iter;
  iter = std::next(doc_->spans().cbegin(), index_);

  // selects based on command name
  const std::string name = GetName();
  if (name == kNameDelete) {
    // checks that index is valid
    if (index_ == kSize) {
      wxLogError("Invalid span index. Aborting delete command.");
      return false;
    }

    // backs up span to xml
    const Span& span = *iter;
    node_undo_ = SaveSpanToXml(span);

    // edits doc
    status = DoDelete(iter);
  } else if (name == kNameInsert) {
    status = DoInsert(iter, node_do_);
  } else if (name == kNameModify) {
    // checks that index is valid
    if (index_ == kSize) {
      wxLogError("Invalid span index. Aborting modify command.");
      return false;
    }

    // backs up span to xml
    const Span& span = *iter;
    node_undo_ = SaveSpanToXml(span);

    // edits doc
    status = DoModify(iter, node_do_);
  } else if (name == kNameMoveDown) {
    // checks that index isn't the last span
    if (index_ == kSize - 1) {
      wxLogError("Invalid span index. Aborting move down command.");
      return false;
    }

    status = DoMoveDown(iter);
  } else if (name == kNameMoveUp) {
    // checks that index isn't the first span
    if (index_ == 0) {
      wxLogError("Invalid span index. Aborting move up command.");
      return false;
    }

    status = DoMoveUp(iter);
  } else {
    status = false;

    wxLogError("Invalid command. Aborting.");
    return false;
  }

  // checks if command succeeded
  if (status == true) {
    // posts a view update
    UpdateHint hint(HintType::kSpansEdit);
    doc_->UpdateAllViews(nullptr, &hint);
  } else {
    // logs error and restores previous document state
    wxLogError("Errors were encountered when executing command.");
    Undo();
  }

  return status;
}

wxXmlNode* SpanCommand::SaveSpanToXml(const Span& span) {
  units::UnitSystem units = wxGetApp().config()->units;

  return SpanXmlHandler::CreateNode(span, "", units);;
}

bool SpanCommand::Undo() {
  bool status = false;

  // checks index to see if a valid iterator is possible
  const int kSize = doc_->spans().size();
  if ((index_ < 0) || (kSize < index_)) {
    wxLogError("Invalid span index. Aborting command.");
    return false;
  }

  // gets iterator
  std::list<Span>::const_iterator iter;
  iter = std::next(doc_->spans().cbegin(), index_);

  // selects based on command name
  const std::string name = GetName();
  if (name == kNameDelete) {
    status = DoInsert(iter, node_undo_);
  } else if (name == kNameInsert) {
    status = DoDelete(iter);
  } else if (name == kNameModify) {
    status = DoModify(iter, node_undo_);
  } else if (name == kNameMoveDown) {
    status = DoMoveUp(iter);
  } else if (name == kNameMoveUp) {
    status = DoMoveDown(iter);
  } else {
    status = false;

    wxLogError("Invalid command. Aborting.");
    return false;
  }

  // posts a view update
  UpdateHint hint(HintType::kSpansEdit);
  doc_->UpdateAllViews(nullptr, &hint);

  return status;
}

int SpanCommand::index() const {
  return index_;
}

const wxXmlNode* SpanCommand::node_span() const {
  return node_do_;
}

void SpanCommand::set_index(const int& index) {
  index_ = index;
}

void SpanCommand::set_node_span(const wxXmlNode* node_span) {
  node_do_ = node_span;
}

bool SpanCommand::DoDelete(const std::list<Span>::const_iterator& iter) {
  // deletes from document
  doc_->DeleteSpan(iter);

  return true;
}

bool SpanCommand::DoInsert(const std::list<Span>::const_iterator& iter,
                           const wxXmlNode* node) {
  // builds span from xml node
  Span span;
  SpanXmlHandler::ParseNode(node, "", &data_->cablefiles, &data_->weathercases,
                            span);

  // inserts span to document
  doc_->InsertSpan(iter, span);

  return true;
}

bool SpanCommand::DoModify(const std::list<Span>::const_iterator& iter,
                           const wxXmlNode* node) {
  // builds span from xml node
  Span span;
  SpanXmlHandler::ParseNode(node, "", &data_->cablefiles, &data_->weathercases,
                            span);

  // swaps the span from the document and the command
  doc_->ReplaceSpan(iter, span);

  return true;
}

bool SpanCommand::DoMoveDown(const std::list<Span>::const_iterator& iter) {
  // swaps the spans in the document
  auto iter_to = std::next(iter, 2);
  doc_->MoveSpan(iter, iter_to);

  // increments the command index
  index_ = index_ + 1;

  return true;
}

bool SpanCommand::DoMoveUp(const std::list<Span>::const_iterator& iter) {
  // swaps the spans in the document
  auto iter_to = std::prev(iter, 1);
  doc_->MoveSpan(iter, iter_to);

  // decrements the command index
  index_ = index_ - 1;

  return true;
}
