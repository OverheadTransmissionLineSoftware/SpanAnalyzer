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
  ResultsPane(wxWindow* parent, SpanAnalyzerView* view);
  ~ResultsPane();

  void Update(wxObject* hint = nullptr);

 private:
  void OnChoiceWeathercase(wxCommandEvent& event);
  void OnChoiceCondition(wxCommandEvent& event);
  void SetUnitsStaticText(const units::UnitSystem& units);
  void UpdateSagTensionResults();

  LineCableReloader reloader_;
  SpanAnalyzerView* view_;

  DECLARE_EVENT_TABLE()
};

# endif //  OTLS_SPANANALYZER_RESULTSPANE_H_
