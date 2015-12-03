// This is free and unencumbered software released into the public domain.
// For more information, please refer to <http://unlicense.org/>

#ifndef OTLS_SPANANALYZER_SPANANALYZERAPP_H_
#define OTLS_SPANANALYZER_SPANANALYZERAPP_H_

#include "wx/cmdline.h"
#include "wx/docview.h"
#include "wx/wx.h"

#include "span_analyzer_config.h"
#include "span_analyzer_data.h"
#include "span_analyzer_frame.h"

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
/// The application stores configuration settings, which are stored in a data struct.
///
/// \par DATA
///
/// The application holds general data that is loaded on startup. This information is
/// not stored in the application document, but it is referenced. This includes:
///   - cables
///   - weathercases
class SpanAnalyzerApp : public wxApp {
 public:
  /// \brief Constructor.
  SpanAnalyzerApp();

  /// \brief Destructor.
  ~SpanAnalyzerApp();

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

  /// \brief Gets the application directory.
  /// \return The application directory.
  /// In debug mode, this will return the working directory. Otherwise, it will
  /// return the directory that the application executable is stored.
  wxString directory();

  /// \brief Gets the main application frame.
  /// \return The main application frame.
  SpanAnalyzerFrame* frame();

  /// \brief Gets the document manager.
  /// \return The document manager.
  wxDocManager* manager_doc();

 private:
  /// \var config_
  ///   The application configuration settings.
  SpanAnalyzerConfig config_;

  /// \var data_
  ///   The application data.
  SpanAnalyzerData data_;

  /// \var directory_
  ///   The reference directory of the application. The debug switch will set
  ///   this to the working directory, otherwise it will be the directory that
  ///   the application is located.
  wxString directory_;

  /// \var file_start_
  ///   The file that is loaded on application startup. This is specified as
  ///   a command line option.
  wxString file_start_;

  /// \var frame_
  ///   The main application frame.
  SpanAnalyzerFrame* frame_;

  /// \var manager_doc_
  ///   The document manager.
  wxDocManager* manager_doc_;
};

/// This is an array of command line options.
static const wxCmdLineEntryDesc cmd_line_desc [] = {
  {wxCMD_LINE_SWITCH, nullptr, "help", "show this help message",
      wxCMD_LINE_VAL_NONE, wxCMD_LINE_OPTION_HELP},
  {wxCMD_LINE_SWITCH, nullptr, "debug", "loads application resources, config, "
                                "etc from the working directory instead of "
                                "application directory",
      wxCMD_LINE_VAL_NONE},
  {wxCMD_LINE_PARAM, nullptr, nullptr, "startup file",
      wxCMD_LINE_VAL_STRING, wxCMD_LINE_PARAM_OPTIONAL},

  {wxCMD_LINE_NONE}
};

DECLARE_APP(SpanAnalyzerApp)

#endif  // OTLS_SPANANALYZER_SPANANALYZERAPP_H_
