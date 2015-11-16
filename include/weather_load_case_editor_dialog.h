// This is free and unencumbered software released into the public domain.
// For more information, please refer to <http://unlicense.org/>

#ifndef OTLS_SPANANALYZER_WEATHERLOADCASEEDITORDIALOG_H_
#define OTLS_SPANANALYZER_WEATHERLOADCASEEDITORDIALOG_H_

#include "models/base/units.h"
#include "models/transmissionline/weather_load_case.h"
#include "wx/wx.h"

/// \par OVERVIEW
///
/// This class is a weather load case editor dialog.
///
/// \par GUI CONTROLS AND DATA TRANSFER
///
/// The form controls and layout are defined in the XRC file. The values are
/// transferred between the controls and the converted cable data via
/// validators.
class WeatherLoadCaseEditorDialog : public wxDialog {
 public:
  /// \brief Constructor.
  WeatherLoadCaseEditorDialog(wxWindow* parent, WeatherLoadCase* weathercase,
                              const units::UnitSystem units);

  /// \brief Destructor.
  ~WeatherLoadCaseEditorDialog();

 private:
  /// \brief Handles the cancel button event.
  /// \param[in] event
  ///   The event.
  void OnCancel(wxCommandEvent& event);

  /// \brief Handles the close event.
  /// \param[in] event
  ///   The event.
  void OnClose(wxCloseEvent& event);

  /// \brief Handles the Ok button event.
  /// \param[in] event
  ///   The event.
  void OnOk(wxCommandEvent& event);

  /// \brief Changes static text to display the desired unit system.
  /// \param[in] units
  ///   The unit system to display.
  void SetUnitsStaticText(const units::UnitSystem& units);

  /// \brief Sets the control validators on the form.
  void SetValidators();

  /// \var name_
  ///   The wxString version of the weathercase name. This is tied to a form
  ///    control.
  wxString name_;

  /// \var weathercase_
  ///   The reference weathercase, which the user edits if changes are accepted.
  WeatherLoadCase* weathercase_;

  /// \var weathercase_modified_
  ///   The weathercase that is modified and tied to the dialog controls.
  WeatherLoadCase weathercase_modified_;

  DECLARE_EVENT_TABLE()
};

#endif  // OTLS_SPANANALYZER_WEATHERLOADCASEEDITORDIALOG_H_
