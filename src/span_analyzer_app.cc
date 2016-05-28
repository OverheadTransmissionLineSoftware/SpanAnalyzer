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
  if (parser.Found("config", &filepath_config_)) {
    // nothing to do, file path already captured
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

  path.SetName("cable_directory_editor_dialog");
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
  // the config file path is set when parsing the command line
  const int status_config = FileHandler::LoadConfigFile(filepath_config_,
                                                        config_);
  if (status_config == -1) {
    // manually initializes config
    path = wxFileName(filepath_config_);
    path.SetName("appdata");
    path.SetExt("xml");
    config_.filepath_data = path.GetFullPath();

    config_.perspective = "";
    config_.size_frame = wxSize(0, 0);
    config_.units = units::UnitSystem::kImperial;

    // saves config file
    path = wxFileName(filepath_config_);
    FileHandler::SaveConfigFile(filepath_config_, config_);

    // notifies user
    wxMessageBox("Config file could not be located. New file has been created: "
                 + path.GetFullPath());
  } else if (status_config != 0) {
    // notifies user to correct
    wxMessageBox("Config file: " + filepath_config_ + "Error on line:"
                 + std::to_string(status_config));

    return false;
  };

  // loads application data
  const int status_data = FileHandler::LoadAppData(config_.filepath_data,
                                                   config_.units, data_);
  if (status_data == -1) {
    // manually initializes data
    /// \todo should create a subdirectory for the cable directory
    data_.directory_cables == config_.filepath_data;

    // saves data file
    FileHandler::SaveAppData(config_.filepath_data, data_, config_.units);

    // notifies user
    wxMessageBox("Data file could not be located. New file has been created: "
                 + path.GetFullPath());
  } else if (status_data != 0) {
    // notifies user to correct
    wxMessageBox("Data file: " + config_.filepath_data + "Error on line:"
                 + std::to_string(status_data));

    return false;
  }

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
