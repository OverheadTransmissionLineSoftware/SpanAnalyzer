// This is free and unencumbered software released into the public domain.
// For more information, please refer to <http://unlicense.org/>

#include "edit_pane.h"

#include "wx/xrc/xmlres.h"

#include "cable_editor_dialog.h"
#include "span_analyzer_app.h"
#include "span_analyzer_doc.h"
#include "span_analyzer_view.h"
#include "span_editor_dialog.h"
#include "span_unit_converter.h"
#include "weather_load_case_editor_dialog.h"
#include "weather_load_case_unit_converter.h"

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
  EVT_TREE_BEGIN_DRAG(wxID_ANY, WeathercaseTreeCtrl::OnDragBegin)
  EVT_TREE_END_DRAG(wxID_ANY, WeathercaseTreeCtrl::OnDragEnd)
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

void WeathercaseTreeCtrl::Initialize() {
  // gets root item
  wxTreeItemId root = GetRootItem();

  // deletes all weathercase items in treectrl
  DeleteChildren(root);

  // adds items for all weathercases in document
  const std::list<WeatherLoadCase>& weathercases = doc_->weathercases();
  for (auto iter = weathercases.cbegin(); iter != weathercases.cend();
       iter++) {
    const WeatherLoadCase& weathercase = *iter;
    wxTreeItemId item = AppendItem(root, weathercase.description);
    WeathercaseTreeItemData* data = new WeathercaseTreeItemData();
    data->set_iter(iter);
    SetItemData(item, data);
  }

  // expands root
  Expand(root);
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
  auto iter = doc_->AppendWeathercase(weathercase);

  // adds to treectrl
  wxTreeItemId id = AppendItem(GetRootItem(), weathercase.description);
  WeathercaseTreeItemData* data = new WeathercaseTreeItemData();
  data->set_iter(iter);
  SetItemData(id, data);

  // updates views
  ViewUpdateHint hint;
  hint.set_type(ViewUpdateHint::HintType::kModelWeathercaseEdit);
  doc_->UpdateAllViews(nullptr, &hint);
}

void WeathercaseTreeCtrl::CopyWeathercase(const wxTreeItemId& id) {
  // gets tree item data
  WeathercaseTreeItemData* item = (WeathercaseTreeItemData*)GetItemData(id);

  // copies weathercase, modifies description
  WeatherLoadCase weathercase = *(item->iter());
  int i = 1;
  bool is_unique = false;
  while (is_unique == false) {
    // re-checks if it is unique
    is_unique = doc_->IsUniqueWeathercase(weathercase.description);
    if (is_unique == true) {
      break;
    } else {
      // clears previous description attempt and generates a new one
      /// \todo this needs some work
      weathercase.description.clear();
      weathercase.description = weathercase.description + " Copy"
                                + std::to_string(i);
      i++;
    }
  }

  // gets position and inserts to document
  auto position = std::next(item->iter());
  auto iter = doc_->InsertWeathercase(position, weathercase);

  // adds to treectrl
  wxTreeItemId id_item = InsertItem(GetRootItem(), id, weathercase.description);
  WeathercaseTreeItemData* data = new WeathercaseTreeItemData();
  data->set_iter(iter);
  SetItemData(id_item, data);

  // updates views
  ViewUpdateHint hint;
  hint.set_type(ViewUpdateHint::HintType::kModelWeathercaseEdit);
  doc_->UpdateAllViews(nullptr, &hint);
}

void WeathercaseTreeCtrl::DeleteWeathercase(const wxTreeItemId& id) {
  // gets tree item data
  WeathercaseTreeItemData* item = (WeathercaseTreeItemData *)GetItemData(id);

  // determines if weathercase is referenced, and if so, confirms with user
  // about deletion
  bool is_referenced = doc_->IsReferencedWeathercase(item->iter());
  if (is_referenced == true) {
    std::string message = "This weathercase is referenced by spans within the "
                          "project. Continue deletion?";
    wxMessageDialog dialog(view_->GetFrame(), message, "Weathercase Delete",
                           wxOK | wxCANCEL);
    if (dialog.ShowModal() != wxID_OK) {
      return;
    }
  }

  // erases from document and treectrl
  doc_->DeleteWeathercase(item->iter());
  Delete(id);

  // updates views
  ViewUpdateHint hint;
  hint.set_type(ViewUpdateHint::HintType::kModelWeathercaseEdit);
  doc_->UpdateAllViews(nullptr, &hint);
}

void WeathercaseTreeCtrl::DeleteWeathercases() {
  // gets list and size
  const std::list<WeatherLoadCase>& weathercases = doc_->weathercases();

  // deletes weathercases from doc
  for (auto iter = weathercases.cbegin(); iter != weathercases.cend();
       iter++) {
    doc_->DeleteWeathercase(iter);
  }

  // deletes weathercases from treectrl
  wxTreeItemId root = GetRootItem();
  DeleteChildren(root);

  // updates views
  ViewUpdateHint hint;
  hint.set_type(ViewUpdateHint::HintType::kModelWeathercaseEdit);
  view_->GetDocument()->UpdateAllViews(nullptr, &hint);
}

void WeathercaseTreeCtrl::EditWeathercase(const wxTreeItemId& id) {
  // gets tree item data and copies
  WeathercaseTreeItemData* item = (WeathercaseTreeItemData*)GetItemData(id);
  WeatherLoadCase weathercase = *(item->iter());

  // converts to different unit style
  WeatherLoadCaseUnitConverter::ConvertUnitStyle(wxGetApp().config()->units,
                                                 units::UnitStyle::kConsistent,
                                                 units::UnitStyle::kDifferent,
                                                 weathercase);

  // shows editor
  auto iter = item->iter();
  const int status = ShowEditor(weathercase, &iter);
  if (status == wxID_OK) {
    // converts to consistent unit style
    WeatherLoadCaseUnitConverter::ConvertUnitStyle(wxGetApp().config()->units,
                                                   units::UnitStyle::kDifferent,
                                                   units::UnitStyle::kConsistent,
                                                   weathercase);

    // updates document
    doc_->ReplaceWeathercase(item->iter(), weathercase);

    // updates treectrl item name
    SetItemText(id, weathercase.description);

    // updates views
    ViewUpdateHint hint;
    hint.set_type(ViewUpdateHint::HintType::kModelWeathercaseEdit);
    doc_->UpdateAllViews(nullptr, &hint);
  }
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

void WeathercaseTreeCtrl::OnDragBegin(wxTreeEvent& event) {
  // checks if weathercase is dragged, not the root
  if (event.GetItem() == GetRootItem()) {
    return;
  }

  // stores reference to dragged item and allows event
  item_dragged_ = event.GetItem();
  event.Allow();
}

void WeathercaseTreeCtrl::OnDragEnd(wxTreeEvent& event) {
  // verifies that the end drag is valid
  if ((event.GetItem().IsOk() == false)
      ||(event.GetItem() == GetRootItem())
      || (event.GetItem() == item_dragged_)) {
    item_dragged_ = (wxTreeItemId)0l;
    return;
  }

  // identifies source and destination
  wxTreeItemId item_source = item_dragged_;
  wxTreeItemId item_destination = event.GetItem();

  item_dragged_ = (wxTreeItemId)0l;

  // gets data for source and destination items
  WeathercaseTreeItemData* data_source =
      (WeathercaseTreeItemData*)GetItemData(item_source);
  std::list<WeatherLoadCase>::const_iterator iter_source = data_source->iter();

  WeathercaseTreeItemData* data_destination =
      (WeathercaseTreeItemData*)GetItemData(item_destination);
  std::list<WeatherLoadCase>::const_iterator iter_destination =
      data_destination->iter();
  std::advance(iter_destination, 1);

  // modifies document
  doc_->MoveWeathercase(iter_source, iter_destination);

  // modifies treectrl
  wxString desc = GetItemText(item_source);
  std::list<WeatherLoadCase>::const_iterator iter = data_source->iter();
  Delete(item_source);
  item_source = InsertItem(GetRootItem(), item_destination, desc);
  WeathercaseTreeItemData* data = new WeathercaseTreeItemData();
  data->set_iter(iter);
  SetItemData(item_source, data);

  // updates views
  ViewUpdateHint hint;
  hint.set_type(ViewUpdateHint::HintType::kModelWeathercaseEdit);
  doc_->UpdateAllViews(nullptr, &hint);
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

int WeathercaseTreeCtrl::ShowEditor(
    WeatherLoadCase& weathercase,
    const std::list<WeatherLoadCase>::const_iterator* skip) {
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
    is_unique = doc->IsUniqueWeathercase(weathercase.description, skip);
    if (is_unique == false) {
      std::string message = "Weathercase description is a duplicate. Please "
                            "provide another description.";
      wxMessageBox(message);
    }
  }

  return status;
}

BEGIN_EVENT_TABLE(SpanTreeCtrl, wxTreeCtrl)
  EVT_MENU(wxID_ANY, SpanTreeCtrl::OnContextMenuSelect)
  EVT_TREE_BEGIN_DRAG(wxID_ANY, SpanTreeCtrl::OnDragBegin)
  EVT_TREE_END_DRAG(wxID_ANY, SpanTreeCtrl::OnDragEnd)
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

void SpanTreeCtrl::Initialize() {
  // gets root item
  wxTreeItemId root = GetRootItem();

  // deletes all children items
  DeleteChildren(root);

  // adds items for all spans in document
  const std::list<Span>& spans = doc_->spans();
  for (auto iter = spans.cbegin(); iter != spans.cend(); iter++) {
    const Span& span = *iter;
    wxTreeItemId item = AppendItem(root, span.name);
    SpanTreeItemData* data = new SpanTreeItemData();
    data->set_iter(iter);
    SetItemData(item, data);
  }

  // expands root
  Expand(root);
}

void SpanTreeCtrl::ActivateSpan(const wxTreeItemId& id) {
  if (id.IsOk() == false) {
    return;
  }

  if (id == GetRootItem()) {
    return;
  }

  if (id == item_activated_) {
    return;
  }

  // unbolds previous item
  if (item_activated_.IsOk()) {
    SetItemBold(item_activated_, false);
  }

  // bolds new item and caches
  SetItemBold(id, true);
  item_activated_ = id;

  // caches span in view
  SpanAnalyzerView* view = (SpanAnalyzerView*)view_;
  SpanTreeItemData* data = (SpanTreeItemData*)GetItemData(item_activated_);
  auto iter = data->iter();
  view->set_span(&(*iter));

  // updates views
  ViewUpdateHint hint;
  hint.set_type(ViewUpdateHint::HintType::kModelSpansEdit);
  view_->GetDocument()->UpdateAllViews(nullptr, &hint);
}

void SpanTreeCtrl::AddSpan() {
  // creates new span and editor
  Span span;
  span.name = "NEW";

  // gets referenced objects and makes sure that they exist
  const std::list<CableFile>& cablefiles = wxGetApp().data()->cablefiles;
  if (cablefiles.empty() == true) {
    wxMessageBox("No cables are currently loaded. Add at least one before "
                 "adding span.");
    return;
  }

  const std::list<WeatherLoadCase>& weathercases = doc_->weathercases();
  if (weathercases.empty() == true) {
    wxMessageBox("No weathercases exist in document. Add at least one before "
                 "adding span.");
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
  auto iter = doc_->AppendSpan(span);

  // adds to treectrl
  wxTreeItemId id = AppendItem(GetRootItem(), span.name);
  SpanTreeItemData* data = new SpanTreeItemData();
  data->set_iter(iter);
  SetItemData(id, data);
}

void SpanTreeCtrl::CopySpan(const wxTreeItemId& id) {
  // gets tree item data
  SpanTreeItemData* item = (SpanTreeItemData*)GetItemData(id);

  // copies span
  Span span = *(item->iter());

  // gets position and inserts to document
  auto position = std::next(item->iter());
  auto iter = doc_->InsertSpan(position, span);

  // adds to treectrl
  wxTreeItemId id_item = InsertItem(GetRootItem(), id, span.name);
  SpanTreeItemData* data = new SpanTreeItemData();
  data->set_iter(iter);
  SetItemData(id_item, data);
}

void SpanTreeCtrl::DeleteSpan(const wxTreeItemId& id) {
  // gets tree item data
  SpanTreeItemData* item = (SpanTreeItemData*)GetItemData(id);

  // erases from document and treectrl
  doc_->DeleteSpan(item->iter());

  if (item_activated_ == id) {
    item_activated_ = (wxTreeItemId)0l;
  }

  Delete(id);

  // updates view
  ViewUpdateHint hint;
  hint.set_type(ViewUpdateHint::HintType::kModelSpansEdit);
  doc_->UpdateAllViews(nullptr, &hint);
}

void SpanTreeCtrl::DeleteSpans() {
  // gets list and size
  const std::list<Span>& spans = doc_->spans();

  // deletes spans from doc
  for (auto iter = spans.cbegin(); iter != spans.cend(); iter++) {
    doc_->DeleteSpan(iter);
  }

  // deletes spans from treectrl
  wxTreeItemId root = GetRootItem();
  DeleteChildren(root);

  // updates view
  ViewUpdateHint hint;
  hint.set_type(ViewUpdateHint::HintType::kModelSpansEdit);
  doc_->UpdateAllViews(nullptr, &hint);
}

void SpanTreeCtrl::EditSpan(const wxTreeItemId& id) {
  // gets tree item data and copies span
  SpanTreeItemData* item = (SpanTreeItemData*)GetItemData(id);
  Span span = *(item->iter());

  // converts span to different unit style
  SpanUnitConverter::ConvertUnitStyle(wxGetApp().config()->units,
                                      units::UnitStyle::kConsistent,
                                      units::UnitStyle::kDifferent,
                                      span);

  // gets referenced objects and makes sure that they exist
  const std::list<CableFile>& cablefiles = wxGetApp().data()->cablefiles;
  if (cablefiles.empty() == true) {
    wxMessageBox("No cables are currently loaded. Add at least one before "
                 "editing span.");
    return;
  }

  const std::list<WeatherLoadCase>& weathercases = doc_->weathercases();
  if (weathercases.empty() == true) {
    wxMessageBox("No weathercases exist in document. Add at least one before "
                 "editing span.");
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
    doc_->ReplaceSpan(item->iter(), span);

    // updates treectrl name
    SetItemText(id, span.name);

    // updates view
    ViewUpdateHint hint;
    hint.set_type(ViewUpdateHint::HintType::kModelSpansEdit);
    doc_->UpdateAllViews(nullptr, &hint);
  }
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

void SpanTreeCtrl::OnDragBegin(wxTreeEvent& event) {
  // checks if weathercase is dragged, not the root
  if (event.GetItem() == GetRootItem()) {
    return;
  }

  // stores reference to dragged item and allows event
  item_dragged_ = event.GetItem();
  event.Allow();
}

void SpanTreeCtrl::OnDragEnd(wxTreeEvent& event) {
  // verifies that the end drag is valid
  if ((event.GetItem().IsOk() == false)
      ||(event.GetItem() == GetRootItem())
      || (event.GetItem() == item_dragged_)) {
    item_dragged_ = (wxTreeItemId)0l;
    return;
  }

  // identifies source and destination
  wxTreeItemId item_source = item_dragged_;
  wxTreeItemId item_destination = event.GetItem();

  item_dragged_ = (wxTreeItemId)0l;

  // gets data for source and destination items
  SpanTreeItemData* data_source = (SpanTreeItemData*)GetItemData(item_source);
  std::list<Span>::const_iterator iter_source = data_source->iter();

  SpanTreeItemData* data_destination =
      (SpanTreeItemData*)GetItemData(item_destination);
  std::list<Span>::const_iterator iter_destination = data_destination->iter();
  std::advance(iter_destination, 1);

  // modifies document
  doc_->MoveSpan(iter_source, iter_destination);

  // modifies treectrl
  wxString desc = GetItemText(item_source);
  std::list<Span>::const_iterator iter = data_source->iter();

  bool is_activated_item = false;
  if (item_activated_ == item_source) {
    is_activated_item = true;
  }

  Delete(item_source);
  item_source = InsertItem(GetRootItem(), item_destination, desc);
  SpanTreeItemData* data = new SpanTreeItemData();
  data->set_iter(iter);
  SetItemData(item_source, data);

  if (is_activated_item == true) {
    item_activated_ = item_source;
    SetItemBold(item_source);
  }

  // updates views
  ViewUpdateHint hint;
  hint.set_type(ViewUpdateHint::HintType::kModelSpansEdit);
  doc_->UpdateAllViews(nullptr, &hint);
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
    treectrl_spans_->Initialize();
    treectrl_weathercases_->Initialize();
  }
}
