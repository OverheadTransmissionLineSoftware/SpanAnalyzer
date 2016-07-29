// This is free and unencumbered software released into the public domain.
// For more information, please refer to <http://unlicense.org/>

#ifndef OTLS_SPANANALYZER_REPORTTABLE_H_
#define OTLS_SPANANALYZER_REPORTTABLE_H_

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
/// Srting is handled by the listctrl and does NOT affect the sort order of the
/// provided data, but rather, just inside the control itself. It is structured
/// this way to use the wxListCtrl::Sort function.
///
/// \par COPY TO CLIPBOARD
///
/// This class supports several copy to clipboard commands: copy a row, column,
/// or the entire table. All of the data is taken from the listctrl itself, so
/// it will reflect the sorted order (if any).
class ReportTable : public wxPanel {
 public:
  /// \brief Constructor.
  ReportTable(wxWindow* parent);

  /// \brief Destructor.
  ~ReportTable();

  /// \brief Refreshes the listctrl with the data.
  void Refresh();

  /// \brief Gets the data that is displayed.
  const ReportData* data() const;

  /// \brief Sets the data to be displayed.
  /// \param[n] data
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

 private:
  /// \brief Clears the listctrl.
  void ClearListCtrl();

  /// \brief Copies table information to the clipboard.
  /// This information is copied directly from the listctrl, so it will reflect
  /// sorting.
  void CopyReportToClipboard(const bool& is_included_headers = false) const;

  /// \brief Copies the row to the clipboard.
  /// This information is copied directly from the listctrl, so it will reflect
  /// sorting.
  void CopyRowToClipboard() const;

  /// \brief Handles the column click event.
  /// \param[in] event
  ///   The event.
  void OnColumnClick(wxListEvent& event);

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

  /// \var index_sorted_
  ///   The column index that is currently sorted. Set to -1 if none are sorted.
  int index_sorted_;

  /// \var data_
  ///   The data that is passed onto the listctrl.
  const ReportData* data_;

  /// \var type_sort_
  ///   The type of sorting that is currently applied.
  SortOrderType type_sort_;

  /// \var listctrl_
  ///   The listctrl that is used to display the data.
  wxListCtrl* listctrl_;

  DECLARE_EVENT_TABLE()
};

#endif  // OTLS_SPANANALYZER_REPORTTABLE_H_
