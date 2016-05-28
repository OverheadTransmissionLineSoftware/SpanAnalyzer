// This is free and unencumbered software released into the public domain.
// For more information, please refer to <http://unlicense.org/>

#include "span_analyzer_doc.h"

#include "wx/xml/xml.h"

#include "span_analyzer_app.h"
#include "span_analyzer_doc_xml_handler.h"
#include "span_unit_converter.h"
#include "weather_load_case_unit_converter.h"

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

void SpanAnalyzerDoc::ConvertUnitStyle(const units::UnitSystem& system,
                                       const units::UnitStyle& style_from,
                                       const units::UnitStyle& style_to) {
  if (style_from == style_to) {
    return;
  }

  // converts weathercases
  for (auto it = weathercases_.begin(); it != weathercases_.end(); it++) {
    WeatherLoadCase& weathercase = *it;
    WeatherLoadCaseUnitConverter::ConvertUnitStyle(system, style_from,
                                                   style_to, weathercase);
  }

  // converts spans
  for (auto it = spans_.begin(); it != spans_.end(); it++) {
    Span& span = *it;
    SpanUnitConverter::ConvertUnitStyle(system, style_from,
                                        style_to, span);
  }
}

void SpanAnalyzerDoc::ConvertUnitSystem(const units::UnitSystem& system_from,
                                        const units::UnitSystem& system_to) {
  if (system_from == system_to) {
    return;
  }

  // converts weathercases
  for (auto it = weathercases_.begin(); it != weathercases_.end(); it++) {
    WeatherLoadCase& weathercase = *it;
    WeatherLoadCaseUnitConverter::ConvertUnitSystem(system_from, system_to,
                                                    weathercase);
  }

  // converts spans
  for (auto it = spans_.begin(); it != spans_.end(); it++) {
    Span& span = *it;
    SpanUnitConverter::ConvertUnitSystem(system_from, system_to, span);
  }
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

/// \todo This needs to check for weathercase duplicates and remove them if necessary.
wxInputStream& SpanAnalyzerDoc::LoadObject(wxInputStream& stream) {
  // attempts to load an xml document from the input stream
  wxXmlDocument doc_xml;
  if (doc_xml.Load(stream) == false) {
    // sets stream to invalid state and returns
    stream.Reset(wxSTREAM_READ_ERROR);
    return stream;
  }

  // checks for valid xml root
  const wxXmlNode* root = doc_xml.GetRoot();
  if (root->GetName() != "span_analyzer_doc") {
    // notifies user of error
    wxString message = "Span Analyzer Document Error\n"
                       "File doesn't begin with the correct xml node. \n\n"
                       "The document will close.";
    wxMessageBox(message);

    // sets stream to invalide state and returns
    stream.Reset(wxSTREAM_READ_ERROR);
    return stream;
  }

  // gets unit system attribute from file
  wxString str_units;
  units::UnitSystem units_file;
  if (root->GetAttribute("units", &str_units) == true) {
    if (str_units == "Imperial") {
      units_file = units::UnitSystem::kImperial;
    } else if (str_units == "Metric") {
      units_file = units::UnitSystem::kMetric;
    } else {
      // notifies user of error
      wxString message = "Span Analyzer Document Error\n"
                         "Parser didn't recognize unit system. \n\n"
                         "The document will close.";
      wxMessageBox(message);

      // sets stream to invalide state and returns
      stream.Reset(wxSTREAM_READ_ERROR);
      return stream;
    }
  } else {
    // notifies user of error
    wxString message = "Span Analyzer Document Error\n"
                       "Parser didn't recognize unit system. \n\n"
                       "The document will close.";
    wxMessageBox(message);

    // sets stream to invalide state and returns
    stream.Reset(wxSTREAM_READ_ERROR);
    return stream;
  }

  // parses the XML node and loads into the document
  int line_number = SpanAnalyzerDocXmlHandler::ParseNode(
      root, &wxGetApp().data()->cables, *this, units_file);
  if (line_number != 0) {
    // notifies user of error
    wxString message = "Span Analyzer Document Error: "
                       "Line " + std::to_wstring(line_number) + "\n"
                       "Parser didn't recognize input.\n\n"
                       "The document will close.";
    wxMessageBox(message);

    // sets stream to invalide state and returns
    stream.Reset(wxSTREAM_READ_ERROR);
    return stream;
  }

  // converts units to consistent style
  ConvertUnitStyle(units_file,
                   units::UnitStyle::kDifferent,
                   units::UnitStyle::kConsistent);

  // converts unit systems if the file doesn't match applicaton config
  units::UnitSystem units_config = wxGetApp().config()->units;
  if (units_file != units_config) {
    ConvertUnitSystem(units_file, units_config);
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
  // gets the unit system from app config
  units::UnitSystem units = wxGetApp().config()->units;

  // converts to a different unit style for saving
  ConvertUnitStyle(units, units::UnitStyle::kConsistent,
                   units::UnitStyle::kDifferent);

  // generates an xml node
  wxXmlNode* root = SpanAnalyzerDocXmlHandler::CreateNode(*this, units);

  // adds unit attribute to xml node
  // this attribute should be added at this step vs the xml handler because
  // the attribute describes all values in the file, and is consistent
  // with how the FileHandler functions work
  if (units == units::UnitSystem::kImperial) {
    root->AddAttribute("units", "Imperial");
  } else if (units == units::UnitSystem::kMetric) {
    root->AddAttribute("units", "Metric");
  }

  // creates an XML document and savaes to stream
  wxXmlDocument doc_xml;
  doc_xml.SetRoot(root);
  doc_xml.Save(stream);

  // converts back to a consistent unit style
  ConvertUnitStyle(units, units::UnitStyle::kDifferent,
                   units::UnitStyle::kConsistent);

  return stream;
}

const std::list<Span>& SpanAnalyzerDoc::spans() const {
  return spans_;
}

const std::list<WeatherLoadCase>& SpanAnalyzerDoc::weathercases() const {
  return weathercases_;
}
