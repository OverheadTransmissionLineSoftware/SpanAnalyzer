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
/// \par GUI CONTROLS AND DATA TRANSFER
///
/// The form controls and layout are defined in the XRC file. The values are
/// transferred between the controls and the converted cable data via
/// validators.
class CableEditorDialog : public wxDialog {
 public:
  /// \brief Default constructor.
  /// \param[in] parent
  ///   The parent window.
  /// \param[in] cable
  ///   The cable to be modified with the dialog.
  /// \param[in] units
  ///   The unit system to display on the form.
  CableEditorDialog(wxWindow* parent, Cable* cable,
                    const units::UnitSystem& units);

  /// \brief Destructor.
  ~CableEditorDialog() {};

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

  /// \var cable_
  ///   The cable that is edited.
  Cable* cable_;

  /// \var cable_modified_
  ///   The cable that is modified and tied to the dialog controls.
  Cable cable_modified_;

  /// \var name_
  ///   The wxString version of the cable name. This is tied to a form control.
  wxString name_;

  DECLARE_EVENT_TABLE()
};

#endif  // OTLS_SPANANALYZER_CABLEEDITORDIALOG_H_
