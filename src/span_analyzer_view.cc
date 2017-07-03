// This is free and unencumbered software released into the public domain.
// For more information, please refer to <http://unlicense.org/>

#include "span_analyzer_view.h"

#include "appcommon/widgets/status_bar_log.h"
#include "wx/cmdproc.h"

#include "span_analyzer_app.h"

IMPLEMENT_DYNAMIC_CLASS(SpanAnalyzerView, wxView)

BEGIN_EVENT_TABLE(SpanAnalyzerView, wxView)
  EVT_NOTEBOOK_PAGE_CHANGED(wxID_ANY, SpanAnalyzerView::OnNotebookPageChange)
END_EVENT_TABLE()

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
  notebook_plot_ = new wxNotebook(frame, wxID_ANY, wxDefaultPosition,
                                  wxDefaultSize, wxNB_TOP);

  pane_profile_ = new ProfilePlotPane(notebook_plot_, this);
  notebook_plot_->AddPage(pane_profile_, "Profile");

  pane_cable_ = new CableElongationModelPlotPane(notebook_plot_, this);
  notebook_plot_->AddPage(pane_cable_, "Cable Model");

  manager->AddPane(notebook_plot_, info);

  info = wxAuiPaneInfo();
  info.Name("Results");
  info.BestSize(700, 300);
  info.Caption("Results");
  info.CloseButton(false);
  info.Floatable(true);
  info.MinSize(350, 150);
  info.Top();
  pane_results_ = new ResultsPane(frame, this);
  manager->AddPane(pane_results_, info);

  info = wxAuiPaneInfo();
  info.Name("Edit");
  info.BestSize(300, 700);
  info.Caption("Edit");
  info.CloseButton(false);
  info.Floatable(true);
  info.Layer(1);
  info.Left();
  info.MinSize(150, 350);
  pane_edit_ = new EditPane(frame, this);
  manager->AddPane(pane_edit_, info);

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
  manager->DetachPane(notebook_plot_);
  manager->DetachPane(pane_edit_);
  manager->DetachPane(pane_results_);
  manager->Update();

  // destroys panes
  notebook_plot_->Destroy();
  pane_edit_->Destroy();
  pane_results_->Destroy();

  // resets frame to document-less state
  SpanAnalyzerFrame* frame = wxGetApp().frame();
  frame->Refresh();
  frame->SetTitle(wxGetApp().GetAppDisplayName());

  // resets statusbar
  status_bar_log::SetText("Ready", 0);
  status_bar_log::SetText("", 1);

  // resets menubar
  const int index_menu = frame->GetMenuBar()->FindMenu("Edit");
  wxMenu* menu = frame->GetMenuBar()->GetMenu(index_menu);
  menu->SetLabel(wxID_UNDO, "Undo \tCtrl+Z");
  menu->SetLabel(wxID_REDO, "Redo \tCtrl+Y");

  return true;
}

void SpanAnalyzerView::OnDraw(wxDC *dc) {
}

void SpanAnalyzerView::OnNotebookPageChange(wxBookCtrlEvent& event) {
  // skips if old page is invalid
  // this is usually only true on initialization
  if (event.GetOldSelection() == wxNOT_FOUND) {
    return;
  }

  // creates an update hint for the plot pane
  UpdateHint hint(HintType::kViewSelect);

  // gets page index and sends update to corresponding pane
  const int index_page = event.GetSelection();
  if (index_page == 0) {
    pane_profile_->Update(&hint);
  } else if (index_page == 1) {
    pane_cable_->Update(&hint);
  } else {
    wxLogError("Invalid page selection");
  }
}

void SpanAnalyzerView::OnUpdate(wxView* sender, wxObject* hint) {
  // passes to base class first
  wxView::OnUpdate(sender, hint);

  // updates statusbar
  status_bar_log::PushText("Updating view", 0);

  // don't need to distinguish sender - all frames are grouped under one view
  pane_edit_->Update(hint);
  pane_results_->Update(hint);

  // determines which pane is displayed in the notebook
  const int index_page = notebook_plot_->GetSelection();
  if (index_page == 0) {
    pane_profile_->Update(hint);
  } else if (index_page == 1) {
    pane_cable_->Update(hint);
  }

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
