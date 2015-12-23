// This is free and unencumbered software released into the public domain.
// For more information, please refer to <http://unlicense.org/>

#ifndef OTLS_SPANANALYZER_CABLEDIRECTORYEDITORDIALOG_H_
#define OTLS_SPANANALYZER_CABLEDIRECTORYEDITORDIALOG_H_

#include "wx/filepicker.h"
#include "wx/wx.h"

/// \par OVERVIEW
///
/// This is a dialog that edits the cable directory.
class CableDirectoryEditorDialog : public wxDialog {
 public:
  /// \brief Constructor.
  CableDirectoryEditorDialog(wxWindow* parent,
                             std::string* directory);

  /// \brief Destructor.
  ~CableDirectoryEditorDialog();

 private:
  /// \brief Handles the cancel button event.
  /// \param[in] event
  ///   The event.
  void OnButtonCancel(wxCommandEvent& event);

  /// \brief Handles the Ok button event.
  /// \param[in] event
  ///   The event.
  void OnButtonOk(wxCommandEvent& event);

  /// \brief Handles the close event.
  /// \param[in] event
  ///   The event.
  void OnClose(wxCloseEvent& event);

  /// \brief Handles the directory picker change event.
  /// \param[in] event
  ///   The event.
  void OnDirPickerCtrlChange(wxFileDirPickerEvent& event);

  /// \brief Handles the listbox double click event.
  /// \param[in] event
  ///   The event.
  void OnListBoxDoubleClick(wxCommandEvent& event);

  /// \var directory_
  ///   The cable directory.
  std::string* directory_;

  DECLARE_EVENT_TABLE()
};

#endif  // OTLS_SPANANALYZER_CABLEDIRECTORYEDITORDIALOG_H_
