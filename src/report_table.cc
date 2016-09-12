// This is free and unencumbered software released into the public domain.
// For more information, please refer to <http://unlicense.org/>

#include "report_table.h"

#include "models/base/helper.h"
#include "wx/clipbrd.h"
#include "wx/imaglist.h"

#include "../res/sort_arrow_up.xpm"
#include "../res/sort_arrow_down.xpm"

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

/// \par OVERVIEW
///
/// This is the enumeration for the context menu.
enum {
  kCopyHeaders = 0,
  kCopyRow = 1,
  kCopyTable = 2
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
  // gets sort column and order
  const SortData* data = (SortData*)data_sort;

  // gets string from row1
  const ReportRow* row1 = (ReportRow*)item1;
  const std::string& str1 = *std::next(row1->values.cbegin(), data->index);

  // gets string from row2
  const ReportRow* row2 = (ReportRow*)item2;
  const std::string& str2 = *std::next(row2->values.cbegin(), data->index);

  // attempts to compare strings as numbers
  int status;
  if (helper::IsNumeric(str1) && helper::IsNumeric(str2) == true) {
    // compares as numbers
    const double dbl1 = std::stod(str1);
    const double dbl2 = std::stod(str2);

    if (dbl1 == dbl2) {
      status = 0;
    } else if (dbl1 < dbl2) {
      status = -1;
    } else {
      status = 1;
    }
  } else {
    // compares as strings
    status = helper::CompareStrings(str1, str2);
  }

  // applies sort order
  if (data->type == SortOrderType::kAscending) {
    return status;
  } else {
    return -1 * status;
  }
}

BEGIN_EVENT_TABLE(ReportTable, wxPanel)
  EVT_LIST_COL_CLICK(wxID_ANY, ReportTable::OnColumnClick)
  EVT_LIST_COL_RIGHT_CLICK(wxID_ANY, ReportTable::OnColumnRightClick)
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

  // assigns images to listctrl
  wxImageList* images = new wxImageList(16, 16, true);
  images->Add(wxBitmap(sort_arrow_down_xpm), wxColour(255, 0, 0));
  images->Add(wxBitmap(sort_arrow_up_xpm), wxColour(255, 0, 0));
  listctrl_->AssignImageList(images, wxIMAGE_LIST_SMALL);
}

ReportTable::~ReportTable() {
}

long ReportTable::IndexFocused() const {
  // searches for an item that has a selected state
  // initializes item at -1 so that it searches at beginning
  // only one item can be selected at a time for this control
  return listctrl_->GetNextItem(-1, wxLIST_NEXT_ALL, wxLIST_STATE_FOCUSED);
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
    item.SetImage(-1);
    listctrl_->InsertItem(item);

    // fills remaining values for each column
    for (auto it = row.values.cbegin(); it != row.values.cend(); it++) {
      const std::string& str = *it;
      const int j = std::distance(row.values.cbegin(), it);

      // adds value to cell
      listctrl_->SetItem(i, j, str);
    }
  }

  // updates the column image and sorts
  if (listctrl_->GetColumnCount() <= index_sorted_) {
    index_sorted_ = -1;
  }

  ChangeColumnSortImage(index_sorted_, type_sort_);
  Sort();
}

const ReportData* ReportTable::data() const {
  return data_;
}

long ReportTable::index_sorted() const {
  return index_sorted_;
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

void ReportTable::set_index_focused(const long& index) {
  if (index < 0) {
    return;
  }

  if (listctrl_->GetItemCount() <= index) {
    return;
  }

  listctrl_->SetItemState(index, wxLIST_STATE_SELECTED, wxLIST_STATE_SELECTED);
}

void ReportTable::set_index_sorted(const long& index) {
  if (index < 0) {
    index_sorted_ = -1;
  } else if (listctrl_->GetColumnCount() <= index) {
    index_sorted_ = -1;
  } else {
    index_sorted_ = index;
  }
}

void ReportTable::set_type_sort(const SortOrderType& type_sort) {
  type_sort_ = type_sort;
}

SortOrderType ReportTable::type_sort() const {
  return type_sort_;
}

void ReportTable::ChangeColumnSortImage(const int& index,
                                        const SortOrderType& type_sort) {
  if (index < 0) {
    return;
  }
  
  wxListItem item;
  item.SetMask(wxLIST_MASK_IMAGE);

  if (type_sort == SortOrderType::kAscending) {
    item.SetImage(0);
  } else if (type_sort == SortOrderType::kDescending) {
    item.SetImage(1);
  } else {
    item.SetImage(-1);
  }

  listctrl_->SetColumn(index, item);
}

void ReportTable::ClearListCtrl() {
  listctrl_->ClearAll();
}

std::string ReportTable::ClipboardStringHeaders() const {
  std::string str;
  const int kColumnMax = listctrl_->GetColumnCount() - 1;
  for (int index_column = 0; index_column <= kColumnMax; index_column++) {
    wxListItem item;
    listctrl_->GetColumn(index_column, item);
    str = str + item.GetText();

    // adds tab or new line character
    if (index_column == kColumnMax) {
      str = str + "\n";
    } else {
      str = str + "\t";
    }
  }

  return str;
}

std::string ReportTable::ClipboardStringRow(const int& index) const {
  // initializes
  std::string str;
  const int kColumnMax = listctrl_->GetColumnCount() - 1;

  // iterates over the number of columns
  for (int index_column = 0; index_column <= kColumnMax; index_column++) {
    // adds string from row/column index
    str = str + listctrl_->GetItemText(index, index_column);

    // adds tab or new line character
    if (index_column == kColumnMax) {
      str = str + "\n";
    } else {
      str = str + "\t";
    }
  }

  return str;
}

std::string ReportTable::ClipBoardStringTable() const {
  std::string str;
  const int kRowMax = listctrl_->GetItemCount() - 1;
  for (int index_row = 0; index_row <= kRowMax; index_row++) {
    str = str + ClipboardStringRow(index_row);
  }

  return str;
}

void ReportTable::CopyToClipboard(const std::string& str) {
  if (wxTheClipboard->Open()) {
    // creates a text data object to store clipboard information
    wxTextDataObject* data = new wxTextDataObject();
    data->SetText(str);

    // copies to the clipboard and closes
    wxTheClipboard->SetData(data);
    wxTheClipboard->Close();
  }
}

void ReportTable::OnColumnClick(wxListEvent& event) {
  // updates sorted column index
  const int index_sorted_old = index_sorted_;
  index_sorted_ = event.GetColumn();

  // updates sort order
  if (index_sorted_old == index_sorted_) {
    // inverses sort order if same column is selected
    if (type_sort_ == SortOrderType::kAscending) {
      type_sort_ = SortOrderType::kDescending;
    } else if (type_sort_ == SortOrderType::kDescending) {
      type_sort_ = SortOrderType::kAscending;
    }
  } else if (index_sorted_old == -1) {
    // initializes sort order if no columns were sorted before
    type_sort_ = SortOrderType::kAscending;
  }

  // changes column header images
  ChangeColumnSortImage(index_sorted_old, SortOrderType::kNone);
  ChangeColumnSortImage(index_sorted_, type_sort_);

  // sorts the listctrl
  Sort();
}

void ReportTable::OnColumnRightClick(wxListEvent& event) {
  // displays a context menu based on the item that was right clicked
  wxMenu menu;
  menu.Append(kCopyHeaders, "Copy Headers");
  menu.Append(kCopyTable, "Copy Table");

  // shows context menu
  // the event is caught by the the report table
  PopupMenu(&menu, event.GetPoint());

  // stops processing event (needed to allow pop-up menu to catch its event)
  event.Skip();
}

void ReportTable::OnContextMenuSelect(wxCommandEvent& event) {
  // gets context menu selection and sends to handler function
  const int id_event = event.GetId();
  if (id_event == kCopyHeaders) {
    std::string str = ClipboardStringHeaders();
    CopyToClipboard(str);
  } else if (id_event == kCopyRow) {
    long index = IndexFocused();
    std::string str = ClipboardStringRow(index);
    CopyToClipboard(str);
  } else if (id_event == kCopyTable) {
    std::string str = ClipBoardStringTable();
    CopyToClipboard(str);
  }
}

void ReportTable::OnItemRightClick(wxListEvent& event) {
  // displays a context menu based on the item that was right clicked
  wxMenu menu;
  menu.Append(kCopyRow, "Copy Row");
  menu.Append(kCopyTable, "Copy Table");

  // shows context menu
  // the event is caught by the report table
  PopupMenu(&menu, event.GetPoint());

  // stops processing event (needed to allow pop-up menu to catch its event)
  event.Skip();
}

void ReportTable::Sort() {
  // checks if sort column is selected
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
