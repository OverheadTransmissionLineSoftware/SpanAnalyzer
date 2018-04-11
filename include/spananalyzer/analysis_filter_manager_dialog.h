// This is free and unencumbered software released into the public domain.
// For more information, please refer to <http://unlicense.org/>

#ifndef OTLS_SPANANALYZER_ANALYSISFILTERMANAGERDIALOG_H_
#define OTLS_SPANANALYZER_ANALYSISFILTERMANAGERDIALOG_H_

#include <list>

#include "wx/listctrl.h"
#include "wx/wx.h"

#include "spananalyzer/span_analyzer_data.h"

/// \par OVERVIEW
///
class AnalysisFilterManagerDialog : public wxDialog {
 public:
  /// \brief Constructor.
  /// \param[in] parent
  ///   The parent window.
  /// \param[in] weathercases
  ///   The application weathercases.
  /// \param[in,out] groups_filters
  ///   The filter groups to be managed.
  AnalysisFilterManagerDialog(wxWindow* parent,
                              const std::list<WeatherLoadCase*>* weathercases,
                              std::list<AnalysisFilterGroup>* groups_filters);

  /// \brief Destructor.
  ~AnalysisFilterManagerDialog();

 private:
  /// \brief Handles the filter add button event.
  /// \param[in] event
  ///   The event.
  void OnButtonFilterAdd(wxCommandEvent& event);

  /// \brief Handles the filter delete button event.
  /// \param[in] event
  ///   The event.
  void OnButtonFilterDelete(wxCommandEvent& event);

  /// \brief Handles the filter edit button event.
  /// \param[in] event
  ///   The event.
  void OnButtonFilterEdit(wxCommandEvent& event);

  /// \brief Handles the group add button event.
  /// \param[in] event
  ///   The event.
  void OnButtonGroupAdd(wxCommandEvent& event);

  /// \brief Handles the group delete button event.
  /// \param[in] event
  ///   The event.
  void OnButtonGroupDelete(wxCommandEvent& event);

  /// \brief Handles the cancel button event.
  /// \param[in] event
  ///   The event.
  void OnCancel(wxCommandEvent &event);

  /// \brief Handles the close event.
  /// \param[in] event
  ///   The event.
  void OnClose(wxCloseEvent& event);

  /// \brief Handles the group listbox double click event.
  /// \param[in] event
  ///   The event.
  void OnListBoxGroupDoubleClick(wxCommandEvent& event);

  /// \brief Handles the group listbox selection event.
  /// \param[in] event
  ///   The event.
  void OnListBoxGroupSelect(wxCommandEvent& event);

  /// \brief Handles the filter listctrl select event.
  /// \param[in] event
  ///   The event.
  void OnListCtrlFilterSelect(wxListEvent& event);

  /// \brief Handles the Ok button event.
  /// \param[in] event
  ///   The event.
  void OnOk(wxCommandEvent& event);

  /// \brief Handles the filter spin button down event.
  /// \param[in] event
  ///   The event.
  void OnSpinButtonFilterDown(wxSpinEvent& event);

  /// \brief Handles the filter spin button up event.
  /// \param[in] event
  ///   The event.
  void OnSpinButtonFilterUp(wxSpinEvent& event);

  /// \brief Handles the group spin button down event.
  /// \param[in] event
  ///   The event.
  void OnSpinButtonGroupDown(wxSpinEvent& event);

  /// \brief Handles the group spin button up event.
  /// \param[in] event
  ///   The event.
  void OnSpinButtonGroupUp(wxSpinEvent& event);

  /// \brief Updates the listctrl row.
  /// \param[in] index
  ///   The index.
  /// \param[in] filter
  ///   The filter.
  void UpdateListCtrlRow(const long& index, const AnalysisFilter& filter);

  /// \var groups_
  ///   The filter groups.
  std::list<AnalysisFilterGroup>* groups_;

  /// \var groups_modified_
  ///   The user-modified analysis filter groups.
  std::list<AnalysisFilterGroup> groups_modified_;

  /// \var index_filter_selected_
  ///   The index of the selected item in the listctrl.
  long index_filter_selected_;

  /// \var index_group_selected_
  ///   The index of the activated group.
  int index_group_selected_;

  /// \var listbox_groups_
  ///   The listbox containing analysis filter groups.
  wxListBox* listbox_groups_;

  /// \var listctrl_filters_
  ///   The listbox containing filters.
  wxListCtrl* listctrl_filters_;

  /// \var weathercases_
  ///   The weathercases for reference only.
  const std::list<WeatherLoadCase*>* weathercases_;

  DECLARE_EVENT_TABLE()
};

#endif  // OTLS_SPANANALYZER_ANALYSISFILTERMANAGERDIALOG_H_
