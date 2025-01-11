/*
 * Copyright 2006 Milan Digital Audio LLC
 * Copyright 2009-2025 GrandOrgue contributors (see AUTHORS)
 * License GPL-2.0 or later
 * (https://www.gnu.org/licenses/old-licenses/gpl-2.0.html).
 */

#include "GOLabelControl.h"

#include <wx/intl.h>

#include "GODocument.h"
#include "GOOrganController.h"
#include "config/GOConfig.h"

static const wxString WX_MIDI_TYPE_CODE = wxT("Label");
static const wxString WX_MIDI_TYPE_NAME = _("Label");

GOLabelControl::GOLabelControl(GOOrganController *organController)
  : GOMidiObject(
    *organController,
    WX_MIDI_TYPE_CODE,
    WX_MIDI_TYPE_NAME,
    m_Name,
    &m_sender,
    nullptr,
    nullptr,
    nullptr),
    m_OrganController(organController),
    m_sender(*organController, MIDI_SEND_LABEL) {
  m_OrganController->RegisterMidiConfigurator(this);
  m_OrganController->RegisterSoundStateHandler(this);
}

void GOLabelControl::Init(GOConfigReader &cfg, wxString group, wxString name) {
  m_OrganController->RegisterSaveableObject(this);
  m_group = group;
  m_Name = name;
  m_sender.Load(cfg, m_group, m_OrganController->GetSettings().GetMidiMap());
}

void GOLabelControl::Load(GOConfigReader &cfg, wxString group, wxString name) {
  m_OrganController->RegisterSaveableObject(this);
  m_group = group;
  m_Name = name;
  m_sender.Load(cfg, m_group, m_OrganController->GetSettings().GetMidiMap());
}

void GOLabelControl::Save(GOConfigWriter &cfg) {
  m_sender.Save(cfg, m_group, m_OrganController->GetSettings().GetMidiMap());
}

const wxString &GOLabelControl::GetContent() { return m_Content; }

void GOLabelControl::SetContent(wxString name) {
  m_Content = name;
  m_sender.SetLabel(m_Content);
  m_OrganController->SendControlChanged(this);
}

void GOLabelControl::AbortPlayback() {
  m_sender.SetLabel(wxEmptyString);
  m_sender.SetName(wxEmptyString);
}

void GOLabelControl::PreparePlayback() { m_sender.SetName(m_Name); }

void GOLabelControl::PrepareRecording() { m_sender.SetLabel(m_Content); }

wxString GOLabelControl::GetElementStatus() { return m_Content; }

std::vector<wxString> GOLabelControl::GetElementActions() {
  std::vector<wxString> actions;
  return actions;
}

void GOLabelControl::TriggerElementActions(unsigned no) {
  // Never called
}
