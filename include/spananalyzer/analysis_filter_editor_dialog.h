// This is free and unencumbered software released into the public domain.
// For more information, please refer to <http://unlicense.org/>

#ifndef SPANANALYZER_ANALYSIS_FILTER_EDITOR_DIALOG_H_
#define SPANANALYZER_ANALYSIS_FILTER_EDITOR_DIALOG_H_

#include <list>

#include "wx/wx.h"

#include "spananalyzer/span_analyzer_data.h"

/// \par OVERVIEW
///
/// This class is a analysis filter editor dialog.
///
/// \par GUI CONTROLS AND DATA TRANSFER
///
/// The form controls and layout are defined in the XRC file. The values are
/// transferred between the controls and the filter via validators.
class AnalysisFilterEditorDialog : public wxDialog {
 public:
  /// \brief Constructor.
  /// \param[in] parent
  ///   The parent window.
  /// \param[in] weathercases
  ///   The weathercases to select from.
  /// \param[in,out] filter
  ///   The analysis filter.
  AnalysisFilterEditorDialog(wxWindow* parent,
                             const std::list<WeatherLoadCase*>* weathercases,
                             AnalysisFilter* filter);

  /// \brief Destructor.
  ~AnalysisFilterEditorDialog();

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
  ///   the reference object.
  void TransferCustomDataFromWindow();

  /// \brief Transfers data that isn't tied to a validator from the reference
  ///   object to the window.
  void TransferCustomDataToWindow();

  /// \var filter_
  ///   The reference filter, which the user edits if changes are accepted.
  AnalysisFilter* filter_;

  /// \var filter_modified_
  ///   The filter that is modified and tied to the dialog controls.
  AnalysisFilter filter_modified_;

  /// \var weathercases_
  ///   The weathercases.
  const std::list<WeatherLoadCase*>* weathercases_;

  DECLARE_EVENT_TABLE()
};

#endif  // SPANANALYZER_ANALYSIS_FILTER_EDITOR_DIALOG_H_
