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
                         std::list<CableFile*>* cablefiles);

  /// \brief Destructor.
  ~CableFileManagerDialog();

 private:
  /// \brief Deletes any extra cable files.
  /// \param[in] list_keep
  ///   The list of cable files to keep.
  /// \param[in] list_master
  ///   The master list that contains all of the cable files.
  /// This function will free the allocated memory as well.
  void DeleteExtraCableFiles(const std::list<CableFile*>* list_keep,
                             std::list<CableFile*>* list_master);

  /// \brief Determines if the cable is referenced.
  /// \param[in] name
  ///   The name to check.
  /// \return If the cable file is referenced by the document.
  /// This function scans the document to see if the cable is referenced.
  bool IsReferencedByDocument(const std::string& name) const;

  /// \brief Determines if the cable name is unique.
  /// \param[in] name
  ///   The name to check.
  /// \param[in] index_ignore
  ///   The index to skip (i.e. editing the name of an existing cable).
  /// \return If the cable name is unique.
  bool IsUniqueName(const std::string& name,
                    const int& index_ignore = -1) const;

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

  /// \brief Handles the set spin button down event.
  /// \param[in] event
  ///   The event.
  void OnSpinButtonDown(wxSpinEvent& event);

  /// \brief Handles the set spin button up event.
  /// \param[in] event
  ///   The event.
  void OnSpinButtonUp(wxSpinEvent& event);

  /// \var cablefiles_
  ///   The master list of cable files allocated by the application. This list
  ///   includes the cable files that are in the original and modified lists.
  ///   When exiting this dialog, any extra cable files will be deallocated.
  std::list<CableFile*>* cablefiles_;

  /// \var cablefiles_modified_
  ///   The cables that are modified by the manager (presented to the user).
  ///   This list is committed to the master list only when the user selects OK
  ///   on the form.
  std::list<CableFile*> cablefiles_modified_;

  /// \var cablefiles_original_
  ///   The cable files that are originally passed to the dialog. This list is
  ///   committed to the master list when the user cancels or closes the form.
  std::list<CableFile*> cablefiles_original_;

  /// \var listctrl_
  ///   The listctrl that displays the cables.
  wxListCtrl* listctrl_;

  /// \var index_selected_
  ///   The index of the selected item in the listctrl.
  long index_selected_;

  /// \var units_
  ///   The unit system.
  units::UnitSystem units_;

  DECLARE_EVENT_TABLE()
};

#endif  // OTLS_SPANANALYZER_CABLEFILEMANAGERDIALOG_H_
