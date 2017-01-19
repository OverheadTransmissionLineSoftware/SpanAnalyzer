// This is free and unencumbered software released into the public domain.
// For more information, please refer to <http://unlicense.org/>

#include "cable_elongation_model_plot_pane.h"

#include "models/base/helper.h"
#include "wx/dcbuffer.h"

#include "circle_renderer_2d.h"
#include "line_renderer_2d.h"
#include "span_analyzer_doc.h"
#include "span_analyzer_view.h"
#include "status_bar_log.h"
#include "text_renderer_2d.h"

/// \par OVERVIEW
///
/// This is the enumeration for the context menu.
enum {
  kFitPlotData = 0,
};

BEGIN_EVENT_TABLE(CableElongationModelPlotPane, PlotPane2d)
  EVT_MENU(wxID_ANY, CableElongationModelPlotPane::OnContextMenuSelect)
  EVT_MOTION(CableElongationModelPlotPane::OnMouse)
  EVT_RIGHT_DOWN(CableElongationModelPlotPane::OnMouse)
END_EVENT_TABLE()

CableElongationModelPlotPane::CableElongationModelPlotPane(
    wxWindow* parent, wxView* view)
    : PlotPane2d(parent) {
  view_ = view;
}

CableElongationModelPlotPane::~CableElongationModelPlotPane() {
}

void CableElongationModelPlotPane::Update(wxObject* hint) {
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
    RenderPlot(dc_buf);
  } else if (hint_update->type() == HintType::kAnalysisFilterGroupEdit) {
    UpdatePlotDatasets();
    UpdatePlotRenderers();
    RenderPlot(dc_buf);
  } else if (hint_update->type() == HintType::kAnalysisFilterGroupSelect) {
    UpdatePlotDatasets();
    UpdatePlotRenderers();
    RenderPlot(dc_buf);
  } else if (hint_update->type() == HintType::kAnalysisFilterSelect) {
    UpdatePlotDatasets();
    UpdatePlotRenderers();
    RenderPlot(dc_buf);
  } else if (hint_update->type() == HintType::kCablesEdit) {
    UpdatePlotDatasets();
    UpdatePlotRenderers();
    RenderPlot(dc_buf);
  } else if (hint_update->type() == HintType::kPreferencesEdit) {
    UpdatePlotDatasets();
    UpdatePlotRenderers();
    RenderPlot(dc_buf);
  } else if (hint_update->type() == HintType::kSpansEdit) {
    UpdatePlotDatasets();
    UpdatePlotRenderers();
    RenderPlot(dc_buf);
  } else if (hint_update->type() == HintType::kWeathercasesEdit) {
    UpdatePlotDatasets();
    UpdatePlotRenderers();
    RenderPlot(dc_buf);
  }
}

void CableElongationModelPlotPane::ClearDataSets() {
  dataset_axis_lines_.Clear();
  dataset_axis_text_.Clear();
  dataset_core_.Clear();
  dataset_markers_.Clear();
  dataset_shell_.Clear();
  dataset_total_.Clear();
}

void CableElongationModelPlotPane::OnContextMenuSelect(wxCommandEvent& event) {
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

void CableElongationModelPlotPane::OnMouse(wxMouseEvent& event) {
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
    const Point2d point_data = plot_.PointGraphicsToData(point_graphics);

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

void CableElongationModelPlotPane::UpdateDataSetAxes(
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
  text->position = Text2d::BoundaryPosition::kRightUpper;
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
  text->position = Text2d::BoundaryPosition::kRightUpper;
  dataset_axis_text_.Add(text);
}

void CableElongationModelPlotPane::UpdateDataSetCable(
    const CableElongationModel& model,
    const CableElongationModel::ComponentType& type_component,
    LineDataSet2d& dataset) {
  std::list<Point2d> points;
  const double strain_begin = -0.001;
  const double strain_increment = 0.00002;
  const double kStrengthRated = *model.cable()->strength_rated();
  for (int i = 0; i <= 550; i++) {
    Point2d point;
    point.x = strain_begin + (static_cast<double>(i) * strain_increment);
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
    const Point2d& p0 = *iter;
    const Point2d& p1 = *(std::next(iter, 1));

    // creates a line
    Line2d* line = new Line2d();
    line->p0.x = p0.x;
    line->p0.y = p0.y;
    line->p1.x = p1.x;
    line->p1.y = p1.y;

    dataset.Add(line);
  }
}

void CableElongationModelPlotPane::UpdateDataSetMarker(
    const CableElongationModel& model,
    const SagTensionAnalysisResult* result) {
  Circle2d* circle = nullptr;

  circle = new Circle2d();
  circle->center.y = result->tension_average_core;
  circle->center.x = model.Strain(CableElongationModel::ComponentType::kCore,
                                  circle->center.y);
  circle->radius = 3;
  dataset_markers_.Add(circle);

  circle = new Circle2d();
  circle->center.y = result->tension_average_shell;
  circle->center.x = model.Strain(CableElongationModel::ComponentType::kShell,
                                  circle->center.y);
  circle->radius = 3;
  dataset_markers_.Add(circle);

  circle = new Circle2d();
  circle->center.y = result->tension_average;
  circle->center.x = model.Strain(CableElongationModel::ComponentType::kCombined,
                                  circle->center.y);
  circle->radius = 3;
  dataset_markers_.Add(circle);
}

void CableElongationModelPlotPane::UpdatePlotDatasets() {
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
  cable.set_cable_base(span->linecable.cable);

  CableElongationModel model;
  model.set_cable(&cable);
  model.set_state(result->state);
  model.set_state_stretch(*doc->StretchState(result->condition));

  // updates datasets
  UpdateDataSetAxes(0, 0.01, 0, *cable.strength_rated());

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

void CableElongationModelPlotPane::UpdatePlotRenderers() {
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

  Point2d point_offset = plot_.offset();
  point_offset.y = point_offset.y * (ratio_plot_prev / ratio_plot_new);
  plot_.set_offset(point_offset);
}
