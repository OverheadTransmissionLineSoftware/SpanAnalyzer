// This is free and unencumbered software released into the public domain.
// For more information, please refer to <http://unlicense.org/>

#include "span_analyzer_app.h"

#include "wx/filename.h"
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

bool SpanAnalyzerApp::OnCmdLineParsed(wxCmdLineParser& parser) {
  // sets directory based on debug switch
  if (parser.Found("debug")) {
    directory_ = wxFileName::GetCwd();
  } else {
    // gets the application file name and solves for path
    wxFileName path(wxStandardPaths::Get().GetExecutablePath());
    directory_ = path.GetPath();
  }

  // gets the config file path
  wxString filepath_config;
  if (parser.Found("config", &filepath_config)) {
    filepath_config_ = filepath_config;
  } else {
    // default to a specific file in the application directory
    if (filepath_config_.empty() == true) {
      wxFileName path(directory_, "spananalyzer", "conf");
      filepath_config_ = path.GetFullPath();
    }
  }

  // captures the start file which will be loaded when doc manager is created
  if (parser.GetParamCount() == 1) {
    filepath_start_ = parser.GetParam(0);
  }

  return true;
}

int SpanAnalyzerApp::OnExit() {
  // disables logging because all windows are destroyed
  wxLog::EnableLogging(false);

  // saves config file
  FileHandler::SaveConfigFile(filepath_config_, config_);

  // cleans up allocated resources
  delete manager_doc_;

  // continues exit process
  return wxApp::OnExit();
}

bool SpanAnalyzerApp::OnInit() {
  // initializes command line
  if (wxApp::OnInit() == false) {
    return false;
  }

  // sets application name
  SetAppDisplayName("Span Analyzer");
  SetAppName("SpanAnalyzer");

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

  path.SetName("analysis_weather_load_case_manager_dialog");
  if (!wxXmlResource::Get()->LoadFile(path)) {
    return false;
  };

  path.SetName("cable_editor_dialog");
  if (!wxXmlResource::Get()->LoadFile(path)) {
    return false;
  };

  path.SetName("cable_file_manager_dialog");
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

  // sets application logging to a modeless dialog managed by the frame
  wxLogTextCtrl* log = new wxLogTextCtrl(frame_->pane_log()->textctrl());
  wxLog::SetActiveTarget(log);

  // manually initailizes application config defaults
  config_.filepath_data = directory_ + "appdata.xml";
  config_.level_log = wxLOG_Message;
  config_.perspective = "";
  config_.size_frame = wxSize(0, 0);
  config_.units = units::UnitSystem::kImperial;

  // loads config settings from file
  // any settings defined in the file will override the app defaults
  // filehandler handles all logging
  path = filepath_config_;
  FileHandler::LoadConfigFile(filepath_config_, config_);

  // sets log level specified in app config
  wxLog::SetLogLevel(config_.level_log);

  // checks if data file exists, creates one if not
  path = config_.filepath_data;
  if (path.Exists() == false) {
    // logs
    wxLogError("Applicaton data file could not be located. Setting to "
               "application default.");

    // defines default data file path and updates config
    path = wxFileName(directory_, "appdata", "xml");
    config_.filepath_data = path.GetFullPath();

    // saves new data file if default file doesn't already exist
    if (path.Exists() == false) {
      FileHandler::SaveAppData(config_.filepath_data, data_, config_.units);
    }
  }

  // loads application data file
  // filehandler handles all logging
  FileHandler::LoadAppData(config_.filepath_data, config_.units, data_);

  // loads a document if defined in command line
  if (filepath_start_ != wxEmptyString) {
    manager_doc_->CreateDocument(filepath_start_);
  }

  // sets application frame based on config setting and shows
  if (config_.size_frame.GetHeight() < 100
      || config_.size_frame.GetWidth() < 100) {
    frame_->Maximize();
  } else {
    frame_->SetSize(config_.size_frame);
  }
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
