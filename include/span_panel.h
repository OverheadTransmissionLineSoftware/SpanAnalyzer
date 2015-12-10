// This is free and unencumbered software released into the public domain.
// For more information, please refer to <http://unlicense.org/>

#ifndef OTLS_SPANANALYZER_SPANPANEL_H_
#define OTLS_SPANANALYZER_SPANPANEL_H_

#include "models/base/units.h"
#include "wx/wx.h"

#include "sag_tension_analysis_results.h"

/// \par OVERVIEW
///
/// This panel displays a single sag-tension result.
///
/// \par SAG-TENSION RESULTS
///
/// The sag-tension results are calculated independently of this panel. This
/// panel only provides an interface for viewing the sag-tension results.
///
/// \par CATENARY RESULTS
///
/// The catenary results are calculated within this panel. The catenary curve
/// (H/w) is defined by the sag-tension results, but the spacing is defined by
/// the controls on the panel.
class SpanPanel : public wxPanel {
 public:
  /// \brief Constructor.
  /// \param[in] parent
  ///   The parent window.
  /// \param[in] results
  ///   The sag-tension analysis results.
  SpanPanel(wxWindow* parent, const SagTensionAnalysisResultSet* results);

  /// \brief Destructor.
  ~SpanPanel();

  /// \brief Updates the pane.
  /// \param[in] hint
  ///   The hint describing what kind of update is needed.
  /// This function is called as part of the view update process.
  void UpdateView(wxObject* hint = nullptr);

 private:
  /// \brief Handles the weathercase choice event.
  /// \param[in] event
  ///   The event.
  void OnChoiceWeathercase(wxCommandEvent& event);

  /// \brief Handles the condition choice event.
  /// \param[in] event
  ///   The event.
  void OnChoiceCondition(wxCommandEvent& event);

  /// \brief Updates the panel sag-tension results.
  void UpdateData();

  /// \brief Updates the weathercases in the panel.
  void UpdateWeathercases();

  /// \brief Changes static text to display the desired unit system.
  /// \param[in] units
  ///   The unit system to display.
  void SetUnitsStaticText(const units::UnitSystem& units);

  /// \var results_
  ///   The sag-tension analysis results.
  const SagTensionAnalysisResultSet* results_;

  DECLARE_EVENT_TABLE()
};

#endif  // OTLS_SPANANALYZER_SPANPANEL_H_
