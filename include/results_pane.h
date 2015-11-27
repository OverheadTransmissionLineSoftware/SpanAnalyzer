// This is free and unencumbered software released into the public domain.
// For more information, please refer to <http://unlicense.org/>

#ifndef OTLS_SPANANALYZER_RESULTSPANE_H_
#define OTLS_SPANANALYZER_RESULTSPANE_H_

#include "models/base/units.h"
#include "models/sagtension/line_cable_reloader.h"
#include "wx/docview.h"
#include "wx/wx.h"

#include "span_analyzer_view.h"

/// \par OVERVIEW
class ResultsPane : public wxPanel {
 public:
  /// \brief Constructor.
  /// \param[in] parent
  ///   The parent window.
  /// \param[in] view
  ///   The view.
  ResultsPane(wxWindow* parent, SpanAnalyzerView* view);

  /// \brief Destructor.
  ~ResultsPane();

  /// \brief Updates the pane.
  /// \param[in] hint
  ///   The hint describing what kind of update is needed.
  /// This function is called as part of the view update process.
  void Update(wxObject* hint = nullptr);

 private:
  /// \brief Handles the weathercase choice event.
  /// \param[in] event
  ///   The event.
  void OnChoiceWeathercase(wxCommandEvent& event);

  /// \brief Handles the condition choice event.
  /// \param[in] event
  ///   The event.
  void OnChoiceCondition(wxCommandEvent& event);

  /// \brief Changes static text to display the desired unit system.
  /// \param[in] units
  ///   The unit system to display.
  void SetUnitsStaticText(const units::UnitSystem& units);

  /// \brief Updates the sag-tension results.
  void UpdateSagTensionResults();

  /// \var reloader_
  ///   The reloader that calcuates the sag-tension values.
  LineCableReloader reloader_;

  /// \var view_
  ///   The view.
  SpanAnalyzerView* view_;

  DECLARE_EVENT_TABLE()
};

# endif //  OTLS_SPANANALYZER_RESULTSPANE_H_
