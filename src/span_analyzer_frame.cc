// This is free and unencumbered software released into the public domain.
// For more information, please refer to <http://unlicense.org/>

#include "span_analyzer_frame.h"

#include "wx/aboutdlg.h"
#include "wx/xrc/xmlres.h"

#include "file_handler.h"
#include "preferences_dialog.h"
#include "span_analyzer_app.h"

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
  EVT_MENU(XRCID("menuitem_edit_cable_edit"), SpanAnalyzerFrame::OnMenuEditCableEdit)
  EVT_MENU(XRCID("menuitem_edit_cable_new"), SpanAnalyzerFrame::OnMenuEditCableNew)
  EVT_MENU(XRCID("menuitem_edit_weathercases_edit"), SpanAnalyzerFrame::OnMenuEditWeatherCasesEdit)
  EVT_MENU(XRCID("menuitem_file_preferences"), SpanAnalyzerFrame::OnMenuFilePreferences)
  EVT_MENU(XRCID("menuitem_help_about"), SpanAnalyzerFrame::OnMenuHelpAbout)
  EVT_MENU(XRCID("menuitem_debug_event"), SpanAnalyzerFrame::OnMenuDebugEvent)
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
  // saves UI configuration before frame closes
  SpanAnalyzerConfig* config = wxGetApp().config();
  FileHandler::SaveConfigFile(*config);
}

void SpanAnalyzerFrame::OnMenuEditCableEdit(wxCommandEvent& event) {
}

void SpanAnalyzerFrame::OnMenuEditCableNew(wxCommandEvent& event) {
}

void SpanAnalyzerFrame::OnMenuEditWeatherCasesEdit(
    wxCommandEvent& event) {
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

// DEBUG
void SpanAnalyzerFrame::OnMenuDebugEvent(wxCommandEvent& event) {
}
// DEBUG
