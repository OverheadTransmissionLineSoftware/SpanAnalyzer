// This is free and unencumbered software released into the public domain.
// For more information, please refer to <http://unlicense.org/>

#ifndef OTLS_SPANANALYZER_SPANANALYZERAPP_H_
#define OTLS_SPANANALYZER_SPANANALYZERAPP_H_

#include <string>

#include "wx/cmdline.h"
#include "wx/docview.h"
#include "wx/html/helpctrl.h"
#include "wx/wx.h"

#include "spananalyzer/span_analyzer_config.h"
#include "spananalyzer/span_analyzer_data.h"
#include "spananalyzer/span_analyzer_doc.h"
#include "spananalyzer/span_analyzer_frame.h"

/// \par OVERVIEW
///
/// This is the SpanAnalyzer application class.
///
/// \par DOCUMENT / VIEW FRAMEWORK
///
/// This class implements the wxWidgets Document/View framework, which allows
/// loading/saving/printing of application files.
///
/// \par CONFIGURATION SETTINGS
///
/// The application stores configuration settings. This includes:
///   - AUI pane layout and frame size
///   - data file location
///
/// \par DATA
///
/// The application holds general data that is loaded on startup. This
/// information is not stored in the application document, but it can
/// be referenced. This includes:
///   - cables
///   - weathercases
class SpanAnalyzerApp : public wxApp {
 public:
  /// \brief Constructor.
  SpanAnalyzerApp();

  /// \brief Destructor.
  ~SpanAnalyzerApp();

  /// \brief Gets the current document.
  /// \return The document. If no document is open, a nullptr is returned.
  SpanAnalyzerDoc* GetDocument() const;

  /// \brief Parses the command line options provided to the application on
  ///   startup.
  /// \param[in] parser
  ///   The command line parser, which is provided by wxWidgets.
  /// \return The success of command line parsing.
  virtual bool OnCmdLineParsed(wxCmdLineParser& parser);

  /// \brief Cleans up application resources.
  /// \return The wxApp::OnExit return value.
  virtual int OnExit();

  /// \brief Initializes the application.
  /// \return The success of the application initialization.
  virtual bool OnInit();

  /// \brief Initializes the command line parser settings.
  /// \param[in] parser
  ///   The command line parser, which is provided by wxWidgets.
  virtual void OnInitCmdLine(wxCmdLineParser& parser);

  /// \brief Gets the application configuration settings.
  /// \return The application configuration settings.
  SpanAnalyzerConfig* config();

  /// \brief Gets the application data.
  /// \return The application data.
  SpanAnalyzerData* data();

  /// \brief Gets the main application frame.
  /// \return The main application frame.
  SpanAnalyzerFrame* frame();

  /// \brief Gets the help controller.
  /// \return The help controller.
  wxHtmlHelpController* help();

  /// \brief Gets the document manager.
  /// \return The document manager.
  wxDocManager* manager_doc();

  /// \brief Gets the application version.
  /// \return The application version.
  std::string version() const;

 private:
  /// \var config_
  ///   The application configuration settings.
  SpanAnalyzerConfig config_;

  /// \var data_
  ///   The application data.
  SpanAnalyzerData data_;

  /// \var filepath_config_
  ///   The config file that is loaded on application startup. This is specified
  ///   as a command line option.
  std::string filepath_config_;

  /// \var filepath_start_
  ///   The file that is loaded on application startup. This is specified as
  ///   a command line option.
  std::string filepath_start_;

  /// \var frame_
  ///   The main application frame.
  SpanAnalyzerFrame* frame_;

  /// \var help_
  ///   The help controller.
  wxHtmlHelpController* help_;

  /// \var manager_doc_
  ///   The document manager.
  wxDocManager* manager_doc_;

  /// \var version_
  ///   The app version.
  std::string version_;
};

/// This is an array of command line options.
static const wxCmdLineEntryDesc cmd_line_desc [] = {
  {wxCMD_LINE_SWITCH, nullptr, "help", "shows this help message",
      wxCMD_LINE_VAL_NONE, wxCMD_LINE_OPTION_HELP},
  {wxCMD_LINE_OPTION, nullptr, "config", "the application configuration file",
      wxCMD_LINE_VAL_STRING},
  {wxCMD_LINE_PARAM, nullptr, nullptr, "file",
      wxCMD_LINE_VAL_STRING, wxCMD_LINE_PARAM_OPTIONAL},

  {wxCMD_LINE_NONE}
};

DECLARE_APP(SpanAnalyzerApp)

#endif  // OTLS_SPANANALYZER_SPANANALYZERAPP_H_
