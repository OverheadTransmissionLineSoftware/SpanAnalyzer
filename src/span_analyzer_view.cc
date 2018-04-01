// This is free and unencumbered software released into the public domain.
// For more information, please refer to <http://unlicense.org/>

#include "span_analyzer_view.h"

#include "appcommon/widgets/status_bar_log.h"
#include "wx/cmdproc.h"

#include "span_analyzer_app.h"
#include "span_analyzer_printout.h"

IMPLEMENT_DYNAMIC_CLASS(SpanAnalyzerView, wxView)

BEGIN_EVENT_TABLE(SpanAnalyzerView, wxView)
  EVT_MENU(wxID_PREVIEW, SpanAnalyzerView::OnPrintPreview)
  EVT_MENU(wxID_PRINT, SpanAnalyzerView::OnPrint)
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

wxRect SpanAnalyzerView::GraphicsPlotRect() const {
  // gets active plot pane rect
  if (notebook_plot_->GetSelection() == 0) {
    return pane_profile_->GetClientRect();
  } else if (notebook_plot_->GetSelection() == 1) {
    return pane_cable_->GetClientRect();
  } else {
    return wxRect();
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

bool SpanAnalyzerView::OnClose(bool WXUNUSED(deleteWindow)) {
  if (!GetDocument()->Close()) {
    return false;
  }

  // gets aui manager from main frame
  wxAuiManager* manager = wxAuiManager::GetManager(GetFrame());

  // saves AUI perspective
  wxGetApp().config()->perspective = manager->SavePerspective();

  // detaches panes
  manager->DetachPane(notebook_plot_);
  manager->DetachPane(pane_edit_);
  manager->DetachPane(pane_results_);

  // adjusts log if the pane was docked
  wxAuiPaneInfo& info = manager->GetPane("Log");
  if (info.IsDocked() == true) {
    info.Float();
    info.Hide();
  }

  // updates manager to show changes
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

bool SpanAnalyzerView::OnCreate(wxDocument *doc, long flags) {
  if (!wxView::OnCreate(doc, flags)) {
    return false;
  }

  // initializes cached references
  group_filters_ = nullptr;
  index_filter_ = -1;
  target_render_ = RenderTarget::kScreen;

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

  pane_cable_ = new CablePlotPane(notebook_plot_, this);
  notebook_plot_->AddPage(pane_cable_, "Cable");

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

wxPrintout* SpanAnalyzerView::OnCreatePrintout() {
  return new SpanAnalyzerPrintout(this);
}

void SpanAnalyzerView::OnDraw(wxDC *dc) {
  // gets active plot pane
  PlotPane2d* pane = nullptr;
  if (notebook_plot_->GetSelection() == 0) {
    pane = pane_profile_;
  } else if (notebook_plot_->GetSelection() == 1) {
    pane = pane_cable_;
  }

  // caches the current background
  // alters the background to white if view is rendering to print
  const wxBrush brush = pane->background();
  if (target_render_ == RenderTarget::kPrint) {
    pane->set_background(*wxWHITE_BRUSH);
  }

  // draws the active plot pane
  pane->RenderPlot(*dc);

  // resets to the original background
  pane->set_background(brush);
}

void SpanAnalyzerView::OnNotebookPageChange(wxBookCtrlEvent& event) {
  // skips if old page is invalid
  // this is usually only true on initialization
  if (event.GetOldSelection() == wxNOT_FOUND) {
    return;
  }

  // creates an update hint for the plot pane
  UpdateHint hint(UpdateHint::Type::kViewSelect);

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

void SpanAnalyzerView::OnPrint(wxCommandEvent& event) {
  // gets printout
  wxPrintout* printout = OnCreatePrintout();

  // sets up printer and prints
  wxPrintDialogData data_print(wxGetApp().config()->data_page->GetPrintData());
  wxPrinter printer(&data_print);
  if (printer.Print(wxGetApp().frame(), printout) == false) {
    if (wxPrinter::GetLastError() == wxPRINTER_ERROR) {
      wxLogError("There was a problem printing.");
    }
  }

  delete printout;
}

void SpanAnalyzerView::OnPrintPreview(wxCommandEvent& event) {
  // gets printouts
  // one is for previewing, the other is for printing
  wxPrintout* printout_view = OnCreatePrintout();
  wxPrintout* printout_print = OnCreatePrintout();

  // creates a print dialog data using app data
  wxPrintDialogData data(wxGetApp().config()->data_page->GetPrintData());

  // generates a print preview
  wxPrintPreview* preview = new wxPrintPreview(printout_view, printout_print,
                                               &data);
  if (preview->IsOk() == false) {
    delete preview;
    wxLogError("There was a problem previewing.");
    return;
  }

  // creates and shows a preview frame
  wxPreviewFrame* frame = new wxPreviewFrame(preview, wxGetApp().frame(),
                                             "Print Preview",
                                             wxPoint(100, 100),
                                             wxSize(850, 750));
  frame->Centre(wxBOTH);
  frame->Initialize();
  frame->Show();
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

CablePlotPane* SpanAnalyzerView::pane_cable() {
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

void SpanAnalyzerView::set_target_render(
    const SpanAnalyzerView::RenderTarget& target_render) {
  target_render_ = target_render;
}

SpanAnalyzerView::RenderTarget SpanAnalyzerView::target_render() const {
  return target_render_;
}
