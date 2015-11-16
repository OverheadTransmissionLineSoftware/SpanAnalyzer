// This is free and unencumbered software released into the public domain.
// For more information, please refer to <http://unlicense.org/>

#include "span_analyzer_view.h"

#include "span_analyzer_app.h"

#include "edit_panel.h"
#include "messages_panel.h"
#include "results_panel.h"

IMPLEMENT_DYNAMIC_CLASS(SpanAnalyzerView, wxView)

SpanAnalyzerView::SpanAnalyzerView() {
}

SpanAnalyzerView::~SpanAnalyzerView() {
}

bool SpanAnalyzerView::OnCreate(wxDocument *doc, long flags) {
  if (!wxView::OnCreate(doc, flags)) {
    return false;
  }

  // gets main application frame
  wxFrame* frame = ((wxFrame *)wxGetApp().GetTopWindow());

  // notify application of new view
  SetFrame(frame);
  Activate(true);

  // tells aui manager to manage this frame
  manager_.SetManagedWindow(frame);

  // creates AUI windows and adds to manager
  wxAuiPaneInfo info;

  info = wxAuiPaneInfo();
  info.Name("Results");
  info.CenterPane();
  panel_results_ = new ResultsPanel(frame, this);
  manager_.AddPane(panel_results_, info);

  info = wxAuiPaneInfo();
  info.Name("Edit");
  info.Floatable(true);
  info.Left();
  info.Caption("Edit");
  info.CloseButton(false);
  panel_edit_ = new EditPanel(frame, this);
  manager_.AddPane(panel_edit_, info);

  manager_.Update();

  return true;
}

void SpanAnalyzerView::OnDraw(wxDC *dc) {
}

void SpanAnalyzerView::OnUpdate(wxView* sender, wxObject* hint) {
  // passes to base class first
  wxView::OnUpdate(sender, hint);

  // don't need to distinguish sender - all frames are grouped under one view
  panel_edit_->Update(hint);
  panel_results_->Update(hint);
}

bool SpanAnalyzerView::OnClose(bool WXUNUSED(deleteWindow)) {
  if (!GetDocument()->Close()) {
    return false;
  }

  // detaches frames and un-init manager
  manager_.DetachPane(panel_edit_);
  manager_.DetachPane(panel_results_);
  manager_.UnInit();

  // destroys frames
  panel_edit_->Destroy();
  panel_results_->Destroy();

  // resets frame to document-less state
  SpanAnalyzerFrame* frame = wxGetApp().frame();
  frame->Refresh();
  frame->SetTitle("SpanAnalyzer");

  return true;
}

EditPanel* SpanAnalyzerView::panel_edit() {
  return panel_edit_;
}

ResultsPanel* SpanAnalyzerView::panel_results() {
  return panel_results_;
}

wxString SpanAnalyzerView::GetPerspective() {
  return manager_.SavePerspective();
}