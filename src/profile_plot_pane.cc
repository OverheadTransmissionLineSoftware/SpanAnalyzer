// This is free and unencumbered software released into the public domain.
// For more information, please refer to <http://unlicense.org/>

#include "profile_plot_pane.h"

#include <algorithm>

#include "appcommon/graphics/line_renderer_2d.h"
#include "appcommon/graphics/text_renderer_2d.h"
#include "appcommon/widgets/status_bar_log.h"
#include "models/base/helper.h"
#include "wx/dcbuffer.h"

#include "profile_plot_options_dialog.h"
#include "span_analyzer_app.h"
#include "span_analyzer_doc.h"
#include "span_analyzer_view.h"

/// \par OVERVIEW
///
/// This is the enumeration for the context menu.
enum {
  kFitPlotData = 0,
  kOptions,
};

BEGIN_EVENT_TABLE(ProfilePlotPane, PlotPane2d)
  EVT_MENU(wxID_ANY, ProfilePlotPane::OnContextMenuSelect)
  EVT_MOTION(ProfilePlotPane::OnMouse)
  EVT_RIGHT_DOWN(ProfilePlotPane::OnMouse)
END_EVENT_TABLE()

ProfilePlotPane::ProfilePlotPane(wxWindow* parent, wxView* view)
    : PlotPane2d(parent) {
  view_ = view;

  // gets options from config
  SpanAnalyzerConfig* config = wxGetApp().config();
  options_ = &config->options_plot_profile;

  // sets plot defaults
  const wxBrush* brush =
      wxTheBrushList->FindOrCreateBrush(config->color_background);
  plot_.set_background(*brush);
  plot_.set_is_fitted(true);
  plot_.set_scale_x(options_->scale_horizontal);
  plot_.set_scale_y(options_->scale_vertical);
  plot_.set_zoom_factor_fitted(1.0 / 1.2);
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

  // updates plot based on app config
  SpanAnalyzerConfig* config = wxGetApp().config();
  const wxBrush* brush =
      wxTheBrushList->FindOrCreateBrush(config->color_background);
  plot_.set_background(*brush);

  // updates plot based on options
  plot_.set_scale_x(options_->scale_horizontal);
  plot_.set_scale_y(options_->scale_vertical);

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

void ProfilePlotPane::ClearDataSets() {
  dataset_catenary_.Clear();
  dataset_dimension_lines_.Clear();
  dataset_dimension_text_.Clear();
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
  } else if (id_event == kOptions) {
    // creates dialog and shows
    ProfilePlotOptionsDialog dialog(this, options_);
    if (dialog.ShowModal() != wxID_OK) {
      return;
    }

    // updates plot and redraws
    UpdateHint hint(HintType::kViewSelect);
    Update(&hint);
  }
}

void ProfilePlotPane::OnMouse(wxMouseEvent& event) {
  // overrides for right mouse click
  if (event.RightDown() == true) {
    // builds a context menu
    wxMenu menu;

    menu.AppendCheckItem(kFitPlotData, "Fit Plot");
    menu.Check(kFitPlotData, plot_.is_fitted());
    menu.AppendSeparator();
    menu.Append(kOptions, "Options");

    // shows context menu
    // the event is caught by the pane
    PopupMenu(&menu, event.GetPosition());

    // stops processing event (needed to allow pop-up menu to catch its event)
    event.Skip();
  } else {
    // calls base function
    PlotPane2d::OnMouse(event);
  }

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
}

void ProfilePlotPane::UpdateDatasetCatenary(const Catenary3d& catenary) {
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

void ProfilePlotPane::UpdateDatasetDimensions(const Catenary3d& catenary) {
  // initializes working points
  Point3d<double> point_3d(-999999, -999999, -999999);
  Point2d<float> point(-999999, -999999);
  Line2d* line = nullptr;
  Text2d* text = nullptr;

  // maps 3d catenary geometry to 2d
  Vector2d spacing;
  spacing.set_x(catenary.spacing_endpoints().x());
  spacing.set_y(catenary.spacing_endpoints().z());

  // defines catenary end points
  const Point2d<float> point_data_start(0, 0);
  const Point2d<float> point_data_end(spacing.x(), spacing.y());

  // solves for chord line connecting end points
  line = new Line2d();
  line->p0.x = point_data_start.x;
  line->p0.y = point_data_start.y;
  line->p1.x = point_data_end.x;
  line->p1.y = point_data_end.y;
  dataset_dimension_lines_.Add(line);

  // solves for horizontal dimension lines and text
  line = new Line2d();
  line->p0.x = point_data_start.x;
  line->p0.y = std::max(point_data_start.y, point_data_end.y);
  line->p1.x = point_data_end.x;
  line->p1.y = std::max(point_data_start.y, point_data_end.y);
  dataset_dimension_lines_.Add(line);

  point.x = (line->p1.x - line->p0.x) / 2;
  point.y = line->p0.y;

  text = new Text2d();
  text->angle = 0;
  text->message = helper::DoubleToFormattedString(spacing.x(), 2);
  text->offset = Point2d<int>(0, 5);
  text->point = point;
  text->position = Text2d::BoundaryPosition::kCenterLower;
  dataset_dimension_text_.Add(text);

  // solves for vertical dimension lines and text
  if (spacing.y() < 0) {
    // solves for dimension to the right of the catenary
    line = new Line2d();
    line->p0.x = point_data_end.x;
    line->p0.y = 0;
    line->p1.x = point_data_end.x;
    line->p1.y = point_data_end.y;
    dataset_dimension_lines_.Add(line);

    point.x = line->p0.x;
    point.y = (line->p1.y - line->p0.y) / 2;

    text = new Text2d();
    text->angle = 0;
    text->message = helper::DoubleToFormattedString(spacing.y(), 2);
    text->offset = Point2d<int>(5, 0);
    text->point = point;
    text->position = Text2d::BoundaryPosition::kLeftCenter;
    dataset_dimension_text_.Add(text);
  } else if (0 < spacing.y()) {
    // solves for dimension to the left of the catenary
    line = new Line2d();
    line->p0.x = point_data_start.x;
    line->p0.y = 0;
    line->p1.x = point_data_start.x;
    line->p1.y = point_data_end.y;
    dataset_dimension_lines_.Add(line);

    point.x = line->p0.x;
    point.y = (line->p1.y - line->p0.y) / 2;

    text = new Text2d();
    text->angle = 0;
    text->message = helper::DoubleToFormattedString(spacing.y(), 2);
    text->offset = Point2d<int>(-5, 0);
    text->point = point;
    text->position = Text2d::BoundaryPosition::kRightCenter;
    dataset_dimension_text_.Add(text);
  } else {
    // y = 0, and no dimension is necessary
  }

  // solves for sag dimension lines and text
  const double position_sag = catenary.PositionFractionSagPoint();
  line = new Line2d();

  point_3d = catenary.CoordinateChord(position_sag);
  line->p0.x = point_3d.x;
  line->p0.y = point_3d.z;
  point_3d = catenary.Coordinate(position_sag);
  line->p1.x = point_3d.x;
  line->p1.y = point_3d.z;
  dataset_dimension_lines_.Add(line);

  point.x = line->p0.x + (line->p1.x - line->p0.x) / 2;
  point.y = line->p0.y + (line->p1.y - line->p0.y) / 2;

  text = new Text2d();
  text->angle = 0;
  text->message = helper::DoubleToFormattedString(line->p0.y - line->p1.y, 2);
  text->offset = Point2d<int>(5, 0);
  text->point = point;
  text->position = Text2d::BoundaryPosition::kLeftCenter;
  dataset_dimension_text_.Add(text);
}

void ProfilePlotPane::UpdatePlotDatasets() {
  wxLogVerbose("Updating profile plot dataset.");

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

  // gets span from document
  const Span* span = doc->SpanAnalysis();

  // creates a catenary with the result parameters
  Catenary3d catenary;
  catenary.set_spacing_endpoints(span->spacing_catenary);
  catenary.set_tension_horizontal(result->tension_horizontal);
  catenary.set_weight_unit(result->weight_unit);

  // updates datasets
  UpdateDatasetCatenary(catenary);
  UpdateDatasetDimensions(catenary);
}

void ProfilePlotPane::UpdatePlotRenderers() {
  wxLogVerbose("Updating profile plot renderers.");

  // clears existing renderers
  plot_.ClearRenderers();

  // checks if dataset has any data
  if (dataset_catenary_.data()->empty() == true) {
    return;
  }

  // creates renderer
  LineRenderer2d* renderer_line = nullptr;
  TextRenderer2d* renderer_text = nullptr;
  const wxPen* pen = nullptr;

  // adds catenary renderer
  pen = wxThePenList->FindOrCreatePen(options_->color_catenary,
                                      options_->thickness_line);
  renderer_line = new LineRenderer2d();
  renderer_line->set_dataset(&dataset_catenary_);
  renderer_line->set_pen(pen);
  plot_.AddRenderer(renderer_line);

  // adds dimension line renderer
  pen = wxThePenList->FindOrCreatePen(*wxWHITE,
                                      1,
                                      wxPENSTYLE_SHORT_DASH);
  renderer_line = new LineRenderer2d();
  renderer_line->set_dataset(&dataset_dimension_lines_);
  renderer_line->set_pen(pen);
  plot_.AddRenderer(renderer_line);

  // adds dimension text renderer
  renderer_text = new TextRenderer2d();
  renderer_text->set_dataset(&dataset_dimension_text_);
  renderer_text->set_color(wxWHITE);
  plot_.AddRenderer(renderer_text);
}
