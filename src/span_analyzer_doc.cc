// This is free and unencumbered software released into the public domain.
// For more information, please refer to <http://unlicense.org/>

#include "spananalyzer/span_analyzer_doc.h"

#include "appcommon/widgets/status_bar_log.h"
#include "wx/cmdproc.h"
#include "wx/xml/xml.h"

#include "spananalyzer/span_analyzer_app.h"
#include "spananalyzer/span_analyzer_doc_xml_handler.h"
#include "spananalyzer/span_unit_converter.h"

IMPLEMENT_DYNAMIC_CLASS(SpanAnalyzerDoc, wxDocument)

SpanAnalyzerDoc::SpanAnalyzerDoc() {
}

SpanAnalyzerDoc::~SpanAnalyzerDoc() {
}

bool SpanAnalyzerDoc::AppendSpan(const Span& span) {
  spans_.push_back(span);

  Modify(true);

  return true;
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
    SpanUnitConverter::ConvertUnitStyle(system, style_from, style_to,
                                        true, span);
  }

  // clears commands in the processor
  wxCommandProcessor* processor = GetCommandProcessor();
  processor->ClearCommands();
}

void SpanAnalyzerDoc::ConvertUnitSystem(const units::UnitSystem& system_from,
                                        const units::UnitSystem& system_to) {
  if (system_from == system_to) {
    return;
  }

  // converts spans
  for (auto it = spans_.begin(); it != spans_.end(); it++) {
    Span& span = *it;
    SpanUnitConverter::ConvertUnitSystem(system_from, system_to, true, span);
  }

  // clears commands in the processor
  wxCommandProcessor* processor = GetCommandProcessor();
  processor->ClearCommands();
}

bool SpanAnalyzerDoc::DeleteSpan(const int& index) {
  // checks index
  if (IsValidIndex(index, false) == false) {
    return false;
  }

  // deletes from span list
  auto iter = std::next(spans_.begin(), index);
  spans_.erase(iter);

  // marks as modified
  Modify(true);

  // updates activated span index
  if (index == index_activated_) {
    index_activated_ = -1;
  } else if (index < index_activated_) {
    index_activated_--;
  }

  // syncs controller
  SyncAnalysisController();

  return true;
}

int SpanAnalyzerDoc::IndexSpan(const Span* span) {
  // searches list of spans for a match
  for (auto iter = spans_.cbegin(); iter != spans_.cend(); iter++) {
    const Span* span_doc = &(*iter);
    if (span == span_doc) {
      return std::distance(spans_.cbegin(), iter);
    }
  }

  // if it reaches this point, no match was found
  return -1;
}

bool SpanAnalyzerDoc::InsertSpan(const int& index, const Span& span) {
  // checks index
  if (IsValidIndex(index, true) == false) {
    return false;
  }

  // inserts span
  auto iter = std::next(spans_.begin(), index);
  spans_.insert(iter, span);

  // marks as modified
  Modify(true);

  // updates activated span index
  if (index < index_activated_) {
    index_activated_++;
  }

  // syncs controller
  SyncAnalysisController();

  return true;
}

bool SpanAnalyzerDoc::IsUniqueName(const std::string& name) const {
  // searches all spans for a match
  for (auto iter = spans_.cbegin(); iter != spans_.cend(); iter++) {
    const Span& span = *iter;

    // compares and breaks if name is not unique
    if (span.name == name) {
      return false;
    }
  }

  // if it makes it to this point, the name is unique
  return true;
}

bool SpanAnalyzerDoc::IsValidIndex(const int& index,
                                   const bool& is_included_end) const {
  const int kSizeSpans = spans_.size();
  if ((0 <= index) && (index < kSizeSpans)) {
    return true;
  } else if ((index == kSizeSpans) && (is_included_end == true)) {
    return true;
  } else {
    return false;
  }
}

wxInputStream& SpanAnalyzerDoc::LoadObject(wxInputStream& stream) {
  wxBusyCursor cursor;

  std::string message;

  message = "Loading document file: " + this->GetFilename();
  wxLogVerbose(message.c_str());
  status_bar_log::PushText(message, 0);

  // attempts to load an xml document from the input stream
  wxXmlDocument doc_xml;
  if (doc_xml.Load(stream) == false) {
    // notifies user of error
    message = GetFilename() + "  --  "
              "Document file contains an invalid xml structure. The document "
              "will close.";
    wxLogError(message.c_str());
    wxMessageBox(message);

    status_bar_log::PopText(0);

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

    status_bar_log::PopText(0);

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

      status_bar_log::PopText(0);

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

    status_bar_log::PopText(0);

    // sets stream to invalide state and returns
    stream.Reset(wxSTREAM_READ_ERROR);
    return stream;
  }

  // parses the XML node and loads into the document
  std::string filename = this->GetFilename();

  const std::list<WeatherLoadCase*>& weathercases =
      wxGetApp().data()->weathercases;

  const std::list<CableFile*>& cablefiles =
      wxGetApp().data()->cablefiles;

  const bool status_node = SpanAnalyzerDocXmlHandler::ParseNode(
      root, filename, &cablefiles, &weathercases, *this);
  if (status_node == false) {
    // notifies user of error
    message = GetFilename() + "  --  "
              "Document file contains parsing error(s). Check logs.";
    wxMessageBox(message);
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

  status_bar_log::PopText(0);

  return stream;
}

bool SpanAnalyzerDoc::ModifySpan(const int& index, const Span& span) {
  // checks index
  if (IsValidIndex(index, false) == false) {
    return false;
  }

  // modifies span in list
  auto iter = std::next(spans_.begin(), index);
  *iter = Span(span);

  // sets document flag as modified
  Modify(true);

  // runs analysis if necessary
  if (index == index_activated_) {
    controller_analysis_.RunAnalysis();
  }

  return true;
}

bool SpanAnalyzerDoc::MoveSpan(const int& index_from, const int& index_to) {
  // checks indexes
  if (IsValidIndex(index_from, false) == false) {
    return false;
  }

  if (IsValidIndex(index_to, true) == false) {
    return false;
  }

  // gets an iterator to the activated span
  std::list<Span>::const_iterator iter_activated;
  if (index_activated_ != -1) {
    iter_activated = std::next(spans_.cbegin(), index_activated_);
  }

  // moves span in list
  auto iter_from = std::next(spans_.begin(), index_from);
  auto iter_to = std::next(spans_.begin(), index_to);

  spans_.splice(iter_to, spans_, iter_from);

  // marks as modified
  Modify(true);

  // updates activated index
  if (index_activated_ != -1) {
    index_activated_ = std::distance(spans_.cbegin(), iter_activated);
  }

  // syncs controller
  SyncAnalysisController();

  return true;
}

bool SpanAnalyzerDoc::OnCreate(const wxString& path, long flags) {
  // initializes activated span
  index_activated_ = -1;

  // initializes analysis controller
  controller_analysis_.set_weathercases(&wxGetApp().data()->weathercases);

  // initializes base structure
  StructureAttachment attachment;
  attachment.offset_longitudinal = 0;
  attachment.offset_transverse = 0;
  attachment.offset_vertical_top = 0;

  structure_.name = "";
  structure_.height = 100;
  structure_.attachments.push_back(attachment);

  // initializes hardware
  hardware_.name = "";
  hardware_.area_cross_section = 0;
  hardware_.length = 0;
  hardware_.type = Hardware::HardwareType::kDeadEnd;
  hardware_.weight = 0;

  // initializes line structures
  LineStructure line_structure;
  line_structure.set_height_adjustment(0);
  line_structure.set_offset(0);
  line_structure.set_rotation(0);
  line_structure.set_structure(&structure_);
  line_structure.AttachHardware(0, &hardware_);

  line_structure.set_station(0);
  line_structures_.push_back(line_structure);

  line_structure.set_station(1000);
  line_structures_.push_back(line_structure);

  // calls base class function
  return wxDocument::OnCreate(path, flags);
}

const SagTensionAnalysisResult* SpanAnalyzerDoc::Result(
    const int& index_weathercase,
    const CableConditionType& condition) const {
  return controller_analysis_.Result(index_weathercase, condition);
}

const std::vector<SagTensionAnalysisResult>* SpanAnalyzerDoc::Results(
    const CableConditionType& condition) const {
  return controller_analysis_.Results(condition);
}

void SpanAnalyzerDoc::RunAnalysis() const {
  controller_analysis_.RunAnalysis();
}

wxOutputStream& SpanAnalyzerDoc::SaveObject(wxOutputStream& stream) {
  wxBusyCursor cursor;

  // logs
  std::string message = "Saving document file: " + GetFilename();
  wxLogVerbose(message.c_str());
  status_bar_log::PushText(message, 0);

  // gets the unit system from app config
  units::UnitSystem units = wxGetApp().config()->units;

  // converts to a different unit style for saving
  ConvertUnitStyle(units, units::UnitStyle::kConsistent,
                   units::UnitStyle::kDifferent);

  // disconnects activated line cable
  DisconnectLineCableActivated();

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

  // creates an XML document and saves to stream
  wxXmlDocument doc_xml;
  doc_xml.SetRoot(root);
  doc_xml.Save(stream);

  // connects activated line cable
  ConnectLineCableActivated();

  // converts back to a consistent unit style
  ConvertUnitStyle(units, units::UnitStyle::kDifferent,
                   units::UnitStyle::kConsistent);

  // clears commands in the processor
  wxCommandProcessor* processor = GetCommandProcessor();
  processor->ClearCommands();

  status_bar_log::PopText(0);

  return stream;
}

const Span* SpanAnalyzerDoc::SpanActivated() const {
  return controller_analysis_.span();
}

const CableStretchState* SpanAnalyzerDoc::StretchState(
    const CableConditionType& condition) {
  return controller_analysis_.StretchState(condition);
}

int SpanAnalyzerDoc::index_activated() const {
  return index_activated_;
}

bool SpanAnalyzerDoc::set_index_activated(const int& index) {
  // checks if span is to be deactivated
  if (index == -1) {
    index_activated_ = index;
    SyncAnalysisController();
    return true;
  }

  // checks index
  if (IsValidIndex(index, false) == false) {
    return false;
  }

  // disconnects the existing line cable
  DisconnectLineCableActivated();

  // updates activated index
  index_activated_ = index;

  // connects the newly activated line cable
  ConnectLineCableActivated();

  // syncs controller
  SyncAnalysisController();

  return true;
}

const std::list<Span>& SpanAnalyzerDoc::spans() const {
  return spans_;
}

void SpanAnalyzerDoc::ConnectLineCableActivated() {
  if (index_activated_ == -1) {
    return;
  }

  Span& span = *std::next(spans_.begin(), index_activated_);
  LineCable& line_cable = span.linecable;
  LineCableConnection connection;

  connection.line_structure = &line_structures_[0];
  connection.index_attachment = 0;
  line_cable.AddConnection(connection);

  connection.line_structure = &line_structures_[1];
  connection.index_attachment = 0;
  line_cable.AddConnection(connection);
}

void SpanAnalyzerDoc::DisconnectLineCableActivated() {
  if (index_activated_ != -1) {
    Span& span = *std::next(spans_.begin(), index_activated_);
    span.linecable.ClearConnections();
  }
}

void SpanAnalyzerDoc::SyncAnalysisController() {
  // exits if no span is activated
  if (index_activated_ == -1) {
    controller_analysis_.set_span(nullptr);
    controller_analysis_.ClearResults();
    return;
  }

  // gets a pointer to the activated span
  const Span* span = &(*std::next(spans_.cbegin(), index_activated_));

  // forces controller to update if spans don't match
  if (span != controller_analysis_.span()) {
    controller_analysis_.set_span(span);
    controller_analysis_.RunAnalysis();
  }
}
