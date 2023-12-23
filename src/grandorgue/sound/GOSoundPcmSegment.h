/*
 * Copyright 2006 Milan Digital Audio LLC
 * Copyright 2009-2023 GrandOrgue contributors (see AUTHORS)
 * License GPL-2.0 or later
 * (https://www.gnu.org/licenses/old-licenses/gpl-2.0.html).
 */

#ifndef GOSOUNDPCMSEGMENT_H
#define GOSOUNDPCMSEGMENT_H

#include "GOInt.h"

/**
 * References a piece of pcm data in the source audio format.
 * This instance does not own the pcm data and may reference to a part of data
 * of another pcm segments.
 * The pcm data is in an interleaved format:
 */
class GOSoundPcmSegment {
  unsigned m_BitsPerSample;
  unsigned m_NChannels;

  // pointer to the start of data
  void *p_data;
  // the offset where this segment starts. In samples, not in bytes
  unsigned m_StartOffset;
  // the segment length. In samples, not in bytes
  unsigned m_length;

public:
  GOSoundPcmSegment() {}

  void Init(unsigned bitsPerSample, unsigned nChannels);

  void Setup(void *pData, unsigned startOffset, unsigned length);

  GOSoundPcmSegment(
    unsigned bitsPerSample,
    unsigned nChannels,
    void *pData,
    unsigned startOffset,
    unsigned length);

  inline int GetSampleData(unsigned position, unsigned channel) const {
    int res = 0;

    if (position < m_length) {
      unsigned i = m_NChannels * (m_StartOffset + position) + channel;

      if (m_BitsPerSample <= 8) {
        GOInt8 *data = (GOInt8 *)p_data;

        res = data[i];
      } else if (m_BitsPerSample <= 16) {
        GOInt16 *data = (GOInt16 *)p_data;

        res = data[i];
      } else if (m_BitsPerSample <= 24) {
        GOInt24 *data = (GOInt24 *)p_data;

        res = data[i];
      } else
        assert(
          0
          && "broken sampler type: more than 24 bits per sample is not "
             "supported");
    }
    return res;
  }

  inline void SetSampleData(unsigned position, unsigned channel, int value) {
    if (position < m_length) {
      unsigned i = m_NChannels * (m_StartOffset + position) + channel;

      if (m_BitsPerSample <= 8) {
        GOInt8 *data = (GOInt8 *)p_data;

        data[i] = value;
      } else if (m_BitsPerSample <= 16) {
        GOInt16 *data = (GOInt16 *)p_data;

        data[i] = value;
      } else if (m_BitsPerSample <= 24) {
        GOInt24 *data = (GOInt24 *)p_data;

        data[i] = value;
      } else
        assert(
          0
          && "broken sampler type: more than 24 bits per sample is not "
             "supported");
    }
  }

  /**
   * Fill beginning of each loop of this segment with the crossfade from the
   * sound stored in this segment to the sound in the sound stored in fadeTo
   *
   * Assume fadeLength <= loopLength <= m_lenght
   * Assume fadeLength <= fadeTo.m_lenght
   *
   * @param fadeTo - the segment with the sound to fade to
   * @param loopLength - length of one loop in samples. The first loop starts
   *   at startOffset
   * @param fadeLength - length of fade
   */
  void FillWithCrossfadeOf(
    const GOSoundPcmSegment &fadeTo, unsigned loopLength, unsigned fadeLength);
};

#endif /* GOSOUNDPCMSEGMENT_H */
