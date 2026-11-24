/* SPDX-License-Identifier: MPL-2.0
 * Copyright (C) 2026 swtdr34mz (ARIA) */
/* The parameters: names, kinds and defaults (the INIT sound: a plain saw through an open low-pass). */
#include "aria.h"

static const char *const C_OCT[] = { "16'", "8'", "4'", "2'" };
static const char *const C_MOW[] = { "SINE", "SAW", "RAMP", "SQUARE", "NOISE" };
static const char *const C_D1[] = { "OSC 1", "OSC 2", "BOTH" };
static const char *const C_DF[] = { "FILTER 1", "FILTER 2", "BOTH" };
static const char *const C_KB[] = { "OFF", "1:2", "1:1" };
static const char *const C_ORD[] = { "SERIAL", "STEREO", "PARALLEL" };
static const char *const C_LW[] = { "SINE", "TRI", "SAW", "RAMP", "SQUARE", "S+H" };
static const char *const C_LD[] = { "OFF", "PITCH", "OSC 1", "OSC 2", "FILTER 1", "FILTER 2", "FILTERS", "PW", "AMP",
                                    "PAN", "MOD OSC", "FM" };
static const char *const C_VC[] = { "SINGLE", "SPLIT", "STACK" };
static const char *const C_MAP[] = { "1", "2", "3", "4", "5", "6", "7", "8" };
static const char *const C_VA[] = { "1", "2", "3", "4", "5", "6", "7" };
static const char *const C_Q[] = { "ECO 1X", "HIGH 2X", "ULTRA 4X" };
static const char *const C_ARP[] = { "UP", "DOWN", "UP-DN", "ORDER", "RANDOM" };
static const char *const C_AOCT[] = { "1 OCT", "2 OCT", "3 OCT", "4 OCT" };
static const char *const C_LEN[] = { "1", "2", "3", "4", "5", "6", "7", "8", "9", "10", "11", "12", "13", "14", "15", "16" };
static const char *const C_DIV[] = { "1/4", "1/8", "1/8T", "1/16", "1/16T", "1/32" };

#define K(n, d) { n, PK_KNOB, 0, d, 0 }
#define B(n, d) { n, PK_BIPOLAR, 0, d, 0 }
#define S(n, d) { n, PK_SWITCH, 2, d, 0 }
#define C(n, c, d) { n, PK_CHOICE, sizeof c / sizeof c[0], d, c }

const aria_param_t ARIA_TPARAMS[TP_COUNT] = {
    [TP_MO_FREQ] = K("MOD OSC FREQ", 0.4f),
    [TP_MO_AUDIO] = S("MOD OSC AUDIO", 0),
    [TP_MO_WAVE] = C("MOD OSC WAVE", C_MOW, MOW_SINE),
    [TP_MO_KBTRK] = S("MOD OSC KB TRACK", 0),
    [TP_MO_KBRESET] = S("MOD OSC KB RESET", 0),
    [TP_MO_UNI] = S("MOD OSC UNIPOLAR", 0),
    [TP_MO_PITCH] = K("MOD OSC PITCH AMT", 0),
    [TP_MO_PITCH_DEST] = C("MOD OSC PITCH DEST", C_D1, DEST1_BOTH),
    [TP_MO_FILTER] = K("MOD OSC FILTER AMT", 0),
    [TP_MO_FILTER_DEST] = C("MOD OSC FILTER DEST", C_DF, DESTF_BOTH),
    [TP_MO_PWM] = K("MOD OSC PWM AMT", 0),
    [TP_MO_VCA] = K("MOD OSC VCA AMT", 0),
    [TP_MO_PAN] = K("MOD OSC PAN AMT", 0),
    [TP_O1_OCT] = C("OSC 1 OCTAVE", C_OCT, OCT_8),
    [TP_O1_FREQ] = B("OSC 1 FREQ", 0.5f),
    [TP_O1_TRISAW] = K("OSC 1 TRI-SAW", 1),
    [TP_O1_PW] = K("OSC 1 PULSE WIDTH", 0.5f),
    [TP_O1_MIX] = K("OSC 1 WAVE MIX", 0),
    [TP_O2_OCT] = C("OSC 2 OCTAVE", C_OCT, OCT_8),
    [TP_O2_FREQ] = B("OSC 2 FREQ", 0.5f),
    [TP_O2_TRISAW] = K("OSC 2 TRI-SAW", 1),
    [TP_O2_PW] = K("OSC 2 PULSE WIDTH", 0.5f),
    [TP_O2_MIX] = K("OSC 2 WAVE MIX", 0),
    [TP_O2_SYNC] = S("OSC 2 SYNC", 0),
    [TP_FM_AMT] = K("FM AMOUNT", 0),
    [TP_FM_21] = S("FM 2>1", 0),
    [TP_FM_12] = S("FM 1>2", 0),
    [TP_MX_O1] = K("MIXER OSC 1", 0.8f),
    [TP_MX_RING] = K("MIXER RING MOD", 0),
    [TP_MX_O2] = K("MIXER OSC 2", 0),
    [TP_MX_MOD] = K("MIXER MOD OSC", 0),
    [TP_MX_NOISE] = K("MIXER NOISE", 0),
    [TP_MX_OVERLOAD] = K("MIXER OVERLOAD", 0),
    [TP_F1_CUT] = K("FILTER 1 CUTOFF", 1),
    [TP_F1_RES] = K("FILTER 1 RESONANCE", 0),
    [TP_F1_ENV] = B("FILTER 1 ENV AMT", 0.5f),
    [TP_F1_HP] = S("FILTER 1 HIGH PASS", 0),
    [TP_F1_KB] = C("FILTER 1 KB TRACK", C_KB, KB_OFF),
    [TP_F2_CUT] = K("FILTER 2 CUTOFF", 1),
    [TP_F2_RES] = K("FILTER 2 RESONANCE", 0),
    [TP_F2_ENV] = B("FILTER 2 ENV AMT", 0.5f),
    [TP_F2_KB] = C("FILTER 2 KB TRACK", C_KB, KB_OFF),
    [TP_F_LINK] = S("FILTER LINK", 0),
    [TP_F_ORDER] = C("FILTER ORDER", C_ORD, ORD_SERIAL),
    [TP_FE_A] = K("FILTER ENV ATTACK", 0),
    [TP_FE_D] = K("FILTER ENV DECAY", 0.45f),
    [TP_FE_S] = K("FILTER ENV SUSTAIN", 0.5f),
    [TP_FE_R] = K("FILTER ENV RELEASE", 0.35f),
    [TP_FE_CURVE] = K("FILTER ENV CURVE", 0.5f),
    [TP_FE_LOOP] = S("FILTER ENV LOOP", 0),
    [TP_FE_VEL] = S("FILTER ENV VELOCITY", 0),
    [TP_AE_A] = K("AMP ENV ATTACK", 0),
    [TP_AE_D] = K("AMP ENV DECAY", 0.45f),
    [TP_AE_S] = K("AMP ENV SUSTAIN", 1),
    [TP_AE_R] = K("AMP ENV RELEASE", 0.3f),
    [TP_AE_CURVE] = K("AMP ENV CURVE", 0.5f),
    [TP_AE_LOOP] = S("AMP ENV LOOP", 0),
    [TP_AE_VEL] = S("AMP ENV VELOCITY", 1),
    [TP_VCA_LEVEL] = K("VCA LEVEL", 0.8f),
    [TP_VCA_PAN] = B("VCA PAN", 0.5f),
    [TP_VCA_SPREAD] = K("VCA PAN SPREAD", 0),
    [TP_L1_RATE] = K("LFO 1 RATE", 0.45f),
    [TP_L1_AMT] = K("LFO 1 AMOUNT", 0),
    [TP_L1_WAVE] = C("LFO 1 WAVE", C_LW, LW_TRI),
    [TP_L1_DEST] = C("LFO 1 DEST", C_LD, LD_PITCH),
    [TP_L2_RATE] = K("LFO 2 RATE", 0.3f),
    [TP_L2_AMT] = K("LFO 2 AMOUNT", 0),
    [TP_L2_WAVE] = C("LFO 2 WAVE", C_LW, LW_SINE),
    [TP_L2_DEST] = C("LFO 2 DEST", C_LD, LD_FILTERS),
    [TP_PL_RATE] = K("PITCH LFO RATE", 0.55f),
    [TP_PL_SHAPE] = K("PITCH LFO SHAPE", 0),
    [TP_PL_AMT] = K("PITCH LFO AMOUNT", 0),
    [TP_GLIDE] = K("GLIDE", 0),
    [TP_DETUNE] = K("DETUNE", 0.15f),
    [TP_UNISON] = S("UNISON", 0),
    [TP_MONO] = S("MONO", 0),
    [TP_BEND] = K("BEND RANGE", 2.0f / 12.0f),
};

const aria_param_t ARIA_GPARAMS[ARIA_NPARAM - GP_FIRST] = {
    [GP_VC_MODE - GP_FIRST] = C("VOICE MODE", C_VC, VC_SINGLE),
    [GP_VC_SPLIT - GP_FIRST] = K("SPLIT POINT", 60.0f / 127.0f),
    [GP_VC_VOICES_A - GP_FIRST] = C("TIMBRE A VOICES", C_VA, 3),
    [GP_DL_TIME_L - GP_FIRST] = K("DELAY TIME L", 0.62f),
    [GP_DL_TIME_R - GP_FIRST] = K("DELAY TIME R", 0.68f),
    [GP_DL_LINK - GP_FIRST] = S("DELAY LINK", 0),
    [GP_DL_FB - GP_FIRST] = K("DELAY FEEDBACK", 0.35f),
    [GP_DL_CHAR - GP_FIRST] = B("DELAY CHARACTER", 0.5f),
    [GP_DL_DIFF - GP_FIRST] = K("DELAY DIFFUSION", 0.3f),
    [GP_DL_MIX - GP_FIRST] = K("DELAY MIX", 0),
    [GP_DL_A - GP_FIRST] = S("DELAY TIMBRE A", 1),
    [GP_DL_B - GP_FIRST] = S("DELAY TIMBRE B", 1),
    [GP_OUT_VOL - GP_FIRST] = K("VOLUME", 0.75f),
    [GP_OUT_LOWCUT - GP_FIRST] = K("LOW CUT", 0),
    [GP_CH_ON - GP_FIRST] = S("CHORD ON", 0),
    [GP_CH_KEY - GP_FIRST] = S("CHORD KEY", 0),
    [GP_CH_MAP - GP_FIRST] = C("CHORD KEY MAP", C_MAP, 0),
    [GP_DL_MOD - GP_FIRST] = K("DELAY CHORUS", 0.25f),
    [GP_QUALITY - GP_FIRST] = C("QUALITY", C_Q, Q_HIGH),
    [GP_ARP_ON - GP_FIRST] = S("ARP ON", 0),
    [GP_ARP_MODE - GP_FIRST] = C("ARP MODE", C_ARP, ARP_UP),
    [GP_ARP_OCT - GP_FIRST] = C("ARP OCTAVES", C_AOCT, 0),
    [GP_ARP_DIV - GP_FIRST] = C("ARP RATE", C_DIV, DIV_16),
    [GP_ARP_GATE - GP_FIRST] = K("ARP GATE", 0.5f),
    [GP_ARP_SWING - GP_FIRST] = K("ARP SWING", 0),
    [GP_ARP_LATCH - GP_FIRST] = S("ARP LATCH", 0),
    [GP_ARP_LEN - GP_FIRST] = C("ARP PATTERN LENGTH", C_LEN, 15),
    [GP_ARP_STEP1 + 0 - GP_FIRST] = S("ARP STEP 1", 1), [GP_ARP_STEP1 + 1 - GP_FIRST] = S("ARP STEP 2", 1),
    [GP_ARP_STEP1 + 2 - GP_FIRST] = S("ARP STEP 3", 1), [GP_ARP_STEP1 + 3 - GP_FIRST] = S("ARP STEP 4", 1),
    [GP_ARP_STEP1 + 4 - GP_FIRST] = S("ARP STEP 5", 1), [GP_ARP_STEP1 + 5 - GP_FIRST] = S("ARP STEP 6", 1),
    [GP_ARP_STEP1 + 6 - GP_FIRST] = S("ARP STEP 7", 1), [GP_ARP_STEP1 + 7 - GP_FIRST] = S("ARP STEP 8", 1),
    [GP_ARP_STEP1 + 8 - GP_FIRST] = S("ARP STEP 9", 1), [GP_ARP_STEP1 + 9 - GP_FIRST] = S("ARP STEP 10", 1),
    [GP_ARP_STEP1 + 10 - GP_FIRST] = S("ARP STEP 11", 1), [GP_ARP_STEP1 + 11 - GP_FIRST] = S("ARP STEP 12", 1),
    [GP_ARP_STEP1 + 12 - GP_FIRST] = S("ARP STEP 13", 1), [GP_ARP_STEP1 + 13 - GP_FIRST] = S("ARP STEP 14", 1),
    [GP_ARP_STEP1 + 14 - GP_FIRST] = S("ARP STEP 15", 1), [GP_ARP_STEP1 + 15 - GP_FIRST] = S("ARP STEP 16", 1),
};

const aria_param_t *aria_param_info(int p)
{
    if (p < 0 || p >= ARIA_NPARAM) return 0;
    if (p < GP_FIRST) return &ARIA_TPARAMS[p % TP_COUNT];
    return &ARIA_GPARAMS[p - GP_FIRST];
}

float aria_param_max(int p)
{
    const aria_param_t *i = aria_param_info(p);
    if (!i) return 1;
    return i->kind == PK_CHOICE ? (float)(i->count - 1) : 1.0f;
}
