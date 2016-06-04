// This is free and unencumbered software released into the public domain.
// For more information, please refer to <http://unlicense.org/>

#include "sag_tension_table_panel.h"

#include "models/base/helper.h"
#include "wx/clipbrd.h"
#include "wx/xrc/xmlres.h"

#include "span_analyzer_view.h"

enum {
  kCopyTable = 0
};

BEGIN_EVENT_TABLE(SagTensionTablePanel, wxPanel)
  EVT_CHOICE(XRCID("choice_condition"), SagTensionTablePanel::OnChoiceCondition)
  EVT_LIST_COL_RIGHT_CLICK(wxID_ANY, SagTensionTablePanel::OnColumnRightClick)
  EVT_LIST_ITEM_RIGHT_CLICK(wxID_ANY, SagTensionTablePanel::OnItemRightClick)
  EVT_MENU(wxID_ANY, SagTensionTablePanel::OnContextMenuSelect)
END_EVENT_TABLE()

SagTensionTablePanel::SagTensionTablePanel(
    wxWindow* parent,
    const SagTensionAnalysisResultSet* results) {
  wxXmlResource::Get()->LoadPanel(this, parent, "sag_tension_table_panel");

  results_ = results;

  // intializes choice
  wxChoice* choice = XRCCTRL(*this, "choice_condition", wxChoice);
  choice->Append("Initial");
  choice->Append("Load");

  // initializes the listctrl
  listctrl_ = XRCCTRL(*this, "listctrl_table", wxListCtrl);
}

SagTensionTablePanel::~SagTensionTablePanel() {
}

void SagTensionTablePanel::UpdateView(wxObject* hint) {
  if (hint == nullptr) {
    return;
  }

  // interprets hint type
  ViewUpdateHint* hint_update = (ViewUpdateHint*)hint;
  if (hint_update->type() ==
       ViewUpdateHint::HintType::kModelAnalysisWeathercaseEdit) {
    listctrl_->DeleteAllItems();
    FillData();
  } else if (hint_update->type() ==
    ViewUpdateHint::HintType::kModelPreferencesEdit) {
    FillData();
  } else if (hint_update->type() ==
       ViewUpdateHint::HintType::kModelSpansEdit) {
    FillData();
  } else if (hint_update->type() ==
       ViewUpdateHint::HintType::kModelWeathercaseEdit) {
    listctrl_->DeleteAllItems();
    FillData();
  } else if (hint_update->type() ==
      ViewUpdateHint::HintType::kViewWeathercasesSetChange) {
    listctrl_->DeleteAllItems();
    FillData();
  }
}

void SagTensionTablePanel::CopyTableToClipboard() {
  if (wxTheClipboard->Open()) {
    // creates a single string with all of the table data
    // the \t character is used to separate entries on the same row
    // the \n character is used to specify a new line
    std::string str;
    const int kRowMax = listctrl_->GetItemCount() - 1;
    const int kColumnMax = listctrl_->GetColumnCount() - 1;
    for (int index_row = 0; index_row <= kRowMax; index_row++) {

      for (int index_column = 0; index_column <= kColumnMax; index_column++) {
        // adds string from row/column index
        str = str + listctrl_->GetItemText(index_row, index_column);

        // adds tab character for all but the last column
        if (index_column != kColumnMax) {
          str = str + "\t";
        }
      }

      // adds new line character for all but the last row
      if (index_row != kRowMax) {
        str = str + "\n";
      }
    }

    // creates a text data object to store clipboard information
    wxTextDataObject* data = new wxTextDataObject();
    data->SetText(str);

    // copies to the clipboard
    wxTheClipboard->SetData(data);

    wxTheClipboard->Close();
  }
}

void SagTensionTablePanel::FillBlank() {
  // gets list of weathercase descriptions
  const std::list<std::string>& descriptions =
      results_->descriptions_weathercase;

  // creates as many rows as weathercases
  for (auto iter = descriptions.cbegin(); iter != descriptions.cend(); iter++) {
    long index_row = std::distance(descriptions.cbegin(), iter);

    // adds list item (row) to listctrl
    wxListItem item;
    item.SetId(index_row);
    listctrl_->InsertItem(item);
  }
}

void SagTensionTablePanel::FillData() {
  // gets data based on choice selection
  const std::list<SagTensionAnalysisResult>* results = nullptr;
  wxChoice* choice = XRCCTRL(*this, "choice_condition", wxChoice);
  wxString str = choice->GetStringSelection();
  if (str == "Initial") {
    results = &(results_->results_initial);
  } else if (str == "Load") {
    results = &(results_->results_load);
  }

  if (results == nullptr) {
    return;
  }

  // makes sure there are blanks already inserted in table
  const unsigned int kCountItems = listctrl_->GetItemCount();
  if (kCountItems != results->size()) {
    listctrl_->DeleteAllItems();
    FillBlank();
  }

  // fills each row with data
  auto iter_weathercases = results_->descriptions_weathercase.cbegin();
  auto iter_result = results->cbegin();

  for (auto index_row = 0; index_row < listctrl_->GetItemCount(); index_row++) {
    // gets weathercase description and sag-tension result
    const std::string& description = *iter_weathercases;
    const SagTensionAnalysisResult& result = *iter_result;
    int index_col = -9999;

    double value;
    std::string str;

    index_col = 0;
    str = description;
    listctrl_->SetItem(index_row, index_col, str);

    index_col++;
    value = result.weight_unit.z();
    str = helper::DoubleToFormattedString(value, 3);
    listctrl_->SetItem(index_row, index_col, str);

    index_col++;
    value = result.weight_unit.y();
    str = helper::DoubleToFormattedString(value, 3);
    listctrl_->SetItem(index_row, index_col, str);

    index_col++;
    value = result.weight_unit.Magnitude();
    str = helper::DoubleToFormattedString(value, 3);
    listctrl_->SetItem(index_row, index_col, str);

    index_col++;
    value = result.tension_horizontal;
    str = helper::DoubleToFormattedString(value, 1);
    listctrl_->SetItem(index_row, index_col, str);

    index_col++;
    value = result.tension_horizontal_shell;
    str = helper::DoubleToFormattedString(value, 1);
    listctrl_->SetItem(index_row, index_col, str);

    index_col++;
    value = result.tension_horizontal_core;
    str = helper::DoubleToFormattedString(value, 1);
    listctrl_->SetItem(index_row, index_col, str);

    index_col++;
    value = result.tension_horizontal / result.weight_unit.Magnitude();
    str = helper::DoubleToFormattedString(value, 1);
    listctrl_->SetItem(index_row, index_col, str);

    iter_weathercases++;
    iter_result++;
  }
}

void SagTensionTablePanel::OnChoiceCondition(wxCommandEvent& event) {
  FillData();
}

void SagTensionTablePanel::OnColumnRightClick(wxListEvent& event) {
  // displays a context menu based on the item that was right clicked
  wxMenu menu;
  menu.Append(kCopyTable, "Copy Table");

  // shows context menu
  // the event is caught by the edit panel
  PopupMenu(&menu, event.GetPoint());

  // stops processing event (needed to allow pop-up menu to catch its event)
  event.Skip();
}

void SagTensionTablePanel::OnContextMenuSelect(wxCommandEvent& event) {
  // gets context menu selection and sends to handler function
  const int id_event = event.GetId();
  if (id_event == kCopyTable) {
    CopyTableToClipboard();
  }
}

void SagTensionTablePanel::OnItemRightClick(wxListEvent& event) {
  // displays a context menu based on the item that was right clicked
  wxMenu menu;
  menu.Append(kCopyTable, "Copy Table");

  // shows context menu
  // the event is caught by the edit panel
  PopupMenu(&menu, event.GetPoint());

  // stops processing event (needed to allow pop-up menu to catch its event)
  event.Skip();
}
