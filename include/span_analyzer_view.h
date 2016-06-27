// This is free and unencumbered software released into the public domain.
// For more information, please refer to <http://unlicense.org/>

#ifndef OTLS_SPANANALYZER_SPANANALYZERVIEW_H_
#define OTLS_SPANANALYZER_SPANANALYZERVIEW_H_

#include <list>

#include "wx/docview.h"

class EditPane;
class MessagesPane;
class ResultsPane;

/// \par OVERVIEW
///
/// This class is a used in the view update process. Any information that is
/// helpful is added to the class so the remaining views can update.
class ViewUpdateHint : public wxObject {
 public:
  /// This enum class contains types of view update hints.
  enum class HintType {
    kModelAnalysisWeathercaseEdit,
    kModelCableEdit,
    kModelPreferencesEdit,
    kModelSpansEdit,
    kModelWeathercaseEdit,
    kViewWeathercasesSetChange
  };

  /// \brief Sets the hint type.
  /// \param[in] type
  ///   The hint type.
  void set_type(const HintType& type) {type_ = type;};

  /// \brief Gets the hint type.
  /// \return The hint type.
  const HintType& type() const {return type_;};

 private:
  /// \var type_
  ///   The hint type.
  HintType type_;
};

/// \par OVERVIEW
///
/// This is the SpanAnalyzer application view, which is responsible for
/// displaying information and results from the SpanAnalyzerDoc.
class SpanAnalyzerView : public wxView {
 public:
  /// \brief Constructor.
  SpanAnalyzerView();

  /// \brief Destructor.
  ~SpanAnalyzerView();

  /// \brief Handles closing the view.
  /// \param[in] deleteWindow
  ///   An indicator that determines if the child windows should be deleted.
  /// \return true
  /// This function is called by wxWidgets.
  virtual bool OnClose(bool deleteWindow = true);

  /// \brief Creates the view and initializes.
  /// \param[in] doc
  ///   The document.
  /// \param[in] flags
  ///   The parameters used by wxWidgets.
  /// \return Always true.
  /// This function is called by wxWidgets.
  virtual bool OnCreate(wxDocument *doc, long flags);

  /// \brief Handles drawing/rendering the view.
  /// \param[in] dc
  ///   The device context.
  /// This function is called by wxWidgets.
  virtual void OnDraw(wxDC *dc);

  /// \brief Handles updating of the view.
  /// \param[in] sender
  ///   The view that sent the update request. Since this is a single view
  ///   application, this will remain nullptr.
  /// \param[in] hint
  ///   The update hint that helps child windows optimize the update. This is
  ///   provided by the function/action that triggered the update.
  /// This function is called by wxWidgets.
  virtual void OnUpdate(wxView *sender, wxObject *hint = nullptr);

  /// \brief Gets the edit pane.
  /// \return The edit pane.
  EditPane* pane_edit();

  /// \brief Gets the results pane.
  /// \return The results pane.
  ResultsPane* pane_results();

 private:
  /// \var pane_edit_
  ///   The edit pane.
  EditPane* pane_edit_;

  /// \var pane_results_
  ///   The results pane.
  ResultsPane* pane_results_;

  /// \brief This allows wxWidgets to create this class dynamically as part of
  ///   the docview framework.
  wxDECLARE_DYNAMIC_CLASS(SpanAnalyzerView);
};

#endif //  OTLS_SPANANALYZER_SPANANALYZERVIEW_H_
