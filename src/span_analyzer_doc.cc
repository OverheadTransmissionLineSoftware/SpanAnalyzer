// This is free and unencumbered software released into the public domain.
// For more information, please refer to <http://unlicense.org/>

#include "span_analyzer_doc.h"

#include "wx/xml/xml.h"

#include "span_analyzer_app.h"
#include "span_analyzer_doc_xml_handler.h"
#include "span_unit_converter.h"

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

void SpanAnalyzerDoc::ConvertUnitStyle(const units::UnitSystem& system,
                                       const units::UnitStyle& style_from,
                                       const units::UnitStyle& style_to) {
  if (style_from == style_to) {
    return;
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

  // determines if span matches analysis span
  bool is_selected = false;
  if (&(*iter) == controller_analysis_.span()) {
    is_selected = true;
  }

  // deletes from span list
  spans_.erase(iter);

  // marks as modified
  Modify(true);

  // updates if span matched analysis span
  if (is_selected == true) {
    controller_analysis_.set_span(nullptr);
    controller_analysis_.ClearResults();

    UpdateHint hint(HintType::kSpansEdit);
    UpdateAllViews(nullptr, &hint);
  }
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

wxInputStream& SpanAnalyzerDoc::LoadObject(wxInputStream& stream) {
  std::string message;

  message = "Loading document file: " + this->GetFilename();
  wxLogMessage(message.c_str());

  // attempts to load an xml document from the input stream
  wxXmlDocument doc_xml;
  if (doc_xml.Load(stream) == false) {
    // notifies user of error
    message = GetFilename() + "  --  "
              "Document file contains an invalid xml structure. The document "
              "will close.";
    wxLogError(message.c_str());
    wxMessageBox(message);

    // sets stream to invalid state and returns
    stream.Reset(wxSTREAM_READ_ERROR);
    return stream;
  }

  // checks for valid xml root
  const wxXmlNode* root = doc_xml.GetRoot();
  if (root->GetName() != "span_analyzer_doc") {
    // notifies user of error
    message = GetFilename() + "  --  "
              "Document file contains an invalid xml root. The document "
              "will close.";
    wxLogError(message.c_str());
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
      message = GetFilename() + "  --  "
                "Document file contains an invalid units attribute. The "
                "document will close.";
      wxLogError(message.c_str());
      wxMessageBox(message);

      // sets stream to invalide state and returns
      stream.Reset(wxSTREAM_READ_ERROR);
      return stream;
    }
  } else {
    // notifies user of error
    message = GetFilename() + "  --  "
              "Document file is missing units attribute. The document will "
              "close.";
    wxLogError(message.c_str());
    wxMessageBox(message);

    // sets stream to invalide state and returns
    stream.Reset(wxSTREAM_READ_ERROR);
    return stream;
  }

  // parses the XML node and loads into the document
  std::string filename = this->GetFilename();

  const std::list<WeatherLoadCase>& weathercases =
      wxGetApp().data()->groups_weathercase.cbegin()->weathercases;

  const std::list<CableFile>& cablefiles =
      wxGetApp().data()->cablefiles;

  int line_number = SpanAnalyzerDocXmlHandler::ParseNode(
      root, filename, &cablefiles, &weathercases, *this);
  if (line_number != 0) {
    // notifies user of error
    message = GetFilename() + ":" + std::to_string(line_number) + "  --  "
              "Document file contains a critical parsing error. The document "
              "will close.";
    wxLogError(message.c_str());
    wxMessageBox(message);

    // marks document as unmodified to avoid additional popup
    Modify(false);

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

bool SpanAnalyzerDoc::OnCreate(const wxString& path, long flags) {
  // initializes analysis controller
  controller_analysis_.set_data(wxGetApp().data());

  // calls base class function
  return wxDocument::OnCreate(path, flags);
}

void SpanAnalyzerDoc::ReplaceSpan(
    const std::list<Span>::const_iterator& element,
    const Span& span) {
  // gets iterator with edit capability
  const unsigned int index = std::distance(spans_.cbegin(), element);
  auto iter = std::next(spans_.begin(), index);

  // determines if span matches analysis span
  bool is_selected = false;
  if (&(*iter) == controller_analysis_.span()) {
    is_selected = true;
  }

  // replaces span in list
  *iter = Span(span);

  // sets document flag as modified
  Modify(true);

  // updates if span matched analysis span
  if (is_selected == true) {
    controller_analysis_.RunAnalysis();

    UpdateHint hint(HintType::kSpansEdit);
    UpdateAllViews(nullptr, &hint);
  }
}

const std::list<SagTensionAnalysisResultGroup>* SpanAnalyzerDoc::Results()
    const {
  return &controller_analysis_.results();
}

const std::list<SagTensionAnalysisResult>* SpanAnalyzerDoc::ResultsFiltered(
    const WeatherLoadCaseGroup& group_weathercases,
    const CableConditionType& condition) const {
  // gets the results
  const std::list<SagTensionAnalysisResultGroup>* results = Results();
  if (results == nullptr) {
    return nullptr;
  }

  // searches results groups for a matching weathercase group
  const SagTensionAnalysisResultGroup* group_results = nullptr;
  for (auto iter = results->cbegin(); iter != results->cend(); iter++) {
    group_results = &(*iter);
    if (group_results->group_weathercases == &group_weathercases) {
      break;
    }
  }

  if (group_results == nullptr) {
    return nullptr;
  }

  // gets the result list based on the current display condition
  const std::list<SagTensionAnalysisResult>* list_results = nullptr;
  if (condition == CableConditionType::kInitial) {
    list_results = &group_results->results_initial;
  } else if (condition == CableConditionType::kLoad) {
    list_results = &group_results->results_load;
  }

  return list_results;
}

void SpanAnalyzerDoc::RunAnalysis() const {
  controller_analysis_.RunAnalysis();
}

wxOutputStream& SpanAnalyzerDoc::SaveObject(wxOutputStream& stream) {
  // logs
  std::string message = "Saving document file: " + GetFilename();
  wxLogMessage(message.c_str());

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

void SpanAnalyzerDoc::SetSpanAnalysis(const Span* span) {
  controller_analysis_.set_span(span);
  controller_analysis_.RunAnalysis();

  UpdateHint hint(HintType::kSpansEdit);
  UpdateAllViews(nullptr, &hint);
}

const Span* SpanAnalyzerDoc::SpanAnalysis() const {
  return controller_analysis_.span();
}

const std::list<Span>& SpanAnalyzerDoc::spans() const {
  return spans_;
}
