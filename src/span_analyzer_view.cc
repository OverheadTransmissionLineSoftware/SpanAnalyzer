// This is free and unencumbered software released into the public domain.
// For more information, please refer to <http://unlicense.org/>

#include "span_analyzer_view.h"

#include "span_analyzer_app.h"

IMPLEMENT_DYNAMIC_CLASS(SpanAnalyzerView, wxView)

SpanAnalyzerView::SpanAnalyzerView() {
}

SpanAnalyzerView::~SpanAnalyzerView() {
}

bool SpanAnalyzerView::OnCreate(wxDocument *doc, long flags) {
  if (!wxView::OnCreate(doc, flags)) {
    return false;
  }

  // initializes cached references
  group_weathercases_ = nullptr;
  condition_ = CableConditionType::kInitial;
  index_weathercase_ = -1;

  // gets main application frame
  wxFrame* frame = ((wxFrame *)wxGetApp().GetTopWindow());

  // notify application of new view
  SetFrame(frame);
  Activate(true);

  // gets aui manager
  wxAuiManager* manager = wxAuiManager::GetManager(frame);

  // creates AUI windows and adds to manager
  wxAuiPaneInfo info;

  info = wxAuiPaneInfo();
  info.Name("Plot");
  info.CenterPane();
  pane_plot_ = new PlotPane(frame, this);
  manager->AddPane(pane_plot_, info);

  info = wxAuiPaneInfo();
  info.Name("Results");
  info.Floatable(true);
  info.Bottom();
  info.Caption("Results");
  info.CloseButton(false);
  pane_results_ = new ResultsPane(frame, this);
  manager->AddPane(pane_results_, info);

  info = wxAuiPaneInfo();
  info.Name("Edit");
  info.Floatable(true);
  info.Left();
  info.Caption("Edit");
  info.CloseButton(false);
  pane_edit_ = new EditPane(frame, this);
  manager->AddPane(pane_edit_, info);

  // loads perspective and updates
  std::string perspective = wxGetApp().config()->perspective;
  if (perspective == "") {
    manager->Update();
  } else {
    manager->LoadPerspective(wxGetApp().config()->perspective);
  }

  return true;
}

bool SpanAnalyzerView::OnClose(bool WXUNUSED(deleteWindow)) {
  if (!GetDocument()->Close()) {
    return false;
  }

  // gets aui manager from main frame
  wxAuiManager* manager = wxAuiManager::GetManager(GetFrame());

  // saves AUI perspective
  wxGetApp().config()->perspective = manager->SavePerspective();

  // detaches panes and un-init manager
  manager->DetachPane(pane_edit_);
  manager->DetachPane(pane_plot_);
  manager->DetachPane(pane_results_);
  manager->Update();

  // destroys panes
  pane_edit_->Destroy();
  pane_plot_->Destroy();
  pane_results_->Destroy();

  // resets frame to document-less state
  SpanAnalyzerFrame* frame = wxGetApp().frame();
  frame->Refresh();
  frame->SetTitle(wxGetApp().GetAppDisplayName());

  return true;
}

void SpanAnalyzerView::OnDraw(wxDC *dc) {
}

void SpanAnalyzerView::OnUpdate(wxView* sender, wxObject* hint) {
  // passes to base class first
  wxView::OnUpdate(sender, hint);

  // don't need to distinguish sender - all frames are grouped under one view
  pane_edit_->Update(hint);
  pane_plot_->Update(hint);
  pane_results_->Update(hint);
}

const CableConditionType& SpanAnalyzerView::condition() const {
  return condition_;
}

const WeatherLoadCaseGroup* SpanAnalyzerView::group_weathercases() const {
  return group_weathercases_;
}

const int SpanAnalyzerView::index_weathercase() const {
  return index_weathercase_;
}

EditPane* SpanAnalyzerView::pane_edit() {
  return pane_edit_;
}

ResultsPane* SpanAnalyzerView::pane_results() {
  return pane_results_;
}

void SpanAnalyzerView::set_condition(const CableConditionType& condition) {
  condition_ = condition;
}

void SpanAnalyzerView::set_group_weathercase(
    const WeatherLoadCaseGroup* group) {
  group_weathercases_ = group;
}

void SpanAnalyzerView::set_index_weathercase(const int& index_weathercase) {
  index_weathercase_ = index_weathercase;
}
