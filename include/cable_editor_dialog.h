// This is free and unencumbered software released into the public domain.
// For more information, please refer to <http://unlicense.org/>

#ifndef OTLS_SPANANALYZER_CABLEEDITORDIALOG_H_
#define OTLS_SPANANALYZER_CABLEEDITORDIALOG_H_

#include "models/base/units.h"
#include "models/transmissionline/cable.h"
#include "wx/wx.h"

/// \par OVERVIEW
///
/// This class is a transmission cable editor dialog.
///
/// \par UNITS
///
/// This class supports both Imperial and Metric unit systems. This class also
/// supports similar-system unit conversions (ex: meters->millimeters). This is
/// done so the user can input using preferred units while maintaining
/// compatibility with the modeling libraries that require consistent units.
/// The expected consistent units are:
///   - Length = [meters or feet]
///   - Load = [N or lbs]
///   - Temperature = [deg C or deg F]
///
/// \par GUI CONTROLS AND DATA TRANSFER
///
/// The form controls and layout are defined in the XRC file. The values are
/// transferred between the controls and the converted cable data via
/// validators.
class CableEditorDialog : public wxDialog {
 public:
  /// \brief Default constructor.
  /// \param[in] cable
  ///   The cable to be modified with the dialog.
  /// \param[in] units
  ///   The unit system to display on the form.
  CableEditorDialog(wxWindow* parent, Cable* cable,
                    const units::UnitSystem& units);

  /// \brief Destructor.
  ~CableEditorDialog() {};

 private:
  /// \brief Converts the cable from consistent to dialog form units.
  /// \param[in] cable_reference
  ///   The reference cable, which should have dialog units.
  /// \param[out] cable_converted
  ///   The converted cable, which will have consistent units.
  void ConvertCableToConsistentUnits(const Cable& cable_reference,
                                     Cable& cable_converted);

  /// \brief Converts the cable from consistent to dialog form units.
  /// \param[in] cable_reference
  ///   The reference cable, which should have consistent units.
  /// \param[out] cable_converted
  ///   The converted cable, which will have dialog units.
  void ConvertCableToDialogUnits(const Cable& cable_reference,
                                 Cable& cable_converted);

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
  void SetUnitsStaticText(units::UnitSystem& units);

  /// \brief Sets the control validators on the form.
  void SetValidators();

  /// \var cable_
  ///   The cable that is edited.
  Cable* cable_;

  /// \var cable_converted_
  ///   The cable that contains the converted (dialog) units. The data stored
  ///   in this is tied to the dialog controls.
  Cable cable_converted_;

  /// \var name_
  ///   The control data that contains the cable name.
  wxString name_;

  /// \var units_
  ///   The unit system to display and convert to/from.
  units::UnitSystem units_;

  DECLARE_EVENT_TABLE()
};

#endif  // OTLS_SPANANALYZER_CABLEEDITORDIALOG_H_
