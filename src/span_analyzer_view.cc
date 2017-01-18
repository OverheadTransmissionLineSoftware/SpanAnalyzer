// This is free and unencumbered software released into the public domain.
// For more information, please refer to <http://unlicense.org/>

#include "span_analyzer_view.h"

#include "wx/cmdproc.h"

#include "span_analyzer_app.h"
#include "status_bar_log.h"

IMPLEMENT_DYNAMIC_CLASS(SpanAnalyzerView, wxView)

SpanAnalyzerView::SpanAnalyzerView() {
}

SpanAnalyzerView::~SpanAnalyzerView() {
}

const AnalysisFilter* SpanAnalyzerView::AnalysisFilterActive() const {
  // checks for valid analysis filter group
  if (group_filters_ == nullptr) {
    return nullptr;
  }

  // checks for valid index
  const int kSize = group_filters_->filters.size();
  if ((index_filter_ < 0) || (kSize < index_filter_)) {
    return nullptr;
  } else {
    auto iter = group_filters_->filters.cbegin();
    std::advance(iter, index_filter_);
    return &(*iter);
  }
}

int SpanAnalyzerView::IndexWeathercase(const AnalysisFilter& filter) const {
  const std::list<WeatherLoadCase*>* weathercases =
      &wxGetApp().data()->weathercases;

  int index = 0;
  for (auto iter = weathercases->cbegin(); iter != weathercases->cend();
       iter++) {
    const WeatherLoadCase* weathercase = *iter;
    if (filter.weathercase == weathercase) {
      return index;
    } else {
      index++;
    }
  }

  // match wasn't found
  return -1;
}

bool SpanAnalyzerView::OnCreate(wxDocument *doc, long flags) {
  if (!wxView::OnCreate(doc, flags)) {
    return false;
  }

  // initializes cached references
  group_filters_ = nullptr;
  index_filter_ = -1;

  // gets main application frame
  wxFrame* frame = dynamic_cast<wxFrame*>(wxGetApp().GetTopWindow());

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
  pane_profile_ = new ProfilePlotPane(frame, this);
  manager->AddPane(pane_profile_, info);

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

  info = wxAuiPaneInfo();
  info.Name("Cable");
  info.Floatable(true);
  info.Caption("Cable Elongation Model");
  info.CloseButton(false);
  pane_cable_ = new CableElongationModelPlotPane(frame, this);
  manager->AddPane(pane_cable_, info);

  // loads perspective and updates
  std::string perspective = wxGetApp().config()->perspective;
  if (perspective == "") {
    manager->Update();
  } else {
    manager->LoadPerspective(wxGetApp().config()->perspective);
  }

  // resets statusbar
  status_bar_log::SetText("Ready", 0);

  // links command processor to edit menu
  const int index_menu = frame->GetMenuBar()->FindMenu("Edit");
  wxMenu* menu = frame->GetMenuBar()->GetMenu(index_menu);

  wxCommandProcessor* processor = GetDocument()->GetCommandProcessor();
  processor->SetEditMenu(menu);

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
  manager->DetachPane(pane_cable_);
  manager->DetachPane(pane_edit_);
  manager->DetachPane(pane_profile_);
  manager->DetachPane(pane_results_);
  manager->Update();

  // destroys panes
  pane_cable_->Destroy();
  pane_edit_->Destroy();
  pane_profile_->Destroy();
  pane_results_->Destroy();

  // resets frame to document-less state
  SpanAnalyzerFrame* frame = wxGetApp().frame();
  frame->Refresh();
  frame->SetTitle(wxGetApp().GetAppDisplayName());

  // resets statusbar
  status_bar_log::SetText("Ready", 0);

  // resets menubar
  const int index_menu = frame->GetMenuBar()->FindMenu("Edit");
  wxMenu* menu = frame->GetMenuBar()->GetMenu(index_menu);
  menu->SetLabel(wxID_UNDO, "Undo \tCtrl+Z");
  menu->SetLabel(wxID_REDO, "Redo \tCtrl+Y");

  return true;
}

void SpanAnalyzerView::OnDraw(wxDC *dc) {
}

void SpanAnalyzerView::OnUpdate(wxView* sender, wxObject* hint) {
  // passes to base class first
  wxView::OnUpdate(sender, hint);

  // updates statusbar
  status_bar_log::PushText("Updating view", 0);

  // don't need to distinguish sender - all frames are grouped under one view
  pane_edit_->Update(hint);
  pane_results_->Update(hint);
  pane_profile_->Update(hint);
  pane_cable_->Update(hint);

  // resets status bar
  status_bar_log::PopText(0);
}

const AnalysisFilterGroup* SpanAnalyzerView::group_filters() const {
  return group_filters_;
}

const int SpanAnalyzerView::index_filter() const {
  return index_filter_;
}

CableElongationModelPlotPane* SpanAnalyzerView::pane_cable() {
  return pane_cable_;
}

EditPane* SpanAnalyzerView::pane_edit() {
  return pane_edit_;
}

ProfilePlotPane* SpanAnalyzerView::pane_profile() {
  return pane_profile_;
}

ResultsPane* SpanAnalyzerView::pane_results() {
  return pane_results_;
}

void SpanAnalyzerView::set_group_filters(const AnalysisFilterGroup* group) {
  group_filters_ = group;
}

void SpanAnalyzerView::set_index_filter(const int& index_filter) {
  index_filter_ = index_filter;
}
