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

std::list<Span>::const_iterator SpanAnalyzerDoc::AppendSpan(
    const Span& span) {
  spans_.push_back(span);

  Modify(true);

  return std::prev(spans_.end());
}

std::list<WeatherLoadCase>::const_iterator SpanAnalyzerDoc::AppendWeathercase(
    const WeatherLoadCase& weathercase) {
  weathercases_.push_back(weathercase);

  Modify(true);

  return std::prev(weathercases_.cend());
}

void SpanAnalyzerDoc::DeleteSpan(
    const std::list<Span>::const_iterator& element) {
  // gets iterator with edit capability
  const unsigned int index = std::distance(spans_.cbegin(), element);
  auto iter = std::next(spans_.begin(), index);

  // deletes from span list
  spans_.erase(iter);

  Modify(true);
}

void SpanAnalyzerDoc::DeleteWeathercase(
    const std::list<WeatherLoadCase>::const_iterator& element) {
  const WeatherLoadCase* weathercase = &(*element);

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

  // gets iterator with edit capability
  const unsigned int index = std::distance(weathercases_.cbegin(), element);
  auto iter = std::next(weathercases_.begin(), index);

  // deletes from weathercases list
  weathercases_.erase(iter);

  Modify(true);
}

  std::list<Span>::const_iterator SpanAnalyzerDoc::InsertSpan(
      const std::list<Span>::const_iterator& position,
      const Span& span) {
  // gets iterator with edit capability
  const unsigned int index = std::distance(spans_.cbegin(), position);
  auto iter = std::next(spans_.begin(), index);

  // adds span to vector
  spans_.insert(iter, span);

  Modify(true);

  return std::prev(position);
}

std::list<WeatherLoadCase>::const_iterator SpanAnalyzerDoc::InsertWeathercase(
    const std::list<WeatherLoadCase>::const_iterator& position,
    const WeatherLoadCase& weathercase) {
  // gets iterator with edit capability
  const unsigned int index = std::distance(weathercases_.cbegin(), position);
  auto iter = std::next(weathercases_.begin(), index);

  // adds weathercase to list
  weathercases_.insert(iter, weathercase);

  Modify(true);

  return std::prev(position);
}

/// This function compares the memory address of the weathercases.
bool SpanAnalyzerDoc::IsReferencedWeathercase(
    const std::list<WeatherLoadCase>::const_iterator& element) const {
  // gets weathercase
  const WeatherLoadCase* weathercase = &(*element);

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

bool SpanAnalyzerDoc::IsUniqueWeathercase(
    const std::string& description,
    const std::list<WeatherLoadCase>::const_iterator* skip) const {
  // gets the weathercase that will be skipped
  const WeatherLoadCase* weathercase_skip = nullptr;
  if (skip != nullptr) {
    weathercase_skip = &(**skip);
  }

  // searches weathercases for matching description
  for (auto iter = weathercases_.cbegin(); iter != weathercases_.cend();
       iter++) {
    const WeatherLoadCase* weathercase = &(*iter);

    if (description == weathercase->description) {
      if (weathercase_skip == weathercase) {
        // do nothing - continue searching
      } else {
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
    wxString message = "SpanAnalyzerDoc: Invalid XML file.";
    wxMessageBox(message);
  } else {
    units::UnitSystem units;

    // parses the XML root and loads into the cable object
    const wxXmlNode* root = doc_xml.GetRoot();
    int line_number = SpanAnalyzerDocXmlHandler::ParseNode(
        root, &wxGetApp().data()->cables, *this, units);
    if (line_number != 0) {
      wxString message = "Span Analyzer Document: Error at line "
                         + std::to_wstring(line_number);
      wxMessageBox(message);
    }

    // temporarily disables until metric units are supported
    if (units == units::UnitSystem::kMetric) {
      wxString message = "Span Analyzer Document: Metric units are not yet "
                         "supported";
      wxMessageBox(message);
    }
  }

  // resets modified status to false because the xml parser uses functions
  // that mark it as modified
  Modify(false);

  return stream;
}

void SpanAnalyzerDoc::MoveSpan(
      const std::list<Span>::const_iterator& element,
      const std::list<Span>::const_iterator& position) {
  // gets iterators with edit capability
  const unsigned int index_element =
      std::distance(spans_.cbegin(), element);
  auto iter_element = std::next(spans_.begin(), index_element);

  const unsigned int index_position =
      std::distance(spans_.cbegin(), position);
  auto iter_position = std::next(spans_.begin(), index_position);

  spans_.splice(iter_position, spans_, iter_element);

  Modify(true);

  return;
}

void SpanAnalyzerDoc::MoveWeathercase(
      const std::list<WeatherLoadCase>::const_iterator& element,
      const std::list<WeatherLoadCase>::const_iterator& position) {
  // gets iterators with edit capability
  const unsigned int index_element =
      std::distance(weathercases_.cbegin(), element);
  auto iter_element = std::next(weathercases_.begin(), index_element);

  const unsigned int index_position =
      std::distance(weathercases_.cbegin(), position);
  auto iter_position = std::next(weathercases_.begin(), index_position);

  weathercases_.splice(iter_position, weathercases_, iter_element);

  Modify(true);

  return;
}

void SpanAnalyzerDoc::ReplaceSpan(
    const std::list<Span>::const_iterator& element,
    const Span& span) {
  // gets iterator with edit capability
  const unsigned int index = std::distance(spans_.cbegin(), element);
  auto iter = std::next(spans_.begin(), index);

  // replaces span in list
  *iter = Span(span);

  Modify(true);
}

void SpanAnalyzerDoc::ReplaceWeathercase(
    const std::list<WeatherLoadCase>::const_iterator& element,
    const WeatherLoadCase& weathercase) {
  // gets iterator with edit capability
  const unsigned int index = std::distance(weathercases_.cbegin(), element);
  auto iter = std::next(weathercases_.begin(), index);

  // replaces weathercase in list
  *iter = WeatherLoadCase(weathercase);

  Modify(true);
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

const std::list<Span>& SpanAnalyzerDoc::spans() const {
  return spans_;
}

const std::list<WeatherLoadCase>& SpanAnalyzerDoc::weathercases() const {
  return weathercases_;
}
