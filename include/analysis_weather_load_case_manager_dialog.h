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
      std::list<std::string>* descriptions_weathercases_analysis,
      std::list<std::list<WeatherLoadCase>>* weathercases_analysis);

  /// \brief Destructor.
  ~AnalysisWeatherLoadCaseManagerDialog();

 private:
  /// \brief Handles the set add button event.
  /// \param[in] event
  ///   The event.
  void OnButtonSetAdd(wxCommandEvent& event);

  /// \brief Handles the set delete button event.
  /// \param[in] event
  ///   The event.
  void OnButtonSetDelete(wxCommandEvent& event);

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

  /// \brief Handles the set listbox selection event.
  /// \param[in] event
  ///   The event.
  void OnListBoxSetSelect(wxCommandEvent& event);

  /// \brief Handles the weathercase listbox double click event.
  /// \param[in] event
  ///   The event.
  void OnListBoxWeatherCaseDoubleClick(wxCommandEvent& event);

  /// \brief Handles the Ok button event.
  /// \param[in] event
  ///   The event.
  void OnOk(wxCommandEvent& event);

  /// \brief Handles the set spin button down event.
  /// \param[in] event
  ///   The event.
  void OnSpinButtonSetDown(wxSpinEvent& event);

  /// \brief Handles the set spin button up event.
  /// \param[in] event
  ///   The event.
  void OnSpinButtonSetUp(wxSpinEvent& event);

  /// \brief Handles the weathercase spin button down event.
  /// \param[in] event
  ///   The event.
  void OnSpinButtonWeathercaseDown(wxSpinEvent& event);

  /// \brief Handles the weathercase spin button up event.
  /// \param[in] event
  ///   The event.
  void OnSpinButtonWeathercaseUp(wxSpinEvent& event);

  /// \var descriptions_
  ///   The weathercase set descriptions.
  std::list<std::string>* descriptions_;

  /// \var descriptions_modified_
  ///   The user-modified weathercase set descriptions.
  std::list<std::string> descriptions_modified_;

  /// \var listbox_sets_
  ///   The listbox containing weathercase sets.
  wxListBox* listbox_sets_;

  /// \var listbox_weathercases_
  ///   The listbox containing weathercases.
  wxListBox* listbox_weathercases_;

  /// \var index_set_activated_
  ///   The index of the activated set.
  int index_set_activated_;

  /// \var sets_
  ///   The analysis weathercase sets.
  std::list<std::list<WeatherLoadCase>>* sets_;

  /// \var sets_modified_
  ///   The user-modified analysis weathercase sets.
  std::list<std::list<WeatherLoadCase>> sets_modified_;

  /// \var units_
  ///   The unit system.
  const units::UnitSystem* units_;

  DECLARE_EVENT_TABLE()
};

#endif  // OTLS_SPANANALYZER_ANALYSISWEATHERLOADCASEMANAGERDIALOG_H_
