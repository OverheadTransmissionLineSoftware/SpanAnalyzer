// This is free and unencumbered software released into the public domain.
// For more information, please refer to <http://unlicense.org/>

#ifndef TLS_SPANANALYZER_WEATHERCASEEDITORDIALOG_H_
#define TLS_SPANANALYZER_WEATHERCASEEDITORDIALOG_H_

#include "transmissionline/weather_load_case.h"
#include "wx/wx.h"

class WeatherCaseEditorDialog : public wxDialog {
 public:
  /// \brief Constructor.
  WeatherCaseEditorDialog(wxWindow* parent, WeatherLoadCase* weathercase);

  /// \brief Destructor.
  ~WeatherCaseEditorDialog();

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

  DECLARE_EVENT_TABLE()
};

#endif  // TLS_SPANANALYZER_WEATHERCASEEDITORDIALOG_H_