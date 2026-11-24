/* SPDX-License-Identifier: MPL-2.0
 * Copyright (C) 2026 swtdr34mz (ARIA) */
/* ARIA: an 8-voice, bi-timbral analogue-modelling polysynth.
 *
 *   voice     OSCILLATOR 1 + 2 (triangle-saw crossfade, pulse width, a wave mix between the two, octave 16'..2',
 *             +-7 semitones, FM 2>1 and 1>2, sync 2 to 1), a MOD OSC per voice (LFO or audio rate, sine, saw,
 *             ramp, square, noise), ring mod 1x2, noise, a mixer with an asymmetric OVERLOAD,
 *             two 4-pole ladder filters (F1 low- or high-pass, F2 low-pass) in series, stereo or parallel, LINK,
 *             two ADSRs with curve, loop and velocity, a stereo VCA with pan and pan spread
 *   timbres   A and B, each with its own full sound; SINGLE (A only), SPLIT (A below the split point), STACK (both)
 *   global    two LFOs and a PITCH LFO per timbre, glide, detune, unison, mono, the chord memory (KEY mode with
 *             8 key maps), the DIFFUSION DELAY, the output (volume, low cut)
 *
 * One aria_t is one instance (one plugin): no globals. Parameters are floats in their own units (see PARAMS: a knob
 * 0..1, a choice or a switch its index). All calls from one thread (the audio thread). */
#pragma once
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#define ARIA_VOICES 8
#define ARIA_TIMBRES 2

/* ------------------------------------------------------------- the parameters --- */
/* a timbre's sound; timbre B's are the same, at TP_COUNT further on (aria_param(a, TP(B, TP_O1_FREQ)) */
enum {
    TP_MO_FREQ, TP_MO_AUDIO, TP_MO_WAVE, TP_MO_KBTRK, TP_MO_KBRESET, TP_MO_UNI,        /* MOD OSC */
    TP_MO_PITCH, TP_MO_PITCH_DEST, TP_MO_FILTER, TP_MO_FILTER_DEST, TP_MO_PWM, TP_MO_VCA, TP_MO_PAN,
    TP_O1_OCT, TP_O1_FREQ, TP_O1_TRISAW, TP_O1_PW, TP_O1_MIX,                          /* OSCILLATOR 1 */
    TP_O2_OCT, TP_O2_FREQ, TP_O2_TRISAW, TP_O2_PW, TP_O2_MIX, TP_O2_SYNC,              /* OSCILLATOR 2 */
    TP_FM_AMT, TP_FM_21, TP_FM_12,                                                     /* FM */
    TP_MX_O1, TP_MX_RING, TP_MX_O2, TP_MX_MOD, TP_MX_NOISE, TP_MX_OVERLOAD,            /* MIXER */
    TP_F1_CUT, TP_F1_RES, TP_F1_ENV, TP_F1_HP, TP_F1_KB,                               /* FILTER 1 */
    TP_F2_CUT, TP_F2_RES, TP_F2_ENV, TP_F2_KB,                                         /* FILTER 2 */
    TP_F_LINK, TP_F_ORDER,
    TP_FE_A, TP_FE_D, TP_FE_S, TP_FE_R, TP_FE_CURVE, TP_FE_LOOP, TP_FE_VEL,            /* FILTER ENVELOPE */
    TP_AE_A, TP_AE_D, TP_AE_S, TP_AE_R, TP_AE_CURVE, TP_AE_LOOP, TP_AE_VEL,            /* AMP ENVELOPE */
    TP_VCA_LEVEL, TP_VCA_PAN, TP_VCA_SPREAD,                                           /* VCA */
    TP_L1_RATE, TP_L1_AMT, TP_L1_WAVE, TP_L1_DEST,                                     /* LFO 1 */
    TP_L2_RATE, TP_L2_AMT, TP_L2_WAVE, TP_L2_DEST,                                     /* LFO 2 */
    TP_PL_RATE, TP_PL_SHAPE, TP_PL_AMT,                                                /* PITCH LFO */
    TP_GLIDE, TP_DETUNE, TP_UNISON, TP_MONO, TP_BEND,                                  /* VOICE */
    TP_COUNT
};
#define TP(timbre, p) ((timbre) * TP_COUNT + (p))
enum {
    GP_FIRST = ARIA_TIMBRES * TP_COUNT,
    GP_VC_MODE = GP_FIRST, GP_VC_SPLIT, GP_VC_VOICES_A,                                /* VOICE CONTROL */
    GP_DL_TIME_L, GP_DL_TIME_R, GP_DL_LINK, GP_DL_FB, GP_DL_CHAR, GP_DL_DIFF, GP_DL_MIX, GP_DL_A, GP_DL_B,
    GP_OUT_VOL, GP_OUT_LOWCUT,                                                         /* OUTPUT */
    GP_CH_ON, GP_CH_KEY, GP_CH_MAP,                                                    /* CHORD MEMORY */
    GP_DL_MOD, GP_QUALITY,                                             /* (added: DELAY CHORUS, the oversampling) */
    ARIA_NPARAM
};

enum { PK_KNOB, PK_BIPOLAR, PK_CHOICE, PK_SWITCH };
typedef struct {
    const char *name;                    /* "OSC 1 FREQ" (timbre B's: the same, the host adds A/B) */
    uint8_t kind;
    uint8_t count;                       /* PK_CHOICE: the choices */
    float def;                           /* the default */
    const char *const *choice;           /* PK_CHOICE: their names */
} aria_param_t;
extern const aria_param_t ARIA_TPARAMS[TP_COUNT];
extern const aria_param_t ARIA_GPARAMS[ARIA_NPARAM - GP_FIRST];
const aria_param_t *aria_param_info(int p);
float aria_param_max(int p);             /* 1 for a knob, count - 1 for a choice, 1 for a switch */

/* the choices */
enum { OCT_16, OCT_8, OCT_4, OCT_2 };
enum { MOW_SINE, MOW_SAW, MOW_RAMP, MOW_SQUARE, MOW_NOISE, NMOW };
enum { DEST1_OSC1, DEST1_OSC2, DEST1_BOTH };              /* MOD OSC PITCH: which oscillators */
enum { DESTF_F1, DESTF_F2, DESTF_BOTH };                  /* MOD OSC FILTER: which filters */
enum { KB_OFF, KB_HALF, KB_FULL };
enum { ORD_SERIAL, ORD_STEREO, ORD_PARALLEL };
enum { LW_SINE, LW_TRI, LW_SAW, LW_RAMP, LW_SQUARE, LW_SH, NLW };
enum { LD_OFF, LD_PITCH, LD_OSC1, LD_OSC2, LD_F1, LD_F2, LD_FILTERS, LD_PW, LD_AMP, LD_PAN, LD_MOFREQ, LD_FM, NLD };
enum { VC_SINGLE, VC_SPLIT, VC_STACK };
enum { Q_ECO, Q_HIGH, Q_ULTRA };                          /* the voices at 1x, 2x, 4x the rate */

/* ----------------------------------------------------------------- the engine --- */
typedef struct aria aria_t;
aria_t *aria_new(float sample_rate);
void aria_free(aria_t *a);
void aria_set_rate(aria_t *a, float sample_rate);   /* (silences) */
void aria_param(aria_t *a, int p, float v);
float aria_get(const aria_t *a, int p);
void aria_defaults(aria_t *a);

/* notes: id names the key (the same id again strikes it again), note a MIDI note, vel 0..1 */
void aria_note_on(aria_t *a, int id, int note, float vel);
void aria_note_off(aria_t *a, int id);
void aria_sustain(aria_t *a, int on);
void aria_bend(aria_t *a, float v);              /* -1..1 */
void aria_modwheel(aria_t *a, float v);          /* 0..1: more vibrato (the PITCH LFO) and LFO 1 */
void aria_pressure(aria_t *a, float v);          /* 0..1: aftertouch, opens the filters a little */
void aria_tempo(aria_t *a, float bpm);
void aria_all_off(aria_t *a);
void aria_panic(aria_t *a);
void aria_render(aria_t *a, float *l, float *r, int n);

/* the chord memory: ON, KEY and LEARN as on the panel; LEARN collects the notes played next (up to 8, they sound), a
 * second aria_chord_learn confirms them: without KEY they are the chord, with KEY the next key played takes it. */
void aria_chord_learn(aria_t *a);
void aria_chord_cancel(aria_t *a);
int aria_chord_learning(const aria_t *a);        /* 0 no, 1 collecting notes, 2 waiting for the key */
void aria_chord_clear_map(aria_t *a, int map);
/* the chord memory's state (the learned chord and the 8 key maps), for the host's state: a blob */
int aria_chord_state_size(void);
void aria_chord_save(const aria_t *a, uint8_t *out);
int aria_chord_load(aria_t *a, const uint8_t *in, int size);   /* 0: not ours */
/* a key map set from outside (factory maps): map, key, notes */
void aria_chord_set_key(aria_t *a, int map, int key, const int8_t *notes, int count);

/* for the chord memory's screen: a key's chord in a key map, the notes the last key played (with the key) and the
 * notes LEARN has collected so far; each returns the count */
int aria_chord_get_key(const aria_t *a, int map, int key, int8_t out[8]);
int aria_last_played(const aria_t *a, int8_t out[8], int *key);
int aria_chord_learned(const aria_t *a, int8_t out[8]);   /* LEARN's notes, or the chord (without KEY) */
int aria_held_notes(const aria_t *a, int8_t out[16]);    /* every note the keys held now play, low to high */

/* for the screen and the tests */
int aria_voices_active(const aria_t *a);
float aria_voice_env(const aria_t *a, int v);

#ifdef __cplusplus
}
#endif
