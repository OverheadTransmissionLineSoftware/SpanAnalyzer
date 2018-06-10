// This is free and unencumbered software released into the public domain.
// For more information, please refer to <http://unlicense.org/>

#ifndef SPANANALYZER_WEATHER_LOAD_CASE_MANAGER_DIALOG_H_
#define SPANANALYZER_WEATHER_LOAD_CASE_MANAGER_DIALOG_H_

#include <list>
#include <string>

#include "models/base/units.h"
#include "wx/wx.h"

#include "spananalyzer/span_analyzer_data.h"

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
      std::list<WeatherLoadCase*>* weathercases);

  /// \brief Destructor.
  ~WeatherLoadCaseManagerDialog();

 private:
  /// \brief Deletes any extra weathercases.
  /// \param[in] list_keep
  ///   The list of weathercases to keep.
  /// \param[in] list_master
  ///   The master list that contains all of the weathercases.
  /// This function will free the allocated memory as well.
  void DeleteExtraWeathercases(const std::list<WeatherLoadCase*>* list_keep,
                               std::list<WeatherLoadCase*>* list_master);

  /// \brief Determines if the weathercase is referemced by analysis filters.
  /// \param[in] name
  ///   The name to check.
  /// \return If the weathercase is referenced by analysis filters.
  bool IsReferencedByAnalysisFilters(const std::string& name) const;

  /// \brief Determines if the weathercase is referenced by the document.
  /// \param[in] name
  ///   The name to check.
  /// \return If the weathercase is referenced by the document.
  /// This function scans the document to see if the weathercase is referenced.
  bool IsReferencedByDocument(const std::string& name) const;

  /// \brief Determines if the weathercase name is unique.
  /// \param[in] name
  ///   The name to check.
  /// \param[in] index_ignore
  ///   The index to skip (i.e. editing the name of an existing weathercase).
  /// \return If the weathercase is unique.
  bool IsUniqueName(const std::string& name,
                    const int& index_ignore = -1) const;

  /// \brief Handles the weathercase add button event.
  /// \param[in] event
  ///   The event.
  void OnButtonAdd(wxCommandEvent& event);

  /// \brief Handles the weathercase delete button event.
  /// \param[in] event
  ///   The event.
  void OnButtonDelete(wxCommandEvent& event);

  /// \brief Handles the cancel button event.
  /// \param[in] event
  ///   The event.
  void OnCancel(wxCommandEvent &event);

  /// \brief Handles the close event.
  /// \param[in] event
  ///   The event.
  void OnClose(wxCloseEvent& event);

  /// \brief Handles the weathercase listbox double click event.
  /// \param[in] event
  ///   The event.
  void OnListBoxDoubleClick(wxCommandEvent& event);

  /// \brief Handles the weathercase listbox selection event.
  /// \param[in] event
  ///   The event.
  void OnListBoxSelect(wxCommandEvent& event);

  /// \brief Handles the Ok button event.
  /// \param[in] event
  ///   The event.
  void OnOk(wxCommandEvent& event);

  /// \brief Handles the weathercase spin button down event.
  /// \param[in] event
  ///   The event.
  void OnSpinButtonDown(wxSpinEvent& event);

  /// \brief Handles the weathercase spin button up event.
  /// \param[in] event
  ///   The event.
  void OnSpinButtonUp(wxSpinEvent& event);

  /// \var index_selected_
  ///   The selected listbox item.
  int index_selected_;

  /// \var listbox_weathercases_
  ///   The listbox containing weathercases.
  wxListBox* listbox_weathercases_;

  /// \var units_
  ///   The unit system.
  const units::UnitSystem* units_;

  /// \var weathercases_
  ///   The master list of weathercases allocated by the application. This list
  ///   includes the weathercases that are in the original and modified lists.
  ///   When exiting this dialog, any extra weathercases will be deallocated.
  std::list<WeatherLoadCase*>* weathercases_;

  /// \var weathercases_modified_
  ///   The weathercases that are modified by the manager (presented to the
  ///   user). This list is committed to the master list only when the user
  ///   selects OKon the form.
  std::list<WeatherLoadCase*> weathercases_modified_;

  /// \var weathercases_original_
  ///   The weathercases that are originally passed to the dialog. This list is
  ///   committed to the master list when the user cancels or closes the form.
  std::list<WeatherLoadCase*> weathercases_original_;

  DECLARE_EVENT_TABLE()
};

#endif  // SPANANALYZER_WEATHER_LOAD_CASE_MANAGER_DIALOG_H_
