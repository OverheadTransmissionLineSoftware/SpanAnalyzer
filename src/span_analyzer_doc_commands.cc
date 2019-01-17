// This is free and unencumbered software released into the public domain.
// For more information, please refer to <http://unlicense.org/>

#include "spananalyzer/span_analyzer_doc_commands.h"

#include "wx/wx.h"

#include "spananalyzer/span_analyzer_app.h"
#include "spananalyzer/span_analyzer_doc.h"
#include "spananalyzer/span_xml_handler.h"

// These are the string definitions for the various command types.
const std::string SpanCommand::kNameDelete = "Delete Span";
const std::string SpanCommand::kNameInsert = "Insert Span";
const std::string SpanCommand::kNameModify = "Modify Span";
const std::string SpanCommand::kNameMoveDown = "Move Span Down";
const std::string SpanCommand::kNameMoveUp = "Move Span Up";

SpanCommand::SpanCommand(const std::string& name)
    : wxCommand(true, name) {
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
                                                     units::UnitSystem::kNull,
                                                     false,
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

  // clears undo node so it's ready to cache the existing state
  if (node_undo_ != nullptr) {
    delete node_undo_;
  }

  // selects based on command name
  const std::string name = GetName();
  if (name == kNameDelete) {
    // caches span to xml and then does command
    if (doc_->IsValidIndex(index_, false) == false) {
      wxLogError("Invalid span index. Aborting command.");
      return false;
    }
    const Span& span = *std::next(doc_->spans().cbegin(), index_);
    node_undo_ = SaveSpanToXml(span);
    status = DoDelete();
  } else if (name == kNameInsert) {
    // does command
    status = DoInsert(node_do_);
  } else if (name == kNameModify) {
    // caches span to xml and then does command
    if (doc_->IsValidIndex(index_, false) == false) {
      wxLogError("Invalid span index. Aborting command.");
      return false;
    }
    const Span& span = *std::next(doc_->spans().cbegin(), index_);
    node_undo_ = SaveSpanToXml(span);
    status = DoModify(node_do_);
  } else if (name == kNameMoveDown) {
    // does command
    status = DoMoveDown();
  } else if (name == kNameMoveUp) {
    // does command
    status = DoMoveUp();
  } else {
    status = false;

    wxLogError("Invalid command. Aborting.");
    return false;
  }

  // checks if command succeeded
  if (status == true) {
    // posts a view update
    UpdateHint hint(UpdateHint::Type::kSpansEdit);
    doc_->UpdateAllViews(nullptr, &hint);
  } else {
    // logs error
    wxLogError("Do command failed.");
  }

  return status;
}

wxXmlNode* SpanCommand::SaveSpanToXml(const Span& span) {
  // copies span and removes connections
  Span span_modified = span;
  span_modified.linecable.ClearConnections();

  // gets unit system and returns xml node
  units::UnitSystem units = wxGetApp().config()->units;
  return SpanXmlHandler::CreateNode(span_modified, "", units,
                                    units::UnitStyle::kConsistent);;
}

bool SpanCommand::Undo() {
  bool status = false;

  // selects based on command name
  const std::string name = GetName();
  if (name == kNameDelete) {
    status = DoInsert(node_undo_);
  } else if (name == kNameInsert) {
    status = DoDelete();
  } else if (name == kNameModify) {
    status = DoModify(node_undo_);
  } else if (name == kNameMoveDown) {
    status = DoMoveUp();
  } else if (name == kNameMoveUp) {
    status = DoMoveDown();
  } else {
    status = false;

    wxLogError("Invalid command. Aborting.");
    return false;
  }

  // checks if command succeeded
  if (status == true) {
    // posts a view update
    UpdateHint hint(UpdateHint::Type::kSpansEdit);
    doc_->UpdateAllViews(nullptr, &hint);
  } else {
    // logs error
    wxLogError("Undo command failed.");
  }

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

bool SpanCommand::DoDelete() {
  // checks index
  if (doc_->IsValidIndex(index_, false) == false) {
    wxLogError("Invalid index. Aborting delete command.");
    return false;
  }

  // deletes from document
  return doc_->DeleteSpan(index_);
}

bool SpanCommand::DoInsert(const wxXmlNode* node) {
  // checks index
  if (doc_->IsValidIndex(index_, true) == false) {
    wxLogError("Invalid index. Aborting insert command.");
    return false;
  }

  // builds span from xml node
  Span span;
  CreateSpanFromXml(node, span);

  // inserts span to document
  return doc_->InsertSpan(index_, span);
}

bool SpanCommand::DoModify(const wxXmlNode* node) {
  // checks index
  if (doc_->IsValidIndex(index_, false) == false) {
    wxLogError("Invalid index. Aborting modify command.");
    return false;
  }

  // builds span from xml node
  Span span;
  bool status = CreateSpanFromXml(node, span);
  if (status == false) {
    return false;
  }

  // modifies the document
  return doc_->ModifySpan(index_, span);
}

bool SpanCommand::DoMoveDown() {
  // checks index, and checks against the last valid index
  const int kSizeSpans = doc_->spans().size();
  if ((doc_->IsValidIndex(index_, false) == false)
      || (index_ == kSizeSpans - 1)) {
    wxLogError("Invalid index. Aborting move down command.");
    return false;
  }

  // swaps the spans in the document
  bool status = doc_->MoveSpan(index_, index_ + 2);

  // increments the command index
  index_ = index_ + 1;

  return status;
}

bool SpanCommand::DoMoveUp() {
  // checks index, and checks against the first valid index
  if ((doc_->IsValidIndex(index_, false) == false)
      || (index_ == 0)) {
    wxLogError("Invalid index. Aborting move up command.");
    return false;
  }

  // swaps the spans in the document
  bool status = doc_->MoveSpan(index_, index_ - 1);

  // decrements the command index
  index_ = index_ - 1;

  return status;
}
