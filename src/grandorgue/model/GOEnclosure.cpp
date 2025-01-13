/*
 * Copyright 2006 Milan Digital Audio LLC
 * Copyright 2009-2025 GrandOrgue contributors (see AUTHORS)
 * License GPL-2.0 or later
 * (https://www.gnu.org/licenses/old-licenses/gpl-2.0.html).
 */

#include "GOEnclosure.h"

#include <wx/intl.h>

#include "config/GOConfig.h"
#include "config/GOConfigReader.h"
#include "config/GOConfigWriter.h"

#include "GOOrganModel.h"

static const wxString WX_MIDI_TYPE_CODE = wxT("Enclosure");
static const wxString WX_MIDI_TYPE_NAME = _("Enclosure");

GOEnclosure::GOEnclosure(GOOrganModel &organModel)
  : GOMidiObject(
    organModel,
    WX_MIDI_TYPE_CODE,
    WX_MIDI_TYPE_NAME,
    &m_sender,
    &m_midi,
    &m_shortcut,
    nullptr),
    r_OrganModel(organModel),
    m_midi(organModel, MIDI_RECV_ENCLOSURE),
    m_sender(organModel, MIDI_SEND_ENCLOSURE),
    m_shortcut(KEY_RECV_ENCLOSURE),
    m_AmpMinimumLevel(0),
    m_MIDIInputNumber(0),
    m_MIDIValue(0),
    m_Displayed1(false),
    m_Displayed2(false) {
  r_OrganModel.RegisterEventHandler(this);
}

GOEnclosure::~GOEnclosure() { r_OrganModel.UnRegisterEventHandler(this); }

void GOEnclosure::LoadMidiObject(
  GOConfigReader &cfg, const wxString &group, GOMidiMap &midiMap) {
  m_midi.Load(cfg, group, midiMap);
  m_sender.Load(cfg, group, midiMap);
  m_shortcut.Load(cfg, group);
}

void GOEnclosure::Init(
  GOConfigReader &cfg,
  const wxString &group,
  const wxString &name,
  unsigned defValue) {
  GOMidiObject::Init(cfg, group, name);
  Set(cfg.ReadInteger(
    CMBSetting, m_group, wxT("Value"), 0, 127, false, defValue));
  m_AmpMinimumLevel = 0;
}

void GOEnclosure::Load(
  GOConfigReader &cfg, const wxString &group, int enclosureNb) {
  GOMidiObject::Load(
    cfg, group, cfg.ReadStringNotEmpty(ODFSetting, group, wxT("Name")));
  m_Displayed1
    = cfg.ReadBoolean(ODFSetting, m_group, wxT("Displayed"), false, true);
  m_Displayed2
    = cfg.ReadBoolean(ODFSetting, m_group, wxT("Displayed"), false, false);
  m_AmpMinimumLevel
    = cfg.ReadInteger(ODFSetting, m_group, wxT("AmpMinimumLevel"), 0, 100);
  m_MIDIInputNumber = cfg.ReadInteger(
    ODFSetting, m_group, wxT("MIDIInputNumber"), 0, 200, false, 0);
  Set(cfg.ReadInteger(CMBSetting, m_group, wxT("Value"), 0, 127, false, 127));
  m_midi.SetIndex(enclosureNb);
}

void GOEnclosure::SaveMidiObject(
  GOConfigWriter &cfg, const wxString &group, GOMidiMap &midiMap) {
  m_midi.Save(cfg, group, midiMap);
  m_sender.Save(cfg, group, midiMap);
  m_shortcut.Save(cfg, group);
}

void GOEnclosure::Save(GOConfigWriter &cfg) {
  GOMidiObject::Save(cfg);
  cfg.WriteInteger(m_group, wxT("Value"), m_MIDIValue);
}

void GOEnclosure::Set(int n) {
  if (n < 0)
    n = 0;
  if (n > 127)
    n = 127;
  if (n != m_MIDIValue) {
    m_MIDIValue = n;
    m_sender.SetValue(m_MIDIValue);
  }
  r_OrganModel.UpdateVolume();
  r_OrganModel.SendControlChanged(this);
}

int GOEnclosure::GetMIDIInputNumber() { return m_MIDIInputNumber; }

float GOEnclosure::GetAttenuation() {
  static const float scale = 1.0 / 12700.0;
  return (float)(m_MIDIValue * (100 - m_AmpMinimumLevel) + 127 * m_AmpMinimumLevel)
    * scale;
}

void GOEnclosure::Scroll(bool scroll_up) {
  Set(m_MIDIValue + (scroll_up ? 4 : -4));
}

void GOEnclosure::ProcessMidi(const GOMidiEvent &event) {
  int value;
  if (m_midi.Match(event, value) == MIDI_MATCH_CHANGE)
    Set(value);
}

void GOEnclosure::HandleKey(int key) {
  switch (m_shortcut.Match(key)) {
  case KEY_MATCH:
    Set(m_MIDIValue + 8);
    break;

  case KEY_MATCH_MINUS:
    Set(m_MIDIValue - 8);
    break;
  default:
    break;
  }
}

int GOEnclosure::GetValue() { return m_MIDIValue; }

bool GOEnclosure::IsDisplayed(bool new_format) {
  if (new_format)
    return m_Displayed2;
  else
    return m_Displayed1;
}

void GOEnclosure::AbortPlayback() {
  m_sender.SetValue(0);
  m_sender.SetName(wxEmptyString);
}

void GOEnclosure::PreparePlayback() {
  m_midi.PreparePlayback();
  m_sender.SetName(GetName());
}

void GOEnclosure::PrepareRecording() { m_sender.SetValue(m_MIDIValue); }

void GOEnclosure::SetElementID(int id) {
  m_midi.SetElementID(id);
  m_sender.SetElementID(id);
}

wxString GOEnclosure::GetElementStatus() {
  return wxString::Format(_("%.3f %%"), (m_MIDIValue * 100.0 / 127));
}

std::vector<wxString> GOEnclosure::GetElementActions() {
  std::vector<wxString> actions;
  actions.push_back(_("-"));
  actions.push_back(_("+"));
  return actions;
}

void GOEnclosure::TriggerElementActions(unsigned no) {
  if (no == 0)
    Scroll(false);
  if (no == 1)
    Scroll(true);
}
