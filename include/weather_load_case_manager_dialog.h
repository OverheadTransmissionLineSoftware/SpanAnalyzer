// This is free and unencumbered software released into the public domain.
// For more information, please refer to <http://unlicense.org/>

#ifndef OTLS_SPANANALYZER_WEATHERLOADCASEMANAGERDIALOG_H_
#define OTLS_SPANANALYZER_WEATHERLOADCASEMANAGERDIALOG_H_

#include "models/base/units.h"
#include "wx/wx.h"

#include "span_analyzer_data.h"

/// \par OVERVIEW
///
/// This dialog manages the application weathercase groups and allows the
/// user to edit them.
class WeatherLoadCaseManagerDialog : public wxDialog {
 public:
  /// \brief Constructor.
  WeatherLoadCaseManagerDialog(
      wxWindow* parent,
      const units::UnitSystem& units,
      std::list<WeatherLoadCaseGroup>* groups_weathercase);

  /// \brief Destructor.
  ~WeatherLoadCaseManagerDialog();

 private:
  /// \brief Determines if the cable is referenced.
  /// \param[in] name
  ///   The name to check.
  /// \return If the cable file is referenced by the document.
  /// This function scans the document to see if the cable is referenced.
  bool IsReferencedByDocument(const std::string& name) const;

  /// \brief Determines if the cable name is unique.
  /// \param[in] name
  ///   The name to check.
  /// \param[in] index_ignore
  ///   The index to skip (i.e. editing the name of an existing cable).
  /// \return If the cable name is unique.
  bool IsUniqueName(const std::string& name,
                    const int& index_ignore = -1) const;

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

  /// \var groups_
  ///   The weathercase groups.
  std::list<WeatherLoadCaseGroup>* groups_;

  /// \var groups_modified_
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

#endif  // OTLS_SPANANALYZER_WEATHERLOADCASEMANAGERDIALOG_H_
