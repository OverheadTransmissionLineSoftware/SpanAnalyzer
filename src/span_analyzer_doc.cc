// This is free and unencumbered software released into the public domain.
// For more information, please refer to <http://unlicense.org/>

#include "span_analyzer_doc.h"

#include "wx/xml/xml.h"

#include "span_analyzer_app.h"
#include "span_analyzer_doc_xml_handler.h"

IMPLEMENT_DYNAMIC_CLASS(SpanAnalyzerDoc, wxDocument)

SpanAnalyzerDoc::SpanAnalyzerDoc() {
}

SpanAnalyzerDoc::~SpanAnalyzerDoc() {
}

wxInputStream& SpanAnalyzerDoc::LoadObject(wxInputStream& stream) {
  // attempts to load an xml document from the input stream
  wxXmlDocument doc_xml;
  if (doc_xml.Load(stream) == false) {
    wxWindow* window = this->GetDocumentWindow();
    wxString message = "SpanAnalyzerDoc: Invalid XML file.";
    wxMessageDialog dialog(window, message);
    dialog.ShowModal();
  } else {
    units::UnitSystem units;

    // parses the XML root and loads into the cable object
    const wxXmlNode* root = doc_xml.GetRoot();
    int line_number = SpanAnalyzerDocXmlHandler::ParseNode(
        root, &wxGetApp().data()->cables, *this, units);
    if (line_number != 0) {
      wxWindow* window = this->GetDocumentWindow();
      wxString message = "Span Analyzer Document: Error at line "
                         + std::to_wstring(line_number);
      wxMessageDialog dialog(window, message);
      dialog.ShowModal();
    }

    // temporarily disables until metric units are supported
    if (units == units::UnitSystem::kMetric) {
      wxWindow* parent = this->GetDocumentWindow();
      wxString message = "Span Analyzer Document: Metric units are not yet "
                         "supported";
      wxMessageDialog dialog(parent, message);
      dialog.ShowModal();
    }
  }

  return stream;
}

wxOutputStream& SpanAnalyzerDoc::SaveObject(wxOutputStream& stream) {
  // gets unit setting
  units::UnitSystem units = wxGetApp().config()->units;

  // creates an xml root
  wxXmlNode* root = SpanAnalyzerDocXmlHandler::CreateNode(*this, units);

  // creates an XML document and puts in stream
  wxXmlDocument doc_xml;
  doc_xml.SetRoot(root);
  doc_xml.Save(stream);

  return stream;
}

std::vector<Span>* SpanAnalyzerDoc::spans() {
  return &spans_;
}

std::vector<WeatherLoadCase>* SpanAnalyzerDoc::weathercases() {
  return &weathercases_;
}
