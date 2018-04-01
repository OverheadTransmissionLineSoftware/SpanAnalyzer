// This is free and unencumbered software released into the public domain.
// For more information, please refer to <http://unlicense.org/>

#include "results_pane.h"

#include "models/base/helper.h"
#include "models/transmissionline/catenary.h"
#include "wx/xrc/xmlres.h"

#include "span_analyzer_app.h"
#include "span_analyzer_doc.h"
#include "span_analyzer_view.h"

BEGIN_EVENT_TABLE(ResultsPane, wxPanel)
  EVT_CHOICE(XRCID("choice_report"), ResultsPane::OnChoiceReport)
  EVT_CHOICE(XRCID("choice_filter_group"), ResultsPane::OnChoiceFilterGroup)
  EVT_LIST_ITEM_SELECTED(wxID_ANY, ResultsPane::OnListCtrlSelect)
END_EVENT_TABLE()

ResultsPane::ResultsPane(wxWindow* parent, wxView* view) {
  // loads dialog from virtual xrc file system
  wxXmlResource::Get()->LoadPanel(this, parent, "results_pane");

  // saves view reference
  view_ = view;

  // initializes report choice
  type_report_ = ReportType::kSagTension;

  wxChoice* choice = XRCCTRL(*this, "choice_report", wxChoice);
  choice->Append("Sag-Tension");
  choice->Append("Tension Distribution");
  choice->Append("Catenary - Curve");
  choice->Append("Catenary - Endpoints");
  choice->Append("Length");
  choice->SetSelection(0);

  // initializes filter group choice
  UpdateFilterGroupChoice();

  // creates a report table
  table_ = new ReportTable(this);
  table_->set_data(&data_);

  wxBoxSizer* sizer = dynamic_cast<wxBoxSizer*>(GetSizer());
  sizer->Add(table_, 1, wxEXPAND);

  this->Fit();
}

ResultsPane::~ResultsPane() {
}

void ResultsPane::Update(wxObject* hint) {
  // interprets hint
  const UpdateHint* hint_update = dynamic_cast<UpdateHint*>(hint);
  if (hint_update == nullptr) {
    // do nothing, this is only passed when pane is created
  } else if (hint_update->type()
      == UpdateHint::Type::kAnalysisFilterGroupEdit) {
    UpdateFilterGroupChoice();
    UpdateReportData();
    table_->Refresh();
  } else if (hint_update->type()
      == UpdateHint::Type::kAnalysisFilterGroupSelect) {
    UpdateReportData();
    table_->Refresh();
  } else if (hint_update->type() == UpdateHint::Type::kAnalysisFilterSelect) {
    // do nothing
  } else if (hint_update->type() == UpdateHint::Type::kCablesEdit) {
    UpdateReportData();
    table_->Refresh();
  } else if (hint_update->type() == UpdateHint::Type::kPreferencesEdit) {
    UpdateReportData();
    table_->Refresh();
  } else if (hint_update->type() == UpdateHint::Type::kSpansEdit) {
    UpdateReportData();
    table_->Refresh();
  } else if (hint_update->type() == UpdateHint::Type::kWeathercasesEdit) {
    UpdateReportData();
    table_->Refresh();
  }
}

void ResultsPane::OnChoiceFilterGroup(wxCommandEvent& event) {
  // not creating busy cursor to avoid cursor flicker

  // gets weathercase set from application data
  wxChoice* choice = XRCCTRL(*this, "choice_filter_group", wxChoice);
  wxString str_selection = choice->GetStringSelection();

  // updates the selected/cached weathercases
  UpdateFilterGroupSelected();

  // updates views
  UpdateHint hint(UpdateHint::Type::kAnalysisFilterGroupSelect);
  view_->GetDocument()->UpdateAllViews(nullptr, &hint);
}

void ResultsPane::OnChoiceReport(wxCommandEvent& event) {
  // not creating busy cursor to avoid cursor flicker

  // gets choice selection
  wxChoice* choice = XRCCTRL(*this, "choice_report", wxChoice);
  wxString str = choice->GetStringSelection();

  // updates report type
  if (str == "Sag-Tension") {
    type_report_ = ReportType::kSagTension;
  } else if (str == "Tension Distribution") {
    type_report_ = ReportType::kTensionDistribution;
  } else if (str == "Catenary - Curve") {
    type_report_ = ReportType::kCatenary;
  } else if (str == "Catenary - Endpoints") {
    type_report_ = ReportType::kCatenaryEndpoints;
  } else if (str == "Length") {
    type_report_ = ReportType::kLength;
  } else {
    return;
  }

  // this update only affects this pane, so a view update is not sent
  // updates the report data and table
  UpdateReportData();
  table_->Refresh();
}

void ResultsPane::OnListCtrlSelect(wxListEvent& event) {
  // not creating busy cursor to avoid cursor flicker

  wxLogVerbose("Updating displayed analysis filter index.");

  // gets view
  SpanAnalyzerView* view = dynamic_cast<SpanAnalyzerView*>(view_);

  // gets selected index
  const long index_selected = event.GetItem().GetId();

  // updates report table
  table_->set_index_selected(index_selected);

  // gets selected index with no sorting applied
  // this still may not be the correct index, as invalid results are excluded
  // from the table
  const long index_unsorted = table_->IndexReportRow(index_selected);

  // gets the selected-sorted-unfiltered index
  // this will account for any invalid results that were left out of the table
  // this is done by comparing the weathercase/condition combination

  // extracts string from table
  long index_document = -1;
  const std::string& str_table = table_->ValueTable(index_unsorted, 0) + ","
                                 + table_->ValueTable(index_unsorted, 1);

  // searches analysis filters for a match
  const std::list<AnalysisFilter>& filters = view->group_filters()->filters;
  for (auto iter = filters.cbegin(); iter != filters.cend(); iter++) {
    // gets filter
    const AnalysisFilter& filter = *iter;

    // extracts string from filter
    std::string str_filter = filter.weathercase->description + ",";
    if (filter.condition == CableConditionType::kCreep) {
      str_filter += "Creep";
    } else if (filter.condition == CableConditionType::kInitial) {
      str_filter += "Initial";
    } else if (filter.condition == CableConditionType::kLoad) {
      str_filter += "Load";
    }

    // compares table string to filter string
    if (str_table == str_filter) {
      index_document = std::distance(filters.cbegin(), iter);
      break;
    }
  }

  // updates view index
  view->set_index_filter(index_document);

  // updates views
  UpdateHint hint(UpdateHint::Type::kAnalysisFilterSelect);
  view_->GetDocument()->UpdateAllViews(nullptr, &hint);
}

void ResultsPane::UpdateFilterGroupChoice() {
  // gets choice control
  wxChoice* choice = XRCCTRL(*this, "choice_filter_group", wxChoice);

  // saves current choice string
  std::string str_choice = choice->GetStringSelection();

  // clears choice control
  choice->Clear();

  // appends filter groups stored in the application data
  const std::list<AnalysisFilterGroup>& groups =
      wxGetApp().data()->groups_filters;
  for (auto iter = groups.cbegin(); iter != groups.cend(); iter ++) {
    const AnalysisFilterGroup& group = *iter;
    choice->Append(group.name);
  }

  // attempts to find the old filter group
  choice->SetSelection(choice->FindString(str_choice));

  // updates the selected/cached filter
  UpdateFilterGroupSelected();
}

void ResultsPane::UpdateFilterGroupSelected() {
  wxLogVerbose("Updating displayed analysis filters.");

  // initializes filters cached in view
  SpanAnalyzerView* view = dynamic_cast<SpanAnalyzerView*>(view_);
  view->set_group_filters(nullptr);

  // searches the choice control to see if a filter group is selected
  wxChoice* choice = XRCCTRL(*this, "choice_filter_group", wxChoice);
  std::string str_selection = choice->GetStringSelection();

  // gets filter groups from app data
  const std::list<AnalysisFilterGroup>& groups_filter =
       wxGetApp().data()->groups_filters;
  const AnalysisFilterGroup* group = nullptr;
  for (auto iter = groups_filter.cbegin(); iter != groups_filter.cend();
       iter++) {
    const AnalysisFilterGroup& group_temp = *iter;
    if (group_temp.name == str_selection) {
      group = &group_temp;
      break;
    }
  }

  // updates view with filter group
  view->set_group_filters(group);

  if (group == nullptr) {
    return;
  }

  // updates the view if the index if it is not valid anymore
  if ((int)group->filters.size() <= view->index_filter()) {
    view->set_index_filter(-1);
  }
}

void ResultsPane::UpdateReportData() {
  wxLogVerbose("Updating report table data.");

  // gets view display information
  SpanAnalyzerView* view = dynamic_cast<SpanAnalyzerView*>(view_);
  const AnalysisFilterGroup* group_filters = view->group_filters();

  // gets filters
  const std::list<AnalysisFilter>* filters = nullptr;
  if (group_filters != nullptr) {
    filters = &group_filters->filters;
  }

  // creates a list of results depending on filter
  const SpanAnalyzerDoc* doc =
      dynamic_cast<SpanAnalyzerDoc*>(view_->GetDocument());
  std::list<const SagTensionAnalysisResult*> results;

  if (filters != nullptr) {
    for (auto iter = filters->cbegin(); iter != filters->cend(); iter++) {
      const AnalysisFilter& filter = *iter;
      const int index = view->IndexWeathercase(filter);
      const SagTensionAnalysisResult* result = doc->Result(index,
                                                           filter.condition);
      if (result != nullptr) {
        results.push_back(result);
      }
    }
  }

  // selects based on report type
  if (type_report_ == ReportType::kCatenary) {
    UpdateReportDataCatenaryCurve(&results);
  } else if (type_report_ == ReportType::kCatenaryEndpoints) {
    UpdateReportDataCatenaryEndpoints(&results);
  } else if (type_report_ == ReportType::kLength) {
    UpdateReportDataLength(&results);
  } else if (type_report_ == ReportType::kSagTension) {
    UpdateReportDataSagTension(&results);
  } else if (type_report_ == ReportType::kTensionDistribution) {
    UpdateReportDataTensionDistribution(&results);
  }

  // resets view filter index if no data is present
  if (results.empty() == true) {
    view->set_index_filter(-1);
  }
}

void ResultsPane::UpdateReportDataCatenaryCurve(
    const std::list<const SagTensionAnalysisResult*>* results) {
  // initializes data
  data_.headers.clear();
  data_.rows.clear();

  // fills column headers
  ReportColumnHeader header;
  header.title = "Weathercase";
  header.format = wxLIST_FORMAT_LEFT;
  header.width = 200;
  data_.headers.push_back(header);

  header.title = "Condition";
  header.format = wxLIST_FORMAT_CENTER;
  header.width = wxLIST_AUTOSIZE;
  data_.headers.push_back(header);

  header.title = "H";
  header.format = wxLIST_FORMAT_CENTER;
  header.width = wxLIST_AUTOSIZE;
  data_.headers.push_back(header);

  header.title = "w";
  header.format = wxLIST_FORMAT_CENTER;
  header.width = wxLIST_AUTOSIZE;
  data_.headers.push_back(header);

  header.title = "H/w";
  header.format = wxLIST_FORMAT_CENTER;
  header.width = wxLIST_AUTOSIZE;
  data_.headers.push_back(header);

  header.title = "Sag";
  header.format = wxLIST_FORMAT_CENTER;
  header.width = wxLIST_AUTOSIZE;
  data_.headers.push_back(header);

  header.title = "L";
  header.format = wxLIST_FORMAT_CENTER;
  header.width = wxLIST_AUTOSIZE;
  data_.headers.push_back(header);

  header.title = "Ls";
  header.format = wxLIST_FORMAT_CENTER;
  header.width = wxLIST_AUTOSIZE;
  data_.headers.push_back(header);

  header.title = "Swing";
  header.format = wxLIST_FORMAT_CENTER;
  header.width = wxLIST_AUTOSIZE;
  data_.headers.push_back(header);

  // checks if results has any data
  if (results->empty() == true) {
    return;
  }

  // gets the selected span from the document
  const SpanAnalyzerDoc* doc =
      dynamic_cast<SpanAnalyzerDoc*>(view_->GetDocument());
  const Span* span = doc->SpanActivated();

  // fills each row with data
  for (auto iter = results->cbegin(); iter != results->cend(); iter++) {
    const SagTensionAnalysisResult* result = *iter;

    // creates a report row, which will be filled out by each result
    ReportRow row;

    // gets the weathercase string
    const std::string& str_weathercase = result->weathercase->description;

    // gets condition string
    std::string str_condition;
    if (result->condition == CableConditionType::kCreep) {
      str_condition = "Creep";
    } else if (result->condition == CableConditionType::kInitial) {
      str_condition = "Initial";
    } else if (result->condition == CableConditionType::kLoad) {
      str_condition = "Load";
    }

    // creates a catenary to calculate results
    Catenary3d catenary;
    catenary.set_spacing_endpoints(span->spacing_attachments);
    catenary.set_tension_horizontal(result->tension_horizontal);
    catenary.set_weight_unit(result->weight_unit);

    double value;
    std::string str;

    // adds weathercase
    row.values.push_back(str_weathercase);

    // adds condition
    row.values.push_back(str_condition);

    // adds H
    value = catenary.tension_horizontal();
    str = helper::DoubleToFormattedString(value, 1);
    row.values.push_back(str);

    // adds w
    value = catenary.weight_unit().Magnitude();
    str = helper::DoubleToFormattedString(value, 3);
    row.values.push_back(str);

    // adds H/w
    value = catenary.Constant();
    str = helper::DoubleToFormattedString(value, 1);
    row.values.push_back(str);

    // adds sag
    value = catenary.Sag();
    str = helper::DoubleToFormattedString(value, 1);
    row.values.push_back(str);

    // adds L
    value = catenary.Length();
    str = helper::DoubleToFormattedString(value, 2);
    row.values.push_back(str);

    // adds Ls
    value = catenary.LengthSlack();
    str = helper::DoubleToFormattedString(value, 2);
    row.values.push_back(str);

    // adds swing
    value = catenary.SwingAngle();
    str = helper::DoubleToFormattedString(value, 1);
    row.values.push_back(str);

    // appends row to list
    data_.rows.push_back(row);
  }
}

void ResultsPane::UpdateReportDataCatenaryEndpoints(
    const std::list<const SagTensionAnalysisResult*>* results) {
  // initializes data
  data_.headers.clear();
  data_.rows.clear();

  // fills column headers
  ReportColumnHeader header;
  header.title = "Weathercase";
  header.format = wxLIST_FORMAT_LEFT;
  header.width = 200;
  data_.headers.push_back(header);

  header.title = "Condition";
  header.format = wxLIST_FORMAT_CENTER;
  header.width = wxLIST_AUTOSIZE;
  data_.headers.push_back(header);

  header.title = "Ts";
  header.format = wxLIST_FORMAT_CENTER;
  header.width = wxLIST_AUTOSIZE;
  data_.headers.push_back(header);

  header.title = "Tv";
  header.format = wxLIST_FORMAT_CENTER;
  header.width = wxLIST_AUTOSIZE;
  data_.headers.push_back(header);

  header.title = "A";
  header.format = wxLIST_FORMAT_CENTER;
  header.width = wxLIST_AUTOSIZE;
  data_.headers.push_back(header);

  header.title = "";
  header.format = wxLIST_FORMAT_CENTER;
  header.width = wxLIST_AUTOSIZE;
  data_.headers.push_back(header);

  header.title = "Ts";
  header.format = wxLIST_FORMAT_CENTER;
  header.width = wxLIST_AUTOSIZE;
  data_.headers.push_back(header);

  header.title = "Tv";
  header.format = wxLIST_FORMAT_CENTER;
  header.width = wxLIST_AUTOSIZE;
  data_.headers.push_back(header);

  header.title = "A";
  header.format = wxLIST_FORMAT_CENTER;
  header.width = wxLIST_AUTOSIZE;
  data_.headers.push_back(header);

  // checks if results has any data
  if (results->empty() == true) {
    return;
  }

  // gets the selected span from the document
  const SpanAnalyzerDoc* doc =
      dynamic_cast<SpanAnalyzerDoc*>(view_->GetDocument());

  const Span* span = doc->SpanActivated();

  // fills each row with data
  for (auto iter = results->cbegin(); iter != results->cend(); iter++) {
    const SagTensionAnalysisResult* result = *iter;

    // creates a report row, which will be filled out by each result
    ReportRow row;

    // gets the weathercase string
    const std::string& str_weathercase = result->weathercase->description;

    // gets condition string
    std::string str_condition;
    if (result->condition == CableConditionType::kCreep) {
      str_condition = "Creep";
    } else if (result->condition == CableConditionType::kInitial) {
      str_condition = "Initial";
    } else if (result->condition == CableConditionType::kLoad) {
      str_condition = "Load";
    }

    // creates a catenary to calculate results
    Catenary3d catenary;
    catenary.set_spacing_endpoints(span->spacing_attachments);
    catenary.set_tension_horizontal(result->tension_horizontal);
    catenary.set_weight_unit(result->weight_unit);

    double value;
    std::string str;

    // adds weathercase
    row.values.push_back(str_weathercase);

    // adds condition
    row.values.push_back(str_condition);

    // adds Ts
    value = catenary.Tension(0);
    str = helper::DoubleToFormattedString(value, 1);
    row.values.push_back(str);

    // adds Tv
    value = catenary.Tension(0, AxisDirectionType::kPositive).z();
    str = helper::DoubleToFormattedString(value, 1);
    row.values.push_back(str);

    // adds A
    value = catenary.TangentAngleVertical(0, AxisDirectionType::kPositive);
    str = helper::DoubleToFormattedString(value, 1);
    row.values.push_back(str);

    // adds blank
    row.values.push_back("");

    // adds Ts
    value = catenary.Tension(1);
    str = helper::DoubleToFormattedString(value, 1);
    row.values.push_back(str);

    // adds Tv
    value = catenary.Tension(1, AxisDirectionType::kNegative).z();
    str = helper::DoubleToFormattedString(value, 1);
    row.values.push_back(str);

    // adds A
    value = catenary.TangentAngleVertical(1, AxisDirectionType::kNegative);
    str = helper::DoubleToFormattedString(value, 1);
    row.values.push_back(str);

    // appends row to list
    data_.rows.push_back(row);
  }
}

void ResultsPane::UpdateReportDataLength(
    const std::list<const SagTensionAnalysisResult*>* results) {
  // initializes data
  data_.headers.clear();
  data_.rows.clear();

  // fills column headers
  ReportColumnHeader header;
  header.title = "Weathercase";
  header.format = wxLIST_FORMAT_LEFT;
  header.width = 200;
  data_.headers.push_back(header);

  header.title = "Condition";
  header.format = wxLIST_FORMAT_CENTER;
  header.width = wxLIST_AUTOSIZE;
  data_.headers.push_back(header);

  header.title = "Lu";
  header.format = wxLIST_FORMAT_CENTER;
  header.width = wxLIST_AUTOSIZE;
  data_.headers.push_back(header);

  header.title = "Ll";
  header.format = wxLIST_FORMAT_CENTER;
  header.width = wxLIST_AUTOSIZE;
  data_.headers.push_back(header);

  // checks if results has any data
  if (results->empty() == true) {
    return;
  }

  // gets the selected span from the document
  const SpanAnalyzerDoc* doc =
      dynamic_cast<SpanAnalyzerDoc*>(view_->GetDocument());

  const Span* span = doc->SpanActivated();

  // fills each row with data
  for (auto iter = results->cbegin(); iter != results->cend(); iter++) {
    const SagTensionAnalysisResult* result = *iter;

    // creates a report row, which will be filled out by each result
    ReportRow row;

    // gets the weathercase string
    const std::string& str_weathercase = result->weathercase->description;

    // gets condition string
    std::string str_condition;
    if (result->condition == CableConditionType::kCreep) {
      str_condition = "Creep";
    } else if (result->condition == CableConditionType::kInitial) {
      str_condition = "Initial";
    } else if (result->condition == CableConditionType::kLoad) {
      str_condition = "Load";
    }

    // creates a catenary to calculate results
    Catenary3d catenary;
    catenary.set_spacing_endpoints(span->spacing_attachments);
    catenary.set_tension_horizontal(result->tension_horizontal);
    catenary.set_weight_unit(result->weight_unit);

    double value;
    std::string str;

    // adds weathercase
    row.values.push_back(str_weathercase);

    // adds condition
    row.values.push_back(str_condition);

    // adds Lu
    value = result->length_unloaded;
    str = helper::DoubleToFormattedString(value, 2);
    row.values.push_back(str);

    // adds Ll
    value = catenary.Length();
    str = helper::DoubleToFormattedString(value, 2);
    row.values.push_back(str);

    // appends row to list
    data_.rows.push_back(row);
  }
}

void ResultsPane::UpdateReportDataSagTension(
    const std::list<const SagTensionAnalysisResult*>* results) {
  // initializes data
  data_.headers.clear();
  data_.rows.clear();

  // fills column headers
  ReportColumnHeader header;
  header.title = "Weathercase";
  header.format = wxLIST_FORMAT_LEFT;
  header.width = 200;
  data_.headers.push_back(header);

  header.title = "Condition";
  header.format = wxLIST_FORMAT_CENTER;
  header.width = wxLIST_AUTOSIZE;
  data_.headers.push_back(header);

  header.title = "Wv";
  header.format = wxLIST_FORMAT_CENTER;
  header.width = wxLIST_AUTOSIZE;
  data_.headers.push_back(header);

  header.title = "Wt";
  header.format = wxLIST_FORMAT_CENTER;
  header.width = wxLIST_AUTOSIZE;
  data_.headers.push_back(header);

  header.title = "Wr";
  header.format = wxLIST_FORMAT_CENTER;
  header.width = wxLIST_AUTOSIZE;
  data_.headers.push_back(header);

  header.title = "H";
  header.format = wxLIST_FORMAT_CENTER;
  header.width = wxLIST_AUTOSIZE;
  data_.headers.push_back(header);

  header.title = "H/w";
  header.format = wxLIST_FORMAT_CENTER;
  header.width = wxLIST_AUTOSIZE;
  data_.headers.push_back(header);

  // checks if results has any data
  if (results->empty() == true) {
    return;
  }

  // fills each row with data
  for (auto iter = results->cbegin(); iter != results->cend(); iter++) {
    const SagTensionAnalysisResult* result = *iter;

    // creates a report row, which will be filled out by each result
    ReportRow row;

    // gets the weathercase string
    const std::string& str_weathercase = result->weathercase->description;

    // gets condition string
    std::string str_condition;
    if (result->condition == CableConditionType::kCreep) {
      str_condition = "Creep";
    } else if (result->condition == CableConditionType::kInitial) {
      str_condition = "Initial";
    } else if (result->condition == CableConditionType::kLoad) {
      str_condition = "Load";
    }

    double value;
    std::string str;

    // adds weathercase
    row.values.push_back(str_weathercase);

    // adds condition
    row.values.push_back(str_condition);

    // adds Wv
    value = result->weight_unit.z();
    str = helper::DoubleToFormattedString(value, 3);
    row.values.push_back(str);

    // adds Wt
    value = result->weight_unit.y();
    str = helper::DoubleToFormattedString(value, 3);
    row.values.push_back(str);

    // adds Wr
    value = result->weight_unit.Magnitude();
    str = helper::DoubleToFormattedString(value, 3);
    row.values.push_back(str);

    // adds H
    value = result->tension_horizontal;
    str = helper::DoubleToFormattedString(value, 1);
    row.values.push_back(str);

    // adds H/w
    value = result->tension_horizontal / result->weight_unit.Magnitude();
    str = helper::DoubleToFormattedString(value, 1);
    row.values.push_back(str);

    // appends row to list
    data_.rows.push_back(row);
  }
}

void ResultsPane::UpdateReportDataTensionDistribution(
    const std::list<const SagTensionAnalysisResult*>* results) {
  // initializes data
  data_.headers.clear();
  data_.rows.clear();

  // fills column headers
  ReportColumnHeader header;
  header.title = "Weathercase";
  header.format = wxLIST_FORMAT_LEFT;
  header.width = 200;
  data_.headers.push_back(header);

  header.title = "Condition";
  header.format = wxLIST_FORMAT_CENTER;
  header.width = wxLIST_AUTOSIZE;
  data_.headers.push_back(header);

  header.title = "Hs";
  header.format = wxLIST_FORMAT_CENTER;
  header.width = wxLIST_AUTOSIZE;
  data_.headers.push_back(header);

  header.title = "Hc";
  header.format = wxLIST_FORMAT_CENTER;
  header.width = wxLIST_AUTOSIZE;
  data_.headers.push_back(header);

  // checks if results has any data
  if (results->empty() == true) {
    return;
  }

  // fills each row with data
  for (auto iter = results->cbegin(); iter != results->cend(); iter++) {
    const SagTensionAnalysisResult* result = *iter;

    // creates a report row, which will be filled out by each result
    ReportRow row;

    // gets the weathercase string
    const std::string& str_weathercase = result->weathercase->description;

    // gets condition string
    std::string str_condition;
    if (result->condition == CableConditionType::kCreep) {
      str_condition = "Creep";
    } else if (result->condition == CableConditionType::kInitial) {
      str_condition = "Initial";
    } else if (result->condition == CableConditionType::kLoad) {
      str_condition = "Load";
    }

    double value;
    std::string str;

    // adds weathercase
    row.values.push_back(str_weathercase);

    // adds condition
    row.values.push_back(str_condition);

    // adds Hs
    value = result->tension_horizontal_shell;
    str = helper::DoubleToFormattedString(value, 1);
    row.values.push_back(str);

    // adds Hc
    value = result->tension_horizontal_core;
    str = helper::DoubleToFormattedString(value, 1);
    row.values.push_back(str);

    // appends row to list
    data_.rows.push_back(row);
  }
}
