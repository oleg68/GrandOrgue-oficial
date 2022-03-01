/*
 * Copyright 2006 Milan Digital Audio LLC
 * Copyright 2009-2021 GrandOrgue contributors (see AUTHORS)
 * License GPL-2.0 or later
 * (https://www.gnu.org/licenses/old-licenses/gpl-2.0.html).
 */

#ifndef GOMIDIEVENTDIALOG_H
#define GOMIDIEVENTDIALOG_H

#include <wx/propdlg.h>

#include "dialogs/common/GOTabbedDialog.h"
#include "document-base/GOView.h"

class GOKeyReceiver;
class GOMidi;
class GOMidiListener;
class GOMidiReceiverBase;
class GOMidiSender;
class GOConfig;
class GOMidiEventKeyTab;
class GOMidiEventRecvTab;
class GOMidiEventSendTab;

class GOMidiEventDialog : public GOTabbedDialog, public GOView {
private:
  GOMidiEventRecvTab *m_recvPage;
  GOMidiEventSendTab *m_sendPage;
  GOMidiEventSendTab *m_sendDivisionPage;
  GOMidiEventKeyTab *m_keyPage;

public:
  GOMidiEventDialog(
    GODocumentBase *doc,
    /*
      if doc != NULL then the dialog is auto destroyed when closed
      if doc == NULL then the caller should call ShowModal() and then should
      call Destroy() if needed
    */
    wxWindow *parent,
    wxString title,
    GOConfig &settings,
    GOMidiReceiverBase *event,
    GOMidiSender *sender,
    GOKeyReceiver *key,
    GOMidiSender *division = NULL);

  void RegisterMIDIListener(GOMidi *midi);
};

#endif /* MIDIEVENTDIALOG_H_ */
