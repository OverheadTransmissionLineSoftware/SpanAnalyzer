// This is free and unencumbered software released into the public domain.
// For more information, please refer to <http://unlicense.org/>

#include "span_analyzer_frame.h"

#include "wx/aboutdlg.h"
#include "wx/xrc/xmlres.h"

#include "analysis_weather_load_case_manager_dialog.h"
#include "cable_directory_editor_dialog.h"
#include "cable_editor_dialog.h"
#include "cable_unit_converter.h"
#include "cable_xml_handler.h"
#include "file_handler.h"
#include "preferences_dialog.h"
#include "span_analyzer_app.h"
#include "span_analyzer_view.h"

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
  EVT_MENU(XRCID("menuitem_edit_cable"), SpanAnalyzerFrame::OnMenuEditCable)
  EVT_MENU(XRCID("menuitem_edit_cable_directory"), SpanAnalyzerFrame::OnMenuEditCableDirectory)
  EVT_MENU(XRCID("menuitem_edit_new_cable"), SpanAnalyzerFrame::OnMenuEditNewCable)
  EVT_MENU(XRCID("menuitem_file_preferences"), SpanAnalyzerFrame::OnMenuFilePreferences)
  EVT_MENU(XRCID("menuitem_help_about"), SpanAnalyzerFrame::OnMenuHelpAbout)
END_EVENT_TABLE()

SpanAnalyzerFrame::SpanAnalyzerFrame(wxDocManager* manager)
    : wxDocParentFrame(manager, nullptr, wxID_ANY, "Span Analyzer") {
  // loads dialog from virtual xrc file system
  wxXmlResource::Get()->LoadMenuBar(this, "span_analyzer_menubar");

  // sets the frame icon
  SetIcon(wxICON(icon));

  // sets the drag and drop target
  SetDropTarget(new DocumentFileDropTarget(this));
}

SpanAnalyzerFrame::~SpanAnalyzerFrame() {
  // saves frame size to application config
  SpanAnalyzerConfig* config = wxGetApp().config();
  if (this->IsMaximized() == true) {
    config->size_frame = wxSize(0, 0);
  } else {
    config->size_frame = this->GetSize();
  }
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

/// This could be greatly simplified if the cable object, or a derived object
/// maintained the filepath that it was loaded from.
void SpanAnalyzerFrame::OnMenuEditCable(wxCommandEvent& event) {
  // initializes variables
  Cable cable;
  bool is_loaded_into_data = false;
  std::list<Cable>::iterator iter;
  units::UnitSystem units = wxGetApp().config()->units;

  // shows file selector dialog
  wxFileDialog dialog(this, "Cable File Selector", wxEmptyString, "*cable",
                      "*cable", wxFD_OPEN);
  if (dialog.ShowModal() != wxID_OK) {
    return;
  }

  // gets path and loads cable
  const std::string filepath = dialog.GetPath();
  FileHandler::LoadCable(filepath, wxGetApp().config()->units, cable);

  // determines if cable is already loaded into application
  SpanAnalyzerData* data = wxGetApp().data();
  wxFileName filename(dialog.GetPath());
  if (filename.GetPath() == data->directory_cables) {
    // gets iterator to matching cable in application data
    for (iter = data->cables.begin(); iter != data->cables.end(); iter++) {
      const Cable& cable_data = *iter;
      if (cable.name == cable_data.name) {
        is_loaded_into_data = true;
        break;
      }
    }

    if (iter == data->cables.end()) {
      is_loaded_into_data = false;
    }
  } else {
    is_loaded_into_data = false;
  }

  // converts units to different unit style
  CableUnitConverter::ConvertUnitStyle(
      units,
      units::UnitStyle::kConsistent,
      units::UnitStyle::kDifferent,
      cable);

  // creates dialog to edit cable
  CableEditorDialog dialog_edit(this, &cable, units);
  if (dialog_edit.ShowModal() != wxID_OK) {
    return;
  }

  // converts units to consistent unit style
  CableUnitConverter::ConvertUnitStyle(
      wxGetApp().config()->units,
      units::UnitStyle::kDifferent,
      units::UnitStyle::kConsistent,
      cable);

  // saves cable to filesystem
  FileHandler::SaveCable(filepath, cable, units);

  // updates application data, if necessary
  if (is_loaded_into_data == true) {
    *iter = cable;
  }
}

void SpanAnalyzerFrame::OnMenuEditCableDirectory(wxCommandEvent& event) {
  // checks if no documents are currently open
  // doesn't allow swapping cables with open doc, too difficult to replace/match
  // cable references
  wxDocManager* manager = wxGetApp().manager_doc();
  if (manager->GetCurrentDocument() != nullptr) {
    std::string message = "Document is currently open. To change cable "
                          "directory, close document and try again.";
    wxMessageBox(message);

    return;
  }

  // gets application data
  SpanAnalyzerData* data = wxGetApp().data();

  // shows cable directory editor dialog
  CableDirectoryEditorDialog dialog(this, &data->directory_cables);
  if (dialog.ShowModal() == wxID_OK) {
    // saves application data
    FileHandler::SaveAppData(wxGetApp().config()->filepath_data, *data,
                             wxGetApp().config()->units);

    // reloads all cables in the directory
    data->cables.clear();
    FileHandler::LoadCablesFromDirectory(data->directory_cables,
                                         wxGetApp().config()->units);
  }
}

void SpanAnalyzerFrame::OnMenuEditNewCable(wxCommandEvent& event) {
  // creates new cable
  Cable cable;

  // gets units
  units::UnitSystem units = wxGetApp().config()->units;

  // displays an editor dialog
  CableEditorDialog dialog(this, &cable, units);
  if (dialog.ShowModal() != wxID_OK) {
    return;
  }

  // converts unit style to consistent
  CableUnitConverter::ConvertUnitStyle(
      units,
      units::UnitStyle::kDifferent,
      units::UnitStyle::kConsistent,
      cable);

  // gets filepath to save cable
  wxFileDialog dialog_file(this, "Save Cable File", "", ".cable",
                           "Cable Files (*.cable) | *.cable", wxFD_SAVE);
  if (dialog_file.ShowModal() != wxID_OK) {
    return;
  }

  // saves file
  const std::string filepath = dialog_file.GetPath();
  FileHandler::SaveCable(filepath, cable, units);
}

void SpanAnalyzerFrame::OnMenuFilePreferences(wxCommandEvent& event) {
  // creates preferences editor dialog and shows
  PreferencesDialog preferences(this, wxGetApp().config());
  if (preferences.ShowModal() == wxID_OK) {
    /// \todo implement unit system changes
  }
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
