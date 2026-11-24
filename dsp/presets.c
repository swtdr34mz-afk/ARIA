/* SPDX-License-Identifier: MPL-2.0
 * Copyright (C) 2026 swtdr34mz (ARIA) */
/* The factory sounds (each the INIT sound with a few parameters changed) and the factory key maps of the chord
 * memory (the white keys play the chords of a key; the black keys stay single notes). */
#include "presets.h"

#define A_(p) TP(0, p)
#define B_(p) TP(1, p)
#define END { -1, 0 }

static const aria_set_t INIT[] = { END };
static const aria_set_t GLASS_PAD[] = {
    { A_(TP_O2_OCT), OCT_16 }, { A_(TP_O2_FREQ), 0.5057f }, { A_(TP_MX_O2), 0.55f },
    { A_(TP_F_ORDER), ORD_STEREO }, { A_(TP_F1_CUT), 0.52f }, { A_(TP_F2_CUT), 0.58f }, { A_(TP_F1_RES), 0.35f },
    { A_(TP_F2_RES), 0.35f }, { A_(TP_F1_ENV), 0.68f }, { A_(TP_F2_ENV), 0.64f },
    { A_(TP_FE_A), 0.62f }, { A_(TP_FE_D), 0.7f }, { A_(TP_FE_S), 0.4f }, { A_(TP_FE_R), 0.6f },
    { A_(TP_AE_A), 0.58f }, { A_(TP_AE_S), 0.9f }, { A_(TP_AE_R), 0.62f },
    { A_(TP_VCA_SPREAD), 0.8f }, { A_(TP_DETUNE), 0.45f },
    { A_(TP_L2_AMT), 0.25f }, { A_(TP_L2_RATE), 0.25f }, { A_(TP_L2_DEST), LD_FILTERS },
    { GP_DL_MIX, 0.38f }, { GP_DL_FB, 0.55f }, { GP_DL_DIFF, 0.7f }, { GP_DL_CHAR, 0.38f }, END
};
static const aria_set_t OVERLOAD_BASS[] = {
    { A_(TP_MONO), 1 }, { A_(TP_GLIDE), 0.12f }, { A_(TP_O1_OCT), OCT_16 }, { A_(TP_O1_MIX), 1 },
    { A_(TP_MX_O2), 0.7f }, { A_(TP_O2_FREQ), 0.5043f }, { A_(TP_MX_OVERLOAD), 0.55f },
    { A_(TP_F1_CUT), 0.22f }, { A_(TP_F1_RES), 0.55f }, { A_(TP_F1_ENV), 0.86f }, { A_(TP_F2_CUT), 0.6f },
    { A_(TP_FE_D), 0.38f }, { A_(TP_FE_S), 0.1f }, { A_(TP_FE_VEL), 1 },
    { A_(TP_AE_D), 0.5f }, { A_(TP_AE_S), 0.8f }, { A_(TP_AE_R), 0.2f }, END
};
static const aria_set_t FM_KEYS[] = {
    { A_(TP_O1_TRISAW), 0 }, { A_(TP_O2_TRISAW), 0 }, { A_(TP_O2_OCT), OCT_4 },
    { A_(TP_FM_AMT), 0.55f }, { A_(TP_FM_21), 1 }, { A_(TP_MX_O2), 0.15f },
    { A_(TP_F1_CUT), 0.55f }, { A_(TP_F1_ENV), 0.78f }, { A_(TP_F1_KB), KB_FULL },
    { A_(TP_FE_D), 0.55f }, { A_(TP_FE_S), 0 }, { A_(TP_FE_R), 0.5f },
    { A_(TP_AE_D), 0.72f }, { A_(TP_AE_S), 0 }, { A_(TP_AE_R), 0.6f }, { A_(TP_VCA_SPREAD), 0.5f },
    { GP_DL_MIX, 0.25f }, { GP_DL_FB, 0.35f }, { GP_DL_TIME_L, 0.7f }, { GP_DL_TIME_R, 0.75f }, END
};
static const aria_set_t CHORD_KEYS[] = {
    { GP_CH_ON, 1 }, { GP_CH_KEY, 1 }, { GP_CH_MAP, 0 },
    { A_(TP_O1_TRISAW), 0.6f }, { A_(TP_MX_O2), 0.5f }, { A_(TP_O2_FREQ), 0.5071f },
    { A_(TP_F1_CUT), 0.45f }, { A_(TP_F1_RES), 0.25f }, { A_(TP_F1_ENV), 0.7f }, { A_(TP_F1_KB), KB_HALF },
    { A_(TP_FE_A), 0.2f }, { A_(TP_FE_D), 0.6f }, { A_(TP_FE_S), 0.3f },
    { A_(TP_AE_A), 0.25f }, { A_(TP_AE_S), 0.8f }, { A_(TP_AE_R), 0.5f },
    { A_(TP_VCA_SPREAD), 0.7f }, { A_(TP_DETUNE), 0.35f },
    { GP_DL_MIX, 0.3f }, { GP_DL_FB, 0.45f }, { GP_DL_DIFF, 0.5f }, END
};
static const aria_set_t SYNC_LEAD[] = {
    { A_(TP_MONO), 1 }, { A_(TP_GLIDE), 0.18f }, { A_(TP_O2_SYNC), 1 }, { A_(TP_O2_OCT), OCT_4 },
    { A_(TP_O2_FREQ), 0.62f }, { A_(TP_MX_O1), 0 }, { A_(TP_MX_O2), 0.85f },
    { A_(TP_L1_DEST), LD_OSC2 }, { A_(TP_L1_AMT), 0.55f }, { A_(TP_L1_RATE), 0.3f }, { A_(TP_L1_WAVE), LW_TRI },
    { A_(TP_F1_CUT), 0.7f }, { A_(TP_F1_RES), 0.2f }, { A_(TP_PL_AMT), 0.25f }, { A_(TP_MX_OVERLOAD), 0.3f },
    { GP_DL_MIX, 0.3f }, { GP_DL_FB, 0.4f }, { GP_DL_TIME_L, 0.72f }, { GP_DL_TIME_R, 0.66f }, END
};
static const aria_set_t SPLIT_BASS_PAD[] = {   /* timbre A a bass below C3, timbre B a pad above */
    { GP_VC_MODE, VC_SPLIT }, { GP_VC_SPLIT, 48.0f / 127.0f }, { GP_VC_VOICES_A, 0 },
    { A_(TP_MONO), 1 }, { A_(TP_O1_OCT), OCT_16 }, { A_(TP_O1_MIX), 1 }, { A_(TP_MX_O2), 0.6f },
    { A_(TP_F1_CUT), 0.25f }, { A_(TP_F1_RES), 0.4f }, { A_(TP_F1_ENV), 0.8f }, { A_(TP_FE_D), 0.4f },
    { A_(TP_FE_S), 0.15f }, { A_(TP_MX_OVERLOAD), 0.4f },
    { B_(TP_O2_FREQ), 0.507f }, { B_(TP_MX_O2), 0.6f }, { B_(TP_F1_CUT), 0.5f }, { B_(TP_F1_ENV), 0.62f },
    { B_(TP_FE_A), 0.55f }, { B_(TP_FE_D), 0.65f }, { B_(TP_AE_A), 0.5f }, { B_(TP_AE_R), 0.6f },
    { B_(TP_VCA_SPREAD), 0.8f }, { B_(TP_DETUNE), 0.4f }, { B_(TP_F_ORDER), ORD_STEREO }, { B_(TP_F2_CUT), 0.56f },
    { GP_DL_A, 0 }, { GP_DL_MIX, 0.35f }, { GP_DL_FB, 0.5f }, { GP_DL_DIFF, 0.6f }, END
};
static const aria_set_t RING_BELLS[] = {
    { A_(TP_O1_TRISAW), 0 }, { A_(TP_O2_TRISAW), 0 }, { A_(TP_O2_OCT), OCT_4 }, { A_(TP_O2_FREQ), 0.5f + 3.0f / 14.0f },
    { A_(TP_MX_O1), 0.3f }, { A_(TP_MX_RING), 0.85f },
    { A_(TP_F1_CUT), 0.75f }, { A_(TP_AE_D), 0.7f }, { A_(TP_AE_S), 0 }, { A_(TP_AE_R), 0.65f },
    { A_(TP_VCA_SPREAD), 0.9f }, { GP_DL_MIX, 0.4f }, { GP_DL_FB, 0.6f }, { GP_DL_DIFF, 0.8f }, { GP_DL_CHAR, 0.6f }, END
};
static const aria_set_t PULSING_DRONE[] = {
    { A_(TP_MO_FREQ), 0.35f }, { A_(TP_MO_VCA), 0.8f }, { A_(TP_MO_FILTER), 0.35f }, { A_(TP_MO_WAVE), MOW_SQUARE },
    { A_(TP_MO_UNI), 1 }, { A_(TP_O2_OCT), OCT_16 }, { A_(TP_MX_O2), 0.6f }, { A_(TP_O2_FREQ), 0.5085f },
    { A_(TP_F1_CUT), 0.4f }, { A_(TP_F1_RES), 0.5f }, { A_(TP_F_LINK), 1 }, { A_(TP_F2_CUT), 0.45f }, { A_(TP_F_ORDER), ORD_STEREO },
    { A_(TP_AE_A), 0.45f }, { A_(TP_AE_R), 0.7f }, { A_(TP_VCA_SPREAD), 0.9f },
    { GP_DL_MIX, 0.35f }, { GP_DL_FB, 0.6f }, { GP_DL_DIFF, 0.6f }, END
};

const aria_preset_t ARIA_PRESETS[ARIA_NPRESETS] = {
    { "INIT", INIT },
    { "GLASS PAD", GLASS_PAD },
    { "OVERLOAD BASS", OVERLOAD_BASS },
    { "FM KEYS", FM_KEYS },
    { "CHORD KEYS", CHORD_KEYS },
    { "SYNC LEAD", SYNC_LEAD },
    { "SPLIT BASS PAD", SPLIT_BASS_PAD },
    { "RING BELLS", RING_BELLS },
    { "PULSING DRONE", PULSING_DRONE },
};

void aria_preset_values(int i, float *out)
{
    for (int p = 0; p < ARIA_NPARAM; p++) out[p] = aria_param_info(p)->def;
    if (i < 0 || i >= ARIA_NPRESETS) return;
    for (const aria_set_t *s = ARIA_PRESETS[i].set; s->p >= 0; s++) out[s->p] = s->v;
}

/* the key maps: 1 C major 7ths, 2 C major triads, 3 C minor 7ths, 4 C minor 9ths (white keys, every octave) */
void aria_factory_keymaps(aria_t *a)
{
    static const int MAJ[7] = { 0, 2, 4, 5, 7, 9, 11 }, MIN[7] = { 0, 2, 3, 5, 7, 8, 10 };
    static const int WHITE[7] = { 0, 2, 4, 5, 7, 9, 11 };
    static const struct { const int *scale; int notes; } M[4] = { { MAJ, 4 }, { MAJ, 3 }, { MIN, 4 }, { MIN, 5 } };
    for (int m = 0; m < 4; m++) {
        aria_chord_clear_map(a, m);
        for (int oct = 0; oct < 11; oct++)
            for (int d = 0; d < 7; d++) {
                int key = oct * 12 + WHITE[d];
                if (key > 127) continue;
                int8_t n[8]; int c = 0;
                for (int k = 0; k < M[m].notes; k++) {
                    int deg = d + 2 * k;
                    int note = oct * 12 + M[m].scale[deg % 7] + 12 * (deg / 7);
                    if (note <= 127) n[c++] = (int8_t)note;
                }
                aria_chord_set_key(a, m, key, n, c);
            }
    }
}
