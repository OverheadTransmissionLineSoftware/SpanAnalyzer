// This is free and unencumbered software released into the public domain.
// For more information, please refer to <http://unlicense.org/>

#include "weather_case_editor_dialog.h"

#include "wx/valnum.h"
#include "wx/xrc/xmlres.h"

BEGIN_EVENT_TABLE(WeatherCaseEditorDialog, wxDialog)
  EVT_BUTTON(wxID_CANCEL, WeatherCaseEditorDialog::OnCancel)
  EVT_BUTTON(wxID_OK, WeatherCaseEditorDialog::OnOk)
  EVT_CLOSE(WeatherCaseEditorDialog::OnClose)
END_EVENT_TABLE()

WeatherCaseEditorDialog::WeatherCaseEditorDialog(
    wxWindow* parent, WeatherLoadCase* weathercase) {
  // loads dialog from virtual xrc file system
  wxXmlResource::Get()->LoadDialog(this, parent, "weather_case_editor_dialog");
}

WeatherCaseEditorDialog::~WeatherCaseEditorDialog() {
}

void WeatherCaseEditorDialog::OnClose(wxCloseEvent &event) {
  EndModal(wxID_CLOSE);
}

void WeatherCaseEditorDialog::OnCancel(wxCommandEvent &event) {
  EndModal(wxID_CANCEL);
}

void WeatherCaseEditorDialog::OnOk(wxCommandEvent &event) {
  //// validates data from form
  //if (this->Validate() == false) {
  //  wxMessageBox("Errors on form");
  //  return;
  //}

  //// transfers data from dialog controls to cable
  //this->TransferDataFromWindow();

  // ends modal by returning ok indicator
  EndModal(wxID_OK);
}
