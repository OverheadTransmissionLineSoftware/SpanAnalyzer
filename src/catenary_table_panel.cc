// This is free and unencumbered software released into the public domain.
// For more information, please refer to <http://unlicense.org/>

#include "catenary_table_panel.h"

#include "models/base/helper.h"
#include "models/transmissionline/catenary.h"
#include "wx/clipbrd.h"
#include "wx/xrc/xmlres.h"

#include "span_analyzer_view.h"

enum {
  kCopyTable = 0
};

BEGIN_EVENT_TABLE(CatenaryTablePanel, wxPanel)
  EVT_CHOICE(XRCID("choice_condition"), CatenaryTablePanel::OnChoiceCondition)
  EVT_CHOICE(XRCID("choice_side"), CatenaryTablePanel::OnChoiceSide)
  EVT_LIST_COL_RIGHT_CLICK(wxID_ANY, CatenaryTablePanel::OnColumnRightClick)
  EVT_LIST_ITEM_RIGHT_CLICK(wxID_ANY, CatenaryTablePanel::OnItemRightClick)
  EVT_MENU(wxID_ANY, CatenaryTablePanel::OnContextMenuSelect)
END_EVENT_TABLE()

CatenaryTablePanel::CatenaryTablePanel(
    wxWindow* parent,
    const SagTensionAnalysisResultSet* results) {
  wxXmlResource::Get()->LoadPanel(this, parent, "catenary_table_panel");

  results_ = results;

  // intializes choice controls
  wxChoice* choice = nullptr;

  choice = XRCCTRL(*this, "choice_condition", wxChoice);
  choice->Append("Initial");
  choice->Append("Load");

  choice = XRCCTRL(*this, "choice_side", wxChoice);
  choice->Append("");
  choice->Append("BOL");
  choice->Append("AOL");

  // initializes the listctrl
  listctrl_ = XRCCTRL(*this, "listctrl_table", wxListCtrl);
}

CatenaryTablePanel::~CatenaryTablePanel(){
}

void CatenaryTablePanel::UpdateView(wxObject* hint) {
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

void CatenaryTablePanel::CopyTableToClipboard() {
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

void CatenaryTablePanel::FillBlank() {
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

void CatenaryTablePanel::FillData() {
  // gets data from results pane
  const std::list<SagTensionAnalysisResult>* results = nullptr;
  wxChoice* choice = nullptr;
  choice = XRCCTRL(*this, "choice_condition", wxChoice);
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

    // creates a catenary to calculate results
    Catenary3d catenary;
    catenary.set_spacing_endpoints(results_->span->spacing_catenary);
    catenary.set_tension_horizontal(result.tension_horizontal);
    catenary.set_weight_unit(result.weight_unit);

    double value;
    std::string str;

    index_col = 0;
    str = description;
    listctrl_->SetItem(index_row, index_col, str);

    index_col++;
    value = catenary.weight_unit().Magnitude();
    str = helper::DoubleToFormattedString(value, 3);
    listctrl_->SetItem(index_row, index_col, str);

    index_col++;
    value = catenary.SwingAngle();
    str = helper::DoubleToFormattedString(value, 1);
    listctrl_->SetItem(index_row, index_col, str);

    index_col++;
    value = catenary.tension_horizontal();
    str = helper::DoubleToFormattedString(value, 1);
    listctrl_->SetItem(index_row, index_col, str);

    index_col++;
    value = catenary.Constant();
    str = helper::DoubleToFormattedString(value, 1);
    listctrl_->SetItem(index_row, index_col, str);

    index_col++;
    value = catenary.Sag();
    str = helper::DoubleToFormattedString(value, 1);
    listctrl_->SetItem(index_row, index_col, str);

    index_col++;
    value = catenary.Length();
    str = helper::DoubleToFormattedString(value, 1);
    listctrl_->SetItem(index_row, index_col, str);

    index_col++;
    value = catenary.LengthSlack();
    str = helper::DoubleToFormattedString(value, 1);
    listctrl_->SetItem(index_row, index_col, str);

    // updates columns that depend on span end selection
    choice = XRCCTRL(*this, "choice_side", wxChoice);
    str = choice->GetStringSelection();
    if (str == "") {
      index_col++;
      listctrl_->SetItem(index_row, index_col, "");

      index_col++;
      listctrl_->SetItem(index_row, index_col, "");

      index_col++;
      listctrl_->SetItem(index_row, index_col, "");
    } else if (str == "BOL") {
      index_col++;
      value = catenary.Tension(0);
      str = helper::DoubleToFormattedString(value, 1);
      listctrl_->SetItem(index_row, index_col, str);

      index_col++;
      value = catenary.Tension(0, AxisDirectionType::kPositive).z();
      str = helper::DoubleToFormattedString(value, 1);
      listctrl_->SetItem(index_row, index_col, str);

      index_col++;
      value = catenary.TangentAngleVertical(0, AxisDirectionType::kPositive);
      str = helper::DoubleToFormattedString(value, 1);
      listctrl_->SetItem(index_row, index_col, str);
    } else if (str == "AOL") {
      index_col++;
      value = catenary.Tension(1);
      str = helper::DoubleToFormattedString(value, 1);
      listctrl_->SetItem(index_row, index_col, str);

      index_col++;
      value = catenary.Tension(1, AxisDirectionType::kNegative).z();
      str = helper::DoubleToFormattedString(value, 1);
      listctrl_->SetItem(index_row, index_col, str);

      index_col++;
      value = catenary.TangentAngleVertical(1, AxisDirectionType::kNegative);
      str = helper::DoubleToFormattedString(value, 1);
      listctrl_->SetItem(index_row, index_col, str);
    }

    iter_weathercases++;
    iter_result++;
  }
}

void CatenaryTablePanel::OnChoiceCondition(wxCommandEvent& event) {
  FillData();
}

void CatenaryTablePanel::OnChoiceSide(wxCommandEvent& event) {
  FillData();
}

void CatenaryTablePanel::OnColumnRightClick(wxListEvent& event) {
  // displays a context menu based on the item that was right clicked
  wxMenu menu;
  menu.Append(kCopyTable, "Copy Table");

  // shows context menu
  // the event is caught by the edit panel
  PopupMenu(&menu, event.GetPoint());

  // stops processing event (needed to allow pop-up menu to catch its event)
  event.Skip();
}

void CatenaryTablePanel::OnContextMenuSelect(wxCommandEvent& event) {
  // gets context menu selection and sends to handler function
  const int id_event = event.GetId();
  if (id_event == kCopyTable) {
    CopyTableToClipboard();
  }
}

void CatenaryTablePanel::OnItemRightClick(wxListEvent& event) {
  // displays a context menu based on the item that was right clicked
  wxMenu menu;
  menu.Append(kCopyTable, "Copy Table");

  // shows context menu
  // the event is caught by the edit panel
  PopupMenu(&menu, event.GetPoint());

  // stops processing event (needed to allow pop-up menu to catch its event)
  event.Skip();
}
