// This is free and unencumbered software released into the public domain.
// For more information, please refer to <http://unlicense.org/>

#include "edit_pane.h"

#include "wx/xrc/xmlres.h"

#include "span_analyzer_app.h"
#include "span_analyzer_doc.h"
#include "span_analyzer_doc_commands.h"
#include "span_editor_dialog.h"
#include "span_unit_converter.h"

#include "../res/copy.xpm"
#include "../res/minus.xpm"
#include "../res/move_arrow_down.xpm"
#include "../res/move_arrow_up.xpm"
#include "../res/plus.xpm"
#include "../res/wrench.xpm"

/// context menu enum
enum {
  kTreeItemActivate = 0,
  kTreeItemCopy = 1,
  kTreeItemDelete = 2,
  kTreeItemEdit = 3,
  kTreeItemMoveDown = 4,
  kTreeItemMoveUp = 5,
  kTreeRootAdd = 6,
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
  UpdateHint* hint_update = (UpdateHint*)hint;
  if (hint_update == nullptr) {
    InitializeTreeCtrl();
  } else if (hint_update->type() == HintType::kAnalysisFilterGroupEdit) {
    // do nothing
  } else if (hint_update->type() == HintType::kAnalysisFilterGroupSelect) {
    // do nothing
  } else if (hint_update->type() == HintType::kAnalysisFilterSelect) {
    // do nothing
  } else if (hint_update->type() == HintType::kCablesEdit) {
    // do nothing
  } else if (hint_update->type() == HintType::kPreferencesEdit) {
    // do nothing
  } else if (hint_update->type() == HintType::kSpansEdit) {
    UpdateTreeCtrlSpanItems();
  } else if (hint_update->type() == HintType::kWeathercasesEdit) {
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
  SpanAnalyzerDoc* doc = (SpanAnalyzerDoc*)view_->GetDocument();
  SpanTreeItemData* data =
      (SpanTreeItemData*)treectrl_->GetItemData(id);
  auto iter = data->iter();
  doc->SetSpanAnalysis(&(*iter));

  // updates treectrl
  UpdateTreeCtrlSpanItems();
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

  wxLogVerbose("Adding span.");

  // updates document
  SpanAnalyzerDoc* doc = (SpanAnalyzerDoc*)view_->GetDocument();

  SpanCommand* command = new SpanCommand(SpanCommand::kNameInsert);
  command->set_index(doc->spans().size());
  command->set_span(span);

  doc->GetCommandProcessor()->Submit(command);

  // adjusts treectrl focus
  FocusTreeCtrlSpanItem(command->index());
}

void EditPane::CopySpan(const wxTreeItemId& id) {
  // gets tree item data
  SpanTreeItemData* item = (SpanTreeItemData*)treectrl_->GetItemData(id);

  // copies span
  Span span = *(item->iter());

  wxLogVerbose("Copying span.");

  // updates document
  SpanAnalyzerDoc* doc = (SpanAnalyzerDoc*)view_->GetDocument();
  const int index = std::distance(doc->spans().cbegin(), item->iter()) + 1;

  SpanCommand* command = new SpanCommand(SpanCommand::kNameInsert);
  command->set_index(index);
  command->set_span(span);

  doc->GetCommandProcessor()->Submit(command);

  // adjusts treectrl focus
  FocusTreeCtrlSpanItem(command->index());
}

void EditPane::DeleteSpan(const wxTreeItemId& id) {
  // gets tree item data
  SpanTreeItemData* item = (SpanTreeItemData*)treectrl_->GetItemData(id);

  wxLogVerbose("Deleting span.");

  // updates document
  SpanAnalyzerDoc* doc = (SpanAnalyzerDoc*)view_->GetDocument();
  const int index = std::distance(doc->spans().cbegin(), item->iter());

  SpanCommand* command = new SpanCommand(SpanCommand::kNameDelete);
  command->set_index(index);

  doc->GetCommandProcessor()->Submit(command);

  // adjusts treectrl focus
  FocusTreeCtrlSpanItem(command->index() - 1);
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
  if (dialog.ShowModal() != wxID_OK) {
    return;
  }

  wxLogVerbose("Editing span.");

  // converts span to consistent unit style
  SpanUnitConverter::ConvertUnitStyle(wxGetApp().config()->units,
                                      units::UnitStyle::kDifferent,
                                      units::UnitStyle::kConsistent,
                                      span);

  // updates document
  SpanAnalyzerDoc* doc = (SpanAnalyzerDoc*)view_->GetDocument();
  const int index = std::distance(doc->spans().cbegin(), item->iter());

  SpanCommand* command = new SpanCommand(SpanCommand::kNameModify);
  command->set_index(index);
  command->set_span(span);

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
    }
    else {
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
  SpanTreeItemData* item = (SpanTreeItemData*)treectrl_->GetItemData(id);

  wxLogVerbose("Moving spans.");

  // updates document
  SpanAnalyzerDoc* doc = (SpanAnalyzerDoc*)view_->GetDocument();
  const int index = std::distance(doc->spans().cbegin(), item->iter());

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
  SpanTreeItemData* item = (SpanTreeItemData*)treectrl_->GetItemData(id);

  wxLogVerbose("Moving spans.");

  // updates document
  SpanAnalyzerDoc* doc = (SpanAnalyzerDoc*)view_->GetDocument();
  const int index = std::distance(doc->spans().cbegin(), item->iter());

  SpanCommand* command = new SpanCommand(SpanCommand::kNameMoveUp);
  command->set_index(index);

  doc->GetCommandProcessor()->Submit(command);

  // adjusts treectrl focus
  FocusTreeCtrlSpanItem(command->index());
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

  // displays a context menu based on the item that was right clicked
  wxMenu menu;
  if (id == treectrl_->GetRootItem()) {
    menu.Append(kTreeRootAdd, "Add Span");
  } else { // a span is selected
    menu.Append(kTreeItemActivate, "Activate");
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

/// This method attempts to update treectrl items in place if possible. The
/// treectrl item focus is not handled in this method.
void EditPane::UpdateTreeCtrlSpanItems() {
  // gets information from document and treectrl
  SpanAnalyzerDoc* doc = (SpanAnalyzerDoc*)view_->GetDocument();
  const std::list<Span>& spans = doc->spans();
  const Span* span_activated = doc->SpanAnalysis();

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
}
