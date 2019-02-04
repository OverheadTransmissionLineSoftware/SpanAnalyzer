// This is free and unencumbered software released into the public domain.
// For more information, please refer to <http://unlicense.org/>

#ifndef SPANANALYZER_CABLE_CONSTRAINT_EDITOR_DIALOG_H_
#define SPANANALYZER_CABLE_CONSTRAINT_EDITOR_DIALOG_H_

#include <list>

#include "models/base/units.h"
#include "models/transmissionline/cable_constraint.h"
#include "wx/wx.h"

#include "spananalyzer/span_analyzer_data.h"

/// \par OVERVIEW
///
/// This class is a cable constraint editor dialog.
///
/// \par GUI CONTROLS AND DATA TRANSFER
///
/// The form controls and layout are defined in the XRC file. The values are
/// transferred between the controls and the converted cable data via
/// validators.
class CableConstraintEditorDialog : public wxDialog {
 public:
  /// \brief Constructor.
  /// \param[in] parent
  ///   The parent window.
  /// \param[in] cablefiles
  ///   The cable files available for reference.
  /// \param[in] weathercases
  ///   The weathercases available for reference.
  /// \param[in] units
  ///   The unit system to display on the form.
  /// \param[in,out] constraint
  ///   The constraint to be modified with the dialog.
  CableConstraintEditorDialog(wxWindow* parent,
                              const std::list<CableFile*>* cablefiles,
                              const std::list<WeatherLoadCase*>* weathercases,
                              const units::UnitSystem& units,
                              CableConstraint* constraint);

  /// \brief Destructor.
  ~CableConstraintEditorDialog();

 private:
  /// \brief Initializes the choice controls on the form.
  void InitChoiceControls();

  /// \brief Handles the cancel button event.
  /// \param[in] event
  ///   The event.
  void OnCancel(wxCommandEvent& event);

  /// \brief Handles the span type choice event.
  /// \param[in] event
  ///   The event.
  void OnChoiceType(wxCommandEvent& event);

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

  /// \brief Transfers data that isn't tied to a validator from the window to
  ///   the modified constraint object.
  void TransferCustomDataFromWindow();

  /// \brief Transfers data that isn't tied to a validator from the modified
  ///   constraint object to the window.
  void TransferCustomDataToWindow();

  /// \var cablefiles_
  ///   The cable files.
  const std::list<CableFile*>* cablefiles_;

  /// \var constraint_
  ///   The reference constraint, which the user edits if changes are accepted.
  CableConstraint* constraint_;

  /// \var constraint_modified_
  ///   The constraint that is modified and tied to the dialog controls.
  CableConstraint constraint_modified_;

  /// \var units_
  ///   The unit system.
  const units::UnitSystem* units_;

  /// \var weathercases_
  ///   The weathercases.
  const std::list<WeatherLoadCase*>* weathercases_;

  DECLARE_EVENT_TABLE()
};

#endif  // SPANANALYZER_CABLE_CONSTRAINT_EDITOR_DIALOG_H_
