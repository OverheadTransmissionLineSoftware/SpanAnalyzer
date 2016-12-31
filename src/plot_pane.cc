// This is free and unencumbered software released into the public domain.
// For more information, please refer to <http://unlicense.org/>

#include "plot_pane.h"

#include "models/base/helper.h"
#include "models/transmissionline/catenary.h"
#include "wx/dcbuffer.h"

#include "line_renderer_2d.h"
#include "span_analyzer_doc.h"
#include "span_analyzer_view.h"
#include "status_bar_log.h"

/// \par OVERVIEW
///
/// This is the enumeration for the context menu.
enum {
  kFitPlotData = 0,
};

BEGIN_EVENT_TABLE(PlotPane, wxPanel)
  EVT_ERASE_BACKGROUND(PlotPane::OnEraseBackground)
  EVT_LEFT_DOWN(PlotPane::OnMouse)
  EVT_LEFT_UP(PlotPane::OnMouse)
  EVT_ENTER_WINDOW(PlotPane::OnMouse)
  EVT_RIGHT_DOWN(PlotPane::OnMouse)
  EVT_MENU(wxID_ANY, PlotPane::OnContextMenuSelect)
  EVT_MOTION(PlotPane::OnMouse)
  EVT_MOUSEWHEEL(PlotPane::OnMouseWheel)
  EVT_PAINT(PlotPane::OnPaint)
END_EVENT_TABLE()

PlotPane::PlotPane(wxWindow* parent, wxView* view)
    : wxPanel(parent, wxID_ANY, wxDefaultPosition, wxDefaultSize,
              wxTAB_TRAVERSAL) {
  view_ = view;

  plot_.set_background(*wxBLACK_BRUSH);
  plot_.set_is_fitted(true);
  plot_.set_ratio_aspect(10);

  // setting to avoid flickering
  this->SetBackgroundStyle(wxBG_STYLE_PAINT);
}

PlotPane::~PlotPane() {
}

void PlotPane::Update(wxObject* hint) {
  // typically only null on initialization
  if (hint == nullptr) {
    return;
  }

  // gets a buffered dc to prevent flickering
  wxClientDC dc(this);
  wxBufferedDC dc_buf(&dc, bitmap_buffer_);

  // interprets hint
  UpdateHint* hint_update = (UpdateHint*)hint;
  if (hint_update == nullptr) {
    UpdatePlotRenderers();
    RenderPlot(dc_buf);
  } else if (hint_update->type() == HintType::kAnalysisFilterGroupEdit) {
    UpdatePlotRenderers();
    RenderPlot(dc_buf);
  } else if (hint_update->type() == HintType::kAnalysisFilterGroupSelect) {
    UpdatePlotRenderers();
    RenderPlot(dc_buf);
  } else if (hint_update->type() == HintType::kAnalysisFilterSelect) {
    UpdatePlotRenderers();
    RenderPlot(dc_buf);
  } else if (hint_update->type() == HintType::kCablesEdit) {
    UpdatePlotRenderers();
    RenderPlot(dc_buf);
  } else if (hint_update->type() == HintType::kPreferencesEdit) {
    UpdatePlotRenderers();
    RenderPlot(dc_buf);
  } else if (hint_update->type() == HintType::kSpansEdit) {
    UpdatePlotRenderers();
    RenderPlot(dc_buf);
  } else if (hint_update->type() == HintType::kWeathercasesEdit) {
    UpdatePlotRenderers();
    RenderPlot(dc_buf);
  }
}

void PlotPane::OnContextMenuSelect(wxCommandEvent& event) {
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

/// This function overrides the typical window erase background event handling.
/// When used in conjuction with double-buffered device contexts, it will
/// prevent flickering.
void PlotPane::OnEraseBackground(wxEraseEvent& event) {
  // do nothing
}

void PlotPane::OnMouse(wxMouseEvent& event) {
  // skips if no plot renderers are active
  if (plot_.HasRenderers() == false) {
    return;
  }

  if (event.Dragging() == true) {
    // checks if left button is pressed
    if (event.LeftIsDown() == false) {
      return;
    }

    // disables plot fitting if active
    if (plot_.is_fitted() == true) {
      plot_.set_is_fitted(false);
    }

    // gets updated mouse point from event
    wxPoint coord_new;
    coord_new.x = event.GetX();
    coord_new.y = event.GetY();

    // finds difference between cached and new mouse points
    // applies inversion to make plot track mouse position
    const double kShiftX = (coord_new.x - coord_mouse_.x) * -1;
    const double kShiftY = (coord_new.y - coord_mouse_.y);
    plot_.Shift(kShiftX, kShiftY);

    // updates cached mouse point
    coord_mouse_ = coord_new;

    // refreshes window
    this->Refresh();
  } else if (event.Entering() == true) {
    // forces the pane to get focus, which helps catch mouse events
    this->SetFocus();
  } else if (event.LeftDown() == true) {
    // caches the mouse coordinates
    coord_mouse_.x = event.GetX();
    coord_mouse_.y = event.GetY();
  } else if (event.LeftUp() == true) {
    coord_mouse_.x = -999999;
    coord_mouse_.y = -999999;
  } else if (event.RightDown() == true) {
    // builds a context menu
    wxMenu menu;

    menu.AppendCheckItem(kFitPlotData, "Fit Plot");
    menu.Check(kFitPlotData, plot_.is_fitted());

    // shows context menu
    // the event is caught by the pane
    PopupMenu(&menu, event.GetPosition());

    // stops processing event (needed to allow pop-up menu to catch its event)
    event.Skip();
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
                      + helper::DoubleToFormattedString(point_data.x, 2)
                      + "   Y="
                      + helper::DoubleToFormattedString(point_data.y, 2);

    status_bar_log::SetText(str, 1);
  } else {
    status_bar_log::SetText("", 1);
  }
}

void PlotPane::OnMouseWheel(wxMouseEvent& event) {
  // skips if no plot renderers are active
  if (plot_.HasRenderers() == false) {
    return;
  }

  // skips if point is outside of graphics rect
  wxRect rect = GetClientRect();
  if ((event.GetX() < 0) || (rect.GetWidth() < event.GetX())) {
    return;
  }

  if ((event.GetY() < 0) || (rect.GetHeight() < event.GetY())) {
    return;
  }

  // zoom factor
  const double kZoomFactor = 1.2;

  // zoom point
  wxPoint coord_zoom = event.GetPosition();

  if (event.GetWheelRotation() < 0) {
    // zooms in
    plot_.Zoom(kZoomFactor, coord_zoom);
  } else if (0 < event.GetWheelRotation()) {
    // zooms out
    plot_.Zoom(1 / kZoomFactor, coord_zoom);
  }

  // refreshes window
  this->Refresh();
}

void PlotPane::OnPaint(wxPaintEvent& event) {
  // gets a device context
  // a buffered device context helps prevent flickering
  wxBufferedPaintDC dc(this, bitmap_buffer_);

  // renders
  RenderPlot(dc);
}

void PlotPane::RenderPlot(wxDC& dc) {
  plot_.Render(dc, GetClientRect());
}

void PlotPane::UpdatePlotRenderers() {
  wxLogVerbose("Updating plot renderers.");

  // gets view settings
  SpanAnalyzerView* view = (SpanAnalyzerView*)view_;

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
  SpanAnalyzerDoc* doc = (SpanAnalyzerDoc*)view_->GetDocument();
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

  /// \todo remove/minimize copying for faster redraws

  // calculates points
  std::list<Point3d> points;
  const int i_max = 100;
  for (int i = 0; i <= i_max; i++) {
    double pos = double(i) / double(i_max);
    Point3d p = catenary.Coordinate(pos);
    points.push_back(p);
  }

  // converts points to lines
  std::list<Line2d> lines;
  for (auto iter = points.cbegin(); iter != std::prev(points.cend(), 1);
       iter++) {
    // gets current and next point in the list
    const Point3d p0 = *iter;
    const Point3d p1 = *(std::next(iter, 1));

    // creates a line and maps 3d catenary points to 2d points for drawing
    Line2d line;
    line.p0.x = p0.x;
    line.p0.y = p0.z;
    line.p1.x = p1.x;
    line.p1.y = p1.z;

    lines.push_back(line);
  }

  dataset_catenary_.set_data(lines);

  // creates renderer
  LineRenderer2d* renderer = new LineRenderer2d();
  renderer->set_dataset(&dataset_catenary_);
  renderer->set_pen(wxCYAN_PEN);

  // adds renderer 2D plot
  plot_.AddRenderer(renderer);
}
