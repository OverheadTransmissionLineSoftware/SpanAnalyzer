// This is free and unencumbered software released into the public domain.
// For more information, please refer to <http://unlicense.org/>

#ifndef SPANANALYZER_CABLE_CONSTRAINT_MANAGER_DIALOG_H_
#define SPANANALYZER_CABLE_CONSTRAINT_MANAGER_DIALOG_H_

#include <list>

#include "models/base/units.h"
#include "wx/listctrl.h"
#include "wx/wx.h"

#include "spananalyzer/span_analyzer_data.h"

/// \par OVERVIEW
///
/// This dialog manages the cable constraints and allows the user to edit them.
class CableConstraintManagerDialog : public wxDialog {
 public:
  /// \brief Constructor.
  /// \param[in] parent
  ///   The parent window.
  /// \param[in] cablefiles
  ///   The application cable files.
  /// \param[in] weathercases
  ///   The application weathercases.
  /// \param[in,out] constraints
  ///   The filter groups to be managed.
  CableConstraintManagerDialog(wxWindow* parent,
                               const std::list<CableFile*>* cablefiles,
                               const std::list<WeatherLoadCase*>* weathercases,
                               const units::UnitSystem& units,
                               std::list<CableConstraint>* constraints);

  /// \brief Destructor.
  ~CableConstraintManagerDialog();

 private:
  /// \brief Handles the add button event.
  /// \param[in] event
  ///   The event.
  void OnButtonAdd(wxCommandEvent& event);

  /// \brief Handles the delete button event.
  /// \param[in] event
  ///   The event.
  void OnButtonDelete(wxCommandEvent& event);

  /// \brief Handles the edit button event.
  /// \param[in] event
  ///   The event.
  void OnButtonEdit(wxCommandEvent& event);

  /// \brief Handles the cancel button event.
  /// \param[in] event
  ///   The event.
  void OnCancel(wxCommandEvent &event);

  /// \brief Handles the close event.
  /// \param[in] event
  ///   The event.
  void OnClose(wxCloseEvent& event);

  /// \brief Handles the listctrl select event.
  /// \param[in] event
  ///   The event.
  void OnListCtrlSelect(wxListEvent& event);

  /// \brief Handles the Ok button event.
  /// \param[in] event
  ///   The event.
  void OnOk(wxCommandEvent& event);

  /// \brief Handles the spin button down event.
  /// \param[in] event
  ///   The event.
  void OnSpinButtonDown(wxSpinEvent& event);

  /// \brief Handles the spin button up event.
  /// \param[in] event
  ///   The event.
  void OnSpinButtonUp(wxSpinEvent& event);

  /// \brief Updates the listctrl row.
  /// \param[in] index
  ///   The index.
  /// \param[in] constraint
  ///   The constraint.
  void UpdateListCtrlRow(const long& index, const CableConstraint& constraint);

  /// \var cablefiles_
  ///   The cable files.
  const std::list<CableFile*>* cablefiles_;

  /// \var constraints_
  ///   The cable constraints.
  std::list<CableConstraint>* constraints_;

  /// \var constraints_modified_
  ///   The user-modified constraints.
  std::list<CableConstraint> constraints_modified_;

  /// \var index_selected_
  ///   The index of the selected item in the listctrl.
  long index_selected_;

  /// \var listctrl_
  ///   The listctrl that displays the constraints.
  wxListCtrl* listctrl_;

  /// \var units_
  ///   The unit system.
  const units::UnitSystem* units_;

  /// \var weathercases_
  ///   The weathercases for reference only.
  const std::list<WeatherLoadCase*>* weathercases_;

  DECLARE_EVENT_TABLE()
};

#endif  // SPANANALYZER_CABLE_CONSTRAINT_MANAGER_DIALOG_H_
