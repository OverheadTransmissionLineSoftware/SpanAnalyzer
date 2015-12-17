// This is free and unencumbered software released into the public domain.
// For more information, please refer to <http://unlicense.org/>

#include "span_analyzer_view.h"

#include "span_analyzer_app.h"

#include "edit_pane.h"
#include "results_pane.h"

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
  pane_results_ = new ResultsPane(frame, this);
  manager_.AddPane(pane_results_, info);

  info = wxAuiPaneInfo();
  info.Name("Edit");
  info.Floatable(true);
  info.Left();
  info.Caption("Edit");
  info.CloseButton(false);
  pane_edit_ = new EditPane(frame, this);
  manager_.AddPane(pane_edit_, info);

  // loads perspective and updates
  std::string perspective = wxGetApp().config()->perspective;
  if (perspective == "") {
    manager_.Update();
  } else {
    manager_.LoadPerspective(wxGetApp().config()->perspective);
  }

  return true;
}

void SpanAnalyzerView::OnDraw(wxDC *dc) {
}

void SpanAnalyzerView::OnUpdate(wxView* sender, wxObject* hint) {
  // passes to base class first
  wxView::OnUpdate(sender, hint);

  // don't need to distinguish sender - all frames are grouped under one view
  pane_edit_->Update(hint);
  pane_results_->Update(hint);
}

bool SpanAnalyzerView::OnClose(bool WXUNUSED(deleteWindow)) {
  if (!GetDocument()->Close()) {
    return false;
  }

  // saves AUI perspective
  wxGetApp().config()->perspective = manager_.SavePerspective();

  // detaches frames and un-init manager
  manager_.DetachPane(pane_edit_);
  manager_.DetachPane(pane_results_);
  manager_.UnInit();

  // destroys frames
  pane_edit_->Destroy();
  pane_results_->Destroy();

  // resets frame to document-less state
  SpanAnalyzerFrame* frame = wxGetApp().frame();
  frame->Refresh();
  frame->SetTitle(wxGetApp().GetAppDisplayName());

  return true;
}

EditPane* SpanAnalyzerView::pane_edit() {
  return pane_edit_;
}

ResultsPane* SpanAnalyzerView::pane_results() {
  return pane_results_;
}

wxString SpanAnalyzerView::GetPerspective() {
  return manager_.SavePerspective();
}