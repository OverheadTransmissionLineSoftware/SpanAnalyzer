// This is free and unencumbered software released into the public domain.
// For more information, please refer to <http://unlicense.org/>

#include "span_analyzer_printout.h"

#include "span_analyzer_app.h"

SpanAnalyzerPrintout::SpanAnalyzerPrintout(SpanAnalyzerView* view)
    : wxPrintout(wxGetApp().GetDocument()->GetFilename()) {
  view_ = view;
}

SpanAnalyzerPrintout::~SpanAnalyzerPrintout() {
}

void SpanAnalyzerPrintout::GetPageInfo(int* min, int* max, int* from, int* to) {
  *min = 1;
  *max = 1;
  *from = 1;
  *to = 1;
}

bool SpanAnalyzerPrintout::HasPage(int number) {
  if (number == 1) {
    return true;
  } else {
    return false;
  }
}

/// This method scales and shifts the printout dc to match the graphics dc that
/// appears on screen. This makes it compatible with the OnDraw() method in the
/// view.
bool SpanAnalyzerPrintout::OnPrintPage(int number) {
  // checks if page exists
  if (HasPage(number) == false) {
    return false;
  }

  // gets the graphics rect
  wxRect rect_graphics = view_->GraphicsPlotRect();

  // scales the dc associated with this printout to match the graphics dc
  // and adjusts the page size to account for margins
  wxPageSetupDialogData* data = wxGetApp().config()->data_page;
  FitThisSizeToPageMargins(rect_graphics.GetSize(), *data);

  // offsets the origin so the graphics rect is centered in the page rect
  wxRect rect_page = GetLogicalPageMarginsRect(*data);
  wxCoord x_offset = (rect_page.width - rect_graphics.width) / 2;
  wxCoord y_offset = (rect_page.height - rect_graphics.height) / 2;
  OffsetLogicalOrigin(x_offset, y_offset);

  // renders the plot
  // at this point the plot can be rendered according to graphics parameters
  // (rect size, ppi, etc) on the printout dc and everything will appear right
  wxDC* dc = GetDC();
  view_->OnDraw(dc);

  return true;
}
