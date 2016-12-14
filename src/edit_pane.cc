// This is free and unencumbered software released into the public domain.
// For more information, please refer to <http://unlicense.org/>

#include "edit_pane.h"

#include "wx/xrc/xmlres.h"

#include "span_analyzer_app.h"
#include "span_analyzer_doc.h"
#include "span_editor_dialog.h"
#include "span_unit_converter.h"

/// context menu enum
enum {
  kTreeItemActivate = 0,
  kTreeItemCopy = 1,
  kTreeItemDelete = 2,
  kTreeItemEdit = 3,
  kTreeRootAdd = 4,
  kTreeRootDeleteAll = 5
};

BEGIN_EVENT_TABLE(EditPane, wxPanel)
  EVT_BUTTON(XRCID("button_add"), EditPane::OnButtonAdd)
  EVT_BUTTON(XRCID("button_copy"), EditPane::OnButtonCopy)
  EVT_BUTTON(XRCID("button_delete"), EditPane::OnButtonDelete)
  EVT_BUTTON(XRCID("button_edit"), EditPane::OnButtonEdit)
  EVT_MENU(wxID_ANY, EditPane::OnContextMenuSelect)
  EVT_TREE_BEGIN_DRAG(wxID_ANY, EditPane::OnDragBegin)
  EVT_TREE_END_DRAG(wxID_ANY, EditPane::OnDragEnd)
  EVT_TREE_ITEM_ACTIVATED(wxID_ANY, EditPane::OnItemActivate)
  EVT_TREE_ITEM_MENU(wxID_ANY, EditPane::OnItemMenu)
END_EVENT_TABLE()

EditPane::EditPane(wxWindow* parent, wxView* view) {
  // loads dialog from virtual xrc file system
  wxXmlResource::Get()->LoadPanel(this, parent, "edit_pane");

  // saves view reference
  view_ = view;

  // sets up treectrl
  treectrl_ = XRCCTRL(*this, "treectrl", wxTreeCtrl);
  treectrl_->AddRoot("Spans");
  treectrl_->SetIndent(2);
}

EditPane::~EditPane() {
}

void EditPane::Update(wxObject* hint) {
  if (hint == nullptr) {
    InitializeTreeCtrl();
  }
}

void EditPane::ActivateSpan(const wxTreeItemId& id) {
  if (id.IsOk() == false) {
    return;
  }

  if (id == treectrl_->GetRootItem()) {
    return;
  }

  if (id == item_activated_) {
    return;
  }

  // unbolds previous item
  if (item_activated_.IsOk()) {
    treectrl_->SetItemBold(item_activated_, false);
  }

  // bolds new item and caches
  treectrl_->SetItemBold(id, true);
  item_activated_ = id;

  // updates document
  SpanAnalyzerDoc* doc = (SpanAnalyzerDoc*)view_->GetDocument();
  SpanTreeItemData* data =
      (SpanTreeItemData*)treectrl_->GetItemData(item_activated_);
  auto iter = data->iter();
  doc->SetSpanAnalysis(&(*iter));
}

void EditPane::AddSpan() {
  // creates new span and editor
  Span span;
  span.name = "NEW";

  // gets referenced objects and makes sure that they exist
  const std::list<CableFile*>& cablefiles = wxGetApp().data()->cablefiles;
  if (cablefiles.empty() == true) {
    wxMessageBox("No cables are currently loaded. Add at least one before "
                 "adding span.");
    return;
  }

  const std::list<WeatherLoadCase*>& weathercases =
      wxGetApp().data()->weathercases;
  if (weathercases.empty() == true) {
    wxMessageBox("No weathercases are defined. Add at least one before adding "
                 "span.");
    return;
  }

  SpanEditorDialog dialog(view_->GetFrame(),
                          &cablefiles,
                          &weathercases,
                          wxGetApp().config()->units,
                          &span);
  if (dialog.ShowModal() != wxID_OK) {
    return;
  }

  // adds to document
  SpanAnalyzerDoc* doc = (SpanAnalyzerDoc*)view_->GetDocument();
  auto iter = doc->AppendSpan(span);

  // adds to treectrl
  wxTreeItemId id = treectrl_->AppendItem(treectrl_->GetRootItem(), span.name);
  SpanTreeItemData* data = new SpanTreeItemData();
  data->set_iter(iter);
  treectrl_->SetItemData(id, data);
}

void EditPane::CopySpan(const wxTreeItemId& id) {
  // gets tree item data
  SpanTreeItemData* item = (SpanTreeItemData*)treectrl_->GetItemData(id);

  // copies span
  Span span = *(item->iter());

  // gets position and inserts to document
  auto position = std::next(item->iter());

  SpanAnalyzerDoc* doc = (SpanAnalyzerDoc*)view_->GetDocument();
  auto iter = doc->InsertSpan(position, span);

  // adds to treectrl
  wxTreeItemId id_item = treectrl_->InsertItem(
      treectrl_->GetRootItem(), id, span.name);
  SpanTreeItemData* data = new SpanTreeItemData();
  data->set_iter(iter);
  treectrl_->SetItemData(id_item, data);
}

void EditPane::DeleteSpan(const wxTreeItemId& id) {
  // gets tree item data
  SpanTreeItemData* item = (SpanTreeItemData*)treectrl_->GetItemData(id);

  // erases from document and treectrl
  SpanAnalyzerDoc* doc = (SpanAnalyzerDoc*)view_->GetDocument();
  doc->DeleteSpan(item->iter());

  if (item_activated_ == id) {
    item_activated_ = (wxTreeItemId)0l;
  }

  treectrl_->Delete(id);
}

void EditPane::DeleteSpans() {
  // gets list and size
  SpanAnalyzerDoc* doc = (SpanAnalyzerDoc*)view_->GetDocument();
  const std::list<Span>& spans = doc->spans();

  // deletes spans from doc
  std::list<Span>::const_iterator iter = spans.cbegin();
  while (iter != spans.cend()) {
    iter = doc->DeleteSpan(iter);
  }

  // deletes spans from treectrl
  wxTreeItemId root = treectrl_->GetRootItem();
  treectrl_->DeleteChildren(root);
}

void EditPane::EditSpan(const wxTreeItemId& id) {
  // gets tree item data and copies span
  SpanTreeItemData* item = (SpanTreeItemData*)treectrl_->GetItemData(id);
  Span span = *(item->iter());

  // converts span to different unit style
  SpanUnitConverter::ConvertUnitStyle(wxGetApp().config()->units,
                                      units::UnitStyle::kConsistent,
                                      units::UnitStyle::kDifferent,
                                      span);

  // gets referenced objects and makes sure that they exist
  const std::list<CableFile*>& cablefiles = wxGetApp().data()->cablefiles;
  if (cablefiles.empty() == true) {
    wxMessageBox("No cables are currently loaded. Add at least one before "
                 "editing span.");
    return;
  }

  const std::list<WeatherLoadCase*>& weathercases =
      wxGetApp().data()->weathercases;
  if (weathercases.empty() == true) {
    wxMessageBox("No weathercases are defined. Add at least one before editing "
                 "span.");
    return;
  }

  // creates a span editor dialog
  SpanEditorDialog dialog(view_->GetFrame(),
                          &cablefiles,
                          &weathercases,
                          wxGetApp().config()->units,
                          &span);
  if (dialog.ShowModal() == wxID_OK) {
    // converts span to consistent unit style
    SpanUnitConverter::ConvertUnitStyle(wxGetApp().config()->units,
                                        units::UnitStyle::kDifferent,
                                        units::UnitStyle::kConsistent,
                                        span);

    // updates document
    SpanAnalyzerDoc* doc = (SpanAnalyzerDoc*)view_->GetDocument();
    doc->ReplaceSpan(item->iter(), span);

    // updates treectrl name
    treectrl_->SetItemText(id, span.name);
  }
}

void EditPane::InitializeTreeCtrl() {
  // gets root
  wxTreeItemId root = treectrl_->GetRootItem();

  // deletes all children items
  treectrl_->DeleteChildren(root);

  // adds items for all spans in document
  SpanAnalyzerDoc* doc = (SpanAnalyzerDoc*)view_->GetDocument();
  const std::list<Span>& spans = doc->spans();
  for (auto iter = spans.cbegin(); iter != spans.cend(); iter++) {
    const Span& span = *iter;
    wxTreeItemId item = treectrl_->AppendItem(root, span.name);
    SpanTreeItemData* data = new SpanTreeItemData();
    data->set_iter(iter);
    treectrl_->SetItemData(item, data);
  }

  // expands to show all spans
  treectrl_->Expand(root);
}

void EditPane::OnButtonAdd(wxCommandEvent& event) {
  // can't create busy cursor, a dialog is used further along
  
  AddSpan();
}

void EditPane::OnButtonCopy(wxCommandEvent& event) {
  wxBusyCursor cursor;
  
  // gets selected tree item
  wxTreeItemId id_item = treectrl_->GetSelection();
  if (id_item.IsOk() == false) {
    return;
  }

  // checks to make sure item isn't root
  if (id_item == treectrl_->GetRootItem()) {
    return;
  }

  // copies span
  CopySpan(id_item);
}

void EditPane::OnButtonDelete(wxCommandEvent& event) {
  wxBusyCursor cursor;
  
  // gets selected tree item
  wxTreeItemId id_item = treectrl_->GetSelection();
  if (id_item.IsOk() == false) {
    return;
  }

  // checks to make sure item isn't root
  if (id_item == treectrl_->GetRootItem()) {
    return;
  }

  // deletes span
  DeleteSpan(id_item);
}

void EditPane::OnButtonEdit(wxCommandEvent& event) {
  // can't create busy cursor, a dialog is used further along

  // gets selected tree item
  wxTreeItemId id_item = treectrl_->GetSelection();
  if (id_item.IsOk() == false) {
    return;
  }

  // checks to make sure item isn't root
  if (id_item == treectrl_->GetRootItem()) {
    return;
  }

  // edits span
  EditSpan(id_item);
}

void EditPane::OnContextMenuSelect(wxCommandEvent& event) {
  // gets selected tree item data
  wxTreeItemId id_item = treectrl_->GetSelection();
  if (id_item.IsOk() == false) {
    return;
  }

  // gets context menu selection and sends to handler function
  const int id_event = event.GetId();
  if (id_event == kTreeItemActivate) {
    wxBusyCursor cursor;
    ActivateSpan(id_item);
  } else if (id_event == kTreeItemCopy) {
    wxBusyCursor cursor;
    CopySpan(id_item);
  } else if (id_event == kTreeItemDelete) {
    wxBusyCursor cursor;
    DeleteSpan(id_item);
  } else if (id_event == kTreeItemEdit) {
    // can't create busy cursor, a dialog is used further along
    EditSpan(id_item);
  } else if (id_event == kTreeRootAdd) {
    // can't create busy cursor, a dialog is used further along
    AddSpan();
  } else if (id_event == kTreeRootDeleteAll) {
    wxBusyCursor cursor;
    DeleteSpans();
  }
}

void EditPane::OnDragBegin(wxTreeEvent& event) {
  // checks if span is dragged, not the root
  if (event.GetItem() == treectrl_->GetRootItem()) {
    return;
  }

  // stores reference to dragged item and allows event
  item_dragged_ = event.GetItem();
  event.Allow();
}

void EditPane::OnDragEnd(wxTreeEvent& event) {
  wxBusyCursor cursor;
  
  // verifies that the end drag is valid
  if ((event.GetItem().IsOk() == false)
      ||(event.GetItem() == treectrl_->GetRootItem())
      || (event.GetItem() == item_dragged_)) {
    item_dragged_ = (wxTreeItemId)0l;
    return;
  }

  // identifies source and destination
  wxTreeItemId item_source = item_dragged_;
  wxTreeItemId item_destination = event.GetItem();

  item_dragged_ = (wxTreeItemId)0l;

  // gets data for source and destination items
  SpanTreeItemData* data_source =
      (SpanTreeItemData*)treectrl_->GetItemData(item_source);
  std::list<Span>::const_iterator iter_source = data_source->iter();

  SpanTreeItemData* data_destination =
      (SpanTreeItemData*)treectrl_->GetItemData(item_destination);
  std::list<Span>::const_iterator iter_destination = data_destination->iter();
  std::advance(iter_destination, 1);

  // modifies document
  SpanAnalyzerDoc* doc = (SpanAnalyzerDoc*)view_->GetDocument();
  doc->MoveSpan(iter_source, iter_destination);

  // modifies treectrl
  wxString desc = treectrl_->GetItemText(item_source);
  std::list<Span>::const_iterator iter = data_source->iter();

  bool is_activated_item = false;
  if (item_activated_ == item_source) {
    is_activated_item = true;
  }

  treectrl_->Delete(item_source);
  item_source = treectrl_->InsertItem(
      treectrl_->GetRootItem(), item_destination, desc);
  SpanTreeItemData* data = new SpanTreeItemData();
  data->set_iter(iter);
  treectrl_->SetItemData(item_source, data);

  if (is_activated_item == true) {
    item_activated_ = item_source;
    treectrl_->SetItemBold(item_source);
  }
}

void EditPane::OnItemActivate(wxTreeEvent& event) {
  wxBusyCursor cursor;

  // gets activated item from event
  wxTreeItemId id = event.GetItem();
  ActivateSpan(id);
}

void EditPane::OnItemMenu(wxTreeEvent& event) {
  // gets item from event
  wxTreeItemId id = event.GetItem();
  treectrl_->SetFocusedItem(id);

  // displays a context menu based on the item that was right clicked
  wxMenu menu;
  if (id == treectrl_->GetRootItem()) {
    menu.Append(kTreeRootAdd, "Add Span");
    menu.Append(kTreeRootDeleteAll, "Delete All");
  } else { // a span is selected
    menu.Append(kTreeItemActivate, "Activate");
    menu.AppendSeparator();
    menu.Append(kTreeItemEdit, "Edit");
    menu.Append(kTreeItemCopy, "Copy");
    menu.Append(kTreeItemDelete, "Delete");
  }

  // shows context menu
  // the event is caught by the edit panel
  PopupMenu(&menu, event.GetPoint());

  // stops processing event (needed to allow pop-up menu to catch its event)
  event.Skip();
}
