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

void SpanAnalyzerDoc::AppendSpan(const Span& span) {
  spans_.push_back(span);
  Modify(true);
}

void SpanAnalyzerDoc::AppendWeathercase(const WeatherLoadCase& weathercase) {
  weathercases_.push_back(weathercase);
  Modify(true);
}

bool SpanAnalyzerDoc::DeleteSpan(const unsigned int& index) {
  // checks if index is valid
  if (weathercases_.size() < (index + 1)) {
    return false;
  }

  // deletes from span vector
  auto iter = spans_.begin() + index;
  spans_.erase(iter);

  Modify(true);

  return true;
}

bool SpanAnalyzerDoc::DeleteWeathercase(const unsigned int& index) {
  // checks if index is valid
  if (weathercases_.size() < (index + 1)) {
    return false;
  }

  const WeatherLoadCase* weathercase = &weathercases_.at(index);

  // searches all spans and sets reference to nullptr if address matches
  for (auto iter = spans_.begin(); iter != spans_.end(); iter++) {
    Span& span = *iter;
    const WeatherLoadCase* weathercase_span = nullptr;

    // compares constraint weathercase
    weathercase_span = span.linecable.constraint.case_weather;
    if (weathercase_span == weathercase) {
      span.linecable.constraint.case_weather = nullptr;
    }

    // compares stretch-creep weathercase
    weathercase_span = span.linecable.weathercase_stretch_creep;
    if (weathercase_span == weathercase) {
      span.linecable.weathercase_stretch_creep = nullptr;
    }

    // compares stretch-load weathercase
    weathercase_span = span.linecable.weathercase_stretch_load;
    if (weathercase_span == weathercase) {
      span.linecable.weathercase_stretch_load = nullptr;
    }
  }

  // deletes from weathercases vector
  auto iter = weathercases_.begin() + index;
  weathercases_.erase(iter);

  Modify(true);

  return true;
}

bool SpanAnalyzerDoc::InsertSpan(const unsigned int& index,
                                 const Span& span) {
  // checks if index is valid
  if (weathercases_.size() < (index + 1)) {
    return false;
  }

  // adds span to vector
  auto iter = spans_.begin() + index;
  spans_.insert(iter, span);

  Modify(true);

  return true;
}

bool SpanAnalyzerDoc::InsertWeathercase(const unsigned int& index,
                                        const WeatherLoadCase& weathercase) {
  // checks if index is valid
  if (weathercases_.size() < (index + 1)) {
    return false;
  }

  // adds weathercase to vector
  auto iter = weathercases_.begin() + index;
  weathercases_.insert(iter, weathercase);

  Modify(true);

  return true;
}

bool SpanAnalyzerDoc::IsReferencedWeathercase(
    const WeatherLoadCase* weathercase) const {
  // searches all spans to see if weathercase address matches
  for (auto iter = spans_.cbegin(); iter != spans_.cend(); iter++) {
    const Span& span = *iter;
    const WeatherLoadCase* weathercase_span = nullptr;

    // compares constraint weathercase
    weathercase_span = span.linecable.constraint.case_weather;
    if (weathercase_span == weathercase) {
      return true;
    }

    // compares stretch-creep weathercase
    weathercase_span = span.linecable.weathercase_stretch_creep;
    if (weathercase_span == weathercase) {
      return true;
    }

    // compares stretch-load weathercase
    weathercase_span = span.linecable.weathercase_stretch_load;
    if (weathercase_span == weathercase) {
      return true;
    }
  }

  // weathercase is not referenced
  return false;
}

bool SpanAnalyzerDoc::IsUniqueWeathercase(const std::string& description,
                                          const int& index_skip) const {
  for (auto iter = weathercases_.cbegin(); iter != weathercases_.cend();
       iter++) {
    const WeatherLoadCase& weathercase = *iter;

    const int index = iter - weathercases_.cbegin();
    if (index != index_skip) {
      if (description == weathercase.description) {
        // existing weathercase description has been found
        return false;
      }
    }
  }

  // no existing descriptions matched
  return true;
}

/// \todo This needs to check for duplicates and remove them if necessary.
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

  // resets modified status to false because the xml parser uses functions
  // that mark it as modified
  Modify(false);

  return stream;
}

bool SpanAnalyzerDoc::ReplaceSpan(const unsigned int& index,
                                  const Span& span) {
  // checks if index is valid
  if (spans_.size() < (index + 1)) {
    return false;
  }

  // adds weathercase to vector
  Span& span_index = spans_.at(index);
  span_index = Span(span);

  Modify(true);

  return true;
}

bool SpanAnalyzerDoc::ReplaceWeathercase(const unsigned int& index,
                                         const WeatherLoadCase& weathercase) {
  // checks if index is valid
  if (weathercases_.size() < (index + 1)) {
    return false;
  }

  // adds weathercase to vector
  WeatherLoadCase& weathercase_index = weathercases_.at(index);
  weathercase_index = WeatherLoadCase(weathercase);

  Modify(true);

  return true;
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

const std::vector<Span>& SpanAnalyzerDoc::spans() const {
  return spans_;
}

const std::vector<WeatherLoadCase>& SpanAnalyzerDoc::weathercases() const {
  return weathercases_;
}
