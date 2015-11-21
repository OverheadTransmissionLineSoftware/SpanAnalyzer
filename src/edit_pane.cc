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

  // gets document
  doc_ = (SpanAnalyzerDoc*)view_->GetDocument();

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

  // allows user to edit weathercase
  if (ShowEditor(weathercase) != wxID_OK) {
    return;
  }

  // converts to consistent units
  WeatherLoadCaseUnitConverter::ConvertUnitStyle(wxGetApp().config()->units,
                                                 units::UnitStyle::kDifferent,
                                                 units::UnitStyle::kConsistent,
                                                 weathercase);

  // adds to document
  doc_->AppendWeathercase(weathercase);

  // adds to treectrl
  wxTreeItemId id = AppendItem(GetRootItem(), weathercase.description);
  EditTreeItemData* data = new EditTreeItemData(
      EditTreeItemData::Type::kWeathercase, weathercase.description);
  SetItemData(id, data);

  // updates views
  ViewUpdateHint hint(ViewUpdateHint::HintType::kModelWeathercaseEdit);
  doc_->UpdateAllViews(nullptr, &hint);
}

void WeathercaseTreeCtrl::CopyWeathercase(const wxTreeItemId& id) {
  // gets tree item data
  EditTreeItemData* item = (EditTreeItemData *)GetItemData(id);

  // searches document weathercases for matching description
  const std::vector<WeatherLoadCase>& weathercases = doc_->weathercases();
  std::vector<WeatherLoadCase>::const_iterator iter;
  for (iter = weathercases.cbegin(); iter != weathercases.cend(); iter++) {
    const WeatherLoadCase& weathercase = *iter;
    if (weathercase.description == item->description()) {
      break;
    }
  }

  // copies weathercase, modifies description
  WeatherLoadCase weathercase = *iter;
  std::string description = weathercase.description;
  int i = 1;
  bool is_unique = false;
  while (is_unique == false) {
    // re-checks if it is unique
    is_unique = doc_->IsUniqueWeathercase(description, -1);
    if (is_unique == true) {
      // sets weathercase description
      weathercase.description = description;
      break;
    } else {
      // clears previous description attempt and generates a new one
      description.clear();
      description = weathercase.description + " Copy" + std::to_string(i);
      i++;
    }
  }

  // adds to document
  const unsigned int index = (iter - weathercases.cbegin()) + 1;
  doc_->InsertWeathercase(index, weathercase);

  // adds to treectrl
  wxTreeItemId id_item = InsertItem(GetRootItem(), id, weathercase.description);
  EditTreeItemData* data = new EditTreeItemData(
      EditTreeItemData::Type::kWeathercase, weathercase.description);
  SetItemData(id_item, data);

  // updates views
  ViewUpdateHint hint(ViewUpdateHint::HintType::kModelWeathercaseEdit);
  doc_->UpdateAllViews(nullptr, &hint);
}

void WeathercaseTreeCtrl::DeleteWeathercase(const wxTreeItemId& id) {
  // gets tree item data
  EditTreeItemData* item = (EditTreeItemData *)GetItemData(id);

  // searches document weathercases for matching description
  const std::vector<WeatherLoadCase>& weathercases = doc_->weathercases();
  std::vector<WeatherLoadCase>::const_iterator iter;
  for (iter = weathercases.cbegin(); iter != weathercases.cend(); iter++) {
    const WeatherLoadCase& weathercase = *iter;
    if (weathercase.description == item->description()) {
      break;
    }
  }

  // erases from document and treectrl
  const unsigned int index = iter - weathercases.cbegin();
  doc_->DeleteWeathercase(index);
  Delete(id);

  // updates views
  ViewUpdateHint hint(ViewUpdateHint::HintType::kModelWeathercaseEdit);
  doc_->UpdateAllViews(nullptr, &hint);
}

void WeathercaseTreeCtrl::DeleteWeathercases() {
  // gets vector and size
  const std::vector<WeatherLoadCase>& weathercases = doc_->weathercases();
  const unsigned int count = weathercases.size();

  // deletes weathercases from doc
  for (unsigned int index = 0; index < count; index++) {
    doc_->DeleteWeathercase(0);
  }

  // deletes weathercases from treectrl
  wxTreeItemId root = GetRootItem();
  DeleteChildren(root);

  // updates views
  ViewUpdateHint hint(ViewUpdateHint::HintType::kModelWeathercaseEdit);
  view_->GetDocument()->UpdateAllViews(nullptr, &hint);
}

void WeathercaseTreeCtrl::EditWeathercase(const wxTreeItemId& id) {
  // gets tree item data
  EditTreeItemData* item = (EditTreeItemData *)GetItemData(id);

  // searches document spans for one with matching description
  const std::vector<WeatherLoadCase>& weathercases = doc_->weathercases();
  std::vector<WeatherLoadCase>::const_iterator iter;
  WeatherLoadCase weathercase;
  for (iter = weathercases.cbegin(); iter != weathercases.cend(); iter++) {
    weathercase = *iter;
    if (weathercase.description == item->description()) {
      break;
    }
  }

  if (iter == weathercases.cend()) {
    return;
  }

  // converts to different unit style
  WeatherLoadCaseUnitConverter::ConvertUnitStyle(wxGetApp().config()->units,
                                                 units::UnitStyle::kConsistent,
                                                 units::UnitStyle::kDifferent,
                                                 weathercase);

  // shows editor
  const int index = iter - weathercases.cbegin();
  const int status = ShowEditor(weathercase, index);

  // converts to consistent unit style
  WeatherLoadCaseUnitConverter::ConvertUnitStyle(wxGetApp().config()->units,
                                                 units::UnitStyle::kDifferent,
                                                 units::UnitStyle::kConsistent,
                                                 weathercase);

  // if user accepts changes when editing
  if (status == wxID_OK) {
    // updates document
    doc_->ReplaceWeathercase(index, weathercase);

    // updates treectrl item name
    item->set_description(weathercase.description);
    SetItemText(id, item->description());

    // updates views
    ViewUpdateHint hint(ViewUpdateHint::HintType::kModelWeathercaseEdit);
    doc_->UpdateAllViews(nullptr, &hint);
  }
}

void WeathercaseTreeCtrl::InitWeathercases() {
  // gets root item
  wxTreeItemId root = GetRootItem();

  // deletes all weathercase items in treectrl
  DeleteChildren(root);

  // adds items for all weathercases in document
  const std::vector<WeatherLoadCase>& weathercases = doc_->weathercases();
  for (auto iter = weathercases.cbegin(); iter != weathercases.cend();
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
    menu.Append(kTreeRootAdd, "Add Weathercase");
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

int WeathercaseTreeCtrl::ShowEditor(WeatherLoadCase& weathercase,
                                    const int& index_skip) {
  int status = 0;
  WeatherLoadCaseEditorDialog dialog_edit(view_->GetFrame(),
                                          &weathercase,
                                          wxGetApp().config()->units);

  // requires that weathercase description is unique
  SpanAnalyzerDoc* doc = (SpanAnalyzerDoc*)view_->GetDocument();
  bool is_unique = false;
  while (is_unique == false) {
    // shows form, but exits function if user cancels
    status = dialog_edit.ShowModal();
    if (status != wxID_OK) {
      return status;
    }

    // re-checks if name is unique
    is_unique = doc->IsUniqueWeathercase(weathercase.description, index_skip);
    if (is_unique == false) {
      std::string message = "Weathercase description is a duplicate. Please "
                            "provide another description.";
      wxMessageDialog dialog_message(view_->GetFrame(), message);
      dialog_message.ShowModal();
    }
  }

  return status;
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

  // gets the document
  doc_ = (SpanAnalyzerDoc*)view_->GetDocument();

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
  // creates new span and editor
  Span span;
  span.name = "NEW";

  SpanEditorDialog dialog(view_->GetFrame(),
                          &wxGetApp().data()->cables,
                          &doc_->weathercases(),
                          units::UnitSystem::kImperial,
                          &span);
  if (dialog.ShowModal() != wxID_OK) {
    return;
  }

  // adds to document
  doc_->AppendSpan(span);

  // adds to treectrl
  wxTreeItemId id = AppendItem(GetRootItem(), span.name);
  EditTreeItemData* data = new EditTreeItemData(EditTreeItemData::Type::kSpan,
                                                span.name);
  SetItemData(id, data);
}

void SpanTreeCtrl::CopySpan(const wxTreeItemId& id) {
  // gets tree item data
  EditTreeItemData* item = (EditTreeItemData *)GetItemData(id);

  // gets selected span
  // searches document spans for one with matching description
  const std::vector<Span>& spans = doc_->spans();
  std::vector<Span>::const_iterator iter;
  for (iter = spans.cbegin(); iter != spans.cend(); iter++) {
    const Span& span = *iter;
    if (span.name == item->description()) {
      break;
    }
  }

  // copies span
  Span span = *iter;

  // adds to document
  const unsigned int index = iter - spans.cbegin();
  if (index == spans.size()) {
    doc_->AppendSpan(span);
  } else {
    doc_->InsertSpan(index, span);
  }

  // adds to treectrl
  wxTreeItemId id_item = InsertItem(GetRootItem(), id, span.name);
  EditTreeItemData* data = new EditTreeItemData(EditTreeItemData::Type::kSpan,
                                                span.name);
  SetItemData(id_item, data);
}

void SpanTreeCtrl::DeleteSpan(const wxTreeItemId& id) {
  // gets tree item data
  EditTreeItemData* item = (EditTreeItemData *)GetItemData(id);

  // searches document spans for one with matching description
  const std::vector<Span>& spans = doc_->spans();
  std::vector<Span>::const_iterator iter;
  for (iter = spans.cbegin(); iter != spans.cend(); iter++) {
    const Span& span = *iter;
    if (span.name == item->description()) {
      break;
    }
  }

  // erases from document and treectrl
  const unsigned int index = iter - spans.cbegin();
  doc_->DeleteSpan(index);
  Delete(id);

  // updates views
  ViewUpdateHint hint(ViewUpdateHint::HintType::kModelSpansEdit);
  doc_->UpdateAllViews(nullptr, &hint);
}

void SpanTreeCtrl::DeleteSpans() {
  // gets vector and size
  const std::vector<Span>& spans = doc_->spans();
  const unsigned int count = spans.size();

  // deletes spans from doc
  for (unsigned int index = 0; index < count; index++) {
    doc_->DeleteSpan(0);
  }

  // deletes spans from treectrl
  wxTreeItemId root = GetRootItem();
  DeleteChildren(root);

  // updates view
  ViewUpdateHint hint(ViewUpdateHint::HintType::kModelSpansEdit);
  doc_->UpdateAllViews(nullptr, &hint);
}

void SpanTreeCtrl::EditSpan(const wxTreeItemId& id) {
  // gets tree item data
  EditTreeItemData* item = (EditTreeItemData *)GetItemData(id);

  // searches document spans for one with matching description and copies
  const std::vector<Span>& spans = doc_->spans();
  std::vector<Span>::const_iterator iter;
  Span span;
  for (iter = spans.cbegin(); iter != spans.cend(); iter++) {
    span = *iter;
    if (span.name == item->description()) {
      break;
    }
  }

  if (iter == spans.cend()) {
    return;
  }

  // creates a span editor dialog
  SpanEditorDialog dialog(view_->GetFrame(),
                          &wxGetApp().data()->cables,
                          &doc_->weathercases(),
                          units::UnitSystem::kImperial,
                          &span);
  if (dialog.ShowModal() == wxID_OK) {
    // updates treectrl name
    item->set_description(span.name);
    SetItemText(id, item->description());

    // updates view
    ViewUpdateHint hint(ViewUpdateHint::HintType::kModelSpansEdit);
    doc_->UpdateAllViews(nullptr, &hint);
  }
}

void SpanTreeCtrl::InitSpans() {
  // gets root item
  wxTreeItemId root = GetRootItem();

  // deletes all children items
  DeleteChildren(root);

  // adds itmes for all spans in document
  const std::vector<Span>& spans = doc_->spans();
  for (auto iter = spans.cbegin(); iter != spans.cend(); iter++) {
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

const Span* EditPane::ActivatedSpan() {
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
    const std::vector<Span>& spans = doc->spans();
    std::vector<Span>::const_iterator iter;
    for (iter = spans.cbegin(); iter != spans.cend(); iter++) {
      const Span& span = *iter;
      if (span.name == item->description()) {
        break;
      }
    }

    // returns value based on whether span was found
    if (iter != spans.cend()) {
      return &*iter;
    } else {
      return nullptr;
    }
  } else {
    return nullptr;
  }
}
