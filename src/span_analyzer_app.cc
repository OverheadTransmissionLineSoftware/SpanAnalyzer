// This is free and unencumbered software released into the public domain.
// For more information, please refer to <http://unlicense.org/>

#include "span_analyzer_app.h"

#include "span_analyzer_frame.h"
#include "wx/xrc/xmlres.h"

IMPLEMENT_APP(SpanAnalyzerApp)

SpanAnalyzerApp::SpanAnalyzerApp() {
}

bool SpanAnalyzerApp::OnInit() {

  // loads all xml resource files into virtual file system
  wxXmlResource::Get()->InitAllHandlers();

  if (!wxXmlResource::Get()->LoadFile(
      wxFileName("res/cable_editor_dialog.xrc"))) {
    return false;
  };

  if (!wxXmlResource::Get()->LoadFile(
      wxFileName("res/span_analyzer_frame.xrc"))) {
    return false;
  };

  if (!wxXmlResource::Get()->LoadFile(
      wxFileName("res/span_analyzer_menu.xrc"))) {
    return false;
  };

  if (!wxXmlResource::Get()->LoadFile(
      wxFileName("res/weather_case_editor_dialog.xrc"))) {
    return false;
  };
  
  // creates frame
  SpanAnalyzerFrame* frame = new SpanAnalyzerFrame();
  frame->Show(true);
  
  return true;
}