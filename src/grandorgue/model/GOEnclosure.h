/*
 * Copyright 2006 Milan Digital Audio LLC
 * Copyright 2009-2025 GrandOrgue contributors (see AUTHORS)
 * License GPL-2.0 or later
 * (https://www.gnu.org/licenses/old-licenses/gpl-2.0.html).
 */

#ifndef GOENCLOSURE_H_
#define GOENCLOSURE_H_

#include <wx/string.h>

#include "control/GOControl.h"
#include "midi/GOMidiReceiver.h"
#include "midi/GOMidiSender.h"
#include "midi/GOMidiShortcutReceiver.h"
#include "midi/objects/GOMidiObject.h"
#include "sound/GOSoundStateHandler.h"

#include "GOEventHandler.h"
#include "GOSaveableObject.h"

class GOConfigReader;
class GOConfigWriter;
class GOMidiEvent;
class GOMidiMap;
class GOOrganModel;

class GOEnclosure : public GOMidiObject,
                    public GOControl,
                    private GOEventHandler {
private:
  GOOrganModel &r_OrganModel;

  GOMidiReceiver m_midi;
  GOMidiSender m_sender;
  GOMidiShortcutReceiver m_shortcut;
  int m_AmpMinimumLevel;
  int m_MIDIInputNumber;
  int m_MIDIValue;
  bool m_Displayed1;
  bool m_Displayed2;

  void LoadMidiObject(
    GOConfigReader &cfg, const wxString &group, GOMidiMap &midiMap) override;
  void SaveMidiObject(
    GOConfigWriter &cfg, const wxString &group, GOMidiMap &midiMap) override;

  void ProcessMidi(const GOMidiEvent &event) override;
  void HandleKey(int key) override;

  void Save(GOConfigWriter &cfg) override;

  void AbortPlayback() override;
  void PreparePlayback() override;
  void PrepareRecording() override;

public:
  GOEnclosure(GOOrganModel &organModel);
  ~GOEnclosure();

  using GOMidiObject::Init; // for avoiding a warning
  void Init(
    GOConfigReader &cfg,
    const wxString &group,
    const wxString &name,
    unsigned defValue);
  using GOMidiObject::Load; // for avoiding a warning
  void Load(GOConfigReader &cfg, const wxString &group, int enclosureNb);
  void Set(int n);
  int GetValue();
  int GetMIDIInputNumber();
  float GetAttenuation();

  void Scroll(bool scroll_up);
  bool IsDisplayed(bool new_format);
  void SetElementID(int id);

  wxString GetElementStatus() override;
  std::vector<wxString> GetElementActions() override;
  void TriggerElementActions(unsigned no) override;
};

#endif /* GOENCLOSURE_H_ */
