/*
 * GrandOrgue - free pipe organ simulator
 *
 * Copyright 2006 Milan Digital Audio LLC
 * Copyright 2009-2012 GrandOrgue contributors (see AUTHORS)
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of the
 * License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 * MA 02111-1307, USA.
 */

#include "GOrgueFrameGeneral.h"

#include "GOrgueConfigReader.h"
#include "GOrgueConfigWriter.h"
#include "GOrgueCoupler.h"
#include "GOrgueDivisional.h"
#include "GOrgueDivisionalCoupler.h"
#include "GOrgueGeneral.h"
#include "GOrgueManual.h"
#include "GOrgueSetter.h"
#include "GOrgueStop.h"
#include "GOrgueTremulant.h"
#include "GrandOrgueFile.h"

GOrgueFrameGeneral::GOrgueFrameGeneral(GrandOrgueFile* organfile):
	m_organfile(organfile),
	m_group(),
	m_Stops(),
	m_StopManual(),
	m_Couplers(),
	m_CouplerManual(),
	m_Tremulants(),
	m_DivisionalCouplers(),
	m_Protected(false)
{
}

void GOrgueFrameGeneral::Copy(GOrgueFrameGeneral* general)
{
	m_Stops = general->m_Stops;
	m_StopManual = general->m_StopManual;
	m_Couplers = general->m_Couplers;
	m_CouplerManual = general->m_CouplerManual;
	m_Tremulants = general->m_Tremulants;
	m_DivisionalCouplers = general->m_DivisionalCouplers;
	m_organfile->Modified();
}


void GOrgueFrameGeneral::Load(GOrgueConfigReader& cfg, wxString group)
{
	m_group = group;

	m_Protected = cfg.ReadBoolean(ODFSetting, group, wxT("Protected"), false, false);

	LoadCombination(cfg);
}

void GOrgueFrameGeneral::LoadCombination(GOrgueConfigReader& cfg)
{
	wxString buffer;
	unsigned NumberOfStops = cfg.ReadInteger(UserSetting, m_group, wxT("NumberOfStops"), 0, 999);
	unsigned NumberOfCouplers = cfg.ReadInteger(UserSetting, m_group, wxT("NumberOfCouplers"), 0, 999);
	unsigned NumberOfTremulants = cfg.ReadInteger(UserSetting, m_group, wxT("NumberOfTremulants"), 0, 999);
	unsigned NumberOfDivisionalCouplers = cfg.ReadInteger(UserSetting, m_group, wxT("NumberOfDivisionalCouplers"), 0, 999, m_organfile->GeneralsStoreDivisionalCouplers());

	m_Stops.clear();
	m_StopManual.clear();
	m_Couplers.clear();
	m_CouplerManual.clear();
	m_Tremulants.clear();
	m_DivisionalCouplers.clear();

	m_Stops.resize(NumberOfStops);
	m_StopManual.resize(NumberOfStops);
	for (unsigned i = 0; i < NumberOfStops; i++)
	{
		buffer.Printf(wxT("StopManual%03d"), i + 1);
		unsigned m = cfg.ReadInteger(UserSetting, m_group, buffer, m_organfile->GetFirstManualIndex(), m_organfile->GetManualAndPedalCount());
		m_StopManual[i] = m;
		buffer.Printf(wxT("StopNumber%03d"), i + 1);
		m_Stops[i] = cfg.ReadInteger(UserSetting, m_group, buffer, -m_organfile->GetManual(m)->GetStopCount(), m_organfile->GetManual(m)->GetStopCount());
	}

	m_Couplers.resize(NumberOfCouplers);
	m_CouplerManual.resize(NumberOfCouplers);
	for (unsigned i = 0; i < NumberOfCouplers; i++)
	{
		buffer.Printf(wxT("CouplerManual%03d"), i + 1);
		unsigned m = cfg.ReadInteger(UserSetting, m_group, buffer, m_organfile->GetFirstManualIndex(), m_organfile->GetManualAndPedalCount());
		m_CouplerManual[i] = m;
		buffer.Printf(wxT("CouplerNumber%03d"), i + 1);
		m_Couplers[i] = cfg.ReadInteger(UserSetting, m_group, buffer, -999, 999);
	}

	m_Tremulants.resize(NumberOfTremulants);
	for (unsigned i = 0; i < NumberOfTremulants; i++)
	{
		buffer.Printf(wxT("TremulantNumber%03d"), i + 1);
		m_Tremulants[i] = cfg.ReadInteger(UserSetting, m_group, buffer, -m_organfile->GetTremulantCount(), m_organfile->GetTremulantCount());
	}

	m_DivisionalCouplers.resize(NumberOfDivisionalCouplers);
	for (unsigned i = 0; i < NumberOfDivisionalCouplers; i++)
	{
		buffer.Printf(wxT("DivisionalCouplerNumber%03d"), i + 1);
		m_DivisionalCouplers[i] = cfg.ReadInteger(UserSetting, m_group, buffer, -m_organfile->GetDivisionalCouplerCount(), m_organfile->GetDivisionalCouplerCount());
	}
}

void GOrgueFrameGeneral::Push()
{

	bool used = false;

	if (m_organfile->GetSetter()->IsSetterActive())
	{
		if (m_Protected)
			return;
		if (m_organfile->GetSetter()->GetSetterType() == SETTER_REGULAR)
		{
			m_Stops.clear();
			m_StopManual.clear();
			m_Couplers.clear();
			m_CouplerManual.clear();
			m_Tremulants.clear();
			m_DivisionalCouplers.clear();
	
			for (unsigned j = m_organfile->GetFirstManualIndex(); j <= m_organfile->GetManualAndPedalCount(); j++)
			{
				for (unsigned i = 0; i < m_organfile->GetManual(j)->GetStopCount(); i++)
				{
					if (!m_organfile->GetSetter()->StoreInvisibleObjects() && 
					    !m_organfile->CombinationsStoreNonDisplayedDrawstops() && 
					    !m_organfile->GetManual(j)->GetStop(i)->IsDisplayed())
						continue;
					m_StopManual.push_back(j);
					m_Stops.push_back(m_organfile->GetManual(j)->GetStop(i)->IsEngaged() ? i + 1 : -i - 1);
					used |= m_organfile->GetManual(j)->GetStop(i)->IsEngaged();
				}
			}
	
			for (unsigned j = m_organfile->GetFirstManualIndex(); j <= m_organfile->GetManualAndPedalCount(); j++)
			{
				for (unsigned i = 0; i < m_organfile->GetManual(j)->GetCouplerCount(); i++)
				{
					if (!m_organfile->GetSetter()->StoreInvisibleObjects() && 
					    !m_organfile->CombinationsStoreNonDisplayedDrawstops() &&
					    !m_organfile->GetManual(j)->GetCoupler(i)->IsDisplayed())
						continue;
					m_CouplerManual.push_back(j);
					m_Couplers.push_back (m_organfile->GetManual(j)->GetCoupler(i)->IsEngaged() ? i + 1 : -i - 1);
					used |= m_organfile->GetManual(j)->GetCoupler(i)->IsEngaged();
				}
			}
	
			for (unsigned i = 0; i < m_organfile->GetTremulantCount(); i++)
			{
				if (!m_organfile->GetSetter()->StoreInvisibleObjects() &&
				    !m_organfile->CombinationsStoreNonDisplayedDrawstops() &&
				    !m_organfile->GetTremulant(i)->IsDisplayed())
					continue;
				m_Tremulants.push_back (m_organfile->GetTremulant(i)->IsEngaged() ? i + 1 : -i - 1);
				used |= m_organfile->GetTremulant(i)->IsEngaged();
			}
	
			if (m_organfile->GeneralsStoreDivisionalCouplers())
			{
				for (unsigned i = 0; i < m_organfile->GetDivisionalCouplerCount(); i++)
				{
					m_DivisionalCouplers.push_back (m_organfile->GetDivisionalCoupler(i)->IsEngaged() ? i + 1 : -1 - 1);
					used |= m_organfile->GetDivisionalCoupler(i)->IsEngaged();
				}
			}
			m_organfile->Modified();
		}
		if (m_organfile->GetSetter()->GetSetterType() == SETTER_SCOPE)
		{
			m_Stops.clear();
			m_StopManual.clear();
			m_Couplers.clear();
			m_CouplerManual.clear();
			m_Tremulants.clear();
			m_DivisionalCouplers.clear();
	
			for (unsigned j = m_organfile->GetFirstManualIndex(); j <= m_organfile->GetManualAndPedalCount(); j++)
			{
				for (unsigned i = 0; i < m_organfile->GetManual(j)->GetStopCount(); i++)
				{
					if (!m_organfile->GetSetter()->StoreInvisibleObjects() && 
					    !m_organfile->CombinationsStoreNonDisplayedDrawstops() &&
					    !m_organfile->GetManual(j)->GetStop(i)->IsDisplayed())
						continue;
					if (m_organfile->GetManual(j)->GetStop(i)->IsEngaged())
					{
						m_StopManual.push_back(j);
						m_Stops.push_back(i + 1);
					}
				}
			}
	
			for (unsigned j = m_organfile->GetFirstManualIndex(); j <= m_organfile->GetManualAndPedalCount(); j++)
			{
				for (unsigned i = 0; i < m_organfile->GetManual(j)->GetCouplerCount(); i++)
				{
					if (!m_organfile->GetSetter()->StoreInvisibleObjects() &&
					    !m_organfile->CombinationsStoreNonDisplayedDrawstops() &&
					    !m_organfile->GetManual(j)->GetCoupler(i)->IsDisplayed())
						continue;
					if (m_organfile->GetManual(j)->GetCoupler(i)->IsEngaged())
					{
						m_CouplerManual.push_back(j);
						m_Couplers.push_back (i + 1);
					}
				}
			}
	
			for (unsigned i = 0; i < m_organfile->GetTremulantCount(); i++)
			{
				if (!m_organfile->GetSetter()->StoreInvisibleObjects() &&
				    !m_organfile->CombinationsStoreNonDisplayedDrawstops() &&
				    !m_organfile->GetTremulant(i)->IsDisplayed())
					continue;
				if (m_organfile->GetTremulant(i)->IsEngaged())
					m_Tremulants.push_back (i + 1);
			}
	
			if (m_organfile->GeneralsStoreDivisionalCouplers())
			{
				for (unsigned i = 0; i < m_organfile->GetDivisionalCouplerCount(); i++)
				{
					if (m_organfile->GetDivisionalCoupler(i)->IsEngaged())
						m_DivisionalCouplers.push_back (i + 1);
				}
			}
			m_organfile->Modified();
		}
		if (m_organfile->GetSetter()->GetSetterType() == SETTER_SCOPED)
		{
			for (unsigned i = 0; i < m_Stops.size(); i++)
			{
				if (!m_Stops[i])
					continue;
				unsigned k = abs(m_Stops[i]) - 1;
				if (m_StopManual[i] < m_organfile->GetFirstManualIndex() ||
				    m_StopManual[i] > m_organfile->GetManualAndPedalCount() ||
				    k >= m_organfile->GetManual(m_StopManual[i])->GetStopCount())
					continue;
				if (m_organfile->GetManual(m_StopManual[i])->GetStop(k)->IsEngaged())
					m_Stops[i] = k + 1;
				else
					m_Stops[i] = -k - 1;
				used |= m_Stops[i] > 0;
			}
	
			for (unsigned i = 0; i < m_Couplers.size(); i++)
			{
				if (!m_Couplers[i])
					continue;
				unsigned k = abs(m_Couplers[i]) - 1;
				if (m_CouplerManual[i] < m_organfile->GetFirstManualIndex() ||
				    m_CouplerManual[i] > m_organfile->GetManualAndPedalCount() ||
				    k >= m_organfile->GetManual(m_CouplerManual[i])->GetCouplerCount())
					continue;
				if (m_organfile->GetManual(m_CouplerManual[i])->GetCoupler(k)->IsEngaged())
					m_Couplers[i] = k + 1;
				else
					m_Couplers[i] = -k - 1;
				used |= m_Couplers[i] > 0;
			}
	
			for (unsigned i = 0; i < m_Tremulants.size(); i++)
			{
				if (!m_Tremulants[i])
					continue;
				unsigned k = abs(m_Tremulants[i]) - 1;
				if (k >= m_organfile->GetTremulantCount())
					continue;
				if (m_organfile->GetTremulant(k)->IsEngaged())
					m_Tremulants[i] = k + 1;
				else
					m_Tremulants[i] = -k - 1;
				used |= m_Tremulants[i] > 0;
			}
	
			for (unsigned i = 0; i < m_DivisionalCouplers.size(); i++)
			{
				if (!m_DivisionalCouplers[i])
					continue;
				unsigned k = abs(m_DivisionalCouplers[i]) - 1;
				if (k >= m_organfile->GetDivisionalCouplerCount())
					continue;
				if (m_organfile->GetDivisionalCoupler(k)->IsEngaged())
					m_DivisionalCouplers[i] = k + 1;
				else
					m_DivisionalCouplers[i] = -k - 1;
				used |= m_DivisionalCouplers[i] > 0;
			}
			m_organfile->Modified();
		}
	}
	else
	{

		for (unsigned i = 0; i < m_Stops.size(); i++)
		{
			if (!m_Stops[i])
				continue;
			unsigned k = abs(m_Stops[i]) - 1;
			if (m_StopManual[i] < m_organfile->GetFirstManualIndex() ||
			    m_StopManual[i] > m_organfile->GetManualAndPedalCount() ||
			    k >= m_organfile->GetManual(m_StopManual[i])->GetStopCount())
				continue;
			m_organfile->GetManual(m_StopManual[i])->GetStop(k)->Set(m_Stops[i] > 0);
			used |= m_Stops[i] > 0;
		}

		for (unsigned i = 0; i < m_Couplers.size(); i++)
		{
			if (!m_Couplers[i])
				continue;
			unsigned k = abs(m_Couplers[i]) - 1;
			if (m_CouplerManual[i] < m_organfile->GetFirstManualIndex() ||
			    m_CouplerManual[i] > m_organfile->GetManualAndPedalCount() ||
			    k >= m_organfile->GetManual(m_CouplerManual[i])->GetCouplerCount())
				continue;
			m_organfile->GetManual(m_CouplerManual[i])->GetCoupler(k)->Set(m_Couplers[i] > 0);
			used |= m_Couplers[i] > 0;
		}

		for (unsigned i = 0; i < m_Tremulants.size(); i++)
		{
			if (!m_Tremulants[i])
				continue;
			unsigned k = abs(m_Tremulants[i]) - 1;
			if (k >= m_organfile->GetTremulantCount())
				continue;
			m_organfile->GetTremulant(k)->Set(m_Tremulants[i] > 0);
			used |= m_Tremulants[i] > 0;
		}

		for (unsigned i = 0; i < m_DivisionalCouplers.size(); i++)
		{
			if (!m_DivisionalCouplers[i])
				continue;
			unsigned k = abs(m_DivisionalCouplers[i]) - 1;
			if (k >= m_organfile->GetDivisionalCouplerCount())
				continue;
			m_organfile->GetDivisionalCoupler(k)->Set(m_DivisionalCouplers[i] > 0);
			used |= m_DivisionalCouplers[i] > 0;
		}

	}

	for (unsigned k = 0; k < m_organfile->GetGeneralCount(); k++)
	{
		GOrgueGeneral* general = m_organfile->GetGeneral(k);
		general->Display(&general->GetGeneral() == this && used);
	}

	for (unsigned j = m_organfile->GetFirstManualIndex(); j <= m_organfile->GetManualAndPedalCount(); j++)
	{
		for (unsigned k = 0; k < m_organfile->GetManual(j)->GetDivisionalCount(); k++)
			m_organfile->GetManual(j)->GetDivisional(k)->Display(false);
	}

	m_organfile->GetSetter()->ResetDisplay();
}

void GOrgueFrameGeneral::Save(GOrgueConfigWriter& cfg)
{
	cfg.Write(m_group, wxT("NumberOfStops"), (int)m_Stops.size());
	cfg.Write(m_group, wxT("NumberOfCouplers"), (int)m_Couplers.size());
	cfg.Write(m_group, wxT("NumberOfTremulants"), (int)m_Tremulants.size());
	cfg.Write(m_group, wxT("NumberOfDivisionalCouplers"), (int)m_DivisionalCouplers.size());

	wxString buffer;

	for (unsigned i = 0; i < m_Stops.size(); i++)
	{
		buffer.Printf(wxT("StopManual%03d"), i + 1);
		cfg.Write(m_group, buffer, m_StopManual[i], true, true);
		buffer.Printf(wxT("StopNumber%03d"), i + 1);
		cfg.Write(m_group, buffer, m_Stops[i], true);
	}

	for (unsigned i = 0; i < m_Couplers.size(); i++)
	{
		buffer.Printf(wxT("CouplerManual%03d"), i + 1);
		cfg.Write(m_group, buffer, m_CouplerManual[i], true, true);
		buffer.Printf(wxT("CouplerNumber%03d"), i + 1);
		cfg.Write(m_group, buffer, m_Couplers[i], true);
	}

	for (unsigned i = 0; i < m_Tremulants.size(); i++)
	{
		buffer.Printf(wxT("TremulantNumber%03d"), i + 1);
		cfg.Write(m_group, buffer, m_Tremulants[i], true);
	}

	for (unsigned i = 0; i < m_DivisionalCouplers.size(); i++)
	{
		buffer.Printf(wxT("DivisionalCouplerNumber%03d"), i + 1);
		cfg.Write(m_group, buffer, m_DivisionalCouplers[i], true);
	}
}

