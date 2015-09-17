// This is free and unencumbered software released into the public domain.
// For more information, please refer to <http://unlicense.org/>

#ifndef TLS_SPANANALYZER_CABLEEDITORDIALOG_H_
#define TLS_SPANANALYZER_CABLEEDITORDIALOG_H_

//***** TEMPORARY
enum class UnitSystem {
  Imperial,
  Metric
};
//***** TEMPORARY

#include "transmissionline/cable.h"
#include "wx/wx.h"

/// \par OVERVIEW
///
/// This class is a transmission cable editor dialog.
///
/// \par OVERVIEW
///
/// This class supports both Imperial and Metric unit systems. This class also
/// supports similar-system unit conversions (ex: feet->inches) from the cable
/// data to the dialog controls. This is done so the user can input using
/// preferred units while maintaining compatibility with the consistent unit
/// calculation libraries. The base units are:
///   - Length = [feet/meters]
///   - Load = [lbs/N]
///   - Temperature = [deg F / deg C]
class CableEditorDialog : public wxDialog {
 public:
  /// \brief Default constructor.
  /// \param[in] cable
  ///   The cable to be modified with the dialog.
  /// \param[in] units
  ///   The unit system to display on the form.
  /// \param[in] convert_units
  ///   The indicator that tells if the units should be converted between the
  ///   controls and cable.
  CableEditorDialog(wxWindow* parent, Cable* cable,
                    const UnitSystem& units, const bool& convert_units = true);

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
  void UpdateUnitsStaticText(UnitSystem& units);

  ///// \brief
  //void TransferControlsDataToCable();

  ///// \brief Transfers the cable data to the dialog controls.
  ///// \param[in] 
  //void TransferCableDataToControls();

  Cable* cable_;
  
  wxString name_;
  wxString area_electrical_;
  wxString area_physical_;
  wxString diameter_;
  wxString weight_unit_;

  UnitSystem units_;
  bool convert_units_;

  DECLARE_EVENT_TABLE()
};

#endif  // TLS_SPANANALYZER_CABLEEDITORDIALOG_H_
