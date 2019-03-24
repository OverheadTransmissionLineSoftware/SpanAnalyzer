// This is free and unencumbered software released into the public domain.
// For more information, please refer to <http://unlicense.org/>

#ifndef SPANANALYZER_PLAN_PLOT_OPTIONS_DIALOG_H_
#define SPANANALYZER_PLAN_PLOT_OPTIONS_DIALOG_H_

#include "wx/wx.h"

#include "spananalyzer/span_analyzer_config.h"

/// \par OVERVIEW
///
/// This class is a plan plot pane options dialog.
///
/// \par GUI CONTROLS AND DATA TRANSFER
///
/// The form controls and layout are defined in the XRC file. The values are
/// transferred between the controls and the options data by class methods.
class PlanPlotOptionsDialog : public wxDialog {
 public:
  /// \brief Constructor.
  /// \param[in] parent
  ///   The parent window.
  /// \param[in] options
  ///   The plot options.
  PlanPlotOptionsDialog(wxWindow* parent, PlanPlotOptions* options);

  /// \brief Destructor.
  ~PlanPlotOptionsDialog();

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

  /// \brief Transfers data that isn't tied to a validator from the window to
  ///   the modified options object.
  void TransferCustomDataFromWindow();

  /// \brief Transfers data that isn't tied to a validator from the modified
  ///   options object to the window.
  void TransferCustomDataToWindow();

  /// \var options_
  ///   The reference options, which the user edits if changes are accepted.
  PlanPlotOptions* options_;

  /// \var options_modified_
  ///   The options that are modified and tied to the dialog controls.
  PlanPlotOptions options_modified_;

  DECLARE_EVENT_TABLE()
};

#endif  // SPANANALYZER_PLAN_PLOT_OPTIONS_DIALOG_H_
