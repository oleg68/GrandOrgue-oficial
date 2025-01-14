/*
 * Copyright 2006 Milan Digital Audio LLC
 * Copyright 2009-2025 GrandOrgue contributors (see AUTHORS)
 * License GPL-2.0 or later
 * (https://www.gnu.org/licenses/old-licenses/gpl-2.0.html).
 */

#ifndef GOLABELCONTROL_H
#define GOLABELCONTROL_H

#include <wx/string.h>

#include "midi/GOMidiSender.h"
#include "midi/objects/GOMidiObject.h"
#include "sound/GOSoundStateHandler.h"

#include "GOControl.h"
#include "GOSaveableObject.h"

class GOConfigReader;
class GOConfigWriter;
class GOOrganController;

class GOLabelControl : public GOMidiObject, public GOControl {
protected:
  wxString m_Content;
  GOOrganController *m_OrganController;
  GOMidiSender m_sender;

  void LoadMidiObject(
    GOConfigReader &cfg, const wxString &group, GOMidiMap &midiMap) override;
  void SaveMidiObject(
    GOConfigWriter &cfg, const wxString &group, GOMidiMap &midiMap) override;

  void AbortPlayback() override;
  void PreparePlayback() override;
  void PrepareRecording() override;

public:
  GOLabelControl(GOOrganController *organController);
  const wxString &GetContent();
  void SetContent(wxString name);

  wxString GetElementStatus() override;
  std::vector<wxString> GetElementActions() override;
  void TriggerElementActions(unsigned no) override;
};

#endif
