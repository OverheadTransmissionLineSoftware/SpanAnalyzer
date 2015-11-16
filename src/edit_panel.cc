// This is free and unencumbered software released into the public domain.
// For more information, please refer to <http://unlicense.org/>

#include "edit_panel.h"

#include "wx/xrc/xmlres.h"

#include "cable_editor_dialog.h"
#include "span_analyzer_doc.h"
#include "span_analyzer_view.h"
#include "span_editor_dialog.h"
#include "weather_load_case_editor_dialog.h"
#include "weather_load_case_unit_converter.h"

// temporary
#include "span_analyzer_app.h"

/// context menu enum
enum {
  kTreeItemActivate = 0,
  kTreeItemDelete = 1,
  kTreeItemEdit = 2,
  kTreeRootAdd = 3,
  kTreeRootDeleteAll = 4
};

BEGIN_EVENT_TABLE(EditPanel, wxPanel)
  EVT_MENU(wxID_ANY, EditPanel::OnContextMenuSelect)
  EVT_TREE_ITEM_ACTIVATED(wxID_ANY, EditPanel::OnItemActivate)
  EVT_TREE_ITEM_MENU(wxID_ANY, EditPanel::OnItemMenu)
END_EVENT_TABLE()

EditPanel::EditPanel(wxWindow* parent, wxView* view) {
  // loads dialog from virtual xrc file system
  wxXmlResource::Get()->LoadPanel(this, parent, "edit_panel");
  treectrl_doc_ = XRCCTRL(*this, "treectrl_model", wxTreeCtrl);

  // saves view reference
  view_ = view;

  // customizes treectrl and updates
  treectrl_doc_->SetIndent(2);
}

EditPanel::~EditPanel() {
}

Span* EditPanel::ActivatedSpan() {
  // searches tree for bolded item
  wxTreeItemIdValue cookie;
  wxTreeItemId root = treectrl_doc_->GetRootItem();
  wxTreeItemId id = treectrl_doc_->GetFirstChild(root, cookie);
  while (id.IsOk() == true) {
    if (treectrl_doc_->IsBold(id) == true) {
      break;
    } else {
      id = treectrl_doc_->GetNextSibling(id);
    }
  }

  // if bold item was found
  if (id.IsOk()) {
    // gets data associated with id
    EditTreeItemData* item =
        (EditTreeItemData *)treectrl_doc_->GetItemData(id);

    // searches document for span
    SpanAnalyzerDoc* doc = (SpanAnalyzerDoc*)view_->GetDocument();
    std::vector<Span>* spans = doc->spans();
    std::vector<Span>::iterator iter;
    for (iter = spans->begin(); iter != spans->end(); iter++) {
      const Span& span = *iter;
      if (span.name == item->description()) {
        break;
      }
    }

    // returns value based on whether span was found
    if (iter != spans->end()) {
      return &*iter;
    } else {
      return nullptr;
    }

  } else {
    return nullptr;
  }
}

void EditPanel::OnClear(wxCommandEvent& event) {
  treectrl_doc_->DeleteAllItems();
}

void EditPanel::OnContextMenuSelect(wxCommandEvent& event) {
  // gets selected tree item data
  wxTreeItemId id_item = treectrl_doc_->GetSelection();
  if (id_item.IsOk() == false) {
    return;
  }

  // gets context menu selection and sends to handler function
  const int id_event = event.GetId();
  if (id_event == kTreeItemActivate) {
    ActivateSpan(id_item);
  } else if (id_event == kTreeItemDelete) {
    DeleteSpan(id_item);
  } else if (id_event == kTreeItemEdit) {
    EditSpan(id_item);
  } else if (id_event == kTreeRootAdd) {
    AddSpan();
  } else if (id_event == kTreeRootDeleteAll) {
    DeleteSpans();
  }
}

void EditPanel::OnItemActivate(wxTreeEvent& event) {
  // gets activated item from event
  wxTreeItemId id = event.GetItem();
  ActivateSpan(id);
}

void EditPanel::OnItemMenu(wxTreeEvent& event) {
  // gets item from event
  wxTreeItemId id = event.GetItem();
  treectrl_doc_->SetFocusedItem(id);

  // displays a context menu based on the item that was right clicked
  wxMenu menu;
  if (id == treectrl_doc_->GetRootItem()) {
    menu.Append(kTreeRootAdd, "Add Span");
    menu.Append(kTreeRootDeleteAll, "Delete All");
  } else { // a span is selected
    menu.Append(kTreeItemActivate, "Activate");
    menu.AppendSeparator();
    menu.Append(kTreeItemEdit, "Edit");
    menu.Append(kTreeItemDelete, "Delete");
  }

  // shows context menu
  // the event is caught by the edit panel
  PopupMenu(&menu, event.GetPoint());

  // stops processing event (needed to allow pop-up menu to catch its event)
  event.Skip();
}

void EditPanel::Update(wxObject* hint) {
  if (hint == nullptr) {
    InitSpans();
  }
}

void EditPanel::ActivateSpan(const wxTreeItemId& id) {
  if (id.IsOk()) {
    // unbolds all previous items
    wxTreeItemIdValue cookie;
    wxTreeItemId root = treectrl_doc_->GetRootItem();
    wxTreeItemId item = treectrl_doc_->GetFirstChild(root, cookie);
    while (item.IsOk() == true) {
      treectrl_doc_->SetItemBold(item, false);
      item = treectrl_doc_->GetNextSibling(item);
    }

    // bolds new item
    treectrl_doc_->SetItemBold(id, true);
  }

  // updates views
  ViewUpdateHint hint(ViewUpdateHint::HintType::kModelSpansEdit);
  view_->GetDocument()->UpdateAllViews(
      nullptr,
      &hint);
}

void EditPanel::AddSpan() {
  // gets document
  SpanAnalyzerDoc* doc = (SpanAnalyzerDoc*)view_->GetDocument();

  // creates new span and editor
  Span span;
  span.name = "NEW";

  SpanEditorDialog dialog(view_->GetFrame(),
                          &wxGetApp().data()->cables,
                          doc->weathercases(),
                          units::UnitSystem::kImperial,
                          &span);
  if (dialog.ShowModal() != wxID_OK) {
    return;
  }

  // adds to document
  std::vector<Span>* spans = doc->spans();
  spans->push_back(span);

  // adds to treectrl
  wxTreeItemId id = treectrl_doc_->AppendItem(treectrl_doc_->GetRootItem(), span.name);
  EditTreeItemData* data = new EditTreeItemData(EditTreeItemData::Type::kSpan,
                                                span.name);
  treectrl_doc_->SetItemData(id, data);

  // commit changes to doc and update
  doc->Modify(true);
  ViewUpdateHint hint(ViewUpdateHint::HintType::kModelSpansEdit);
  doc->UpdateAllViews(
      nullptr, &hint);
}

void EditPanel::DeleteSpan(const wxTreeItemId& id) {
  // gets tree item data
  EditTreeItemData* item =
      (EditTreeItemData *)treectrl_doc_->GetItemData(id);

  // searches document spans for one with matching description
  SpanAnalyzerDoc* doc = (SpanAnalyzerDoc*)view_->GetDocument();
  std::vector<Span>* spans = doc->spans();
  std::vector<Span>::iterator iter;
  for (iter = spans->begin(); iter != spans->end(); iter++) {
    const Span& span = *iter;
    if (span.name == item->description()) {
      break;
    }
  }

  // erases from document and treectrl
  spans->erase(iter);
  treectrl_doc_->Delete(id);

  // commit changes to doc and update
  doc->Modify(true);
  doc->UpdateAllViews();
}

void EditPanel::DeleteSpans() {
  // deletes spans from model
  SpanAnalyzerDoc* doc = (SpanAnalyzerDoc*)view_->GetDocument();
  std::vector<Span>* spans = doc->spans();
  spans->clear();

  // deletes spans from treectrl
  wxTreeItemId root = treectrl_doc_->GetRootItem();
  treectrl_doc_->DeleteChildren(root);

  // commit changes to doc and update
  doc->Modify(true);
  ViewUpdateHint hint(ViewUpdateHint::HintType::kModelSpansEdit);
  doc->UpdateAllViews(
      nullptr,
      &hint);
}

void EditPanel::EditSpan(const wxTreeItemId& id) {
  // gets tree item data
  EditTreeItemData* item =
      (EditTreeItemData *)treectrl_doc_->GetItemData(id);

  // searches document spans for one with matching description
  SpanAnalyzerDoc* doc = (SpanAnalyzerDoc*)view_->GetDocument();
  std::vector<Span>* spans = doc->spans();
  std::vector<Span>::iterator iter;
  Span* span = nullptr;
  for (iter = spans->begin(); iter != spans->end(); iter++) {
    span = &*iter;
    if (span->name == item->description()) {
      break;
    }
  }

  // creates a span editor dialog
  SpanEditorDialog dialog(view_->GetFrame(),
                          &wxGetApp().data()->cables,
                          doc->weathercases(),
                          units::UnitSystem::kImperial,
                          span);
  if (dialog.ShowModal() == wxID_OK) {
    // commit changes to doc and update
    doc->Modify(true);
    ViewUpdateHint hint(ViewUpdateHint::HintType::kModelSpansEdit);
    doc->UpdateAllViews(
        nullptr,
        &hint);

    // updates treectrl name
    item->set_description(span->name);
    treectrl_doc_->SetItemText(id, item->description());
  }
}

void EditPanel::InitSpans() {
  // clears tree
  treectrl_doc_->DeleteAllItems();

  // gets document
  SpanAnalyzerDoc* doc = (SpanAnalyzerDoc*)view_->GetDocument();

  // adds spans root and all spans in document
  wxTreeItemId root = treectrl_doc_->AddRoot("Spans");
  const std::vector<Span>* spans = doc->spans();
  for (auto iter = spans->cbegin(); iter != spans->cend(); iter++) {
    const Span& span = *iter;
    wxTreeItemId item = treectrl_doc_->AppendItem(root, span.name);
    EditTreeItemData* data = new EditTreeItemData(EditTreeItemData::Type::kSpan, span.name);
    treectrl_doc_->SetItemData(item, data);
  }

  // expands root
  treectrl_doc_->Expand(root);
}
