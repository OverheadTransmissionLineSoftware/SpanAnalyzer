// This is free and unencumbered software released into the public domain.
// For more information, please refer to <http://unlicense.org/>

#include "edit_pane.h"

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
  kTreeItemCopy = 1,
  kTreeItemDelete = 2,
  kTreeItemEdit = 3,
  kTreeRootAdd = 4,
  kTreeRootDeleteAll = 5
};

BEGIN_EVENT_TABLE(WeathercaseTreeCtrl, wxTreeCtrl)
  EVT_MENU(wxID_ANY, WeathercaseTreeCtrl::OnContextMenuSelect)
  EVT_TREE_ITEM_MENU(wxID_ANY, WeathercaseTreeCtrl::OnItemMenu)
END_EVENT_TABLE()

WeathercaseTreeCtrl::WeathercaseTreeCtrl(wxWindow* parent, wxView* view)
    : wxTreeCtrl(parent, wxID_ANY) {
  // saves view reference
  view_ = view;

  // gets weathercases from document
  SpanAnalyzerDoc* doc = (SpanAnalyzerDoc*)view_->GetDocument();
  weathercases_ = doc->weathercases();

  // customizes treectrl
  SetIndent(2);
  AddRoot("Weathercases");
}

WeathercaseTreeCtrl::~WeathercaseTreeCtrl() {
}

void WeathercaseTreeCtrl::Update(wxObject* hint) {
  if (hint == nullptr) {
    InitWeathercases();
  }
}

void WeathercaseTreeCtrl::AddWeathercase() {
  // creates new weathercase and editor
  WeatherLoadCase weathercase;
  weathercase.description = "NEW";

  WeatherLoadCaseEditorDialog dialog(view_->GetFrame(),
                                     &weathercase,
                                     wxGetApp().config()->units);
  if (dialog.ShowModal() != wxID_OK) {
    return;
  }

  // converts to consistent units and adds to weathercases
  WeatherLoadCaseUnitConverter::ConvertUnitStyle(wxGetApp().config()->units,
                                                 units::UnitStyle::kDifferent,
                                                 units::UnitStyle::kConsistent,
                                                 weathercase);

  weathercases_->push_back(weathercase);

  // adds to treectrl
  wxTreeItemId id = AppendItem(GetRootItem(), weathercase.description);
  EditTreeItemData* data = new EditTreeItemData(
      EditTreeItemData::Type::kWeathercase, weathercase.description);
  SetItemData(id, data);

  // marks doc as modified
  view_->GetDocument()->Modify(true);
}

void WeathercaseTreeCtrl::CopyWeathercase(const wxTreeItemId& id) {
  // gets tree item data
  EditTreeItemData* item = (EditTreeItemData *)GetItemData(id);

  // gets selected span
  // searches document spans for one with matching description
  std::vector<WeatherLoadCase>::iterator iter;
  for (iter = weathercases_->begin(); iter != weathercases_->end(); iter++) {
    const WeatherLoadCase& weathercase = *iter;
    if (weathercase.description == item->description()) {
      break;
    }
  }

  // copies span, modifies name
  WeatherLoadCase weathercase = *iter;
  weathercase.description = weathercase.description + "1";

  // adds to document
  weathercases_->insert(iter + 1, weathercase);

  // adds to treectrl
  wxTreeItemId id_item = InsertItem(GetRootItem(), id, weathercase.description);
  EditTreeItemData* data = new EditTreeItemData(
      EditTreeItemData::Type::kWeathercase, weathercase.description);
  SetItemData(id_item, data);

  // marks document as modified
  view_->GetDocument()->Modify(true);
}

void WeathercaseTreeCtrl::DeleteWeathercase(const wxTreeItemId& id) {
  // gets tree item data
  EditTreeItemData* item = (EditTreeItemData *)GetItemData(id);

  // searches document spans for one with matching description
  std::vector<WeatherLoadCase>::iterator iter;
  for (iter = weathercases_->begin(); iter != weathercases_->end(); iter++) {
    const WeatherLoadCase& weathercase = *iter;
    if (weathercase.description == item->description()) {
      break;
    }
  }

  // erases from document and treectrl
  weathercases_->erase(iter);
  Delete(id);

  // marks doc as modified and updates views
  view_->GetDocument()->Modify(true);
  ViewUpdateHint hint(ViewUpdateHint::HintType::kModelWeathercaseEdit);
  view_->GetDocument()->UpdateAllViews(nullptr, &hint);
}
void WeathercaseTreeCtrl::DeleteWeathercases() {
  // deletes weathercases from doc
  weathercases_->clear();

  // deletes weathercases from treectrl
  wxTreeItemId root = GetRootItem();
  DeleteChildren(root);

  // marks document as modified and updates views
  view_->GetDocument()->Modify(true);
  ViewUpdateHint hint(ViewUpdateHint::HintType::kModelWeathercaseEdit);
  view_->GetDocument()->UpdateAllViews(nullptr, &hint);
}

void WeathercaseTreeCtrl::EditWeathercase(const wxTreeItemId& id) {
  // gets tree item data
  EditTreeItemData* item = (EditTreeItemData *)GetItemData(id);

  // searches document spans for one with matching description
  std::vector<WeatherLoadCase>::iterator iter;
  WeatherLoadCase* weathercase = nullptr;
  for (iter = weathercases_->begin(); iter != weathercases_->end(); iter++) {
    weathercase = &*iter;
    if (weathercase->description == item->description()) {
      break;
    }
  }

  // converts to different unit style
  WeatherLoadCaseUnitConverter::ConvertUnitStyle(wxGetApp().config()->units,
                                                 units::UnitStyle::kDifferent,
                                                 units::UnitStyle::kConsistent,
                                                 *weathercase);

  //creates a weathercase editor dialog
  WeatherLoadCaseEditorDialog dialog(view_->GetFrame(),
                                     weathercase,
                                     wxGetApp().config()->units);
  const int status = dialog.ShowModal();

  // converts to different unit style
  WeatherLoadCaseUnitConverter::ConvertUnitStyle(wxGetApp().config()->units,
                                                 units::UnitStyle::kConsistent,
                                                 units::UnitStyle::kDifferent,
                                                 *weathercase);

  if (status == wxID_OK) {
    // marks document as modified and updates views
    view_->GetDocument()->Modify(true);
    ViewUpdateHint hint(ViewUpdateHint::HintType::kModelWeathercaseEdit);
    view_->GetDocument()->UpdateAllViews(nullptr, &hint);

    // updates treectrl item name
    item->set_description(weathercase->description);
    SetItemText(id, item->description());
  }
}

void WeathercaseTreeCtrl::InitWeathercases() {
  // gets root item
  wxTreeItemId root = GetRootItem();

  // deletes all weathercase items in treectrl
  DeleteChildren(root);

  // adds all weathercases in document
  for (auto iter = weathercases_->cbegin(); iter != weathercases_->cend();
       iter++) {
    const WeatherLoadCase& weathercase = *iter;
    wxTreeItemId item = AppendItem(root, weathercase.description);
    EditTreeItemData* data = new EditTreeItemData(
        EditTreeItemData::Type::kWeathercase, weathercase.description);
    SetItemData(item, data);
  }

  // expands root
  Expand(root);
}

void WeathercaseTreeCtrl::OnContextMenuSelect(wxCommandEvent& event) {
  // gets selected tree item data
  wxTreeItemId id_item = GetSelection();
  if (id_item.IsOk() == false) {
    return;
  }

  // gets context menu selection and sends to handler function
  const int id_event = event.GetId();
  if (id_event == kTreeItemCopy) {
    CopyWeathercase(id_item);
  } else if (id_event == kTreeItemDelete) {
    DeleteWeathercase(id_item);
  } else if (id_event == kTreeItemEdit) {
    EditWeathercase(id_item);
  } else if (id_event == kTreeRootAdd) {
    AddWeathercase();
  } else if (id_event == kTreeRootDeleteAll) {
    DeleteWeathercases();
  }
}

void WeathercaseTreeCtrl::OnItemMenu(wxTreeEvent& event) {
  // gets item from event
  wxTreeItemId id = event.GetItem();
  SetFocusedItem(id);

  // displays a context menu based on the item that was right clicked
  wxMenu menu;
  if (id == GetRootItem()) {
    menu.Append(kTreeRootAdd, "Add Span");
    menu.Append(kTreeRootDeleteAll, "Delete All");
  } else { // a weathercase is selected
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

BEGIN_EVENT_TABLE(SpanTreeCtrl, wxTreeCtrl)
  EVT_MENU(wxID_ANY, SpanTreeCtrl::OnContextMenuSelect)
  EVT_TREE_ITEM_ACTIVATED(wxID_ANY, SpanTreeCtrl::OnItemActivate)
  EVT_TREE_ITEM_MENU(wxID_ANY, SpanTreeCtrl::OnItemMenu)
END_EVENT_TABLE()

SpanTreeCtrl::SpanTreeCtrl(wxWindow* parent, wxView* view)
    : wxTreeCtrl(parent, wxID_ANY) {
  // saves view reference
  view_ = view;

  // gets span from document
  SpanAnalyzerDoc* doc = (SpanAnalyzerDoc*)view_->GetDocument();
  spans_ = doc->spans();

  // customizes treectrl
  SetIndent(2);
  AddRoot("Spans");
}

SpanTreeCtrl::~SpanTreeCtrl() {
}

void SpanTreeCtrl::Update(wxObject* hint) {
  if (hint == nullptr) {
    InitSpans();
  }
}

void SpanTreeCtrl::ActivateSpan(const wxTreeItemId& id) {
  if (id.IsOk()) {
    // unbolds all previous items
    wxTreeItemIdValue cookie;
    wxTreeItemId root = GetRootItem();
    wxTreeItemId item = GetFirstChild(root, cookie);
    while (item.IsOk() == true) {
      SetItemBold(item, false);
      item = GetNextSibling(item);
    }

    // bolds new item
    SetItemBold(id, true);
  }

  // updates views
  ViewUpdateHint hint(ViewUpdateHint::HintType::kModelSpansEdit);
  view_->GetDocument()->UpdateAllViews(nullptr, &hint);
}

void SpanTreeCtrl::AddSpan() {
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
  spans_->push_back(span);

  // adds to treectrl
  wxTreeItemId id = AppendItem(GetRootItem(), span.name);
  EditTreeItemData* data = new EditTreeItemData(EditTreeItemData::Type::kSpan,
                                                span.name);
  SetItemData(id, data);

  // marks document as modified
  doc->Modify(true);
}

void SpanTreeCtrl::CopySpan(const wxTreeItemId& id) {
  // gets tree item data
  EditTreeItemData* item = (EditTreeItemData *)GetItemData(id);

  // gets selected span
  // searches document spans for one with matching description
  std::vector<Span>::iterator iter;
  for (iter = spans_->begin(); iter != spans_->end(); iter++) {
    const Span& span = *iter;
    if (span.name == item->description()) {
      break;
    }
  }

  // copies span, modifies name
  Span span = *iter;
  span.name = span.name + "1";

  // adds to document
  spans_->insert(iter + 1, span);

  // adds to treectrl
  wxTreeItemId id_item = InsertItem(GetRootItem(), id, span.name);
  EditTreeItemData* data = new EditTreeItemData(EditTreeItemData::Type::kSpan,
                                                span.name);
  SetItemData(id_item, data);

  // marks document as modified
  view_->GetDocument()->Modify(true);
}

void SpanTreeCtrl::DeleteSpan(const wxTreeItemId& id) {
  // gets tree item data
  EditTreeItemData* item = (EditTreeItemData *)GetItemData(id);

  // searches document spans for one with matching description
  SpanAnalyzerDoc* doc = (SpanAnalyzerDoc*)view_->GetDocument();
  std::vector<Span>::iterator iter;
  for (iter = spans_->begin(); iter != spans_->end(); iter++) {
    const Span& span = *iter;
    if (span.name == item->description()) {
      break;
    }
  }

  // erases from document and treectrl
  spans_->erase(iter);
  Delete(id);

  // commit changes to doc and update
  doc->Modify(true);
  ViewUpdateHint hint(ViewUpdateHint::HintType::kModelSpansEdit);
  doc->UpdateAllViews(nullptr, &hint);
}

void SpanTreeCtrl::DeleteSpans() {
  // deletes spans from model
  spans_->clear();

  // deletes spans from treectrl
  wxTreeItemId root = GetRootItem();
  DeleteChildren(root);

  // commit changes to doc and update
  view_->GetDocument()->Modify(true);
  ViewUpdateHint hint(ViewUpdateHint::HintType::kModelSpansEdit);
  view_->GetDocument()->UpdateAllViews(nullptr, &hint);
}

void SpanTreeCtrl::EditSpan(const wxTreeItemId& id) {
  // gets tree item data
  EditTreeItemData* item = (EditTreeItemData *)GetItemData(id);

  // searches document spans for one with matching description
  SpanAnalyzerDoc* doc = (SpanAnalyzerDoc*)view_->GetDocument();
  std::vector<Span>::iterator iter;
  Span* span = nullptr;
  for (iter = spans_->begin(); iter != spans_->end(); iter++) {
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
    doc->UpdateAllViews(nullptr, &hint);

    // updates treectrl name
    item->set_description(span->name);
    SetItemText(id, item->description());
  }
}

void SpanTreeCtrl::InitSpans() {
  // gets root item
  wxTreeItemId root = GetRootItem();

  // deletes all spans
  DeleteChildren(root);

  // adds spans root and all spans in document
  for (auto iter = spans_->cbegin(); iter != spans_->cend(); iter++) {
    const Span& span = *iter;
    wxTreeItemId item = AppendItem(root, span.name);
    EditTreeItemData* data = new EditTreeItemData(EditTreeItemData::Type::kSpan, span.name);
    SetItemData(item, data);
  }

  // expands root
  Expand(root);
}

void SpanTreeCtrl::OnContextMenuSelect(wxCommandEvent& event) {
  // gets selected tree item data
  wxTreeItemId id_item = GetSelection();
  if (id_item.IsOk() == false) {
    return;
  }

  // gets context menu selection and sends to handler function
  const int id_event = event.GetId();
  if (id_event == kTreeItemActivate) {
    ActivateSpan(id_item);
  } else if (id_event == kTreeItemCopy) {
    CopySpan(id_item);
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

void SpanTreeCtrl::OnItemActivate(wxTreeEvent& event) {
  // gets activated item from event
  wxTreeItemId id = event.GetItem();
  ActivateSpan(id);
}

void SpanTreeCtrl::OnItemMenu(wxTreeEvent& event) {
  // gets item from event
  wxTreeItemId id = event.GetItem();
  SetFocusedItem(id);

  // displays a context menu based on the item that was right clicked
  wxMenu menu;
  if (id == GetRootItem()) {
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

EditPane::EditPane(wxWindow* parent, wxView* view) {
  // loads dialog from virtual xrc file system
  wxXmlResource::Get()->LoadPanel(this, parent, "edit_pane");

  // saves view reference
  view_ = view;

  // creates weathercase treectrl
  // \todo format with sizers to fit panel area
  wxPanel* panel = nullptr;
  wxBoxSizer* sizer_horizontal = nullptr;
  wxBoxSizer* sizer_vertical = nullptr;

  panel = XRCCTRL(*this, "panel_weathercases", wxPanel);
  treectrl_weathercases_ = new WeathercaseTreeCtrl(panel, view_);
  sizer_horizontal = new wxBoxSizer(wxHORIZONTAL);
  sizer_vertical = new wxBoxSizer(wxVERTICAL);
  sizer_vertical->Add(treectrl_weathercases_, 1, wxEXPAND);
  sizer_horizontal->Add(sizer_vertical, 1, wxEXPAND);
  panel->SetSizer(sizer_horizontal);

  panel = XRCCTRL(*this, "panel_spans", wxPanel);
  treectrl_spans_ = new SpanTreeCtrl(panel, view_);
  sizer_horizontal = new wxBoxSizer(wxHORIZONTAL);
  sizer_vertical = new wxBoxSizer(wxVERTICAL);
  sizer_vertical->Add(treectrl_spans_, 1, wxEXPAND);
  sizer_horizontal->Add(sizer_vertical, 1, wxEXPAND);
  panel->SetSizer(sizer_horizontal);
}

EditPane::~EditPane() {
}

void EditPane::Update(wxObject* hint) {
  if (hint == nullptr) {
    treectrl_spans_->Update(hint);
    treectrl_weathercases_->Update(hint);
  }
}

Span* EditPane::ActivatedSpan() {
  // searches tree for bolded item
  wxTreeItemIdValue cookie;
  wxTreeItemId root = treectrl_spans_->GetRootItem();
  wxTreeItemId id = treectrl_spans_->GetFirstChild(root, cookie);
  while (id.IsOk() == true) {
    if (treectrl_spans_->IsBold(id) == true) {
      break;
    } else {
      id = treectrl_spans_->GetNextSibling(id);
    }
  }

  // if bold item was found
  if (id.IsOk()) {
    // gets data associated with id
    EditTreeItemData* item =
        (EditTreeItemData *)treectrl_spans_->GetItemData(id);

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
