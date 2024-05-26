/*
 * Copyright 2006 Milan Digital Audio LLC
 * Copyright 2009-2024 GrandOrgue contributors (see AUTHORS)
 * License GPL-2.0 or later
 * (https://www.gnu.org/licenses/old-licenses/gpl-2.0.html).
 */

#ifndef GOSOUNDRELEASEALIGNTABLE_H
#define GOSOUNDRELEASEALIGNTABLE_H

#include "GOSoundAudioSection.h"

class GOCache;
class GOCacheWriter;

#define PHASE_ALIGN_DERIVATIVES 2
#define PHASE_ALIGN_AMPLITUDES 32
#define PHASE_ALIGN_MIN_FREQUENCY 20 /* Hertz */

class GOSoundReleaseAlignTable {
private:
  int m_PhaseAlignMaxAmplitude;
  int m_PhaseAlignMaxDerivative;
  int m_PositionEntries[PHASE_ALIGN_DERIVATIVES][PHASE_ALIGN_AMPLITUDES];

public:
  GOSoundReleaseAlignTable();
  ~GOSoundReleaseAlignTable();

  bool Load(GOCache &cache);
  bool Save(GOCacheWriter &cache);

  void ComputeTable(
    const GOSoundAudioSection &m_release,
    int phase_align_max_amplitude,
    int phase_align_max_derivative,
    unsigned int sample_rate,
    unsigned start_position);

  void SetupRelease(
    GOSoundAudioSection::Stream &release_sampler,
    const GOSoundAudioSection::Stream &old_sampler) const;
};

#endif /* GOSOUNDRELEASEALIGNTABLE_H */
