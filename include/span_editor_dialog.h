// This is free and unencumbered software released into the public domain.
// For more information, please refer to <http://unlicense.org/>

#ifndef OTLS_SPANANALYZER_SPANEDITORDIALOG_H_
#define OTLS_SPANANALYZER_SPANEDITORDIALOG_H_

#include <list>

#include "models/base/units.h"
#include "wx/wx.h"

#include "span.h"
#include "span_analyzer_data.h"

/// \par OVERVIEW
///
/// This class is a span editor dialog.
///
/// \par GUI CONTROLS AND DATA TRANSFER
///
/// The form controls and layout are defined in the XRC file. The values are
/// transferred between the controls and the converted cable data via
/// validators.
///
/// \par SPAN TYPES
///
/// This editor allows two different types/modes for the span - ruling and
/// dead-end spans. This form toggles between the different modes by enabling
/// and disabling controls as needed.
///
/// Ruling spans have no vertical sag-tension geometry, but allow the catenary
/// geometry to be different. Dead-end spans have a vertical sag-tension
/// geometry, but the catenary geometry is identical.
class SpanEditorDialog : public wxDialog {
 public:
  /// \brief Default constructor.
  /// \param[in] parent
  ///   The parent window.
  /// \param[in] cablefiles
  ///   The cable files available for reference.
  /// \param[in] weathercases
  ///   The weathercases available for reference.
  /// \param[in] units
  ///   The unit system to display on the form.
  /// \param[in] span
  ///   The span to be modified with the dialog.
  SpanEditorDialog(wxWindow* parent,
                   const std::list<CableFile*>* cablefiles,
                   const std::list<WeatherLoadCase*>* weathercases,
                   const units::UnitSystem& units,
                   Span* span);

  /// \brief Destructor.
  ~SpanEditorDialog();

 private:
  /// \brief Initializes the choice controls on the form.
  void InitChoiceControls();

  /// \brief Handles the cancel button event.
  /// \param[in] event
  ///   The event.
  void OnCancel(wxCommandEvent& event);

  /// \brief Handles the close event.
  /// \param[in] event
  ///   The event.
  void OnClose(wxCloseEvent& event);

  /// \brief Handles the match constraint geometry checkbox event.
  /// \param[in] event
  ///   The event.
  void OnCheckboxMatchGeometry(wxCommandEvent& event);

  /// \brief Handles the span type choice event.
  /// \param[in] event
  ///   The event.
  void OnChoiceType(wxCommandEvent& event);

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

  /// \brief Toggles the catenary geometry controls.
  /// \param[in] is_enabled
  ///   Whether the controls are enabled.
  void ToggleCatenaryGeometrySpacingControls(const bool& is_enabled);

  /// \brief Toggles the controls to dead-end span mode.
  /// Controls are disabled, but the values are not cleared.
  void ToggleDeadEndSpanModeControls();

  /// \brief Toggles the controls to ruling span mode.
  /// Controls are disabled, but the values are not cleared.
  void ToggleRulingSpanModeControls();

  /// \brief Transfers data that isn't tied to a validator from the window to
  ///   the span object.
  void TransferCustomDataFromWindow();

  /// \brief Transfers data that isn't tied to a validator from the span object
  ///   to the window.
  void TransferCustomDataToWindow();

  /// \var cablefiles_
  ///   The cable files.
  const std::list<CableFile*>* cablefiles_;

  /// \var span_
  ///   The span that is edited.
  Span* span_;

  /// \var span_modified_
  ///   The span that is modified and tied to the dialog controls.
  Span span_modified_;

  /// \var weathercases_
  ///   The weathercases.
  const std::list<WeatherLoadCase*>* weathercases_;

  DECLARE_EVENT_TABLE()
};

#endif  // OTLS_SPANANALYZER_SPANEDITORDIALOG_H_
