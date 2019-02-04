// This is free and unencumbered software released into the public domain.
// For more information, please refer to <http://unlicense.org/>

#include "spananalyzer/span_analyzer_frame.h"

#include "appcommon/units/cable_unit_converter.h"
#include "appcommon/units/weather_load_case_unit_converter.h"
#include "wx/aboutdlg.h"
#include "wx/printdlg.h"
#include "wx/xrc/xmlres.h"

#include "spananalyzer/analysis_filter_manager_dialog.h"
#include "spananalyzer/cable_constraint_manager_dialog.h"
#include "spananalyzer/cable_file_manager_dialog.h"
#include "spananalyzer/file_handler.h"
#include "spananalyzer/preferences_dialog.h"
#include "spananalyzer/span_analyzer_app.h"
#include "spananalyzer/span_analyzer_doc.h"
#include "spananalyzer/span_analyzer_printout.h"
#include "spananalyzer/span_analyzer_view.h"
#include "spananalyzer/weather_load_case_manager_dialog.h"
#include "xpm/icon.xpm"

DocumentFileDropTarget::DocumentFileDropTarget(wxWindow* parent) {
  parent_ = parent;

  // creates data object to store dropped information
  SetDataObject(new wxFileDataObject());
  SetDefaultAction(wxDragCopy);
}

bool DocumentFileDropTarget::OnDropFiles(wxCoord x, wxCoord y,
                                         const wxArrayString& filenames) {
  // gets data from drag-and-drop operation
  wxFileDataObject* data = dynamic_cast<wxFileDataObject*>(GetDataObject());

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

BEGIN_EVENT_TABLE(SpanAnalyzerFrame, wxDocParentFrame)
  EVT_MENU(XRCID("menuitem_edit_analysisfilters"), SpanAnalyzerFrame::OnMenuEditAnalysisFilters)
  EVT_MENU(XRCID("menuitem_edit_cables"), SpanAnalyzerFrame::OnMenuEditCables)
  EVT_MENU(XRCID("menuitem_edit_constraints"), SpanAnalyzerFrame::OnMenuEditConstraints)
  EVT_MENU(XRCID("menuitem_edit_weathercases"), SpanAnalyzerFrame::OnMenuEditWeathercases)
  EVT_MENU(XRCID("menuitem_file_pagesetup"), SpanAnalyzerFrame::OnMenuFilePageSetup)
  EVT_MENU(XRCID("menuitem_file_preferences"), SpanAnalyzerFrame::OnMenuFilePreferences)
  EVT_MENU(XRCID("menuitem_help_about"), SpanAnalyzerFrame::OnMenuHelpAbout)
  EVT_MENU(XRCID("menuitem_help_manual"), SpanAnalyzerFrame::OnMenuHelpManual)
  EVT_MENU(XRCID("menuitem_view_log"), SpanAnalyzerFrame::OnMenuViewLog)
  EVT_SIZE(SpanAnalyzerFrame::OnResize)
END_EVENT_TABLE()

SpanAnalyzerFrame::SpanAnalyzerFrame(wxDocManager* manager)
    : wxDocParentFrame(manager, nullptr, wxID_ANY,
                       wxGetApp().GetAppDisplayName()) {
  // loads dialog from virtual xrc file system
  wxXmlResource::Get()->LoadMenuBar(this, "span_analyzer_menubar");

  // sets the frame icon
  SetIcon(wxIcon(icon_xpm));

  // sets the drag and drop target
  SetDropTarget(new DocumentFileDropTarget(this));

  // tells aui manager to manage this frame
  manager_.SetManagedWindow(this);

  // creates status bar
  wxStatusBar* status_bar = CreateStatusBar();
  const int kFieldsCount = 2;
  const int widths_field[2] = {-1, 170};
  status_bar->SetFieldsCount(kFieldsCount);
  status_bar->SetStatusWidths(kFieldsCount, widths_field);

  // creates log AUI window and adds to manager
  wxAuiPaneInfo info;
  info.Name("Log");
  info.Float();
  info.Caption("Log");
  info.CloseButton(true);
  info.Show(false);
  pane_log_ = new LogPane(this);
  manager_.AddPane(pane_log_, info);

  manager_.Update();
}

SpanAnalyzerFrame::~SpanAnalyzerFrame() {
  // saves if frame is maximized to application config
  SpanAnalyzerConfig* config = wxGetApp().config();
  config->is_maximized_frame = IsMaximized();

  manager_.UnInit();
}

void SpanAnalyzerFrame::OnMenuEditAnalysisFilters(wxCommandEvent& event) {
  // gets application config and data
  const SpanAnalyzerConfig* config = wxGetApp().config();
  SpanAnalyzerData* data = wxGetApp().data();

  // creates and shows the analysis filter manager dialog
  AnalysisFilterManagerDialog dialog(this, &data->weathercases,
                                     &data->groups_filters);
  if (dialog.ShowModal() == wxID_OK) {
    wxBusyCursor cursor;

    wxLogVerbose("Updating analysis filters.");

    // saves application data
    FileHandler::SaveAppData(config->filepath_data, *data, config->units);

    // updates document/views
    SpanAnalyzerDoc* doc = wxGetApp().GetDocument();
    if (doc != nullptr) {
      UpdateHint hint(UpdateHint::Type::kAnalysisFilterGroupEdit);
      doc->UpdateAllViews(nullptr, &hint);
    }
  }
}

void SpanAnalyzerFrame::OnMenuEditCables(wxCommandEvent& event) {
  // gets application config and data
  const SpanAnalyzerConfig* config = wxGetApp().config();
  SpanAnalyzerData* data = wxGetApp().data();

  // creates and shows the cable file manager dialog
  CableFileManagerDialog dialog(this, config->units, &data->cablefiles);
  if (dialog.ShowModal() == wxID_OK) {
    wxBusyCursor cursor;

    wxLogVerbose("Updating cables.");

    // saves application data
    FileHandler::SaveAppData(config->filepath_data, *data, config->units);
  }

  wxBusyCursor cursor;

  // updates document/views
  SpanAnalyzerDoc* doc = wxGetApp().GetDocument();
  if (doc != nullptr) {
    doc->RunAnalysis();

    UpdateHint hint(UpdateHint::Type::kCablesEdit);
    doc->UpdateAllViews(nullptr, &hint);
  }
}

void SpanAnalyzerFrame::OnMenuEditConstraints(wxCommandEvent& event) {
  // gets application data
  SpanAnalyzerData* data = wxGetApp().data();

  // shows an editor
  CableConstraintManagerDialog dialog(
      this,
      &data->cablefiles,
      &data->weathercases,
      wxGetApp().config()->units,
      &data->constraints);
  if (dialog.ShowModal() == wxID_OK) {
    wxBusyCursor cursor;

    wxLogVerbose("Updating constraints.");

    // saves application data
    FileHandler::SaveAppData(wxGetApp().config()->filepath_data, *data,
                             wxGetApp().config()->units);
  }
}

void SpanAnalyzerFrame::OnMenuEditWeathercases(
    wxCommandEvent& event) {
  // gets application data
  SpanAnalyzerData* data = wxGetApp().data();

  // shows an editor
  WeatherLoadCaseManagerDialog dialog(
      this,
      wxGetApp().config()->units,
      &data->weathercases);
  if (dialog.ShowModal() == wxID_OK) {
    wxBusyCursor cursor;

    wxLogVerbose("Updating weathercases.");

    // saves application data
    FileHandler::SaveAppData(wxGetApp().config()->filepath_data, *data,
                             wxGetApp().config()->units);
  }

  // updates document/views
  SpanAnalyzerDoc* doc = wxGetApp().GetDocument();
  if (doc != nullptr) {
    doc->RunAnalysis();

    UpdateHint hint(UpdateHint::Type::kWeathercasesEdit);
    doc->UpdateAllViews(nullptr, &hint);
  }
}

void SpanAnalyzerFrame::OnMenuFilePageSetup(wxCommandEvent& event) {
  // gets application page setup data
  wxPageSetupDialogData* data_page = wxGetApp().config()->data_page;

  // creates and shows dialog
  wxPageSetupDialog dialog(this, data_page);
  if (dialog.ShowModal() != wxID_OK) {
    return;
  }

  // updates print data
  *data_page = dialog.GetPageSetupDialogData();
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

  wxBusyCursor cursor;

  // application data change is implemented on app restart

  // updates logging level
  wxLog::SetLogLevel(config->level_log);
  if (config->level_log == wxLOG_Message) {
    wxLog::SetVerbose(false);
  } else if (config->level_log == wxLOG_Info) {
    wxLog::SetVerbose(true);
  }

  // converts unit system if it changed
  SpanAnalyzerData* data = wxGetApp().data();
  SpanAnalyzerDoc* doc = wxGetApp().GetDocument();
  if (units_before != config->units) {
    wxLogVerbose("Converting unit system.");

    // converts app data
    for (auto iter = data->weathercases.begin();
         iter != data->weathercases.end(); iter++) {
      WeatherLoadCase* weathercase = *iter;
      WeatherLoadCaseUnitConverter::ConvertUnitSystem(
          units_before,
          config->units,
          *weathercase);
    }

    for (auto iter = data->cablefiles.begin(); iter != data->cablefiles.end();
         iter++) {
      CableFile* cablefile = *iter;
      CableUnitConverter::ConvertUnitSystem(
          units_before,
          config->units,
          true,
          cablefile->cable);
    }

    // updates document
    if (doc != nullptr) {
      doc->ConvertUnitSystem(units_before, config->units);
      doc->RunAnalysis();
    }
  }

  // updates views
  UpdateHint hint(UpdateHint::Type::kPreferencesEdit);
  doc->UpdateAllViews(nullptr, &hint);
}

void SpanAnalyzerFrame::OnMenuHelpAbout(wxCommandEvent& event) {
  // sets the dialog info
  wxAboutDialogInfo info;
  info.SetIcon(wxIcon(icon_xpm));
  info.SetName(wxGetApp().GetAppDisplayName());
  info.SetVersion(wxGetApp().version());
  info.SetCopyright("License:   http://unlicense.org/");
  info.SetDescription(
    "This is an open-source application for calculating the sag-tension "
    "response of transmission line cables.\n"
    "\n"
    "To get involved in the project development or to review the code, see the "
    "website.");
  info.SetWebSite("https://github.com/OverheadTransmissionLineSoftware/SpanAnalyzer");

  // shows the dialog
  wxAboutBox(info, this);
}

void SpanAnalyzerFrame::OnMenuHelpManual(wxCommandEvent& event) {
  wxHtmlHelpController* help = wxGetApp().help();
  help->DisplayContents();
}

void SpanAnalyzerFrame::OnMenuViewLog(wxCommandEvent& event) {
  wxAuiPaneInfo& info = manager_.GetPane("Log");
  if (info.IsShown() == false) {
    info.Show(true);
  } else {
    info.Show(false);
  }

  manager_.Update();
}

void SpanAnalyzerFrame::OnResize(wxSizeEvent& event) {
  // skips caching frame size if maximized
  if (IsMaximized() == true) {
    return;
  }

  // saves frame size to application config
  SpanAnalyzerConfig* config = wxGetApp().config();
  config->size_frame = event.GetSize();
}

LogPane* SpanAnalyzerFrame::pane_log() {
  return pane_log_;
}
