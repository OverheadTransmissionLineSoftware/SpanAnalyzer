// This is free and unencumbered software released into the public domain.
// For more information, please refer to <http://unlicense.org/>

#include "span_analyzer_frame.h"

#include "wx/aboutdlg.h"
#include "wx/xrc/xmlres.h"

#include "analysis_weather_load_case_manager_dialog.h"
#include "cable_file_manager_dialog.h"
#include "cable_unit_converter.h"
#include "file_handler.h"
#include "preferences_dialog.h"
#include "span_analyzer_app.h"
#include "span_analyzer_doc.h"
#include "span_analyzer_view.h"
#include "weather_load_case_unit_converter.h"

#include "../res/icon.xpm"

DocumentFileDropTarget::DocumentFileDropTarget(wxWindow* parent) {
  parent_ = parent;

  // creates data object to store dropped information
  SetDataObject(new wxFileDataObject());
  SetDefaultAction(wxDragCopy);
}

bool DocumentFileDropTarget::OnDropFiles(wxCoord x, wxCoord y,
                                         const wxArrayString& filenames) {
  // gets data from drag-and-drop operation
  wxFileDataObject* data = (wxFileDataObject*)GetDataObject();

  // tests if file exists
  // only the first file in the list is processed
  const wxString& str_file = data->GetFilenames().front();
  if (wxFileName::Exists(str_file) == false) {
    return false;
  }

  // tests if extension matches application document
  wxFileName path(str_file);
  if (path.GetExt() == ".spananalyzer") {
    return false;
  }

  // freezes frame, opens document, and thaws frame
  parent_->Freeze();
  wxGetApp().manager_doc()->CreateDocument(str_file);
  parent_->Thaw();

  return true;
}

BEGIN_EVENT_TABLE(SpanAnalyzerFrame, wxFrame)
  EVT_MENU(XRCID("menuitem_edit_analysis_weathercases"), SpanAnalyzerFrame::OnMenuEditAnalysisWeathercases)
  EVT_MENU(XRCID("menuitem_edit_cables"), SpanAnalyzerFrame::OnMenuEditCables)
  EVT_MENU(XRCID("menuitem_file_preferences"), SpanAnalyzerFrame::OnMenuFilePreferences)
  EVT_MENU(XRCID("menuitem_help_about"), SpanAnalyzerFrame::OnMenuHelpAbout)
  EVT_MENU(XRCID("menuitem_view_log"), SpanAnalyzerFrame::OnMenuViewLog)
END_EVENT_TABLE()

SpanAnalyzerFrame::SpanAnalyzerFrame(wxDocManager* manager)
    : wxDocParentFrame(manager, nullptr, wxID_ANY, "Span Analyzer") {
  // loads dialog from virtual xrc file system
  wxXmlResource::Get()->LoadMenuBar(this, "span_analyzer_menubar");

  // sets the frame icon
  SetIcon(wxICON(icon));

  // sets the drag and drop target
  SetDropTarget(new DocumentFileDropTarget(this));

  // tells aui manager to manage this frame
  manager_.SetManagedWindow(this);

  // creates the log dialog
  dialog_log_ = new LogDialog(this);
}

SpanAnalyzerFrame::~SpanAnalyzerFrame() {
  // saves frame size to application config
  SpanAnalyzerConfig* config = wxGetApp().config();
  if (this->IsMaximized() == true) {
    config->size_frame = wxSize(0, 0);
  } else {
    config->size_frame = this->GetSize();
  }

  manager_.UnInit();
}

void SpanAnalyzerFrame::OnMenuEditAnalysisWeathercases(
    wxCommandEvent& event) {
  // gets application data
  SpanAnalyzerData* data = wxGetApp().data();

  // shows an editor
  AnalysisWeatherLoadCaseManagerDialog dialog(
      this,
      wxGetApp().config()->units,
      &data->descriptions_weathercases_analysis,
      &data->weathercases_analysis);
  if (dialog.ShowModal() == wxID_OK) {
    // saves application data
    FileHandler::SaveAppData(wxGetApp().config()->filepath_data, *data,
                             wxGetApp().config()->units);

    // posts event to update views
    ViewUpdateHint hint;
    hint.set_type(ViewUpdateHint::HintType::kModelAnalysisWeathercaseEdit);
    wxGetApp().manager_doc()->GetCurrentDocument()->UpdateAllViews(nullptr,
                                                                   &hint);
  }
}

void SpanAnalyzerFrame::OnMenuEditCables(wxCommandEvent& event) {
  // checks to make sure document is closed
  wxDocManager* manager = wxGetApp().manager_doc();
  if (manager->GetCurrentDocument() != nullptr) {
    std::string message = "Document is currently open. To modify cables, "
      "close document and try again.";
    wxMessageBox(message);

    return;
  }

  // gets application config and data
  const SpanAnalyzerConfig* config = wxGetApp().config();
  SpanAnalyzerData* data = wxGetApp().data();

  // creates and shows the cable file manager dialog
  CableFileManagerDialog dialog(this, config->units, &data->cablefiles);
  if (dialog.ShowModal() == wxID_OK) {
    // saves application data
    FileHandler::SaveAppData(config->filepath_data, *data, config->units);
  }

  // reloads all cable files in case things get out of sync
  // i.e. user edits cable file, but doesn't accept any changes in file manager
  // logs
  wxLogMessage("Flushing cable files.");
  for (auto iter = data->cablefiles.begin(); iter != data->cablefiles.end();
       iter++) {
    CableFile& cablefile = *iter;
    FileHandler::LoadCable(cablefile.filepath, config->units, cablefile.cable);
  }
}

void SpanAnalyzerFrame::OnMenuFilePreferences(wxCommandEvent& event) {
  // gets the application config
  SpanAnalyzerConfig* config = wxGetApp().config();

  // stores a copy of the unit system before letting user edit
  units::UnitSystem units_before = config->units;

  // creates preferences editor dialog and shows
  // exits if user closes/cancels
  PreferencesDialog preferences(this, config);
  if (preferences.ShowModal() != wxID_OK) {
    return;
  }

  // application data change is implemented on app restart

  // converts unit system if it changed
  SpanAnalyzerData* data = wxGetApp().data();
  if (units_before != config->units) {
    // converts app data
    for (auto iter = data->weathercases_analysis.begin();
         iter != data->weathercases_analysis.end(); iter++) {
      std::list<WeatherLoadCase>& weathercases = *iter;
      for (auto it = weathercases.begin(); it != weathercases.end(); it++) {
        WeatherLoadCase& weathercase = *it;
        WeatherLoadCaseUnitConverter::ConvertUnitSystem(
            units_before,
            config->units,
            weathercase);
      }
    }

    for (auto iter = data->cablefiles.begin(); iter != data->cablefiles.end();
         iter++) {
      CableFile& cablefile = *iter;
      CableUnitConverter::ConvertUnitSystem(
          units_before,
          config->units,
          cablefile.cable);
    }

    // converts doc
    SpanAnalyzerDoc* doc = (SpanAnalyzerDoc*)wxGetApp().manager_doc()->
                               GetCurrentDocument();
    if (doc != nullptr) {
      doc->ConvertUnitSystem(units_before, config->units);

      // updates views
      ViewUpdateHint hint;
      hint.set_type(ViewUpdateHint::HintType::kModelPreferencesEdit);
      doc->UpdateAllViews(nullptr, &hint);
    }
  }

  // updates logging level
  wxLog::SetLogLevel(config->level_log);
}

void SpanAnalyzerFrame::OnMenuHelpAbout(wxCommandEvent& event) {
  // sets the dialog info
  wxAboutDialogInfo info;
  info.SetIcon(wxICON(icon));
  info.SetName(wxGetApp().GetAppDisplayName());
  info.SetVersion("0.1");
  info.SetCopyright("License:   http://unlicense.org/");
  info.SetDescription(
    "This application provides a GUI for calculating the sag-tension response\n"
    "of transmission line cables.\n"
    "\n"
    "This application is part of the Overhead Transmission Line Software\n"
    "suite. For the actual engineering modeling and sag-tension computations,\n"
    "see the Models library.\n"
     "\n"
    "This software was developed so that transmission engineers can know\n"
    "exactly how results are calculated, and so that the software can be\n"
    "freely modified to fit the unique needs of the utility they represent.\n"
    "\n"
    "To get involved in project development, or to review the code, see the\n"
    "website link.");
  info.SetWebSite("https://github.com/OverheadTransmissionLineSoftware/SpanAnalyzer");

  // shows the dialog
  wxAboutBox(info, this);
}

void SpanAnalyzerFrame::OnMenuViewLog(wxCommandEvent& event) {
  if (dialog_log_->IsShown() == false) {
    // shows dialog
    dialog_log_->Show(true);
  } else {
    dialog_log_->Show(false);
  }
}

LogDialog* SpanAnalyzerFrame::dialog_log() {
  return dialog_log_;
}
