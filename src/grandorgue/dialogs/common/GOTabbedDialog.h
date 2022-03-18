/*
 * Copyright 2006 Milan Digital Audio LLC
 * Copyright 2009-2022 GrandOrgue contributors (see AUTHORS)
 * License GPL-2.0 or later
 * (https://www.gnu.org/licenses/old-licenses/gpl-2.0.html).
 */

#ifndef GOTABBEDDIALOG_H
#define GOTABBEDDIALOG_H

#include <vector>

#include <wx/propdlg.h>

#include "GODialogCloser.h"

class wxBookCtrlBase;
class wxPanel;
class wxSizer;

class GODialogTab;

class GOTabbedDialog : public wxPropertySheetDialog, public GODialogCloser {
private:
  const wxString m_name;
  std::vector<wxString> m_TabNames;
  wxBookCtrlBase *p_book;
  wxSizer *p_ButtonSizer;

  void OnHelp(wxCommandEvent &event);

protected:
  GOTabbedDialog(
    wxWindow *win,
    const wxString &name,  // not translated
    const wxString &title, // translated
    long addStyle = 0);

  wxSizer *GetButtonSizer() const { return p_ButtonSizer; }

  void AddTab(wxPanel *tab, const wxString &tabName, const wxString &tabTitle);
  void AddTab(GODialogTab *tab);

public:
  const wxString &GetCurrTabName() const;

  wxBookCtrlBase *GetBook() const { return p_book; }

  void NavigateToTab(const wxString &tabName);

  virtual bool TransferDataToWindow() override;
  virtual bool Validate() override;
  virtual bool TransferDataFromWindow() override;

  DECLARE_EVENT_TABLE()
};

#endif /* GOTABBEDDIALOG_H */
