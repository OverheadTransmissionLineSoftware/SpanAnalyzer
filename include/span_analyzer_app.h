// This is free and unencumbered software released into the public domain.
// For more information, please refer to <http://unlicense.org/>

#ifndef TLS_SPANANALYZER_SPANANALYZERAPP_H_
#define TLS_SPANANALYZER_SPANANALYZERAPP_H_

#include "wx/wx.h"

class SpanAnalyzerApp : public wxApp {
 public:
  /// \brief Constructor.
  SpanAnalyzerApp();

  /// \brief Initializes the application.
  /// \return The success of the application initialization.
  virtual bool OnInit();
};

DECLARE_APP(SpanAnalyzerApp)

#endif  // TLS_SPANANALYZER_SPANANALYZERAPP_H_