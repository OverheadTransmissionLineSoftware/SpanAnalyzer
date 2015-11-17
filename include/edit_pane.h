// This is free and unencumbered software released into the public domain.
// For more information, please refer to <http://unlicense.org/>

#ifndef OTLS_SPANANALYZER_EDITPANE_H_
#define OTLS_SPANANALYZER_EDITPANE_H_

#include "wx/docview.h"
#include "wx/treectrl.h"
#include "wx/wx.h"

#include "span.h"

class EditTreeItemData : public wxTreeItemData {
 public:
  enum class Type {
    kSpan,
  };

  EditTreeItemData(Type type, const wxString& description) {
    type_ = type;
    description_ = description;
  };

  wxString description() {return description_;}
  void set_description(wxString description) {description_ = description;};
  Type type() {return type_;}

 private:
  wxString description_;
  Type type_;
};

/// \par OVERVIEW
class EditPane : public wxPanel {
 public:
  EditPane(wxWindow* parent, wxView* view);
  ~EditPane();

  void OnClear(wxCommandEvent& event);

  void OnContextMenuSelect(wxCommandEvent& event);

  void OnItemActivate(wxTreeEvent& event);

  void OnItemMenu(wxTreeEvent& event);

  Span* ActivatedSpan();

  void Update(wxObject* hint = nullptr);

 private:
  void ActivateSpan(const wxTreeItemId& id);
  void AddSpan();
  void CopySpan(const wxTreeItemId& id);
  void DeleteSpan(const wxTreeItemId& id);
  void DeleteSpans();

  void EditSpan(const wxTreeItemId& id);

  void InitSpans();

  wxView* view_;

  /// \var
  wxTreeCtrl* treectrl_doc_;

  DECLARE_EVENT_TABLE()
};

# endif //  OTLS_SPANANALYZER_EDITPANE_H_
