// This is free and unencumbered software released into the public domain.
// For more information, please refer to <http://unlicense.org/>

#include "span_analyzer_app.h"

#include "wx/stdpaths.h"
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
  // sets directory based on debug switch
  if (parser.Found("debug")) {
    directory_ = wxFileName::GetCwd();
  } else {
    // gets the application file name and solves for path
    wxFileName path(wxStandardPaths::Get().GetExecutablePath());
    directory_ = path.GetPath();
  }

  // captures the start file which will be loaded when doc manager is created
  if (parser.GetParamCount() == 1) {
    file_start_ = parser.GetParam(0);
  }

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
  wxFileName path(directory_, "", "");
  path.AppendDir("res");
  path.SetExt("xrc");

  path.SetName("cable_editor_dialog");
  if (!wxXmlResource::Get()->LoadFile(path)) {
    return false;
  };

  path.SetName("catenary_table_panel");
  if (!wxXmlResource::Get()->LoadFile(path)) {
    return false;
  };

  path.SetName("edit_pane");
  if (!wxXmlResource::Get()->LoadFile(path)) {
    return false;
  };

  path.SetName("error_message_dialog");
  if (!wxXmlResource::Get()->LoadFile(path)) {
    return false;
  };

  path.SetName("preferences_dialog");
  if (!wxXmlResource::Get()->LoadFile(path)) {
    return false;
  };

  path.SetName("results_pane");
  if (!wxXmlResource::Get()->LoadFile(path)) {
    return false;
  };

  path.SetName("sag_tension_table_panel");
  if (!wxXmlResource::Get()->LoadFile(path)) {
    return false;
  };

  path.SetName("span_analyzer_menubar");
  if (!wxXmlResource::Get()->LoadFile(path)) {
    return false;
  };

  path.SetName("span_editor_dialog");
  if (!wxXmlResource::Get()->LoadFile(path)) {
    return false;
  };

  path.SetName("span_panel");
  if (!wxXmlResource::Get()->LoadFile(path)) {
    return false;
  };

  path.SetName("weather_load_case_editor_dialog");
  if (!wxXmlResource::Get()->LoadFile(path)) {
    return false;
  };

  // creates main application frame
  frame_ = new SpanAnalyzerFrame(manager_doc_);
  SetTopWindow(frame_);

  // loads config file
  /// \todo check for existence of config file, a new one may have to
  ///   be generated
  FileHandler::LoadConfigFile(config_);

  // loads application data
  FileHandler::LoadAppData(config_.filepath_data, config_.units, data_);

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

wxString SpanAnalyzerApp::directory() {
  return directory_;
}

SpanAnalyzerFrame* SpanAnalyzerApp::frame() {
  return frame_;
}

wxDocManager* SpanAnalyzerApp::manager_doc() {
  return manager_doc_;
}
