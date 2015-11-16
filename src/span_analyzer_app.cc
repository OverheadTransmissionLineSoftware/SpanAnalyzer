// This is free and unencumbered software released into the public domain.
// For more information, please refer to <http://unlicense.org/>

#include "span_analyzer_app.h"

#include "wx/xrc/xmlres.h"

#include "file_handler.h"
#include "span_analyzer_doc.h"
#include "span_analyzer_view.h"

IMPLEMENT_APP(SpanAnalyzerApp)

SpanAnalyzerApp::SpanAnalyzerApp() {
  frame_ = nullptr;
  manager_doc_ = nullptr;
}

SpanAnalyzerApp::~SpanAnalyzerApp() {
}

int SpanAnalyzerApp::OnExit() {
  delete manager_doc_;

  return wxApp::OnExit();
}

bool SpanAnalyzerApp::OnCmdLineParsed(wxCmdLineParser& parser) {
  // captures the start file which will be loaded when doc manager is created
  parser.Found("file", &file_start_);

  return true;
}

bool SpanAnalyzerApp::OnInit() {
  // initializes command line
  if (wxApp::OnInit() == false) {
    return false;
  }

  // creates a document manager and sets to single document interface
  manager_doc_ = new wxDocManager();
  manager_doc_->SetMaxDocsOpen(1);

  // creates a document/view template relating drawing documents to their views
  (void) new wxDocTemplate(manager_doc_, "SpanAnalyzer", "*.spananalyzer",
                           "", "spananalyzer", "SpanAnalyzerDoc",
                           "SpanAnalyzerView", CLASSINFO(SpanAnalyzerDoc),
                           CLASSINFO(SpanAnalyzerView));

  // loads all xml resource files into virtual file system
  wxXmlResource::Get()->InitAllHandlers();

  if (!wxXmlResource::Get()->LoadFile(
      wxFileName("res/cable_editor_dialog.xrc"))) {
    return false;
  };

  if (!wxXmlResource::Get()->LoadFile(
      wxFileName("res/cable_manager_dialog.xrc"))) {
    return false;
  };

  if (!wxXmlResource::Get()->LoadFile(
      wxFileName("res/edit_panel.xrc"))) {
    return false;
  };

  if (!wxXmlResource::Get()->LoadFile(
      wxFileName("res/messages_panel.xrc"))) {
    return false;
  };

  if (!wxXmlResource::Get()->LoadFile(
      wxFileName("res/results_panel.xrc"))) {
    return false;
  };

  if (!wxXmlResource::Get()->LoadFile(
      wxFileName("res/span_analyzer_menubar.xrc"))) {
    return false;
  };

  if (!wxXmlResource::Get()->LoadFile(
      wxFileName("res/span_analyzer_preferences_dialog.xrc"))) {
    return false;
  };

  if (!wxXmlResource::Get()->LoadFile(
      wxFileName("res/span_editor_dialog.xrc"))) {
    return false;
  };

  if (!wxXmlResource::Get()->LoadFile(
      wxFileName("res/weather_load_case_editor_dialog.xrc"))) {
    return false;
  };

  if (!wxXmlResource::Get()->LoadFile(
      wxFileName("res/weather_load_case_manager_dialog.xrc"))) {
    return false;
  };

  // creates main application frame
  frame_ = new SpanAnalyzerFrame(manager_doc_);
  SetTopWindow(frame_);

  // loads config file
  FileHandler::LoadConfigFile(config_);

  // loads application data
  FileHandler::LoadAppData(config_.cable_directory, config_.units, data_);

  // loads a document if defined in command line
  if (file_start_ != wxEmptyString) {
    manager_doc_->CreateDocument(file_start_);
  }

  // sets application frame based on config setting and shows
  frame_->SetSize(config_.size_frame);
  frame_->Centre(wxBOTH);
  frame_->Show(true);

  return true;
}

void SpanAnalyzerApp::OnInitCmdLine(wxCmdLineParser& parser) {
  // sets command line style and parameters
  parser.EnableLongOptions();
  parser.SetDesc(cmd_line_desc);
  parser.SetSwitchChars("--");
}

SpanAnalyzerConfig* SpanAnalyzerApp::config() {
  return &config_;
}

SpanAnalyzerData* SpanAnalyzerApp::data() {
  return &data_;
}

SpanAnalyzerFrame* SpanAnalyzerApp::frame() {
  return frame_;
}

wxDocManager* SpanAnalyzerApp::manager_doc() {
  return manager_doc_;
}
