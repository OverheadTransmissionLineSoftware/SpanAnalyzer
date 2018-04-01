// This is free and unencumbered software released into the public domain.
// For more information, please refer to <http://unlicense.org/>

#ifndef OTLS_SPANANALYZER_SPANANALYZERDOC_H_
#define OTLS_SPANANALYZER_SPANANALYZERDOC_H_

#include <list>
#include <vector>

#include "models/base/units.h"
#include "models/transmissionline/hardware.h"
#include "models/transmissionline/line_structure.h"
#include "wx/docview.h"

#include "analysis_controller.h"
#include "span.h"

/// \par OVERVIEW
///
/// This class is a used when updating the view associated with the document.
class UpdateHint : public wxObject {
 public:
  /// This enum class contains types of update hints.
  enum class Type {
    kAnalysisFilterGroupEdit,
    kAnalysisFilterGroupSelect,
    kAnalysisFilterSelect,
    kCablesEdit,
    kPreferencesEdit,
    kSpansEdit,
    kViewSelect,
    kWeathercasesEdit
  };

  /// \brief Default Constructor.
  UpdateHint() {};

  /// \brief Alternate constructor.
  UpdateHint(Type hint) {type_ = hint;};

  /// \brief Sets the hint type.
  /// \param[in] type
  ///   The hint type.
  void set_type(const Type& type) {type_ = type;};

  /// \brief Gets the hint type.
  /// \return The hint type.
  const Type& type() const {return type_;};

 private:
  /// \var type_
  ///   The hint type.
  Type type_;
};

/// \par OVERVIEW
///
/// This is a SpanAnalyzer application document, which holds all of the
/// information for a SpanAnalyzer project file. It also holds all of the
/// sag-tension results that are calculated for the selected span.
///
/// \par SPANS
///
/// The document holds all of the spans that can be analyzed and allows them to
/// be edited. Once a span is activated, an analysis will be performed on it.
///
/// The span is connected to dummy structures. This is only for validation error
/// suppression.
///
/// \par SAG-TENSION ANALYSIS CONTROLLER
///
/// The document uses an analysis controller to handle the sag-tension
/// calculations and store the generated results for the selected span. The
/// results can be accessed via public functions.
///
/// \par APPLICATION DATA
///
/// There is application data that this document does not own. This data can be
/// modified outside of the document, so the document needs to be notified of
/// any data changes in order to update the analysis results.
///
/// \par UPDATES
///
/// The document and view(s) need to be updated when:
///  - The application data is changed
///  - The selected analysis span is changed
///
/// Most updates are done with the UpdateAllViews() method. There are also hints
/// that are passed along to help the view update. The document will handle most
/// of the updates for the selected span, but updates will need to be called
/// outside the document when the application data changes. See the function
/// declarations/definitions to see for sure.
///
/// \par wxWIDGETS LIBRARY BUILD NOTE
///
/// This class requires that the wxWidgets library deviate from the standard
/// build. The wxUSE_STD_IOSTREAM configuration flag must be set to 0, which
/// will force wxWidgets to generate wxInputStream/wxOutputStream classes on the
/// LoadObject() and SaveObject() functions. This is required because this class
/// uses a series of XML handlers to load/save the document, and wxWidgets does
/// not allow std::streams to be used with the XmlDocument::Load() and
/// XmlDocument::Save() functions.
class SpanAnalyzerDoc : public wxDocument {
 public:
  /// \brief Constructor.
  SpanAnalyzerDoc();

  /// \brief Destructor.
  ~SpanAnalyzerDoc();

  /// \brief Appends a span.
  /// \param[in] span
  ///   The span being added.
  /// \return Success status.
  bool AppendSpan(const Span& span);

  /// \brief Converts the document between unit styles.
  /// \param[in] system
  ///   The unit system.
  /// \param[in] style_from
  ///   The unit style to convert from.
  /// \param[in] style_to
  ///   The unit style to convert to.
  void ConvertUnitStyle(const units::UnitSystem& system,
                        const units::UnitStyle& style_from,
                        const units::UnitStyle& style_to);

  /// \brief Converts the document between unit systems.
  /// \param[in] system_from
  ///   The unit system to convert from.
  /// \param[in] system_to
  ///   The unit system to convert to.
  /// This function requires that the document is in a 'consistent' unit
  /// style.
  void ConvertUnitSystem(const units::UnitSystem& system_from,
                         const units::UnitSystem& system_to);

  /// \brief Deletes a span.
  /// \param[in] index
  ///   The index.
  /// \return Success status.
  /// This function may trigger an update if it matches the selected span.
  bool DeleteSpan(const int& index);

  /// \brief Gets the index of the span.
  /// \param[in] span
  ///   The span.
  /// \return The index. If no span is matched, -1 is returned.
  int IndexSpan(const Span* span);

  /// \brief Inserts a span before the specified position.
  /// \param[in] index
  ///   The list index to insert before.
  /// \param[in] span
  ///   The span that is inserted.
  /// \return Success status.
  bool InsertSpan(const int& index, const Span& span);

  /// \brief Determines if the index is valid.
  /// \param[in] index
  ///   The list index.
  /// \param[in] is_included_end
  ///   An indicator that tells if an extra index corresponding to the end
  ///   iterator position is valid.
  /// \return If the index is valid.
  bool IsValidIndex(const int& index, const bool& is_included_end) const;

  /// \brief Loads the document.
  /// \param[in] stream
  ///   The input stream generated by wxWidgets.
  /// \return The input stream.
  wxInputStream& LoadObject(wxInputStream& stream);

  /// \brief Replaces the span.
  /// \param[in] index
  ///   The index.
  /// \param[in] span
  ///   The span with modifications.
  /// \return Success status.
  /// This function may trigger an update if it matches the selected span.
  bool ModifySpan(const int& index, const Span& span);

  /// \brief Moves the span position.
  /// \param[in] index_from
  ///   The index item to move.
  /// \param[in] index_to
  ///   The index to move before.
  /// \return Success status.
  bool MoveSpan(const int& index_from, const int& index_to);

  /// \brief Initializes the document.
  /// \param[in] path
  ///   The document filepath.
  /// \param[in] flags
  ///   Document creation flags.
  /// \return If the document is created successfully.
  /// This is called by the wxWidgets framework. All defaults are still being
  /// kept, this function is just being added to and treated as an
  /// initialization constructor.
  virtual bool OnCreate(const wxString& path, long flags);

  /// \brief Gets a set of filtered results.
  /// \param[in] index_weathercase
  ///   The weathercase index.
  /// \param[in] condition
  ///   The cable condition.
  /// \return A single sag-tension result. If no results are available, a
  ///   nullptr is returned.
  const SagTensionAnalysisResult* Result(
      const int& index_weathercase,
      const CableConditionType& condition) const;

  /// \brief Gets the sag-tension results for a specific condition.
  /// \param[in] condition
  ///   The cable condition.
  /// \return The sag-tension results. If no results are available, a nullptr
  ///   is returned.
  const std::vector<SagTensionAnalysisResult>* Results(
      const CableConditionType& condition) const;

  /// \brief Runs the analysis.
  void RunAnalysis() const;

  /// \brief Saves the document.
  /// \param[out] stream
  ///   The output stream generated by wxWidgets.
  /// \return The output stream.
  wxOutputStream& SaveObject(wxOutputStream& stream);

  /// \brief Gets the span activated for analysis.
  /// \return The activated span. If no span is activated, a nullptr is
  ///   returned.
  const Span* SpanActivated() const;

  /// \brief Gets the analysis stretch state for the specified condition.
  /// \param[in] condition
  ///   The condition.
  /// \return The stretch state for the specified condition. If no stretch state
  ///   is available, a nullptr is returned.
  const CableStretchState* StretchState(const CableConditionType& condition);

  /// \brief Gets activated span index.
  /// \return The activated span index. If no span is activated, -1 is returned.
  int index_activated() const;

  /// \brief Sets the index of the activated span.
  /// \param[in] index
  ///   The span index to activate. To deactivate a span, set to -1.
  /// \return If the span index has been activated/deactivated.
  bool set_index_activated(const int& index);

  /// \brief Gets the spans.
  /// \return The spans.
  const std::list<Span>& spans() const;

 private:
  /// \brief Connects the activated line cable to the line structures.
  void ConnectLineCableActivated();

  /// \brief Disconnects the activated line cable from the line structures.
  void DisconnectLineCableActivated();

  /// \brief Updates the analysis controller with the activated span index.
  void SyncAnalysisController();

  /// \var controller_analysis_
  ///   The analysis controller, which generates sag-tension results.
  mutable AnalysisController controller_analysis_;

  /// \var hardware_
  ///   The hardware that the span connects to. This helps suppress validation
  ///   errors related to the line cable not being connected to anything. This
  ///   is not presented to the user, or saved with the rest of the document
  ///   data.
  Hardware hardware_;

  /// \var index_activated_
  ///   The index of the span that is activated for analysis. If no span is
  ///   activated, this should be set to -1.
  int index_activated_;

  /// \var line_structures_
  ///   The line structures that the span connects to. These help suppress
  ///   validation errors related to the line cable not being connected to
  ///   anything. This is not presented to the user or saved with the rest of
  ///   the document data.
  std::vector<LineStructure> line_structures_;

  /// \var spans_
  ///   The spans. This is a list so spans can be added, deleted, or modified
  ///   with a std container efficiently.
  std::list<Span> spans_;

  /// \var structure_
  ///   The base structure that is referenced by the line structures. This helps
  ///   suppress validation errors related to the line cable not being connected
  ///   to anything. This is not presented to the user or saved with the rest
  ///   of the document data.
  Structure structure_;

  /// \brief This allows wxWidgets to create this class dynamically as part of
  ///   the docview framework.
  wxDECLARE_DYNAMIC_CLASS(SpanAnalyzerDoc);
};

#endif  // OTLS_SPANANALYZER_SPANANALYZERDOC_H_
