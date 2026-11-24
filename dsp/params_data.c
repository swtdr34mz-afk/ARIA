/* SPDX-License-Identifier: MPL-2.0
 * Copyright (C) 2026 swtdr34mz (ARIA) */
/* The parameters: names, kinds and defaults (the INIT sound: a plain saw through an open low-pass), and their short
 * names for the MPC's screen. Tables only (C99 designated initialisers); the functions are in params.c. The MPC
 * build, C++ only, has them written out in order by mpc/gen/gen_params.c. */
#include "aria.h"
#include <math.h>
#include <stdio.h>

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
static const char *const C_Q[] = { "ECO 1X", "HIGH 2X", "ULTRA 4X", "MAX 16X", "RENDER 16X" };
static const char *const C_ARP[] = { "UP", "DOWN", "UP-DN", "ORDER", "RANDOM" };
static const char *const C_AOCT[] = { "1 OCT", "2 OCT", "3 OCT", "4 OCT" };
static const char *const C_LEN[] = { "1", "2", "3", "4", "5", "6", "7", "8", "9", "10", "11", "12", "13", "14", "15", "16" };
static const char *const C_LSPLIT[] = { "OFF", "-1 OCT", "-2 OCT", "-3 OCT" };
static const char *const C_SCALE[] = { "CHROMA", "MAJOR", "MINOR", "DORIAN", "PHRYGIAN", "LYDIAN", "MIXOLYD", "HARM MIN",
                                       "PENTA MAJ", "PENTA MIN", "BLUES", "ARABIC" };
static const char *const C_KEY[] = { "C", "C#", "D", "D#", "E", "F", "F#", "G", "G#", "A", "A#", "B" };
static const char *const C_FOLLOW[] = { "KEY", "ROOT", "WHITE" };
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
    [GP_TEMPO - GP_FIRST] = K("TEMPO", 60.0f / 140.0f),
    [GP_LS_SPLIT - GP_FIRST] = C("LOW SPLIT", C_LSPLIT, 0),
    [GP_LS_POINT - GP_FIRST] = K("LOW SPLIT POINT", 36.0f / 72.0f),
    [GP_LS_SCALE_L - GP_FIRST] = C("LOW SPLIT SCALE L", C_SCALE, 0),
    [GP_LS_SCALE_R - GP_FIRST] = C("LOW SPLIT SCALE R", C_SCALE, 0),
    [GP_LS_KEY - GP_FIRST] = C("LOW SPLIT KEY", C_KEY, 0),
    [GP_LS_FOLLOW - GP_FIRST] = C("LOW SPLIT FOLLOW", C_FOLLOW, LSF_ROOT),
    [GP_LS_HOLD - GP_FIRST] = S("LOW HOLD", 0),
    [GP_LS_BALANCE - GP_FIRST] = B("LOW SPLIT BALANCE", 0.5f),
    [GP_HOLD - GP_FIRST] = S("HOLD", 0),
    [GP_ARP_STEP1 + 0 - GP_FIRST] = S("ARP STEP 1", 1), [GP_ARP_STEP1 + 1 - GP_FIRST] = S("ARP STEP 2", 1),
    [GP_ARP_STEP1 + 2 - GP_FIRST] = S("ARP STEP 3", 1), [GP_ARP_STEP1 + 3 - GP_FIRST] = S("ARP STEP 4", 1),
    [GP_ARP_STEP1 + 4 - GP_FIRST] = S("ARP STEP 5", 1), [GP_ARP_STEP1 + 5 - GP_FIRST] = S("ARP STEP 6", 1),
    [GP_ARP_STEP1 + 6 - GP_FIRST] = S("ARP STEP 7", 1), [GP_ARP_STEP1 + 7 - GP_FIRST] = S("ARP STEP 8", 1),
    [GP_ARP_STEP1 + 8 - GP_FIRST] = S("ARP STEP 9", 1), [GP_ARP_STEP1 + 9 - GP_FIRST] = S("ARP STEP 10", 1),
    [GP_ARP_STEP1 + 10 - GP_FIRST] = S("ARP STEP 11", 1), [GP_ARP_STEP1 + 11 - GP_FIRST] = S("ARP STEP 12", 1),
    [GP_ARP_STEP1 + 12 - GP_FIRST] = S("ARP STEP 13", 1), [GP_ARP_STEP1 + 13 - GP_FIRST] = S("ARP STEP 14", 1),
    [GP_ARP_STEP1 + 14 - GP_FIRST] = S("ARP STEP 15", 1), [GP_ARP_STEP1 + 15 - GP_FIRST] = S("ARP STEP 16", 1),
};


/* the MPC's screen: about 12 characters (timbre B's get "B." in front) */
const char *const ARIA_TSHORT[TP_COUNT] = {
    [TP_MO_FREQ] = "MO Freq", [TP_MO_AUDIO] = "MO Audio", [TP_MO_WAVE] = "MO Wave", [TP_MO_KBTRK] = "MO KB Trk",
    [TP_MO_KBRESET] = "MO KB Rst", [TP_MO_UNI] = "MO Unipol", [TP_MO_PITCH] = "MO>Pitch", [TP_MO_PITCH_DEST] = "MO P Dest",
    [TP_MO_FILTER] = "MO>Filter", [TP_MO_FILTER_DEST] = "MO F Dest", [TP_MO_PWM] = "MO>PWM", [TP_MO_VCA] = "MO>VCA",
    [TP_MO_PAN] = "MO>Pan",
    [TP_O1_OCT] = "O1 Octave", [TP_O1_FREQ] = "O1 Freq", [TP_O1_TRISAW] = "O1 TriSaw", [TP_O1_PW] = "O1 Width",
    [TP_O1_MIX] = "O1 WaveMix",
    [TP_O2_OCT] = "O2 Octave", [TP_O2_FREQ] = "O2 Freq", [TP_O2_TRISAW] = "O2 TriSaw", [TP_O2_PW] = "O2 Width",
    [TP_O2_MIX] = "O2 WaveMix", [TP_O2_SYNC] = "O2 Sync",
    [TP_FM_AMT] = "FM Amount", [TP_FM_21] = "FM 2>1", [TP_FM_12] = "FM 1>2",
    [TP_MX_O1] = "Mix Osc1", [TP_MX_RING] = "Mix Ring", [TP_MX_O2] = "Mix Osc2", [TP_MX_MOD] = "Mix ModOsc",
    [TP_MX_NOISE] = "Mix Noise", [TP_MX_OVERLOAD] = "Overload",
    [TP_F1_CUT] = "Cutoff 1", [TP_F1_RES] = "Reso 1", [TP_F1_ENV] = "Env Amt 1", [TP_F1_HP] = "HP 1",
    [TP_F1_KB] = "KB Trk 1",
    [TP_F2_CUT] = "Cutoff 2", [TP_F2_RES] = "Reso 2", [TP_F2_ENV] = "Env Amt 2", [TP_F2_KB] = "KB Trk 2",
    [TP_F_LINK] = "Flt Link", [TP_F_ORDER] = "Flt Order",
    [TP_FE_A] = "F Attack", [TP_FE_D] = "F Decay", [TP_FE_S] = "F Sustain", [TP_FE_R] = "F Release",
    [TP_FE_CURVE] = "F Curve", [TP_FE_LOOP] = "F Loop", [TP_FE_VEL] = "F Velo",
    [TP_AE_A] = "A Attack", [TP_AE_D] = "A Decay", [TP_AE_S] = "A Sustain", [TP_AE_R] = "A Release",
    [TP_AE_CURVE] = "A Curve", [TP_AE_LOOP] = "A Loop", [TP_AE_VEL] = "A Velo",
    [TP_VCA_LEVEL] = "Level", [TP_VCA_PAN] = "Pan", [TP_VCA_SPREAD] = "Pan Spread",
    [TP_L1_RATE] = "LFO1 Rate", [TP_L1_AMT] = "LFO1 Amt", [TP_L1_WAVE] = "LFO1 Wave", [TP_L1_DEST] = "LFO1 Dest",
    [TP_L2_RATE] = "LFO2 Rate", [TP_L2_AMT] = "LFO2 Amt", [TP_L2_WAVE] = "LFO2 Wave", [TP_L2_DEST] = "LFO2 Dest",
    [TP_PL_RATE] = "Vib Rate", [TP_PL_SHAPE] = "Vib Shape", [TP_PL_AMT] = "Vib Amt",
    [TP_GLIDE] = "Glide", [TP_DETUNE] = "Detune", [TP_UNISON] = "Unison", [TP_MONO] = "Mono", [TP_BEND] = "Bend Rng",
};
const char *const ARIA_GSHORT[ARIA_NPARAM - GP_FIRST] = {
    [GP_VC_MODE - GP_FIRST] = "Voice Mode", [GP_VC_SPLIT - GP_FIRST] = "Split Pt", [GP_VC_VOICES_A - GP_FIRST] = "A Voices",
    [GP_DL_TIME_L - GP_FIRST] = "Dly Time L", [GP_DL_TIME_R - GP_FIRST] = "Dly Time R", [GP_DL_LINK - GP_FIRST] = "Dly Link",
    [GP_DL_FB - GP_FIRST] = "Dly Fdbk", [GP_DL_CHAR - GP_FIRST] = "Dly Char", [GP_DL_DIFF - GP_FIRST] = "Dly Diff",
    [GP_DL_MIX - GP_FIRST] = "Dly Mix", [GP_DL_A - GP_FIRST] = "Dly A", [GP_DL_B - GP_FIRST] = "Dly B",
    [GP_OUT_VOL - GP_FIRST] = "Volume", [GP_OUT_LOWCUT - GP_FIRST] = "Low Cut",
    [GP_CH_ON - GP_FIRST] = "Chord On", [GP_CH_KEY - GP_FIRST] = "Chord Key", [GP_CH_MAP - GP_FIRST] = "Key Map",
    [GP_DL_MOD - GP_FIRST] = "Dly Chorus", [GP_QUALITY - GP_FIRST] = "Quality",
    [GP_ARP_ON - GP_FIRST] = "Arp", [GP_ARP_MODE - GP_FIRST] = "Arp Mode", [GP_ARP_OCT - GP_FIRST] = "Arp Oct",
    [GP_ARP_DIV - GP_FIRST] = "Arp Rate", [GP_ARP_GATE - GP_FIRST] = "Arp Gate", [GP_ARP_SWING - GP_FIRST] = "Arp Swing",
    [GP_ARP_LATCH - GP_FIRST] = "Arp Latch", [GP_ARP_LEN - GP_FIRST] = "Arp Length",
    [GP_ARP_STEP1 + 0 - GP_FIRST] = "Step 1", [GP_ARP_STEP1 + 1 - GP_FIRST] = "Step 2", [GP_ARP_STEP1 + 2 - GP_FIRST] = "Step 3",
    [GP_ARP_STEP1 + 3 - GP_FIRST] = "Step 4", [GP_ARP_STEP1 + 4 - GP_FIRST] = "Step 5", [GP_ARP_STEP1 + 5 - GP_FIRST] = "Step 6",
    [GP_ARP_STEP1 + 6 - GP_FIRST] = "Step 7", [GP_ARP_STEP1 + 7 - GP_FIRST] = "Step 8", [GP_ARP_STEP1 + 8 - GP_FIRST] = "Step 9",
    [GP_ARP_STEP1 + 9 - GP_FIRST] = "Step 10", [GP_ARP_STEP1 + 10 - GP_FIRST] = "Step 11", [GP_ARP_STEP1 + 11 - GP_FIRST] = "Step 12",
    [GP_ARP_STEP1 + 12 - GP_FIRST] = "Step 13", [GP_ARP_STEP1 + 13 - GP_FIRST] = "Step 14", [GP_ARP_STEP1 + 14 - GP_FIRST] = "Step 15",
    [GP_ARP_STEP1 + 15 - GP_FIRST] = "Step 16", [GP_TEMPO - GP_FIRST] = "Tempo",
    [GP_LS_SPLIT - GP_FIRST] = "Low Split", [GP_LS_POINT - GP_FIRST] = "LS Point", [GP_LS_SCALE_L - GP_FIRST] = "LS Scale L",
    [GP_LS_SCALE_R - GP_FIRST] = "LS Scale R", [GP_LS_KEY - GP_FIRST] = "LS Key", [GP_LS_FOLLOW - GP_FIRST] = "LS Follow",
    [GP_LS_HOLD - GP_FIRST] = "Low Hold", [GP_LS_BALANCE - GP_FIRST] = "LS Balance", [GP_HOLD - GP_FIRST] = "Hold",
};
