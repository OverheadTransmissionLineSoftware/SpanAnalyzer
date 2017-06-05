// This is free and unencumbered software released into the public domain.
// For more information, please refer to <http://unlicense.org/>

#include "profile_plot_pane.h"

#include "appcommon/graphics/line_renderer_2d.h"
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

BEGIN_EVENT_TABLE(ProfilePlotPane, PlotPane2d)
  EVT_MENU(wxID_ANY, ProfilePlotPane::OnContextMenuSelect)
  EVT_MOTION(ProfilePlotPane::OnMouse)
  EVT_RIGHT_DOWN(ProfilePlotPane::OnMouse)
END_EVENT_TABLE()

ProfilePlotPane::ProfilePlotPane(wxWindow* parent, wxView* view)
    : PlotPane2d(parent) {
  view_ = view;

  plot_.set_ratio_aspect(10);
}

ProfilePlotPane::~ProfilePlotPane() {
}

void ProfilePlotPane::Update(wxObject* hint) {
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

void ProfilePlotPane::OnContextMenuSelect(wxCommandEvent& event) {
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

void ProfilePlotPane::OnMouse(wxMouseEvent& event) {
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
                      + helper::DoubleToFormattedString(point_data.x, 2)
                      + "   Y="
                      + helper::DoubleToFormattedString(point_data.y, 2);

    status_bar_log::SetText(str, 1);
  } else {
    status_bar_log::SetText("", 1);
  }
}

void ProfilePlotPane::UpdatePlotDatasets() {
  wxLogVerbose("Updating profile plot dataset.");

  // gets view settings
  SpanAnalyzerView* view = dynamic_cast<SpanAnalyzerView*>(view_);

  // gets filter group
  const AnalysisFilterGroup* group_filters = view->group_filters();
  if (group_filters == nullptr) {
    dataset_catenary_ = LineDataSet2d();
    plot_.ClearRenderers();
    return;
  }

  // gets analysis result filter
  const AnalysisFilter* filter = view->AnalysisFilterActive();
  if (filter == nullptr) {
    dataset_catenary_ = LineDataSet2d();
    plot_.ClearRenderers();
    return;
  }

  // gets weathercase index
  const int index = view->IndexWeathercase(*filter);

  // gets filtered result from doc
  SpanAnalyzerDoc* doc = dynamic_cast<SpanAnalyzerDoc*>(view_->GetDocument());
  const SagTensionAnalysisResult* result = doc->Result(index,
                                                       filter->condition);
  if (result == nullptr) {
    dataset_catenary_ = LineDataSet2d();
    plot_.ClearRenderers();
    return;
  }

  // gets span from document
  const Span* span = doc->SpanAnalysis();

  // creates a catenary with the result parameters
  Catenary3d catenary;
  catenary.set_spacing_endpoints(span->spacing_catenary);
  catenary.set_tension_horizontal(result->tension_horizontal);
  catenary.set_weight_unit(result->weight_unit);

  // calculates points
  std::list<Point3d<double>> points;
  const int i_max = 100;
  for (int i = 0; i <= i_max; i++) {
    double pos = static_cast<double>(i) / static_cast<double>(i_max);
    Point3d<double> p = catenary.Coordinate(pos);
    points.push_back(p);
  }

  // converts points to lines and adds to dataset
  dataset_catenary_.Clear();
  for (auto iter = points.cbegin(); iter != std::prev(points.cend(), 1);
       iter++) {
    // gets current and next point in the list
    const Point3d<double>& p0 = *iter;
    const Point3d<double>& p1 = *(std::next(iter, 1));

    // creates a line and maps 3d catenary points to 2d points for drawing
    Line2d* line = new Line2d();
    line->p0.x = p0.x;
    line->p0.y = p0.z;
    line->p1.x = p1.x;
    line->p1.y = p1.z;

    dataset_catenary_.Add(line);
  }
}

void ProfilePlotPane::UpdatePlotRenderers() {
  wxLogVerbose("Updating profile plot renderers.");

  // creates renderer
  LineRenderer2d* renderer = new LineRenderer2d();
  renderer->set_dataset(&dataset_catenary_);
  renderer->set_pen(wxCYAN_PEN);

  // adds renderer 2D plot
  plot_.AddRenderer(renderer);
}
