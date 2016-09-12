// This is free and unencumbered software released into the public domain.
// For more information, please refer to <http://unlicense.org/>

#include "plot_pane.h"

#include "models/transmissionline/catenary.h"
#include "wx/dcbuffer.h"

#include "line_renderer_2d.h"
#include "span_analyzer_view.h"

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
  ViewUpdateHint* hint_update = (ViewUpdateHint*)hint;
  if (hint_update == nullptr) {
    UpdatePlot();
    RenderPlot(dc_buf);
  } else if (hint_update->type() ==
       ViewUpdateHint::HintType::kModelAnalysisWeathercaseEdit) {
    UpdatePlot();
    RenderPlot(dc_buf);
  } else if (hint_update->type() ==
       ViewUpdateHint::HintType::kModelPreferencesEdit) {
    UpdatePlot();
    RenderPlot(dc_buf);
  } else if (hint_update->type() ==
       ViewUpdateHint::HintType::kModelSpansEdit) {
    UpdatePlot();
    RenderPlot(dc_buf);
  } else if (hint_update->type() ==
       ViewUpdateHint::HintType::kModelWeathercaseEdit) {
    UpdatePlot();
    RenderPlot(dc_buf);
  } else if (hint_update->type() ==
      ViewUpdateHint::HintType::kViewConditionChange) {
    UpdatePlot();
    RenderPlot(dc_buf);
  } else if (hint_update->type() ==
      ViewUpdateHint::HintType::kViewWeathercaseChange) {
    UpdatePlot();
    RenderPlot(dc_buf);
  } else if (hint_update->type() ==
      ViewUpdateHint::HintType::kViewWeathercasesSetChange) {
    UpdatePlot();
    RenderPlot(dc_buf);
  }
}

void PlotPane::ClearPlot(wxDC& dc) {
  dc.Clear();
}

void PlotPane::OnContextMenuSelect(wxCommandEvent& event) {
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
}

void PlotPane::OnMouseWheel(wxMouseEvent& event) {
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

void PlotPane::UpdatePlot() {
  // gets the results
  SpanAnalyzerView* view = (SpanAnalyzerView*)view_;
  const SagTensionAnalysisResultSet& results = view->results();
  if (results.descriptions_weathercase.empty() == true) {
    wxClientDC dc(this);
    ClearPlot(dc);
    return;
  }

  // gets the result set based on the current display condition
  const std::list<SagTensionAnalysisResult>* result_list = nullptr;
  if (view->condition() == CableConditionType::kInitial) {
    result_list = &results.results_initial;
  } else if (view->condition() == CableConditionType::kLoad) {
    result_list = &results.results_load;
  } else {
    wxClientDC dc(this);
    ClearPlot(dc);
    return;
  }

  // gets the result from the list
  const int index_weathercase = view->index_weathercase();
  if (index_weathercase < 0) {
    plot_.ClearRenderers();
    return;
  }

  const SagTensionAnalysisResult& result =
      *(std::next(result_list->cbegin(), index_weathercase));

  // creates a catenary with the result parameters
  Catenary3d catenary;
  catenary.set_spacing_endpoints(results.span->spacing_catenary);
  catenary.set_tension_horizontal(result.tension_horizontal);
  catenary.set_weight_unit(result.weight_unit);

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
  LineRenderer2d renderer;
  renderer.set_dataset(&dataset_catenary_);
  renderer.set_pen(wxCYAN_PEN);

  // adds renderer 2D plot
  plot_.AddRenderer(renderer);
}
