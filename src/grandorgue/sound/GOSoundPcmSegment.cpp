/*
 * Copyright 2006 Milan Digital Audio LLC
 * Copyright 2009-2023 GrandOrgue contributors (see AUTHORS)
 * License GPL-2.0 or later
 * (https://www.gnu.org/licenses/old-licenses/gpl-2.0.html).
 */

#include "GOSoundPcmSegment.h"

#include "math.h"

void GOSoundPcmSegment::Init(unsigned bitsPerSample, unsigned nChannels) {
  m_BitsPerSample = m_BitsPerSample;
  m_NChannels = nChannels;
}

void GOSoundPcmSegment::Setup(
  void *pData, unsigned startOffset, unsigned length) {
  p_data = pData;
  m_StartOffset = startOffset;
  m_length = length;
}

GOSoundPcmSegment::GOSoundPcmSegment(
  unsigned bitsPerSample,
  unsigned nChannels,
  void *pData,
  unsigned startOffset,
  unsigned length) {
  Init(bitsPerSample, nChannels);
  Setup(pData, startOffset, length);
}

void GOSoundPcmSegment::FillWithCrossfadeOf(
  const GOSoundPcmSegment &fadeTo, unsigned loopLength, unsigned fadeLength) {
  for (unsigned loopOffset = 0; loopOffset < m_length; loopOffset += loopLength)
    for (unsigned pos = 0; pos < fadeLength; pos++)
      for (unsigned j = 0; j < m_NChannels; j++) {
        unsigned thisPos = loopOffset + pos;
        float thisVal = GetSampleData(thisPos, j);
        float fadeToVal = fadeTo.GetSampleData(pos, j);
        // fadeFactor is chanded smoothly from 1 to 0 when pos is changed from 0
        // to fadeLength
        float fadeFactor = (cos(M_PI * (pos + 0.5) / fadeLength) + 1.0) * 0.5;
        float newVal = thisVal * fadeFactor + fadeToVal * (1 - fadeFactor);

        SetSampleData(thisPos, j, (int)newVal);
      }
}
