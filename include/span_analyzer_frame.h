// This is free and unencumbered software released into the public domain.
// For more information, please refer to <http://unlicense.org/>

#ifndef TLS_SPANANALYZER_SPANANALYZERFRAME_H_
#define TLS_SPANANALYZER_SPANANALYZERFRAME_H_

#include "transmissionline/cable.h"
#include "wx/wx.h"

class SpanAnalyzerFrame : public wxFrame {
 public:
  SpanAnalyzerFrame();
  ~SpanAnalyzerFrame();

  void OnCableEdit(wxCommandEvent& event);
  void OnCableNew(wxCommandEvent& event);
  void OnCableFileSelect(wxCommandEvent& event);
  void OnExit(wxCommandEvent& event);
  void OnWeatherCaseEdit(wxCommandEvent& event);

 private:
  Cable* cable_;

  DECLARE_EVENT_TABLE()
};

#endif  // TLS_SPANANALYZER_SPANANALYZERFRAME_H_