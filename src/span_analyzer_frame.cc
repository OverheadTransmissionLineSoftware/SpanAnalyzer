// This is free and unencumbered software released into the public domain.
// For more information, please refer to <http://unlicense.org/>

#include "span_analyzer_frame.h"

#include "wx/xrc/xmlres.h"

#include "file_handler.h"
#include "preferences_dialog.h"
#include "span_analyzer_app.h"

#include "../res/icon.xpm"

// for debug event ONLY
#include "wx/xml/xml.h"
#include "span_editor_dialog.h"
#include "span_analyzer_config_xml_handler.h"
#include "span_analyzer_view.h"

BEGIN_EVENT_TABLE(SpanAnalyzerFrame, wxFrame)
  EVT_MENU(XRCID("menuitem_edit_cable_edit"), SpanAnalyzerFrame::OnMenuEditCableEdit)
  EVT_MENU(XRCID("menuitem_edit_cable_new"), SpanAnalyzerFrame::OnMenuEditCableNew)
  EVT_MENU(XRCID("menuitem_edit_weathercases_edit"), SpanAnalyzerFrame::OnMenuEditWeatherCasesEdit)
  EVT_MENU(XRCID("menuitem_file_exit"), SpanAnalyzerFrame::OnMenuFileExit)
  EVT_MENU(XRCID("menuitem_file_preferences"), SpanAnalyzerFrame::OnMenuFilePreferences)
  EVT_MENU(XRCID("menuitem_debug_event"), SpanAnalyzerFrame::OnMenuDebugEvent)
END_EVENT_TABLE()

SpanAnalyzerFrame::SpanAnalyzerFrame(wxDocManager* manager)
    : wxDocParentFrame(manager, nullptr, wxID_ANY, "Span Analyzer") {
  // loads dialog from virtual xrc file system
  wxXmlResource::Get()->LoadMenuBar(this, "span_analyzer_menubar");

  // sets the frame icon
  SetIcon(wxICON(icon));
}

SpanAnalyzerFrame::~SpanAnalyzerFrame() {
  // saves UI configuration before frame closes
  SpanAnalyzerConfig* config = wxGetApp().config();
  FileHandler::SaveConfigFile(*config);
}

void SpanAnalyzerFrame::OnMenuEditCableEdit(wxCommandEvent& event) {
}

void SpanAnalyzerFrame::OnMenuEditCableNew(wxCommandEvent& event) {
//  // creates a blank cable
//  Cable cable;
//
//  // creates a cable editor dialog and shows
//  CableEditorDialog editor(this, &cable, units::UnitSystem::kImperial);
//
//  if (editor.ShowModal() == wxID_OK) {
//    // generates a virtual XML document
//    wxXmlDocument doc_xml;
//    wxXmlNode* root = CableXmlHandler::CreateNode(cable,
//                                                  controller_->model()->units);
//    doc_xml.SetRoot(root);
//
//    // creates a cable file selector dialog and shows
//    wxFileDialog selector(this, "Save As", wxEmptyString,
//                          wxEmptyString,
//                          "Cable File(*.cable)|*.cable",
//                          wxFD_SAVE | wxFD_OVERWRITE_PROMPT);
//
//    if (selector.ShowModal() == wxID_OK) {
//      if (!doc_xml.Save(selector.GetPath(), 2)) {
//        return;
//      }
//    }
//  }
}

void SpanAnalyzerFrame::OnMenuEditWeatherCasesEdit(
    wxCommandEvent& event) {
//  // gets weathercases from model
//  std::vector<WeatherLoadCase>& weathercases = model_->weathercases;
//
//  // converts weathercases to different unit style
//  for (auto iter = weathercases.begin(); iter != weathercases.end(); iter++) {
//    WeatherLoadCase& weathercase = *iter;
//    WeatherLoadCaseUnitConverter::ConvertUnitStyle(
//        model_->units,
//        units::UnitStyle::kConsistent,
//        units::UnitStyle::kDifferent,
//        weathercase);
//  }
//
//  // shows weather cases manager to allow user to edit
//  WeatherLoadCaseManagerDialog manager(frame_, &weathercases,
//                                       model_->units);
//  if (manager.ShowModal() == wxID_OK) {
//    // send event
//  }
//
//  // converts weathercases to consistent unit style
//  for (auto iter = weathercases.begin(); iter != weathercases.end(); iter++) {
//    WeatherLoadCase& weathercase = *iter;
//    WeatherLoadCaseUnitConverter::ConvertUnitStyle(
//        model_->units,
//        units::UnitStyle::kDifferent,
//        units::UnitStyle::kConsistent,
//        weathercase);
//  }
//
//  // processes the event
//  ProcessEventType(SpanAnalyzerController::EventType::kModelWeathercaseEdit);
}

void SpanAnalyzerFrame::OnMenuFileExit(wxCommandEvent& event) {
  this->Destroy();
}

void SpanAnalyzerFrame::OnMenuFilePreferences(wxCommandEvent& event) {
  // creates preferences editor dialog and shows
  PreferencesDialog preferences(this, wxGetApp().config());
  if (preferences.ShowModal() == wxID_OK) {
    //ConvertModelUnits();
    //ParseCableFiles();

    //controller_->ProcessEventType(SpanAnalyzerController::EventType::kModelPreferencesEdit);
  }
}

// DEBUG
void SpanAnalyzerFrame::OnMenuDebugEvent(wxCommandEvent& event) {
//  Cable cable;
//  CableEditorDialog dialog(this, &cable, units::UnitSystem::kImperial);
//  dialog.ShowModal();

  //SpanAnalyzerDoc* doc = (SpanAnalyzerDoc *)wxGetApp().manager_doc()->GetCurrentDocument();

  ////LineCable linecable;
  //ConfigData* data_config = wxGetApp().data_config();

  //// generates a virtual XML document
  //wxXmlDocument doc_xml;
  //wxXmlNode* root = ConfigXmlHandler::CreateNode(*data_config);
  //doc_xml.SetRoot(root);
  //doc_xml.Save("spananalyzer.config", 2);

  //LineCableEditorDialog dialog(this, &linecable, units::UnitSystem::kImperial);
  //dialog.ShowModal();
  //wxGetApp().controller_event()->ProcessEventType(EventController::EventType::kModelCableEdit);

  SpanAnalyzerView* view = (SpanAnalyzerView*)this->m_docManager->GetCurrentView();
  wxString perspective = view->GetPerspective();
  wxMessageDialog dialog(this, perspective);
  dialog.ShowModal();
}
// DEBUG
