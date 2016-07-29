// This is free and unencumbered software released into the public domain.
// For more information, please refer to <http://unlicense.org/>

#include "report_table.h"

#include "wx/clipbrd.h"

/// \par OVERVIEW
///
/// This struct contains all the control information for the
/// wxListCtrl::SortItems function, which is also passed onto the
/// wxListCompareFunction.
struct SortData {
  /// \var type
  ///   The sort type.
  SortOrderType type;

  /// \var index
  ///   The column index.
  int index;
};

/// \brief This function is used by the wxListCtrl to sort items.
/// \param[in] item1
///   The first item to compare.
/// \param[in] item2
///   The second item to compare.
/// \param[in] sortData
///   The data/settings used to sort the listctrl. Column and whether
///   the sort is ascending/descending need to be detailed.
/// \return This function returns -1 if the first item is less than the second,
///   0 if they are equal, and 1 if the first is greater than the second one.
///   These values are used by wxListCtrl to sort the items.
/// This function uses the data associated with each listctrl item. See
/// wxListCtrl::SetItemData().
int wxCALLBACK wxListCompareFunction(wxIntPtr item1, wxIntPtr item2,
                                     wxIntPtr data_sort) {
  // gets sort column
  const SortData* data = (SortData*)data_sort;

  // iterator for accessing lists
  std::list<std::string>::const_iterator it;

  // gets string from row1
  const ReportRow* row1 = (ReportRow*)item1;
  it = std::next(row1->values.cbegin(), data->index);
  const std::string& str1 = *it;

  // gets string from row2
  const ReportRow* row2 = (ReportRow*)item2;
  it = std::next(row2->values.cbegin(), data->index);
  const std::string& str2 = *it;

  // compares strings
  unsigned int i = 0;
  while (i < str1.length() && i < str2.length()) {
    if (tolower(str1[i]) < tolower(str2[i])) {
      if (data->type == SortOrderType::kAscending) {
        return -1;
      } else {
        return 1;
      }
    } else if (tolower(str1[i]) > tolower(str2[i])) {
      if (data->type == SortOrderType::kAscending) {
        return 1;
      } else {
        return -1;
      }
    }
    ++i;
  }

  return ( str1.length() < str2.length() );
}

enum {
  kCopyTable = 0,
  kcopyRow = 1
};

BEGIN_EVENT_TABLE(ReportTable, wxPanel)
  EVT_LIST_COL_CLICK(wxID_ANY, ReportTable::OnColumnClick)
  EVT_LIST_ITEM_RIGHT_CLICK(wxID_ANY, ReportTable::OnItemRightClick)
  EVT_MENU(wxID_ANY, ReportTable::OnContextMenuSelect)
END_EVENT_TABLE()

ReportTable::ReportTable(wxWindow* parent) : wxPanel(parent, wxID_ANY) {
  // initializes members
  data_ = nullptr;
  index_sorted_ = -1;
  type_sort_ = SortOrderType::kNone;

  // initializes listctrl
  listctrl_ = new wxListCtrl(this, wxID_ANY, wxDefaultPosition, wxDefaultSize,
                             wxLC_REPORT|wxLC_HRULES|wxLC_VRULES
                             |wxLC_SINGLE_SEL);
  wxBoxSizer* sizer = new wxBoxSizer(wxVERTICAL);
  sizer->Add(listctrl_, 1, wxEXPAND);
  SetSizer(sizer);
}

ReportTable::~ReportTable() {
}

void ReportTable::CopyReportToClipboard(const bool& is_included_headers) const {
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

void ReportTable::Refresh() {
  // initializes listctrl
  listctrl_->ClearAll();

  // inserts columns
  for (auto iter = data_->headers.cbegin(); iter != data_->headers.cend();
       iter++) {
    const std::string& str = *iter;
    const int i = std::distance(data_->headers.cbegin(), iter);
    listctrl_->InsertColumn(i, str, wxLIST_FORMAT_CENTER, 80);
  }

  // fills data
  for (auto iter = data_->rows.cbegin(); iter != data_->rows.cend(); iter++) {
    const ReportRow& row = *iter;
    const int i = std::distance(data_->rows.cbegin(), iter);

    // adds list item (row) to listctrl
    wxListItem item;
    item.SetId(i);
    item.SetData((long)&row);
    listctrl_->InsertItem(item);

    // fills remaining values for each column
    for (auto it = row.values.cbegin(); it != row.values.cend(); it++) {
      const std::string& str = *it;
      const int j = std::distance(row.values.cbegin(), it);

      // adds value to cell
      listctrl_->SetItem(i, j, str);
    }
  }

  // sorts
  Sort();
}

const ReportData* ReportTable::data() const {
  return data_;
}

void ReportTable::set_data(const ReportData* data) {
  data_ = data;
}

void ReportTable::set_formatting_column(const int& column, const int& width,
                                        const wxListColumnFormat& align) {
  // checks if column exists
  if (column < 0 || listctrl_->GetColumnCount() - 1 < column) {
    return;
  }

  // modifies column formatting
  wxListItem item;
  listctrl_->GetColumn(column, item);

  item.SetWidth(width);
  item.SetAlign(align);
  listctrl_->SetColumn(column, item);
}

void ReportTable::ClearListCtrl() {
  listctrl_->ClearAll();
}

void ReportTable::OnColumnClick(wxListEvent& event) {
  // gets column index
  const int index_col = event.GetColumn();

  // updates column and sort order
  if (index_col == index_sorted_) {
    if (type_sort_ == SortOrderType::kAscending) {
      type_sort_ = SortOrderType::kDescending;
    } else if (type_sort_ == SortOrderType::kDescending) {
      type_sort_ = SortOrderType::kAscending;
    }
  } else {
    index_sorted_ = index_col;
    type_sort_ = SortOrderType::kAscending;
  }

  // sorts the listctrl
  Sort();
}

void ReportTable::OnContextMenuSelect(wxCommandEvent& event) {
  // gets context menu selection and sends to handler function
  const int id_event = event.GetId();
  if (id_event == kCopyTable) {
    CopyReportToClipboard();
  }
}

void ReportTable::OnItemRightClick(wxListEvent& event) {
  // displays a context menu based on the item that was right clicked
  wxMenu menu;
  menu.Append(kcopyRow, "Copy Row");
  menu.Append(kCopyTable, "Copy Table");

  // shows context menu
  // the event is caught by the edit panel
  PopupMenu(&menu, event.GetPoint());

  // stops processing event (needed to allow pop-up menu to catch its event)
  event.Skip();
}

void ReportTable::Sort() {
  // checks if row is selected
  if (index_sorted_ < 0) {
    return;
  }

  // creates a struct to pass sorting parameters to compare function
  SortData data;
  data.index = index_sorted_;
  data.type = type_sort_;

  // temporarily casts the column number to the sort data parameter
  listctrl_->SortItems(wxListCompareFunction, (wxIntPtr)&data);
}
