// This is free and unencumbered software released into the public domain.
// For more information, please refer to <http://unlicense.org/>

#include "span_analyzer_frame.h"

#include "wx/xml/xml.h"
#include "wx/xrc/xmlres.h"

#include "cable_editor_dialog.h"
#include "cable_unit_converter.h"
#include "cable_xml_handler.h"
#include "weather_load_case_editor_dialog.h"

BEGIN_EVENT_TABLE(SpanAnalyzerFrame, wxFrame)
  EVT_BUTTON(XRCID("button_launch_cable_file_dialog"), SpanAnalyzerFrame::OnCableFileSelect)
  EVT_MENU(XRCID("menuitem_cable_edit"), SpanAnalyzerFrame::OnCableEdit)
  EVT_MENU(XRCID("menuitem_cable_new"), SpanAnalyzerFrame::OnCableNew)
  EVT_MENU(XRCID("menuitem_exit"), SpanAnalyzerFrame::OnExit)
  EVT_MENU(XRCID("menuitem_weathercase_edit"), SpanAnalyzerFrame::OnWeatherCaseEdit)
END_EVENT_TABLE()

SpanAnalyzerFrame::SpanAnalyzerFrame() {
  // loads dialog from virtual xrc file system
  wxXmlResource::Get()->LoadFrame(this, nullptr, "span_analyzer_frame");

  // initializes variables
  cable_ = new Cable();

  // fits the window around the sizers
  this->Fit();
}

SpanAnalyzerFrame::~SpanAnalyzerFrame() {
}

void SpanAnalyzerFrame::OnCableEdit(wxCommandEvent& event) {
  // creates a blank cable
  Cable cable;

  // creates a cable file selector dialog and shows
  wxFileDialog selector(this, "Open", wxEmptyString,
                        wxEmptyString,
                        "Cable File(*.cbl)|*.cbl",
                        wxFD_OPEN | wxFD_FILE_MUST_EXIST);
  if (selector.ShowModal() == wxID_OK) {
    // creates a virtual XML document
    wxXmlDocument doc;
    if (doc.Load(selector.GetFilename()) == false) {
      wxMessageDialog message(this, "Invalid XML file");
      message.ShowModal();
      return;
    }

    // parses the XML root and loads into the cable object
    const wxXmlNode* root = doc.GetRoot();
    int line_number = CableXmlHandler::ParseNode(root,
                                                 cable);
    if (line_number != 0) {
      wxMessageDialog message(this, "Error at line <insert line>");
      message.ShowModal();
      return;
    }
  } else {
    return;
  }

  // creates a cable editor dialog and shows
  CableEditorDialog editor(this, &cable, units::UnitSystem::Imperial);
  if (editor.ShowModal() == wxID_OK) {
    // asks user to confirm file overwrite
    wxMessageDialog message(this, "Ok to overwrite file?",
                            "File Overwrite Warning", wxOK | wxCANCEL);
    if (message.ShowModal() != wxID_OK) {
      return;
    }

    // generates a virtual XML document
    wxXmlDocument doc;
    wxXmlNode* root = CableXmlHandler::CreateNode(
        cable,
        units::UnitSystem::Imperial);
    doc.SetRoot(root);

    // saves virtual XML document to file
    if (doc.Save(selector.GetPath(), 2) == false) {
      return;
    }
  }
}

void SpanAnalyzerFrame::OnCableNew(wxCommandEvent& event) {
  // creates a blank cable
  Cable cable;

  // creates a cable editor dialog and shows
  CableEditorDialog editor(this, &cable, units::UnitSystem::Imperial);

  if (editor.ShowModal() == wxID_OK) {
    // generates a virtual XML document
    wxXmlDocument doc;
    wxXmlNode* root = CableXmlHandler::CreateNode(cable,
                                                  units::UnitSystem::Imperial);
    doc.SetRoot(root);

    // creates a cable file selector dialog and shows
    wxFileDialog selector(this, "Save As", wxEmptyString,
                          wxEmptyString,
                          "Cable File(*.cbl)|*.cbl",
                          wxFD_SAVE | wxFD_OVERWRITE_PROMPT);

    if (selector.ShowModal() == wxID_OK) {
      if (!doc.Save(selector.GetPath(), 2)) {
        return;
      }
    }
  }
}

void SpanAnalyzerFrame::OnCableFileSelect(wxCommandEvent& event) {
  // creates a dialog to select cable file
  wxFileDialog selector(this, "Open", wxEmptyString,
                        wxEmptyString,
                        "Cable File(*.cbl)|*.cbl",
                        wxFD_OPEN | wxFD_FILE_MUST_EXIST);

  // if user selects file
  if (selector.ShowModal() == wxID_OK) {
    // creates a virtual XML document
    wxXmlDocument* doc = new wxXmlDocument();
    if (doc->Load(selector.GetFilename()) == false) {
      wxMessageDialog message(this, "Invalid XML file");
      message.ShowModal();
    }

    // parses the XML root and loads into the cable object
    const wxXmlNode* root = doc->GetRoot();
    int line_number = CableXmlHandler::ParseNode(root,
                                                 *cable_);
    if (line_number != 0) {
      wxMessageDialog message(this, "Error at line <insert line>");
      message.ShowModal();
      return;
    }

    // converts cable to consistent units
    CableUnitConverter::ConvertUnitStyle(units::UnitSystem::Imperial,
                                         UnitStyle::kDifferent,
                                         UnitStyle::kConsistent,
                                         *cable_);

    // updates textctrl to show cable
    wxTextCtrl* textctrl_cable = XRCCTRL(*this,
                                         "textctrl_cable_name",
                                          wxTextCtrl);
    textctrl_cable->SetValue(cable_->name);
  }
}

void SpanAnalyzerFrame::OnExit(wxCommandEvent& event) {
  this->Destroy();
}

void SpanAnalyzerFrame::OnWeatherCaseEdit(
    wxCommandEvent& event) {
  // creates a blank weathercase
  WeatherLoadCase* weathercase = new WeatherLoadCase();

  // creates a weathercase file selector dialog and shows
  WeatherLoadCaseEditorDialog editor(this, weathercase,
                                     units::UnitSystem::Imperial);

  if (editor.ShowModal() == wxID_OK) {
    wxMessageDialog message(this, "Woo");
    message.ShowModal();
  }
}
