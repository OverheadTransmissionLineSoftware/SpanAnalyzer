// This is free and unencumbered software released into the public domain.
// For more information, please refer to <http://unlicense.org/>

#ifndef OTLS_SPANANALYZER_REPORTTABLE_H_
#define OTLS_SPANANALYZER_REPORTTABLE_H_

#include <list>

#include "wx/listctrl.h"
#include "wx/wx.h"

/// \todo Rework how the columns are formatted.

/// \par OVERVIEW
///
/// This struct represents a single row of data.
struct ReportRow {
  /// \var values
  ///   The data values, which are stored as strings.
  std::list<std::string> values;
};

/// \par OVERVIEW
///
/// This struct
struct ReportData {
  /// \var headers
  ///   The column headers of the data.
  std::list<std::string> headers;

  /// \var rows
  ///   The data rows.
  std::list<ReportRow> rows;
};

/// \par OVERVIEW
///
/// This enum contains types of sort orders.
enum class SortOrderType {
  kAscending,
  kDescending,
  kNone
};

/// \par OVERVIEW
///
/// This class uses a listctrl to display data in a report view.
///
/// \par DATA
///
/// This class uses the data provided to generate listctrl entries. The listctrl
/// stores its own strings within the control, but it uses the associated data
/// to sort/organize as needed.
///
/// \par SORTING
///
/// Sorting is handled by the listctrl and does NOT affect the sort order of the
/// provided data, but rather, just inside the control itself. It is structured
/// this way to use the wxListCtrl::Sort function.
///
/// \par COPY TO CLIPBOARD
///
/// This class supports several copy to clipboard commands. All of the data is
/// taken from the listctrl itself, so it will reflect the sorted order (if
/// any).
class ReportTable : public wxPanel {
 public:
  /// \brief Constructor.
  ReportTable(wxWindow* parent);

  /// \brief Destructor.
  ~ReportTable();

  /// \brief Gets the focused listctrl item index.
  /// \return The focused item index. If no item is focused, -1 is returned.
  long IndexFocused() const;

  /// \brief Refreshes the listctrl with the data.
  void Refresh();

  /// \brief Gets the data that is displayed.
  const ReportData* data() const;

  /// \brief Gets the sorted column index.
  /// \return The sorted column index.
  long index_sorted() const;

  /// \brief Sets the data to be displayed.
  /// \param[in] data
  ///   The data.
  void set_data(const ReportData* data);

  /// \brief Sets the column formatting.
  /// \param[in] column
  ///   The column to format.
  /// \param[in] width
  ///   The width of the column.
  /// \param[in] align
  ///   The alignment style of the column.
  void set_formatting_column(const int& column, const int& width,
                             const wxListColumnFormat& align);

  /// \brief Sets the focused row index.
  /// \param[in] index
  ///   The index to be focused.
  void set_index_focused(const long& index);

  /// \brief Sets the sorted column index.
  /// \param[in] index
  ///   The index to be sorted.
  /// Requires a Refresh() in order to take effect.
  void set_index_sorted(const long& index);

  /// \brief Sets the sort type.
  /// \param[in] type_sort
  ///   The sort type.
  /// Requires a Refresh() in order to take effect.
  void set_type_sort(const SortOrderType& type_sort);

  /// \brief Gets the type of sorting being applied.
  /// \return The sort type that is being applied.
  SortOrderType type_sort() const;

private:
  /// \brief Changes the column header sort image.
  /// \param[in] index
  ///   The column index.
  /// \param[in] type_sort
  ///   The sort type.
  void ChangeColumnSortImage(const int& index, const SortOrderType& type_sort);

  /// \brief Clears the listctrl.
  void ClearListCtrl();

  /// \brief Gets a clipboard string for the column headers.
  /// \return A concatenated string. Starting from left to right, with each
  ///   value (column) separated a tab character. The end of the string will
  ///   contain a new line character.
  std::string ClipboardStringHeaders() const;

  /// \brief Gets a clipboard string for a specific row.
  /// \param[in] index
  ///   The row index.
  /// \return A concatenated string. Starting from left to right, with each
  ///   value (column) separated by a tab character. The end of the string will
  ///   contain a new line character.
  std::string ClipboardStringRow(const int& index) const;

  /// \brief Gets a clipboard string for the entire table, excluding headers.
  /// \return A concatenated string. Starting from left to right, with each
  ///   value (column) separated by a tab character. The end of a row will
  ///   contain a new line character.
  std::string ClipBoardStringTable() const;

  /// \brief Copies the string to the clipboard.
  /// \param[in] str
  ///   The string.
  void CopyToClipboard(const std::string& str);

  /// \brief Handles the column click event.
  /// \param[in] event
  ///   The event.
  void OnColumnClick(wxListEvent& event);

  /// \brief Handles the column click event.
  /// \param[in] event
  ///   The event.
  void OnColumnRightClick(wxListEvent& event);

  /// \brief Handles the context menu select event.
  /// \param[in] event
  ///   The event.
  void OnContextMenuSelect(wxCommandEvent& event);

  /// \brief Handles the column right click event.
  /// \param[in] event
  ///   The event.
  void OnItemRightClick(wxListEvent& event);

  /// \brief Sorts the listctrl.
  void Sort();

  /// \var data_
  ///   The data that is passed onto the listctrl.
  const ReportData* data_;

  /// \var index_sorted_
  ///   The column index that is currently sorted. Set to -1 if none are sorted.
  int index_sorted_;

  /// \var type_sort_
  ///   The type of sorting that is currently applied.
  SortOrderType type_sort_;

  /// \var listctrl_
  ///   The listctrl that is used to display the data.
  wxListCtrl* listctrl_;

  DECLARE_EVENT_TABLE()
};

#endif  // OTLS_SPANANALYZER_REPORTTABLE_H_
