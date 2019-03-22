// This is free and unencumbered software released into the public domain.
// For more information, please refer to <http://unlicense.org/>

#include "spananalyzer/span_analyzer_app.h"

#include "appcommon/widgets/status_bar_log.h"
#include "wx/filename.h"
#include "wx/fs_zip.h"
#include "wx/stdpaths.h"
#include "wx/xrc/xmlres.h"

#include "spananalyzer/file_handler.h"
#include "spananalyzer/span_analyzer_doc.h"
#include "spananalyzer/span_analyzer_view.h"

/// This function handles loading all of the xrc files that are processed by
/// the wxWidgets resource compiler. It is declared in an external file so
/// this file doesn't have to be recompiled every time the resources are
/// updated.
extern void InitXmlResource();

IMPLEMENT_APP(SpanAnalyzerApp)

SpanAnalyzerApp::SpanAnalyzerApp() {
  frame_ = nullptr;
  manager_doc_ = nullptr;
}

SpanAnalyzerApp::~SpanAnalyzerApp() {
}

SpanAnalyzerDoc* SpanAnalyzerApp::GetDocument() const {
  return dynamic_cast<SpanAnalyzerDoc*>(manager_doc_->GetCurrentDocument());
}

bool SpanAnalyzerApp::OnCmdLineParsed(wxCmdLineParser& parser) {
  // gets the config file path
  wxString filepath_config;
  if (parser.Found("config", &filepath_config)) {
    // converts filepath to absolute if needed
    wxFileName path(filepath_config);
    if (path.IsAbsolute() == false) {
      path.MakeAbsolute(wxEmptyString, wxPATH_NATIVE);
    }

    filepath_config_ = path.GetFullPath();
  } else {
    wxFileName path(wxEmptyString, "spananalyzer", "conf");

    // detects OS and specifies default config file path for user
    wxOperatingSystemId os = wxGetOsVersion();
    if ((wxOS_UNKNOWN < os) && (os <= wxOS_MAC)) {
      // mac os
      path.SetPath(wxStandardPaths::Get().GetExecutablePath());
    } else if ((wxOS_MAC < os) && (os <= wxOS_WINDOWS)) {
      // windows os
      path.SetPath(wxStandardPaths::Get().GetUserConfigDir());
      path.AppendDir("OTLS");
      path.AppendDir("SpanAnalyzer");
    } else if ((wxOS_WINDOWS < os) && (os <= wxOS_UNIX)) {
      // unix os
      path.SetPath(wxStandardPaths::Get().GetUserConfigDir());
      path.AppendDir(".config");
      path.AppendDir("otls");
      path.AppendDir("span-analyzer");
    } else {
      path.SetPath(wxStandardPaths::Get().GetExecutablePath());
    }

    filepath_config_ = path.GetFullPath();
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
  delete config_.data_page;

  for (auto iter = data_.cablefiles.begin(); iter != data_.cablefiles.end();
       iter++) {
    const CableFile* cablefile = *iter;
    delete cablefile;
  }

  for (auto iter = data_.weathercases.begin(); iter != data_.weathercases.end();
       iter++) {
    const WeatherLoadCase* weathercase = *iter;
    delete weathercase;
  }

  delete help_;
  delete manager_doc_;

  // continues exit process
  return wxApp::OnExit();
}

bool SpanAnalyzerApp::OnInit() {
  wxFileName filename;

  // initializes command line
  if (wxApp::OnInit() == false) {
    return false;
  }

  // sets application info
  SetAppDisplayName("SpanAnalyzer");
  SetAppName("SpanAnalyzer");
  version_ = "1.1.0";

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
  InitXmlResource();

  // creates main application frame
  frame_ = new SpanAnalyzerFrame(manager_doc_);
  SetTopWindow(frame_);

  // sets application logging to a modeless dialog managed by the frame
  wxLogTextCtrl* log = new wxLogTextCtrl(frame_->pane_log()->textctrl());
  wxLog::SetActiveTarget(log);

  // manually initailizes application config defaults
  filename = wxFileName(filepath_config_);
  wxPrintData data_print;
  data_print.SetOrientation(wxLANDSCAPE);
  config_.color_background = *wxBLACK;
  config_.data_page = new wxPageSetupDialogData();
  config_.data_page->SetPrintData(data_print);
  config_.data_page->SetMarginBottomRight(wxPoint(15, 15));
  config_.data_page->SetMarginTopLeft(wxPoint(15, 15));
  config_.filepath_data = filename.GetPathWithSep() + "appdata.xml";
  config_.is_maximized_frame = true;
  config_.level_log = wxLOG_Message;
  config_.options_plot_cable.color_core = *wxRED;
  config_.options_plot_cable.color_markers = *wxGREEN;
  config_.options_plot_cable.color_shell = *wxBLUE;
  config_.options_plot_cable.color_total = *wxYELLOW;
  config_.options_plot_cable.thickness_line = 1;
  config_.options_plot_plan.color_catenary = *wxCYAN;
  config_.options_plot_plan.scale_horizontal = 1;
  config_.options_plot_plan.scale_vertical = 10;
  config_.options_plot_plan.thickness_line = 1;
  config_.options_plot_profile.color_catenary = *wxCYAN;
  config_.options_plot_profile.scale_horizontal = 1;
  config_.options_plot_profile.scale_vertical = 10;
  config_.options_plot_profile.thickness_line = 1;
  config_.perspective = "";
  config_.size_frame = wxSize(400, 400);
  config_.units = units::UnitSystem::kImperial;

  // loads config settings from file, or saves a file if it doesn't exist
  // on loading, any settings defined in the file will override the app defaults
  // filehandler handles all logging
  filename = wxFileName(filepath_config_);
  if (filename.Exists() == true) {
    FileHandler::LoadConfigFile(filepath_config_, config_);
  } else {
    FileHandler::SaveConfigFile(filepath_config_, config_);
  }

  // sets log level specified in app config
  wxLog::SetLogLevel(config_.level_log);
  if (config_.level_log == wxLOG_Info) {
    wxLog::SetVerbose(true);
  }

  // creates help controller and adds content
  help_ = new wxHtmlHelpController(wxHF_DEFAULT_STYLE);
  wxInitAllImageHandlers();
  wxFileSystem::AddHandler(new wxZipFSHandler);

  filename = wxStandardPaths::Get().GetExecutablePath();
  filename.AppendDir("res");
  filename.SetExt("htb");

  filename.SetName("overview");
  if (filename.Exists() == true) {
    if (help_->AddBook(filename) == false) {
      wxLogError("Couldn't load overview help manual.");
    }
  } else {
    wxLogError("Overview help manual file doesn't exist. Needs to be located "
               "at: "
               + filename.GetFullPath());
  }

  filename.SetName("interface");
  if (filename.Exists() == true) {
    if (help_->AddBook(filename) == false) {
      wxLogError("Couldn't load interface help manual.");
    }
  } else {
    wxLogError("Interface help manual file doesn't exist. Needs to be located "
               "at: "
               + filename.GetFullPath());
  }

  filename.SetName("calculations");
  if (filename.Exists() == true) {
    if (help_->AddBook(filename) == false) {
      wxLogError("Couldn't load calculations help manual.");
    }
  } else {
    wxLogError("Calculations help manual file doesn't exist. Needs to be "
               " located at: "
               + filename.GetFullPath());
  }

  // sets application frame based on config setting
  // this needs to be done before any messages are shown
  frame_->SetSize(config_.size_frame);
  if (config_.is_maximized_frame == true) {
    frame_->Maximize();
  }

  // loads app data from file, or saves a file if it doesn't exist
  // filehandler handles all logging
  filename = wxFileName(config_.filepath_data);
  if (filename.Exists() == true) {
    // loads application data file
    const int status_data = FileHandler::LoadAppData(config_.filepath_data,
                                                     config_.units, data_);
    if ((status_data == -1) || (status_data == 1)) {
      // notifies user of error
      wxString message = config_.filepath_data + "  --  "
                "Application data file contains error(s). Check logs.";
      wxMessageBox(message);
    }
  } else {
    // logs
    std::string message = "Application data file doesn't exist. Creating a new "
                          "file.";
    wxLogError(message.c_str());

    // saves new data file
    FileHandler::SaveAppData(config_.filepath_data, data_, config_.units);
  }

  // loads a document if defined in command line
  if (filepath_start_ != wxEmptyString) {
    manager_doc_->CreateDocument(filepath_start_);
  }

  // shows application frame
  frame_->Centre(wxBOTH);
  frame_->Show(true);

  // updates status bar
  status_bar_log::SetText("Ready", 0);

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

wxHtmlHelpController* SpanAnalyzerApp::help() {
  return help_;
}

wxDocManager* SpanAnalyzerApp::manager_doc() {
  return manager_doc_;
}

std::string SpanAnalyzerApp::version() const {
  return version_;
}
