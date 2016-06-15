// This is free and unencumbered software released into the public domain.
// For more information, please refer to <http://unlicense.org/>

#ifndef OTLS_SPANANALYZER_CABLEFILEMANAGERDIALOG_H_
#define OTLS_SPANANALYZER_CABLEFILEMANAGERDIALOG_H_

#include <list>

#include "models/base/units.h"
#include "wx/listctrl.h"
#include "wx/wx.h"

#include "span_analyzer_data.h"

/// \par OVERVIEW
///
/// This class manages the cable files used by the application.
///
/// The user can select the files and modify the cables.
class CableFileManagerDialog : public wxDialog {
 public:
  /// \brief Constructor.
  CableFileManagerDialog(wxWindow* parent,
                         const units::UnitSystem& units,
                         std::list<CableFile>* cablefiles);

  /// \brief Destructor.
  ~CableFileManagerDialog();

 private:
  /// \brief Handles the add button event.
  /// \param[in] event
  ///   The event.
  void OnButtonAdd(wxCommandEvent& event);

  /// \brief Handles the cancel button event.
  /// \param[in] event
  ///   The event.
  void OnButtonCancel(wxCommandEvent& event);

  /// \brief Handles the edit button event.
  /// \param[in] event
  ///   The event.
  void OnButtonEdit(wxCommandEvent& event);

  /// \brief Handles the new button event.
  /// \param[in] event
  ///   The event.
  void OnButtonNew(wxCommandEvent& event);

  /// \brief Handles the Ok button event.
  /// \param[in] event
  ///   The event.
  void OnButtonOk(wxCommandEvent& event);

  /// \brief Handles the remove button event.
  /// \param[in] event
  ///   The event.
  void OnButtonRemove(wxCommandEvent& event);

  /// \brief Handles the close event.
  /// \param[in] event
  ///   The event.
  void OnClose(wxCloseEvent& event);

  /// \brief Handles the item selection event.
  /// \param[in] event
  ///   The event.
  void OnItemSelect(wxListEvent& event);

  /// \var index_selected_
  ///   The index of the selected item in the listctrl.
  long index_selected_;

  /// \var cablefiles_
  ///   The application cables.
  std::list<CableFile>* cablefiles_;

  /// \var cablefiless_modified_
  ///   The cables that are modified by the manager.
  std::list<CableFile> cablefiles_modified_;

  /// \var listctrl_
  ///   The listctrl that displays the cables.
  wxListCtrl* listctrl_;

  /// \var units_
  ///   The unit system.
  units::UnitSystem units_;

  DECLARE_EVENT_TABLE()
};

#endif  // OTLS_SPANANALYZER_CABLEFILEMANAGERDIALOG_H_
