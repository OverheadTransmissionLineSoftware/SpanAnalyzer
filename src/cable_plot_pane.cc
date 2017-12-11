// This is free and unencumbered software released into the public domain.
// For more information, please refer to <http://unlicense.org/>

#include "cable_plot_pane.h"

#include "appcommon/graphics/circle_renderer_2d.h"
#include "appcommon/graphics/line_renderer_2d.h"
#include "appcommon/graphics/text_renderer_2d.h"
#include "appcommon/widgets/status_bar_log.h"
#include "models/base/helper.h"
#include "wx/dcbuffer.h"

#include "span_analyzer_doc.h"
#include "span_analyzer_view.h"

/// \par OVERVIEW
///
/// This is the enumeration for the context menu.
enum {
  kFitPlotData = 0,
};

BEGIN_EVENT_TABLE(CablePlotPane, PlotPane2d)
  EVT_MENU(wxID_ANY, CablePlotPane::OnContextMenuSelect)
  EVT_MOTION(CablePlotPane::OnMouse)
  EVT_RIGHT_DOWN(CablePlotPane::OnMouse)
END_EVENT_TABLE()

CablePlotPane::CablePlotPane(
    wxWindow* parent, wxView* view)
    : PlotPane2d(parent) {
  view_ = view;

  plot_.set_zoom_factor_fitted(1.2);
}

CablePlotPane::~CablePlotPane() {
}

void CablePlotPane::Update(wxObject* hint) {
  // typically only null on initialization
  if (hint == nullptr) {
    return;
  }

  // gets a buffered dc to prevent flickering
  wxClientDC dc(this);
  wxBufferedDC dc_buf(&dc, bitmap_buffer_);

  // interprets hint
  const UpdateHint* hint_update = dynamic_cast<UpdateHint*>(hint);
  if (hint_update == nullptr) {
    UpdatePlotDatasets();
    UpdatePlotRenderers();
    view_->OnDraw(&dc_buf);
  } else if (hint_update->type() == HintType::kAnalysisFilterGroupEdit) {
    UpdatePlotDatasets();
    UpdatePlotRenderers();
    view_->OnDraw(&dc_buf);
  } else if (hint_update->type() == HintType::kAnalysisFilterGroupSelect) {
    UpdatePlotDatasets();
    UpdatePlotRenderers();
    view_->OnDraw(&dc_buf);
  } else if (hint_update->type() == HintType::kAnalysisFilterSelect) {
    UpdatePlotDatasets();
    UpdatePlotRenderers();
    view_->OnDraw(&dc_buf);
  } else if (hint_update->type() == HintType::kCablesEdit) {
    UpdatePlotDatasets();
    UpdatePlotRenderers();
    view_->OnDraw(&dc_buf);
  } else if (hint_update->type() == HintType::kPreferencesEdit) {
    UpdatePlotDatasets();
    UpdatePlotRenderers();
    view_->OnDraw(&dc_buf);
  } else if (hint_update->type() == HintType::kSpansEdit) {
    UpdatePlotDatasets();
    UpdatePlotRenderers();
    view_->OnDraw(&dc_buf);
  } else if (hint_update->type() == HintType::kViewSelect) {
    UpdatePlotDatasets();
    UpdatePlotRenderers();
    view_->OnDraw(&dc_buf);
  } else if (hint_update->type() == HintType::kWeathercasesEdit) {
    UpdatePlotDatasets();
    UpdatePlotRenderers();
    view_->OnDraw(&dc_buf);
  }
}

void CablePlotPane::ClearDataSets() {
  dataset_axis_lines_.Clear();
  dataset_axis_text_.Clear();
  dataset_core_.Clear();
  dataset_markers_.Clear();
  dataset_shell_.Clear();
  dataset_total_.Clear();
  strains_.clear();
}

void CablePlotPane::OnContextMenuSelect(wxCommandEvent& event) {
  // not creating busy cursor to avoid cursor flicker

  // gets context menu selection and sends to handler function
  const int id_event = event.GetId();
  if (id_event == kFitPlotData) {
    // toggles plot fit
    if (plot_.is_fitted() == true) {
      plot_.set_is_fitted(false);
    } else {
      plot_.set_is_fitted(true);
      this->Refresh();
    }
  }
}

void CablePlotPane::OnMouse(wxMouseEvent& event) {
  // skips if no plot renderers are active
  if (plot_.HasRenderers() == false) {
    return;
  }

  // overrides for right mouse click
  if (event.RightDown() == true) {
    // builds a context menu
    wxMenu menu;

    menu.AppendCheckItem(kFitPlotData, "Fit Plot");
    menu.Check(kFitPlotData, plot_.is_fitted());

    // shows context menu
    // the event is caught by the pane
    PopupMenu(&menu, event.GetPosition());

    // stops processing event (needed to allow pop-up menu to catch its event)
    event.Skip();
  } else {
    // calls base function
    PlotPane2d::OnMouse(event);
  }

  // updates status bar
  if (plot_.HasRenderers() == true) {
    // converts graphics point to data point
    wxPoint point_graphics;
    point_graphics.x = event.GetX();
    point_graphics.y = event.GetY();
    const Point2d<float> point_data = plot_.PointGraphicsToData(point_graphics);

    // logs to status bar
    std::string str = "X="
                      + helper::DoubleToFormattedString(point_data.x, 5)
                      + "   Y="
                      + helper::DoubleToFormattedString(point_data.y, 2);

    status_bar_log::SetText(str, 1);
  } else {
    status_bar_log::SetText("", 1);
  }
}

void CablePlotPane::UpdateDataSetAxes(
    const double& x_min, const double& x_max,
    const double& y_min, const double& y_max) {
  Line2d* line = nullptr;
  Text2d* text = nullptr;

  // x-axis line
  line = new Line2d();
  line->p0.x = -0.001;
  line->p0.y = 0;
  line->p1.x = x_max;
  line->p1.y = 0;
  dataset_axis_lines_.Add(line);

  // y-axis line
  line = new Line2d();
  line->p0.x = 0;
  line->p0.y = 0;
  line->p1.x = 0;
  line->p1.y = y_max;
  dataset_axis_lines_.Add(line);

  // x-axis labels
  text = new Text2d();
  text->angle = 0;
  text->message = "Strain";
  text->point.x = (x_max + x_min) / 2;
  text->point.y = 0;
  text->position = Text2d::BoundaryPosition::kCenterUpper;
  dataset_axis_text_.Add(text);

  text = new Text2d();
  text->angle = 0;
  text->message = helper::DoubleToFormattedString(x_min, 2);
  text->point.x = x_min;
  text->point.y = 0;
  text->position = Text2d::BoundaryPosition::kLeftUpper;
  dataset_axis_text_.Add(text);

  text = new Text2d();
  text->angle = 0;
  text->message = helper::DoubleToFormattedString(x_max, 2);
  text->point.x = x_max;
  text->point.y = 0;
  text->position = Text2d::BoundaryPosition::kCenterUpper;
  dataset_axis_text_.Add(text);

  // y-axis labels
  text = new Text2d();
  text->angle = 90;
  text->message = "Load";
  text->point.x = 0;
  text->point.y = (y_max + y_min) / 2;
  text->position = Text2d::BoundaryPosition::kCenterLower;
  dataset_axis_text_.Add(text);

  text = new Text2d();
  text->angle = 0;
  text->message = helper::DoubleToFormattedString(y_min, 0) + " ";
  text->point.x = 0;
  text->point.y = y_min;
  text->position = Text2d::BoundaryPosition::kRightLower;
  dataset_axis_text_.Add(text);

  text = new Text2d();
  text->angle = 0;
  text->message = helper::DoubleToFormattedString(y_max, 0) + " ";
  text->point.x = 0;
  text->point.y = y_max;
  text->position = Text2d::BoundaryPosition::kRightCenter;
  dataset_axis_text_.Add(text);
}

void CablePlotPane::UpdateDataSetCable(
    const CableElongationModel& model,
    const CableElongationModel::ComponentType& type_component,
    LineDataSet2d& dataset) {
  // calculates points
  std::list<Point2d<double>> points;
  const double kStrengthRated = *model.cable()->strength_rated();
  for (auto iter = strains_.cbegin(); iter != strains_.cend(); iter++) {
    Point2d<double> point;
    point.x = *iter;
    point.y = model.Load(type_component, point.x);

    // skips plot point if the load is beyond the cable rated strength
    if (point.y < kStrengthRated) {
      points.push_back(point);
    }
  }

  // converts points to lines and adds to dataset
  for (auto iter = points.cbegin(); iter != std::prev(points.cend(), 1);
       iter++) {
    // gets current and next point in the list
    const Point2d<double>& p0 = *iter;
    const Point2d<double>& p1 = *(std::next(iter, 1));

    // creates a line
    Line2d* line = new Line2d();
    line->p0.x = p0.x;
    line->p0.y = p0.y;
    line->p1.x = p1.x;
    line->p1.y = p1.y;

    dataset.Add(line);
  }
}

void CablePlotPane::UpdateDataSetMarker(
    const CableElongationModel& model,
    const SagTensionAnalysisResult* result) {
  // calculates points
  Point2d<double> point;
  std::list<Point2d<double>> points;

  point.y = result->tension_average_core;
  point.x = model.Strain(CableElongationModel::ComponentType::kCore, point.y);
  points.push_back(point);

  point.y = result->tension_average_shell;
  point.x = model.Strain(CableElongationModel::ComponentType::kShell, point.y);
  points.push_back(point);

  point.y = result->tension_average;
  point.x = model.Strain(CableElongationModel::ComponentType::kCombined,
                         point.y);
  points.push_back(point);

  // updates marker circle dataset
  for (auto iter = points.cbegin(); iter != points.cend(); iter++) {
    const Point2d<double>& point = *iter;

    // skips plot point if the load is beyond the maximum strain or cable rated
    // strength
    if ((0.01 < point.x) || (*model.cable()->strength_rated() < point.y)) {
      continue;
    }

    // creates a circle and adds to dataset
    Circle2d* circle = new Circle2d();
    circle->radius = 3;
    circle->center.x = static_cast<float>(point.x);
    circle->center.y = static_cast<float>(point.y);

    dataset_markers_.Add(circle);
  }
}

void CablePlotPane::UpdatePlotDatasets() {
  wxLogVerbose("Updating cable elongation model plot datasets.");

  ClearDataSets();

  // gets view settings
  SpanAnalyzerView* view = dynamic_cast<SpanAnalyzerView*>(view_);

  // gets filter group
  const AnalysisFilterGroup* group_filters = view->group_filters();
  if (group_filters == nullptr) {
    return;
  }

  // gets analysis result filter
  const AnalysisFilter* filter = view->AnalysisFilterActive();
  if (filter == nullptr) {
    return;
  }

  // gets weathercase index
  const int index = view->IndexWeathercase(*filter);

  // gets filtered result from doc
  SpanAnalyzerDoc* doc = dynamic_cast<SpanAnalyzerDoc*>(view_->GetDocument());
  const SagTensionAnalysisResult* result = doc->Result(index,
                                                       filter->condition);
  if (result == nullptr) {
    return;
  }

  // creates a cable elongation model
  const Span* span = doc->SpanAnalysis();

  SagTensionCable cable;
  cable.set_cable_base(span->linecable.cable());

  CableElongationModel model;
  model.set_cable(&cable);
  model.set_state(result->state);
  model.set_state_stretch(*doc->StretchState(result->condition));

  // updates datasets
  UpdateDataSetAxes(0, 0.01, 0, *cable.strength_rated());

  UpdateStrains(model);

  UpdateDataSetCable(model,
                     CableElongationModel::ComponentType::kCore,
                     dataset_core_);

  UpdateDataSetCable(model,
                     CableElongationModel::ComponentType::kShell,
                     dataset_shell_);

  UpdateDataSetCable(model,
                     CableElongationModel::ComponentType::kCombined,
                     dataset_total_);

  UpdateDataSetMarker(model, result);
}

void CablePlotPane::UpdatePlotRenderers() {
  wxLogVerbose("Updating cable elongation model plot renderers.");

  // clears plot datasets and renderers
  plot_.ClearRenderers();

  // updates renderers
  CircleRenderer2d* renderer_circle = nullptr;
  LineRenderer2d* renderer_line = nullptr;
  TextRenderer2d* renderer_text = nullptr;

  renderer_line = new LineRenderer2d();
  renderer_line->set_dataset(&dataset_core_);
  renderer_line->set_pen(wxRED_PEN);
  plot_.AddRenderer(renderer_line);

  renderer_line = new LineRenderer2d();
  renderer_line->set_dataset(&dataset_shell_);
  renderer_line->set_pen(wxBLUE_PEN);
  plot_.AddRenderer(renderer_line);

  renderer_line = new LineRenderer2d();
  renderer_line->set_dataset(&dataset_total_);
  renderer_line->set_pen(wxYELLOW_PEN);
  plot_.AddRenderer(renderer_line);

  renderer_line = new LineRenderer2d();
  renderer_line->set_dataset(&dataset_axis_lines_);
  renderer_line->set_pen(wxWHITE_PEN);
  plot_.AddRenderer(renderer_line);

  renderer_text = new TextRenderer2d();
  renderer_text->set_dataset(&dataset_axis_text_);
  renderer_text->set_color(wxWHITE);
  plot_.AddRenderer(renderer_text);

  renderer_circle = new CircleRenderer2d();
  renderer_circle->set_dataset(&dataset_markers_);
  renderer_circle->set_brush(wxGREEN_BRUSH);
  renderer_circle->set_pen(wxGREEN_PEN);
  plot_.AddRenderer(renderer_circle);

  // updates so the plot remains in place and square
  const Plot2dDataLimits& limits = plot_.LimitsData();
  const double x_range = limits.x_max - limits.x_min;
  const double y_range = limits.y_max - limits.y_min;

  const double ratio_plot_prev = plot_.ratio_aspect();
  const double ratio_plot_new =  1 / (y_range / x_range);
  plot_.set_ratio_aspect(ratio_plot_new);

  Point2d<float> point_offset = plot_.offset();
  point_offset.y = point_offset.y * (ratio_plot_prev / ratio_plot_new);
  plot_.set_offset(point_offset);
}

void CablePlotPane::UpdateStrains(
    const CableElongationModel& model) {
  // gets the unloaded points
  const double strain_unloaded_core =
      model.Strain(CableElongationModel::ComponentType::kCore, 0);
  const double strain_unloaded_shell =
      model.Strain(CableElongationModel::ComponentType::kShell, 0);

  // adds evenly spaced points for the plot range
  const double kStrainBegin = -0.001;
  const double kStrainIncrement = 0.00002;
  const double kStrainEnd = 0.01000;
  double strain = kStrainBegin;
  while (strain < kStrainEnd) {
    if ((strain < strain_unloaded_core) && (strain < strain_unloaded_shell)) {
      // do nothing - strain is less than unloaded strains
    } else {
      // adds strain point
      strains_.push_back(strain);
    }

    strain += kStrainIncrement;
  }

  // inserts core unloaded point
  for (auto iter = strains_.begin(); iter != strains_.end(); iter++) {
    const double& strain = *iter;
    if (strain_unloaded_core < strain) {
      strains_.insert(iter, strain_unloaded_core);
      break;
    }
  }

  // inserts shell unloaded point
  for (auto iter = strains_.begin(); iter != strains_.end(); iter++) {
    const double& strain = *iter;
    if (strain_unloaded_shell < strain) {
      strains_.insert(iter, strain_unloaded_shell);
      break;
    }
  }
}
