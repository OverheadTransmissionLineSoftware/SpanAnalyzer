// This is free and unencumbered software released into the public domain.
// For more information, please refer to <http://unlicense.org/>

#ifndef OTLS_SPANANALYZER_SPANEDITORDIALOG_H_
#define OTLS_SPANANALYZER_SPANEDITORDIALOG_H_

#include <vector>

#include "models/base/units.h"
#include "wx/wx.h"

#include "span.h"

/// \par OVERVIEW
///
/// This class is a transmission cable editor dialog.
///
/// \par GUI CONTROLS AND DATA TRANSFER
///
/// The form controls and layout are defined in the XRC file. The values are
/// transferred between the controls and the converted cable data via
/// validators.
class SpanEditorDialog : public wxDialog {
 public:
  /// \brief Default constructor.
  /// \param[in] parent
  ///   The parent window.
  /// \param[in] linecable
  ///   The line cable to be modified with the dialog.
  /// \param[in] units
  ///   The unit system to display on the form.

  /// \todo re-order this so that the span is the last parameter
  ///   span is [in,out]
  SpanEditorDialog(wxWindow* parent,
                   const std::vector<Cable>* cables,
                   const std::vector<WeatherLoadCase>* weathercases,
                   const units::UnitSystem& units,
                   Span* span);

  /// \brief Destructor.
  ~SpanEditorDialog();

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

  void InitChoiceControls();

  void TransferCustomDataToWindow();

  void TransferCustomDataFromWindow();

  const std::vector<WeatherLoadCase>* weathercases_;

  const std::vector<Cable>* cables_;

  /// \var linecable_
  ///   The line cable that is edited.
  Span* span_;

  /// \var linecable_modified_
  ///   The cable that is modified and tied to the dialog controls.
  Span span_modified_;

  DECLARE_EVENT_TABLE()
};

#endif  // OTLS_SPANANALYZER_SPANEDITORDIALOG_H_
