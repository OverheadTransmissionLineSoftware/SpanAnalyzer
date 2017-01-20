// This is free and unencumbered software released into the public domain.
// For more information, please refer to <http://unlicense.org/>

#include "plot_pane_2d.h"

#include "models/transmissionline/catenary.h"
#include "wx/dcbuffer.h"

BEGIN_EVENT_TABLE(PlotPane2d, wxPanel)
  EVT_ERASE_BACKGROUND(PlotPane2d::OnEraseBackground)
  EVT_LEFT_DOWN(PlotPane2d::OnMouse)
  EVT_LEFT_UP(PlotPane2d::OnMouse)
  EVT_ENTER_WINDOW(PlotPane2d::OnMouse)
  EVT_MOUSEWHEEL(PlotPane2d::OnMouseWheel)
  EVT_PAINT(PlotPane2d::OnPaint)
END_EVENT_TABLE()

PlotPane2d::PlotPane2d(wxWindow* parent)
    : wxPanel(parent, wxID_ANY, wxDefaultPosition, wxDefaultSize,
              wxTAB_TRAVERSAL) {
  plot_.set_background(*wxBLACK_BRUSH);
  plot_.set_is_fitted(true);

  // setting to avoid flickering
  this->SetBackgroundStyle(wxBG_STYLE_PAINT);
}

PlotPane2d::~PlotPane2d() {
}

/// This function overrides the typical window erase background event handling.
/// When used in conjuction with double-buffered device contexts, it will
/// prevent flickering.
void PlotPane2d::OnEraseBackground(wxEraseEvent& event) {
  // do nothing
}

void PlotPane2d::OnMouse(wxMouseEvent& event) {
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
  }
}

void PlotPane2d::OnMouseWheel(wxMouseEvent& event) {
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

void PlotPane2d::OnPaint(wxPaintEvent& event) {
  // gets a device context
  // a buffered device context helps prevent flickering
  wxBufferedPaintDC dc(this, bitmap_buffer_);

  // renders
  RenderPlot(dc);
}

void PlotPane2d::RenderPlot(wxDC& dc) {
  plot_.Render(dc, GetClientRect());
}
