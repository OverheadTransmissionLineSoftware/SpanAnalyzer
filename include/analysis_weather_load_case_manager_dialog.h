// This is free and unencumbered software released into the public domain.
// For more information, please refer to <http://unlicense.org/>

#ifndef OTLS_SPANANALYZER_ANALYSISWEATHERLOADCASEMANAGERDIALOG_H_
#define OTLS_SPANANALYZER_ANALYSISWEATHERLOADCASEMANAGERDIALOG_H_

#include "models/base/units.h"
#include "wx/wx.h"

#include "span_analyzer_data.h"

/// \par OVERVIEW
///
/// This dialog manages the application analysis weathercase sets and allows the
/// user to edit them.
class AnalysisWeatherLoadCaseManagerDialog : public wxDialog {
 public:
  /// \brief Constructor.
  AnalysisWeatherLoadCaseManagerDialog(
      wxWindow* parent,
      const units::UnitSystem& units,
      std::list<WeatherLoadCaseGroup>* groups_weathercase);

  /// \brief Destructor.
  ~AnalysisWeatherLoadCaseManagerDialog();

 private:
  /// \brief Handles the group add button event.
  /// \param[in] event
  ///   The event.
  void OnButtonGroupAdd(wxCommandEvent& event);

  /// \brief Handles the group delete button event.
  /// \param[in] event
  ///   The event.
  void OnButtonGroupDelete(wxCommandEvent& event);

  /// \brief Handles the weathercase add button event.
  /// \param[in] event
  ///   The event.
  void OnButtonWeathercaseAdd(wxCommandEvent& event);

  /// \brief Handles the weathercase delete button event.
  /// \param[in] event
  ///   The event.
  void OnButtonWeathercaseDelete(wxCommandEvent& event);

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

  /// \brief Handles the weathercase listbox double click event.
  /// \param[in] event
  ///   The event.
  void OnListBoxWeatherCaseDoubleClick(wxCommandEvent& event);

  /// \brief Handles the Ok button event.
  /// \param[in] event
  ///   The event.
  void OnOk(wxCommandEvent& event);

  /// \brief Handles the group spin button down event.
  /// \param[in] event
  ///   The event.
  void OnSpinButtonGroupDown(wxSpinEvent& event);

  /// \brief Handles the group spin button up event.
  /// \param[in] event
  ///   The event.
  void OnSpinButtonGroupUp(wxSpinEvent& event);

  /// \brief Handles the weathercase spin button down event.
  /// \param[in] event
  ///   The event.
  void OnSpinButtonWeathercaseDown(wxSpinEvent& event);

  /// \brief Handles the weathercase spin button up event.
  /// \param[in] event
  ///   The event.
  void OnSpinButtonWeathercaseUp(wxSpinEvent& event);

  /// \var descriptions_
  ///   The weathercase groups.
  std::list<WeatherLoadCaseGroup>* groups_;

  /// \var sets_modified_
  ///   The user-modified weathercase groups.
  std::list<WeatherLoadCaseGroup> groups_modified_;

  /// \var index_group_activated_
  ///   The index of the activated group.
  int index_group_activated_;

  /// \var listbox_groups_
  ///   The listbox containing weathercase groups.
  wxListBox* listbox_groups_;

  /// \var listbox_weathercases_
  ///   The listbox containing weathercases.
  wxListBox* listbox_weathercases_;

  /// \var units_
  ///   The unit system.
  const units::UnitSystem* units_;

  DECLARE_EVENT_TABLE()
};

#endif  // OTLS_SPANANALYZER_ANALYSISWEATHERLOADCASEMANAGERDIALOG_H_
