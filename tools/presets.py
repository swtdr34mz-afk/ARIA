#!/usr/bin/env python3
"""ARIA's factory sounds, written as data and turned into dsp/presets.c (python3 tools/presets.py).
Each sound is INIT with the parameters given changed: 'NAME': value for timbre A ('O1_OCT', 'F1_CUT', ...: TP_ names
without TP_), 'B.NAME' for timbre B, 'GP_...' for the global ones. Choices by their C names (OCT_16, ORD_STEREO, ...).
levels.json (made by tests/levels.c) evens out their loudness: each sound's VCA LEVEL, A and B."""
import json, os, sys

C = '''Category: PAD'''

P = []
def p(name, cat, **kw): P.append((name, cat, kw))

# ------------------------------------------------------------------ pads
p("INIT", "INIT")
p("WARM STRINGS", "PAD", O2_FREQ=0.506, MX_O2=0.7, F1_CUT=0.55, F1_RES=0.15, F1_ENV=0.6, F1_KB="KB_HALF",
  FE_A=0.62, FE_D=0.7, FE_S=0.6, FE_R=0.65, AE_A=0.6, AE_S=1, AE_R=0.65, VCA_SPREAD=0.7, DETUNE=0.4, PL_AMT=0.12,
  L2_AMT=0.15, L2_RATE=0.3, L2_DEST="LD_FILTERS", GP_DL_MIX=0.3, GP_DL_FB=0.45, GP_DL_DIFF=0.75, GP_DL_MOD=0.5)
p("GLASS PAD", "PAD", O2_OCT="OCT_16", O2_FREQ=0.5057, MX_O2=0.55, F_ORDER="ORD_STEREO", F1_CUT=0.52, F2_CUT=0.58,
  F1_RES=0.35, F2_RES=0.35, F1_ENV=0.68, F2_ENV=0.64, FE_A=0.62, FE_D=0.7, FE_S=0.4, FE_R=0.6, AE_A=0.58, AE_S=0.9,
  AE_R=0.62, VCA_SPREAD=0.8, DETUNE=0.45, L2_AMT=0.25, L2_RATE=0.25, L2_DEST="LD_FILTERS", GP_DL_MIX=0.38,
  GP_DL_FB=0.55, GP_DL_DIFF=0.7, GP_DL_CHAR=0.38)
p("VELVET PAD", "PAD", O1_TRISAW=0.5, O2_TRISAW=0.5, O2_FREQ=0.507, MX_O2=0.7, F_ORDER="ORD_STEREO", F1_CUT=0.5,
  F2_CUT=0.56, F1_RES=0.3, F2_RES=0.3, F1_ENV=0.62, F2_ENV=0.6, FE_A=0.7, FE_D=0.75, FE_S=0.5, FE_R=0.7, AE_A=0.68,
  AE_S=1, AE_R=0.72, MO_FREQ=0.3, MO_PAN=0.5, VCA_SPREAD=0.6, DETUNE=0.35, L2_AMT=0.2, L2_RATE=0.22,
  L2_DEST="LD_FILTERS", GP_DL_MIX=0.4, GP_DL_FB=0.6, GP_DL_DIFF=0.85, GP_DL_MOD=0.7)
p("DARK MATTER", "PAD", O1_OCT="OCT_16", O2_FREQ=0.508, MX_O2=0.6, MX_NOISE=0.3, F1_CUT=0.38, F1_RES=0.5, F2_CUT=0.45,
  F1_ENV=0.6, FE_A=0.75, FE_D=0.8, FE_S=0.5, FE_R=0.75, AE_A=0.7, AE_S=1, AE_R=0.75, MX_OVERLOAD=0.35,
  VCA_SPREAD=0.6, DETUNE=0.5, L1_AMT=0.2, L1_RATE=0.2, L1_DEST="LD_F1", GP_DL_MIX=0.35, GP_DL_FB=0.6, GP_DL_DIFF=0.8,
  GP_DL_CHAR=0.35)
p("SHIMMER HALO", "PAD", O1_OCT="OCT_4", O1_TRISAW=0, O2_OCT="OCT_2", O2_TRISAW=0, O2_FREQ=0.505, MX_O2=0.5,
  MX_RING=0.2, F1_HP=1, F1_CUT=0.45, F2_CUT=0.75, AE_A=0.7, AE_S=1, AE_R=0.75, VCA_SPREAD=0.9, DETUNE=0.3,
  PL_AMT=0.1, GP_DL_MIX=0.5, GP_DL_FB=0.75, GP_DL_DIFF=0.9, GP_DL_MOD=0.8, GP_DL_CHAR=0.65)
p("SWEEP PAD", "PAD", O2_FREQ=0.506, MX_O2=0.7, F_ORDER="ORD_STEREO", F1_CUT=0.3, F2_CUT=0.34, F1_RES=0.6,
  F2_RES=0.6, F1_ENV=0.75, F2_ENV=0.72, FE_A=0.82, FE_D=0.85, FE_S=0.7, FE_R=0.8, AE_A=0.6, AE_S=1, AE_R=0.75,
  VCA_SPREAD=0.8, DETUNE=0.4, GP_DL_MIX=0.35, GP_DL_FB=0.5, GP_DL_DIFF=0.7)
p("SLOW MOTION", "PAD", O1_MIX=0.5, O1_PW=0.35, O2_MIX=0.5, O2_PW=0.65, O2_FREQ=0.506, MX_O2=0.7, MO_FREQ=0.28,
  MO_PWM=0.6, MO_UNI=0, F1_CUT=0.48, F1_RES=0.25, F1_ENV=0.55, FE_A=0.7, FE_S=0.8, AE_A=0.72, AE_S=1, AE_R=0.78,
  VCA_SPREAD=0.75, DETUNE=0.35, GP_DL_MIX=0.35, GP_DL_FB=0.55, GP_DL_DIFF=0.8, GP_DL_MOD=0.6)
# ------------------------------------------------------------------ basses
p("OVERLOAD BASS", "BASS", MONO=1, GLIDE=0.12, O1_OCT="OCT_16", O1_MIX=1, MX_O2=0.7, O2_FREQ=0.5043, MX_OVERLOAD=0.55,
  F1_CUT=0.22, F1_RES=0.55, F1_ENV=0.86, F2_CUT=0.6, FE_D=0.38, FE_S=0.1, FE_VEL=1, AE_D=0.5, AE_S=0.8, AE_R=0.2)
p("SUB BASS", "BASS", MONO=1, O1_OCT="OCT_16", O1_TRISAW=0, MX_O1=0.9, F1_CUT=0.45, F1_ENV=0.55, FE_D=0.45, FE_S=0.4,
  AE_D=0.6, AE_S=0.9, AE_R=0.25, MX_OVERLOAD=0.15)
p("ACID LINE", "BASS", MONO=1, GLIDE=0.15, F1_CUT=0.25, F1_RES=0.78, F1_ENV=0.85, FE_D=0.35, FE_S=0, FE_VEL=1,
  MX_OVERLOAD=0.5, AE_S=0.8, AE_R=0.2, GP_DL_MIX=0.15, GP_DL_FB=0.35)
p("FAT FIVE", "BASS", UNISON=1, O1_OCT="OCT_16", O2_OCT="OCT_16", O2_MIX=0.7, O2_FREQ=0.506, MX_O2=0.7, DETUNE=0.5,
  F1_CUT=0.3, F1_RES=0.3, F1_ENV=0.7, FE_D=0.45, FE_S=0.2, MX_OVERLOAD=0.4, AE_S=0.9, AE_R=0.25)
p("PLUCK BASS", "BASS", O1_MIX=1, O1_PW=0.3, F1_CUT=0.2, F1_RES=0.45, F1_ENV=0.82, FE_D=0.32, FE_S=0, AE_D=0.56,
  AE_S=0, AE_R=0.3, MX_OVERLOAD=0.25)
p("RUBBER BASS", "BASS", MONO=1, GLIDE=0.08, O1_MIX=1, FM_21=1, FM_AMT=0.3, O2_OCT="OCT_16", O2_TRISAW=0, MX_O2=0.2,
  F1_CUT=0.3, F1_RES=0.5, F1_ENV=0.75, FE_D=0.4, FE_S=0.15, AE_S=0.85, AE_R=0.2)
p("LADDER GROWL", "BASS", MONO=1, O1_OCT="OCT_16", MX_O2=0.8, O2_FREQ=0.52, F_ORDER="ORD_SERIAL", F1_CUT=0.28,
  F1_RES=0.62, F2_CUT=0.4, F2_RES=0.4, F1_ENV=0.72, F2_ENV=0.62, FE_D=0.5, FE_S=0.3, L1_AMT=0.25, L1_RATE=0.55,
  L1_DEST="LD_FILTERS", MX_OVERLOAD=0.6, AE_S=0.9, AE_R=0.2)
# ------------------------------------------------------------------ keys
p("FM KEYS", "KEYS", O1_TRISAW=0, O2_TRISAW=0, O2_OCT="OCT_4", FM_AMT=0.55, FM_21=1, MX_O2=0.15, F1_CUT=0.55,
  F1_ENV=0.78, F1_KB="KB_FULL", FE_D=0.55, FE_S=0, FE_R=0.5, AE_D=0.72, AE_S=0, AE_R=0.6, VCA_SPREAD=0.5,
  GP_DL_MIX=0.25, GP_DL_FB=0.35, GP_DL_TIME_L=0.7, GP_DL_TIME_R=0.75)
p("VINTAGE EP", "KEYS", O1_TRISAW=0, O2_TRISAW=0, O2_OCT="OCT_4", FM_21=1, FM_AMT=0.35, MX_O2=0.05, F1_CUT=0.6,
  F1_KB="KB_FULL", F1_ENV=0.6, FE_D=0.6, FE_S=0.2, AE_D=0.75, AE_S=0.25, AE_R=0.5, AE_VEL=1, MO_FREQ=0.55,
  MO_VCA=0.25, VCA_SPREAD=0.6, PL_AMT=0.05, GP_DL_MIX=0.15, GP_DL_DIFF=0.5)
p("CLAV FUNK", "KEYS", O1_MIX=1, O1_PW=0.2, F1_HP=1, F1_CUT=0.38, F2_CUT=0.7, F2_RES=0.3, F2_ENV=0.65, FE_D=0.42,
  FE_S=0.1, AE_D=0.52, AE_S=0.05, AE_R=0.25, AE_VEL=1)
p("ORGANIC ORGAN", "KEYS", O1_MIX=0.5, O1_TRISAW=0, O2_OCT="OCT_4", O2_FREQ=1.0, O2_TRISAW=0, MX_O2=0.45,
  MO_AUDIO=1, MO_KBTRK=1, MO_FREQ=0.375, MX_MOD=0.5, F1_CUT=0.85, AE_A=0.25, AE_D=0.3, AE_S=1, AE_R=0.3,
  PL_AMT=0.25, PL_RATE=0.7, VCA_SPREAD=0.4, GP_DL_MIX=0.12, GP_DL_DIFF=0.7)
p("CHORD KEYS", "KEYS", GP_CH_ON=1, GP_CH_KEY=1, GP_CH_MAP=0, O1_TRISAW=0.6, MX_O2=0.5, O2_FREQ=0.5071, F1_CUT=0.45,
  F1_RES=0.25, F1_ENV=0.7, F1_KB="KB_HALF", FE_A=0.2, FE_D=0.6, FE_S=0.3, AE_A=0.25, AE_S=0.8, AE_R=0.5,
  VCA_SPREAD=0.7, DETUNE=0.35, GP_DL_MIX=0.3, GP_DL_FB=0.45, GP_DL_DIFF=0.5)
p("CRYSTAL BELLS", "KEYS", O1_TRISAW=0, O2_TRISAW=0, O2_OCT="OCT_2", O2_FREQ=0.5 + 2 / 14, FM_12=1, FM_21=1,
  FM_AMT=0.45, MX_O2=0.3, F1_CUT=0.8, AE_D=0.8, AE_S=0, AE_R=0.7, VCA_SPREAD=0.8, GP_DL_MIX=0.4, GP_DL_FB=0.55,
  GP_DL_DIFF=0.7)
p("RING BELLS", "KEYS", O1_TRISAW=0, O2_TRISAW=0, O2_OCT="OCT_4", O2_FREQ=0.5 + 3 / 14, MX_O1=0.3, MX_RING=0.85,
  F1_CUT=0.75, AE_D=0.7, AE_S=0, AE_R=0.65, VCA_SPREAD=0.9, GP_DL_MIX=0.4, GP_DL_FB=0.6, GP_DL_DIFF=0.8, GP_DL_CHAR=0.6)
# ------------------------------------------------------------------ leads
p("SYNC LEAD", "LEAD", MONO=1, GLIDE=0.18, O2_SYNC=1, O2_OCT="OCT_4", O2_FREQ=0.62, MX_O1=0, MX_O2=0.85,
  L1_DEST="LD_OSC2", L1_AMT=0.55, L1_RATE=0.3, L1_WAVE="LW_TRI", F1_CUT=0.7, F1_RES=0.2, PL_AMT=0.25, MX_OVERLOAD=0.3,
  GP_DL_MIX=0.3, GP_DL_FB=0.4, GP_DL_TIME_L=0.72, GP_DL_TIME_R=0.66)
p("MOOGY LEAD", "LEAD", MONO=1, GLIDE=0.12, O2_OCT="OCT_4", O2_FREQ=0.505, MX_O2=0.6, F1_CUT=0.45, F1_RES=0.35,
  F1_ENV=0.7, FE_D=0.5, FE_S=0.4, MX_OVERLOAD=0.3, PL_AMT=0.2, PL_RATE=0.55, GP_DL_MIX=0.25, GP_DL_FB=0.4)
p("SQUARE SOLO", "LEAD", MONO=1, GLIDE=0.06, O1_MIX=1, O2_MIX=1, O2_PW=0.45, O2_FREQ=0.506, MX_O2=0.6, F1_CUT=0.6,
  F1_RES=0.2, F1_ENV=0.58, PL_AMT=0.2, GP_DL_MIX=0.2, GP_DL_FB=0.35)
p("SCREAMER", "LEAD", UNISON=1, O2_FREQ=0.508, MX_O2=0.8, DETUNE=0.45, F1_CUT=0.5, F1_RES=0.7, F1_ENV=0.6,
  FE_A=0.3, FE_D=0.6, FE_S=0.5, MX_OVERLOAD=0.7, GP_DL_MIX=0.3, GP_DL_FB=0.45)
# ------------------------------------------------------------------ brass
p("POLY BRASS", "BRASS", O2_FREQ=0.507, MX_O2=0.75, F1_CUT=0.35, F1_RES=0.15, F1_ENV=0.72, F1_KB="KB_HALF",
  FE_A=0.38, FE_D=0.55, FE_S=0.45, FE_R=0.4, AE_A=0.3, AE_S=0.9, AE_R=0.35, VCA_SPREAD=0.5, DETUNE=0.4,
  PL_AMT=0.08, FE_VEL=1, GP_DL_MIX=0.2, GP_DL_DIFF=0.7)
p("BRASS STABS", "BRASS", O2_FREQ=0.508, MX_O2=0.8, F1_CUT=0.32, F1_RES=0.2, F1_ENV=0.75, FE_A=0.3, FE_D=0.45,
  FE_S=0.2, AE_A=0.22, AE_D=0.5, AE_S=0.3, AE_R=0.35, VCA_SPREAD=0.5, DETUNE=0.35, MX_OVERLOAD=0.3, FE_VEL=1,
  GP_DL_MIX=0.2, GP_DL_DIFF=0.8)
# ------------------------------------------------------------------ plucks and arps
p("PLUCK ECHO", "PLUCK", O1_MIX=1, O1_PW=0.4, F1_CUT=0.3, F1_RES=0.5, F1_ENV=0.8, FE_D=0.38, FE_S=0, AE_D=0.5,
  AE_S=0, AE_R=0.45, VCA_SPREAD=0.6, GP_DL_MIX=0.4, GP_DL_FB=0.55, GP_DL_TIME_L=0.62, GP_DL_TIME_R=0.68, GP_DL_DIFF=0.3)
p("ARP CHORDS", "ARP", GP_CH_ON=1, GP_CH_KEY=1, GP_CH_MAP=0, GP_ARP_ON=1, GP_ARP_MODE="ARP_UPDOWN", GP_ARP_OCT=1,
  GP_ARP_DIV="DIV_16", GP_ARP_GATE=0.45, GP_ARP_SWING=0.2, O1_MIX=0.6, MX_O2=0.5, O2_FREQ=0.5057, F1_CUT=0.38,
  F1_RES=0.55, F1_ENV=0.78, F1_KB="KB_HALF", FE_D=0.4, FE_S=0.05, AE_D=0.5, AE_S=0.3, AE_R=0.35, VCA_SPREAD=0.6,
  MX_OVERLOAD=0.25, GP_DL_MIX=0.32, GP_DL_FB=0.5, GP_DL_TIME_L=0.6, GP_DL_TIME_R=0.66, GP_DL_DIFF=0.45)
p("SEQ PULSE", "ARP", GP_ARP_ON=1, GP_ARP_MODE="ARP_ORDER", GP_ARP_OCT=1, GP_ARP_DIV="DIV_16", GP_ARP_GATE=0.3,
  GP_ARP_STEP1_3=0, GP_ARP_STEP1_7=0, GP_ARP_STEP1_10=0, GP_ARP_STEP1_14=0, O1_MIX=1, O1_PW=0.35, F1_CUT=0.35,
  F1_RES=0.6, F1_ENV=0.72, FE_D=0.35, FE_S=0, AE_D=0.4, AE_S=0, AE_R=0.3, VCA_SPREAD=0.7, MX_OVERLOAD=0.35,
  GP_DL_MIX=0.3, GP_DL_FB=0.45, GP_DL_TIME_L=0.58, GP_DL_TIME_R=0.64)
p("TRANCE GATE", "ARP", O2_FREQ=0.507, MX_O2=0.7, F1_CUT=0.55, F1_RES=0.3, AE_A=0.2, AE_S=1, AE_R=0.4,
  MO_FREQ=0.62, MO_WAVE="MOW_SQUARE", MO_UNI=1, MO_VCA=1, MO_KBRESET=1, VCA_SPREAD=0.6, DETUNE=0.4,
  GP_DL_MIX=0.3, GP_DL_FB=0.5, GP_DL_DIFF=0.4)
p("UP AND AWAY", "ARP", GP_ARP_ON=1, GP_ARP_MODE="ARP_UP", GP_ARP_OCT=2, GP_ARP_DIV="DIV_16T", GP_ARP_GATE=0.5,
  O1_TRISAW=0.3, O2_OCT="OCT_4", MX_O2=0.3, F1_CUT=0.5, F1_RES=0.35, F1_ENV=0.65, FE_D=0.42, FE_S=0.1, AE_D=0.5,
  AE_S=0.2, AE_R=0.4, VCA_SPREAD=0.9, GP_DL_MIX=0.35, GP_DL_FB=0.55, GP_DL_DIFF=0.6, GP_DL_MOD=0.4)
# ------------------------------------------------------------------ fx and drones
p("PULSING DRONE", "FX", MO_FREQ=0.35, MO_VCA=0.8, MO_FILTER=0.35, MO_WAVE="MOW_SQUARE", MO_UNI=1, O2_OCT="OCT_16",
  MX_O2=0.6, O2_FREQ=0.5085, F1_CUT=0.4, F1_RES=0.5, F_LINK=1, F2_CUT=0.45, F_ORDER="ORD_STEREO", AE_A=0.45,
  AE_R=0.7, VCA_SPREAD=0.9, GP_DL_MIX=0.35, GP_DL_FB=0.6, GP_DL_DIFF=0.6)
p("WIND TUNNEL", "FX", MX_O1=0, MX_NOISE=0.85, F1_HP=1, F_LINK=1, F1_CUT=0.45, F2_CUT=0.55, F1_RES=0.6, F2_RES=0.7,
  L1_AMT=0.6, L1_RATE=0.25, L1_WAVE="LW_SINE", L1_DEST="LD_FILTERS", L2_AMT=0.6, L2_RATE=0.35, L2_DEST="LD_PAN",
  AE_A=0.8, AE_S=1, AE_R=0.8, GP_DL_MIX=0.4, GP_DL_FB=0.6, GP_DL_DIFF=0.9)
p("ROBOT TALK", "FX", MO_AUDIO=1, MO_KBTRK=1, MO_FREQ=0.56, MO_WAVE="MOW_SQUARE", MO_FILTER=0.4, MX_RING=0.5,
  MX_O1=0.5, O2_FREQ=0.5 + 5 / 14, F_ORDER="ORD_STEREO", F1_CUT=0.55, F2_CUT=0.62, F1_RES=0.6, F2_RES=0.6,
  AE_S=1, AE_R=0.3, GP_DL_MIX=0.25)
p("SPACE SIREN", "FX", MONO=1, GLIDE=0.45, MX_O1=0.2, F1_CUT=0.55, F1_RES=0.95, L1_AMT=0.45, L1_RATE=0.4,
  L1_DEST="LD_F1", PL_AMT=0.3, AE_A=0.4, AE_S=1, AE_R=0.6, GP_DL_MIX=0.45, GP_DL_FB=0.7, GP_DL_DIFF=0.85,
  GP_DL_MOD=0.7)
# ------------------------------------------------------------------ splits and stacks
p("SPLIT BASS PAD", "SPLIT", GP_VC_MODE="VC_SPLIT", GP_VC_SPLIT=48 / 127, GP_VC_VOICES_A=0, MONO=1, O1_OCT="OCT_16",
  O1_MIX=1, MX_O2=0.6, F1_CUT=0.25, F1_RES=0.4, F1_ENV=0.8, FE_D=0.4, FE_S=0.15, MX_OVERLOAD=0.4,
  **{"B.O2_FREQ": 0.507, "B.MX_O2": 0.6, "B.F1_CUT": 0.5, "B.F1_ENV": 0.62, "B.FE_A": 0.55, "B.FE_D": 0.65,
     "B.AE_A": 0.5, "B.AE_R": 0.6, "B.VCA_SPREAD": 0.8, "B.DETUNE": 0.4, "B.F_ORDER": "ORD_STEREO", "B.F2_CUT": 0.56},
  GP_DL_A=0, GP_DL_MIX=0.35, GP_DL_FB=0.5, GP_DL_DIFF=0.6)
p("BASS + LEAD", "SPLIT", GP_VC_MODE="VC_SPLIT", GP_VC_SPLIT=55 / 127, GP_VC_VOICES_A=0, MONO=1, O1_OCT="OCT_16",
  F1_CUT=0.28, F1_RES=0.5, F1_ENV=0.75, FE_D=0.4, FE_S=0.2, MX_OVERLOAD=0.4,
  **{"B.MONO": 1, "B.GLIDE": 0.12, "B.O2_OCT": "OCT_4", "B.O2_FREQ": 0.505, "B.MX_O2": 0.6, "B.F1_CUT": 0.5,
     "B.F1_RES": 0.3, "B.F1_ENV": 0.65, "B.FE_D": 0.5, "B.FE_S": 0.4, "B.PL_AMT": 0.2},
  GP_DL_A=0, GP_DL_MIX=0.3, GP_DL_FB=0.45)
p("STACK STRINGS", "SPLIT", GP_VC_MODE="VC_STACK", GP_VC_VOICES_A=3, O2_FREQ=0.506, MX_O2=0.7, F1_CUT=0.5,
  F1_ENV=0.6, FE_A=0.6, FE_D=0.7, FE_S=0.6, AE_A=0.6, AE_S=1, AE_R=0.65, VCA_SPREAD=0.7, DETUNE=0.4,
  **{"B.O1_OCT": "OCT_4", "B.O1_TRISAW": 0, "B.O2_OCT": "OCT_4", "B.O2_TRISAW": 0, "B.O2_FREQ": 0.508,
     "B.MX_O2": 0.6, "B.F1_CUT": 0.7, "B.AE_A": 0.7, "B.AE_S": 1, "B.AE_R": 0.7, "B.VCA_SPREAD": 0.9, "B.PL_AMT": 0.15},
  GP_DL_MIX=0.35, GP_DL_FB=0.55, GP_DL_DIFF=0.8, GP_DL_MOD=0.5)

def cname(k):
    if k.startswith('GP_ARP_STEP1_'):
        return 'GP_ARP_STEP1 + %s' % k.split('_')[-1]
    if k.startswith('GP_'):
        return k
    if k.startswith('B.'):
        return 'TP(1, TP_%s)' % k[2:]
    return 'TP(0, TP_%s)' % k

def cval(v):
    if isinstance(v, str):
        return v
    return ('%.4ff' % v).replace('.0000f', '.0f')

levels = {}
lp = os.path.join(os.path.dirname(__file__), 'levels.json')
if os.path.exists(lp):
    levels = json.load(open(lp))

out = ['/* SPDX-License-Identifier: MPL-2.0',
       ' * Copyright (C) 2026 swtdr34mz (ARIA) */',
       '/* The factory sounds: made by tools/presets.py (edit them there), each INIT with a few parameters changed. */',
       '#include "presets.h"', '']
names = []
for i, (name, cat, kw) in enumerate(P):
    kw = dict(kw)
    if name in levels:                       # (the loudness evened out)
        kw['VCA_LEVEL'] = levels[name][0]
        if levels[name][1] is not None:
            kw['B.VCA_LEVEL'] = levels[name][1]
    ident = 'S%02d' % i
    out.append('static const aria_set_t %s[] = {   /* %s */' % (ident, name))
    items = ['{ %s, %s }' % (cname(k), cval(v)) for k, v in kw.items()]
    line = '   '
    for it in items + ['{ -1, 0 }']:
        if len(line) + len(it) + 2 > 116:
            out.append(line.rstrip())
            line = '   '
        line += ' ' + it + ','
    out.append(line.rstrip().rstrip(','))
    out.append('};')
    names.append((ident, name, cat))
out.append('')
out.append('const aria_preset_t ARIA_PRESETS[ARIA_NPRESETS] = {')
for ident, name, cat in names:
    out.append('    { "%s", "%s", %s },' % (name, cat, ident))
out.append('};')
out.append('')
src = open(os.path.join(os.path.dirname(__file__), '..', 'dsp', 'presets.c')).read()
tail = src[src.index('void aria_preset_values'):]
out.append(tail.rstrip() + '\n')
open(os.path.join(os.path.dirname(__file__), '..', 'dsp', 'presets.c'), 'w').write('\n'.join(out))
h = os.path.join(os.path.dirname(__file__), '..', 'dsp', 'presets.h')
hs = open(h).read()
import re
hs = re.sub(r'#define ARIA_NPRESETS \d+', '#define ARIA_NPRESETS %d' % len(P), hs)
hs = hs.replace('typedef struct { const char *name; const aria_set_t *set; } aria_preset_t;',
                'typedef struct { const char *name, *category; const aria_set_t *set; } aria_preset_t;')
open(h, 'w').write(hs)
print(len(P), 'sounds')
