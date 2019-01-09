// This is free and unencumbered software released into the public domain.
// For more information, please refer to <http://unlicense.org/>

#include "spananalyzer/edit_pane.h"

#include <sstream>

#include "wx/xrc/xmlres.h"

#include "spananalyzer/span_analyzer_app.h"
#include "spananalyzer/span_analyzer_doc.h"
#include "spananalyzer/span_analyzer_doc_commands.h"
#include "spananalyzer/span_editor_dialog.h"
#include "spananalyzer/span_unit_converter.h"
#include "xpm/copy.xpm"
#include "xpm/minus.xpm"
#include "xpm/move_arrow_down.xpm"
#include "xpm/move_arrow_up.xpm"
#include "xpm/plus.xpm"
#include "xpm/wrench.xpm"

/// context menu enum
enum {
  kTreeItemActivate = 0,
  kTreeItemCopy = 1,
  kTreeItemDeactivate = 2,
  kTreeItemDelete = 3,
  kTreeItemEdit = 4,
  kTreeItemMoveDown = 5,
  kTreeItemMoveUp = 6,
  kTreeRootAdd = 7,
};

BEGIN_EVENT_TABLE(EditPane, wxPanel)
  EVT_BUTTON(XRCID("button_add"), EditPane::OnButtonAdd)
  EVT_BUTTON(XRCID("button_copy"), EditPane::OnButtonCopy)
  EVT_BUTTON(XRCID("button_delete"), EditPane::OnButtonDelete)
  EVT_BUTTON(XRCID("button_edit"), EditPane::OnButtonEdit)
  EVT_BUTTON(XRCID("button_move_down"), EditPane::OnButtonMoveDown)
  EVT_BUTTON(XRCID("button_move_up"), EditPane::OnButtonMoveUp)
  EVT_MENU(wxID_ANY, EditPane::OnContextMenuSelect)
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

  // creates an imagelist for the button bitmaps
  wxImageList images(32, 32, true);
  images.Add(wxBitmap(plus_xpm), wxColour(255, 255, 255));
  images.Add(wxBitmap(minus_xpm), wxColour(255, 255, 255));
  images.Add(wxBitmap(copy_xpm), wxColour(255, 255, 255));
  images.Add(wxBitmap(wrench_xpm), wxColour(255, 255, 255));
  images.Add(wxBitmap(move_arrow_up_xpm), wxColour(255, 255, 255));
  images.Add(wxBitmap(move_arrow_down_xpm), wxColour(255, 255, 255));

  // assigns bitmaps to the buttons
  wxButton* button = nullptr;
  button = XRCCTRL(*this, "button_add", wxButton);
  button->SetBitmap(images.GetBitmap(0));

  button = XRCCTRL(*this, "button_delete", wxButton);
  button->SetBitmap(images.GetBitmap(1));

  button = XRCCTRL(*this, "button_copy", wxButton);
  button->SetBitmap(images.GetBitmap(2));

  button = XRCCTRL(*this, "button_edit", wxButton);
  button->SetBitmap(images.GetBitmap(3));

  button = XRCCTRL(*this, "button_move_up", wxButton);
  button->SetBitmap(images.GetBitmap(4));

  button = XRCCTRL(*this, "button_move_down", wxButton);
  button->SetBitmap(images.GetBitmap(5));
}

EditPane::~EditPane() {
}

void EditPane::Update(wxObject* hint) {
  // interprets hint
  const UpdateHint* hint_update = dynamic_cast<UpdateHint*>(hint);
  if (hint_update == nullptr) {
    InitializeTreeCtrl();
  } else if (hint_update->type()
      == UpdateHint::Type::kAnalysisFilterGroupEdit) {
    // do nothing
  } else if (hint_update->type()
      == UpdateHint::Type::kAnalysisFilterGroupSelect) {
    // do nothing
  } else if (hint_update->type() == UpdateHint::Type::kAnalysisFilterSelect) {
    // do nothing
  } else if (hint_update->type() == UpdateHint::Type::kCablesEdit) {
    // do nothing
  } else if (hint_update->type() == UpdateHint::Type::kPreferencesEdit) {
    // do nothing
  } else if (hint_update->type() == UpdateHint::Type::kSpansEdit) {
    UpdateTreeCtrlSpanItems();
  } else if (hint_update->type() == UpdateHint::Type::kWeathercasesEdit) {
    // do nothing
  }
}

void EditPane::ActivateSpan(const wxTreeItemId& id) {
  if (id.IsOk() == false) {
    return;
  }

  if (id == treectrl_->GetRootItem()) {
    return;
  }

  wxLogVerbose("Activating span.");

  // updates document
  SpanAnalyzerDoc* doc = dynamic_cast<SpanAnalyzerDoc*>(view_->GetDocument());
  SpanTreeItemData* data =
      dynamic_cast<SpanTreeItemData*>(treectrl_->GetItemData(id));
  auto iter = data->iter();
  const int index = doc->IndexSpan(&(*iter));
  doc->set_index_activated(index);

  // posts a view update
  UpdateHint hint(UpdateHint::Type::kSpansEdit);
  doc->UpdateAllViews(nullptr, &hint);

  // updates treectrl focus
  FocusTreeCtrlSpanItem(index);
}

void EditPane::AddSpan() {
  // creates new span and editor
  // initializes values to zero
  Span span;
  span.name = NameVersioned("New");

  CableConstraint constraint;
  constraint.limit = 0;
  span.linecable.set_constraint(constraint);

  span.linecable.set_spacing_attachments_ruling_span(Vector3d(0, 0, 0));
  span.spacing_attachments = Vector3d(0, 0, 0);

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

  wxLogVerbose("Adding span.");

  // updates document
  SpanAnalyzerDoc* doc = dynamic_cast<SpanAnalyzerDoc*>(view_->GetDocument());

  SpanCommand* command = new SpanCommand(SpanCommand::kNameInsert);
  command->set_index(doc->spans().size());

  wxXmlNode* node = SpanCommand::SaveSpanToXml(span);
  command->set_node_span(node);

  doc->GetCommandProcessor()->Submit(command);

  // adjusts treectrl focus
  FocusTreeCtrlSpanItem(command->index());
}

void EditPane::CopySpan(const wxTreeItemId& id) {
  // gets tree item data
  SpanTreeItemData* data =
      dynamic_cast<SpanTreeItemData*>(treectrl_->GetItemData(id));

  // copies span and updates name
  Span span = *(data->iter());
  span.name = NameVersioned(span.name);

  wxLogVerbose("Copying span.");

  // updates document
  SpanAnalyzerDoc* doc = dynamic_cast<SpanAnalyzerDoc*>(view_->GetDocument());

  SpanCommand* command = new SpanCommand(SpanCommand::kNameInsert);
  const int index = std::distance(doc->spans().cbegin(), data->iter()) + 1;
  command->set_index(index);

  wxXmlNode* node = SpanCommand::SaveSpanToXml(span);
  command->set_node_span(node);

  doc->GetCommandProcessor()->Submit(command);

  // adjusts treectrl focus
  FocusTreeCtrlSpanItem(command->index());
}

void EditPane::DeactivateSpan(const wxTreeItemId& id) {
  wxLogVerbose("Deactivating span.");

  // gets iterator to span
  SpanTreeItemData* data =
      dynamic_cast<SpanTreeItemData*>(treectrl_->GetItemData(id));
  auto iter = data->iter();

  // updates document
  SpanAnalyzerDoc* doc = dynamic_cast<SpanAnalyzerDoc*>(view_->GetDocument());
  doc->set_index_activated(-1);

  // posts a view update
  UpdateHint hint(UpdateHint::Type::kSpansEdit);
  doc->UpdateAllViews(nullptr, &hint);

  // updates treectrl focus
  const int index = std::distance(doc->spans().cbegin(), iter);
  FocusTreeCtrlSpanItem(index);
}

void EditPane::DeleteSpan(const wxTreeItemId& id) {
  // gets tree item data
  SpanTreeItemData* data =
      dynamic_cast<SpanTreeItemData*>(treectrl_->GetItemData(id));

  wxLogVerbose("Deleting span.");

  // updates document
  SpanAnalyzerDoc* doc = dynamic_cast<SpanAnalyzerDoc*>(view_->GetDocument());
  const int index = std::distance(doc->spans().cbegin(), data->iter());

  SpanCommand* command = new SpanCommand(SpanCommand::kNameDelete);
  command->set_index(index);

  doc->GetCommandProcessor()->Submit(command);

  // adjusts treectrl focus
  FocusTreeCtrlSpanItem(command->index() - 1);
}

void EditPane::EditSpan(const wxTreeItemId& id) {
  // gets tree item data and copies span
  SpanTreeItemData* data =
      dynamic_cast<SpanTreeItemData*>(treectrl_->GetItemData(id));
  Span span = *(data->iter());

  // converts span to different unit style
  SpanUnitConverter::ConvertUnitStyle(wxGetApp().config()->units,
                                      units::UnitStyle::kConsistent,
                                      units::UnitStyle::kDifferent,
                                      true,
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
  if (dialog.ShowModal() != wxID_OK) {
    return;
  }

  wxLogVerbose("Editing span.");

  // converts span to consistent unit style
  SpanUnitConverter::ConvertUnitStyle(wxGetApp().config()->units,
                                      units::UnitStyle::kDifferent,
                                      units::UnitStyle::kConsistent,
                                      true,
                                      span);

  // updates document
  SpanAnalyzerDoc* doc = dynamic_cast<SpanAnalyzerDoc*>(view_->GetDocument());

  SpanCommand* command = new SpanCommand(SpanCommand::kNameModify);
  const int index = std::distance(doc->spans().cbegin(), data->iter());
  command->set_index(index);

  wxXmlNode* node = SpanCommand::SaveSpanToXml(span);
  command->set_node_span(node);

  doc->GetCommandProcessor()->Submit(command);

  // adjusts treectrl focus
  FocusTreeCtrlSpanItem(command->index());
}

void EditPane::FocusTreeCtrlSpanItem(const int& index) {
  // gets the treectrl item
  wxTreeItemId id;
  wxTreeItemIdValue cookie;
  for (auto i = 0; i <= index; i++) {
    if (i == 0) {
      id = treectrl_->GetFirstChild(treectrl_->GetRootItem(), cookie);
    } else {
      id = treectrl_->GetNextSibling(id);
    }
  }

  if (id.IsOk() == true) {
    // sets focus
    treectrl_->SetFocusedItem(id);

    // sets selection
    treectrl_->UnselectAll();
    treectrl_->SelectItem(id);

    // sets application focus
    treectrl_->SetFocus();
  }
}

void EditPane::InitializeTreeCtrl() {
  // gets root
  wxTreeItemId root = treectrl_->GetRootItem();

  // adds items for all spans in document
  UpdateTreeCtrlSpanItems();

  // expands to show all spans
  treectrl_->Expand(root);
}

void EditPane::MoveSpanDown(const wxTreeItemId& id) {
  // checks to make sure item isn't the last one
  wxTreeItemId id_next = treectrl_->GetNextSibling(id);
  if (id_next.IsOk() == false) {
    return;
  }

  // gets tree item data
  SpanTreeItemData* data =
      dynamic_cast<SpanTreeItemData*>(treectrl_->GetItemData(id));

  wxLogVerbose("Moving spans.");

  // updates document
  SpanAnalyzerDoc* doc = dynamic_cast<SpanAnalyzerDoc*>(view_->GetDocument());
  const int index = std::distance(doc->spans().cbegin(), data->iter());

  SpanCommand* command = new SpanCommand(SpanCommand::kNameMoveDown);
  command->set_index(index);

  doc->GetCommandProcessor()->Submit(command);

  // adjusts treectrl focus
  FocusTreeCtrlSpanItem(command->index());
}

void EditPane::MoveSpanUp(const wxTreeItemId& id) {
  // checks to make sure item isn't the first one
  wxTreeItemId id_prev = treectrl_->GetPrevSibling(id);
  if (id_prev.IsOk() == false) {
    return;
  }

  // gets tree item data
  SpanTreeItemData* data =
      dynamic_cast<SpanTreeItemData*>(treectrl_->GetItemData(id));

  wxLogVerbose("Moving spans.");

  // updates document
  SpanAnalyzerDoc* doc = dynamic_cast<SpanAnalyzerDoc*>(view_->GetDocument());
  const int index = std::distance(doc->spans().cbegin(), data->iter());

  SpanCommand* command = new SpanCommand(SpanCommand::kNameMoveUp);
  command->set_index(index);

  doc->GetCommandProcessor()->Submit(command);

  // adjusts treectrl focus
  FocusTreeCtrlSpanItem(command->index());
}

std::string EditPane::NameVersioned(const std::string& name) const {
  // gets document
  SpanAnalyzerDoc* doc = dynamic_cast<SpanAnalyzerDoc*>(view_->GetDocument());

  // determines is existing name is versioned
  const int pos_end = name.rfind(")", name.size());
  const int pos_start = name.rfind("(", pos_end);

  bool is_versioned = true;
  int version = 1;
  const int kSize = name.size();
  if (pos_end != kSize - 1) {
    // the last character is not a version closing
    is_versioned = false;
  } else if ((name.at(pos_start) == '(') && (name.at(pos_end) == ')')) {
    // extracts the numbers
    const int length = pos_end - pos_start;
    std::string str_extracted = name.substr(pos_start + 1, length - 1);

    // attempts to convert string to integer
    std::stringstream stream(str_extracted);
    stream >> version;

    // checks conversion status
    if (!stream) {
      is_versioned = false;
    }
  }

  // creates base string
  std::string str_base;
  if (is_versioned == false) {
    str_base = name + " ";
  } else {
    str_base = name.substr(0, pos_start);
  }

  // determines a unique description for the span
  std::string str_version;
  std::string name_versioned;
  while (true) {
    // creates version string
    str_version = "(" + std::to_string(version) + ")";

    // combines the base and version names
    name_versioned = str_base + str_version;

    // compares against spans in doc
    if (doc->IsUniqueName(name_versioned) == true) {
      break;
    } else {
      version++;
    }
  }

  return name_versioned;
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

void EditPane::OnButtonMoveDown(wxCommandEvent& event) {
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

  // moves span
  MoveSpanDown(id_item);
}

void EditPane::OnButtonMoveUp(wxCommandEvent& event) {
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

  // moves span
  MoveSpanUp(id_item);
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
  } else if (id_event == kTreeItemDeactivate) {
    wxBusyCursor cursor;
    DeactivateSpan(id_item);
  } else if (id_event == kTreeItemDelete) {
    wxBusyCursor cursor;
    DeleteSpan(id_item);
  } else if (id_event == kTreeItemEdit) {
    // can't create busy cursor, a dialog is used further along
    EditSpan(id_item);
  } else if (id_event == kTreeItemMoveDown) {
    wxBusyCursor cursor;
    MoveSpanDown(id_item);
  } else if (id_event == kTreeItemMoveUp) {
    wxBusyCursor cursor;
    MoveSpanUp(id_item);
  } else if (id_event == kTreeRootAdd) {
    // can't create busy cursor, a dialog is used further along
    AddSpan();
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

  // gets selected span item and the activated span (document)
  // determines if the selected item is currently activated
  SpanTreeItemData* data =
      dynamic_cast<SpanTreeItemData*>(treectrl_->GetItemData(id));
  const Span* span_selected = &(*data->iter());

  SpanAnalyzerDoc* doc = dynamic_cast<SpanAnalyzerDoc*>(view_->GetDocument());
  const Span* span_activated = doc->SpanActivated();

  bool is_activated = false;
  if (span_selected == span_activated) {
    is_activated = true;
  }

  // displays a context menu based on the item that was right clicked
  wxMenu menu;
  if (id == treectrl_->GetRootItem()) {
    menu.Append(kTreeRootAdd, "Add Span");
  } else {  // a span is selected
    if (is_activated == false) {
      menu.Append(kTreeItemActivate, "Activate");
    } else {
      menu.Append(kTreeItemDeactivate, "Deactivate");
    }
    menu.AppendSeparator();
    menu.Append(kTreeItemEdit, "Edit");
    menu.Append(kTreeItemCopy, "Copy");
    menu.Append(kTreeItemDelete, "Delete");
    menu.AppendSeparator();
    menu.Append(kTreeItemMoveUp, "Move Up");
    menu.Append(kTreeItemMoveDown, "Move Down");
  }

  // shows context menu
  // the event is caught by the edit panel
  PopupMenu(&menu, event.GetPoint());

  // stops processing event (needed to allow pop-up menu to catch its event)
  event.Skip();
}

/// This method deletes the treectrl items and re-inserts them. The treectrl
/// item focus is not set.
void EditPane::UpdateTreeCtrlSpanItems() {
  treectrl_->Freeze();

  // gets information from document and treectrl
  SpanAnalyzerDoc* doc = dynamic_cast<SpanAnalyzerDoc*>(view_->GetDocument());
  const std::list<Span>& spans = doc->spans();
  const Span* span_activated = doc->SpanActivated();

  wxTreeItemId root = treectrl_->GetRootItem();
  treectrl_->DeleteChildren(root);

  // iterates over all spans in the document
  for (auto iter = spans.cbegin(); iter != spans.cend(); iter++) {
    const Span* span = &(*iter);

    // creates treectrl item
    wxTreeItemId item = treectrl_->AppendItem(root, span->name);

    SpanTreeItemData* data = new SpanTreeItemData();
    data->set_iter(iter);
    treectrl_->SetItemData(item, data);

    // adjusts bold for activated and non-activated span
    if (span == span_activated) {
      treectrl_->SetItemBold(item, true);
    } else {
      treectrl_->SetItemBold(item, false);
    }
  }

  treectrl_->Thaw();
}
