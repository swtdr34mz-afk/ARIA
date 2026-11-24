/* SPDX-License-Identifier: MPL-2.0
 * Copyright (C) 2026 swtdr34mz (ARIA) */
/* The factory sounds: made by tools/presets.py (edit them there), each INIT with a few parameters changed. */
#include "presets.h"

static const aria_set_t S00[] = {   /* INIT */
    { TP(0, TP_VCA_LEVEL), 0.6710f }, { -1, 0 }
};
static const aria_set_t S01[] = {   /* WARM STRINGS */
    { TP(0, TP_O2_FREQ), 0.5060f }, { TP(0, TP_MX_O2), 0.7000f }, { TP(0, TP_F1_CUT), 0.5500f },
    { TP(0, TP_F1_RES), 0.1500f }, { TP(0, TP_F1_ENV), 0.6000f }, { TP(0, TP_F1_KB), KB_HALF },
    { TP(0, TP_FE_A), 0.6200f }, { TP(0, TP_FE_D), 0.7000f }, { TP(0, TP_FE_S), 0.6000f },
    { TP(0, TP_FE_R), 0.6500f }, { TP(0, TP_AE_A), 0.6000f }, { TP(0, TP_AE_S), 1.0f }, { TP(0, TP_AE_R), 0.6500f },
    { TP(0, TP_VCA_SPREAD), 0.7000f }, { TP(0, TP_DETUNE), 0.4000f }, { TP(0, TP_PL_AMT), 0.1200f },
    { TP(0, TP_L2_AMT), 0.1500f }, { TP(0, TP_L2_RATE), 0.3000f }, { TP(0, TP_L2_DEST), LD_FILTERS },
    { GP_DL_MIX, 0.3000f }, { GP_DL_FB, 0.4500f }, { GP_DL_DIFF, 0.7500f }, { GP_DL_MOD, 0.5000f },
    { TP(0, TP_VCA_LEVEL), 0.7590f }, { -1, 0 }
};
static const aria_set_t S02[] = {   /* GLASS PAD */
    { TP(0, TP_O2_OCT), OCT_16 }, { TP(0, TP_O2_FREQ), 0.5057f }, { TP(0, TP_MX_O2), 0.5500f },
    { TP(0, TP_F_ORDER), ORD_STEREO }, { TP(0, TP_F1_CUT), 0.5200f }, { TP(0, TP_F2_CUT), 0.5800f },
    { TP(0, TP_F1_RES), 0.3500f }, { TP(0, TP_F2_RES), 0.3500f }, { TP(0, TP_F1_ENV), 0.6800f },
    { TP(0, TP_F2_ENV), 0.6400f }, { TP(0, TP_FE_A), 0.6200f }, { TP(0, TP_FE_D), 0.7000f },
    { TP(0, TP_FE_S), 0.4000f }, { TP(0, TP_FE_R), 0.6000f }, { TP(0, TP_AE_A), 0.5800f },
    { TP(0, TP_AE_S), 0.9000f }, { TP(0, TP_AE_R), 0.6200f }, { TP(0, TP_VCA_SPREAD), 0.8000f },
    { TP(0, TP_DETUNE), 0.4500f }, { TP(0, TP_L2_AMT), 0.2500f }, { TP(0, TP_L2_RATE), 0.2500f },
    { TP(0, TP_L2_DEST), LD_FILTERS }, { GP_DL_MIX, 0.3800f }, { GP_DL_FB, 0.5500f }, { GP_DL_DIFF, 0.7000f },
    { GP_DL_CHAR, 0.3800f }, { TP(0, TP_VCA_LEVEL), 0.7830f }, { -1, 0 }
};
static const aria_set_t S03[] = {   /* VELVET PAD */
    { TP(0, TP_O1_TRISAW), 0.5000f }, { TP(0, TP_O2_TRISAW), 0.5000f }, { TP(0, TP_O2_FREQ), 0.5070f },
    { TP(0, TP_MX_O2), 0.7000f }, { TP(0, TP_F_ORDER), ORD_STEREO }, { TP(0, TP_F1_CUT), 0.5000f },
    { TP(0, TP_F2_CUT), 0.5600f }, { TP(0, TP_F1_RES), 0.3000f }, { TP(0, TP_F2_RES), 0.3000f },
    { TP(0, TP_F1_ENV), 0.6200f }, { TP(0, TP_F2_ENV), 0.6000f }, { TP(0, TP_FE_A), 0.7000f },
    { TP(0, TP_FE_D), 0.7500f }, { TP(0, TP_FE_S), 0.5000f }, { TP(0, TP_FE_R), 0.7000f },
    { TP(0, TP_AE_A), 0.6800f }, { TP(0, TP_AE_S), 1.0f }, { TP(0, TP_AE_R), 0.7200f },
    { TP(0, TP_MO_FREQ), 0.3000f }, { TP(0, TP_MO_PAN), 0.5000f }, { TP(0, TP_VCA_SPREAD), 0.6000f },
    { TP(0, TP_DETUNE), 0.3500f }, { TP(0, TP_L2_AMT), 0.2000f }, { TP(0, TP_L2_RATE), 0.2200f },
    { TP(0, TP_L2_DEST), LD_FILTERS }, { GP_DL_MIX, 0.4000f }, { GP_DL_FB, 0.6000f }, { GP_DL_DIFF, 0.8500f },
    { GP_DL_MOD, 0.7000f }, { TP(0, TP_VCA_LEVEL), 0.8510f }, { -1, 0 }
};
static const aria_set_t S04[] = {   /* DARK MATTER */
    { TP(0, TP_O1_OCT), OCT_16 }, { TP(0, TP_O2_FREQ), 0.5080f }, { TP(0, TP_MX_O2), 0.6000f },
    { TP(0, TP_MX_NOISE), 0.3000f }, { TP(0, TP_F1_CUT), 0.3800f }, { TP(0, TP_F1_RES), 0.5000f },
    { TP(0, TP_F2_CUT), 0.4500f }, { TP(0, TP_F1_ENV), 0.6000f }, { TP(0, TP_FE_A), 0.7500f },
    { TP(0, TP_FE_D), 0.8000f }, { TP(0, TP_FE_S), 0.5000f }, { TP(0, TP_FE_R), 0.7500f },
    { TP(0, TP_AE_A), 0.7000f }, { TP(0, TP_AE_S), 1.0f }, { TP(0, TP_AE_R), 0.7500f },
    { TP(0, TP_MX_OVERLOAD), 0.3500f }, { TP(0, TP_VCA_SPREAD), 0.6000f }, { TP(0, TP_DETUNE), 0.5000f },
    { TP(0, TP_L1_AMT), 0.2000f }, { TP(0, TP_L1_RATE), 0.2000f }, { TP(0, TP_L1_DEST), LD_F1 },
    { GP_DL_MIX, 0.3500f }, { GP_DL_FB, 0.6000f }, { GP_DL_DIFF, 0.8000f }, { GP_DL_CHAR, 0.3500f },
    { TP(0, TP_VCA_LEVEL), 0.8750f }, { -1, 0 }
};
static const aria_set_t S05[] = {   /* SHIMMER HALO */
    { TP(0, TP_O1_OCT), OCT_4 }, { TP(0, TP_O1_TRISAW), 0.0f }, { TP(0, TP_O2_OCT), OCT_2 },
    { TP(0, TP_O2_TRISAW), 0.0f }, { TP(0, TP_O2_FREQ), 0.5050f }, { TP(0, TP_MX_O2), 0.5000f },
    { TP(0, TP_MX_RING), 0.2000f }, { TP(0, TP_F1_HP), 1.0f }, { TP(0, TP_F1_CUT), 0.4500f },
    { TP(0, TP_F2_CUT), 0.7500f }, { TP(0, TP_AE_A), 0.7000f }, { TP(0, TP_AE_S), 1.0f },
    { TP(0, TP_AE_R), 0.7500f }, { TP(0, TP_VCA_SPREAD), 0.9000f }, { TP(0, TP_DETUNE), 0.3000f },
    { TP(0, TP_PL_AMT), 0.1000f }, { GP_DL_MIX, 0.5000f }, { GP_DL_FB, 0.7500f }, { GP_DL_DIFF, 0.9000f },
    { GP_DL_MOD, 0.8000f }, { GP_DL_CHAR, 0.6500f }, { TP(0, TP_VCA_LEVEL), 1.0f }, { -1, 0 }
};
static const aria_set_t S06[] = {   /* SWEEP PAD */
    { TP(0, TP_O2_FREQ), 0.5060f }, { TP(0, TP_MX_O2), 0.7000f }, { TP(0, TP_F_ORDER), ORD_STEREO },
    { TP(0, TP_F1_CUT), 0.3000f }, { TP(0, TP_F2_CUT), 0.3400f }, { TP(0, TP_F1_RES), 0.6000f },
    { TP(0, TP_F2_RES), 0.6000f }, { TP(0, TP_F1_ENV), 0.7500f }, { TP(0, TP_F2_ENV), 0.7200f },
    { TP(0, TP_FE_A), 0.8200f }, { TP(0, TP_FE_D), 0.8500f }, { TP(0, TP_FE_S), 0.7000f },
    { TP(0, TP_FE_R), 0.8000f }, { TP(0, TP_AE_A), 0.6000f }, { TP(0, TP_AE_S), 1.0f }, { TP(0, TP_AE_R), 0.7500f },
    { TP(0, TP_VCA_SPREAD), 0.8000f }, { TP(0, TP_DETUNE), 0.4000f }, { GP_DL_MIX, 0.3500f }, { GP_DL_FB, 0.5000f },
    { GP_DL_DIFF, 0.7000f }, { TP(0, TP_VCA_LEVEL), 0.6870f }, { -1, 0 }
};
static const aria_set_t S07[] = {   /* SLOW MOTION */
    { TP(0, TP_O1_MIX), 0.5000f }, { TP(0, TP_O1_PW), 0.3500f }, { TP(0, TP_O2_MIX), 0.5000f },
    { TP(0, TP_O2_PW), 0.6500f }, { TP(0, TP_O2_FREQ), 0.5060f }, { TP(0, TP_MX_O2), 0.7000f },
    { TP(0, TP_MO_FREQ), 0.2800f }, { TP(0, TP_MO_PWM), 0.6000f }, { TP(0, TP_MO_UNI), 0.0f },
    { TP(0, TP_F1_CUT), 0.4800f }, { TP(0, TP_F1_RES), 0.2500f }, { TP(0, TP_F1_ENV), 0.5500f },
    { TP(0, TP_FE_A), 0.7000f }, { TP(0, TP_FE_S), 0.8000f }, { TP(0, TP_AE_A), 0.7200f }, { TP(0, TP_AE_S), 1.0f },
    { TP(0, TP_AE_R), 0.7800f }, { TP(0, TP_VCA_SPREAD), 0.7500f }, { TP(0, TP_DETUNE), 0.3500f },
    { GP_DL_MIX, 0.3500f }, { GP_DL_FB, 0.5500f }, { GP_DL_DIFF, 0.8000f }, { GP_DL_MOD, 0.6000f },
    { TP(0, TP_VCA_LEVEL), 1.0f }, { -1, 0 }
};
static const aria_set_t S08[] = {   /* OVERLOAD BASS */
    { TP(0, TP_MONO), 1.0f }, { TP(0, TP_GLIDE), 0.1200f }, { TP(0, TP_O1_OCT), OCT_16 },
    { TP(0, TP_O1_MIX), 1.0f }, { TP(0, TP_MX_O2), 0.7000f }, { TP(0, TP_O2_FREQ), 0.5043f },
    { TP(0, TP_MX_OVERLOAD), 0.5500f }, { TP(0, TP_F1_CUT), 0.2200f }, { TP(0, TP_F1_RES), 0.5500f },
    { TP(0, TP_F1_ENV), 0.8600f }, { TP(0, TP_F2_CUT), 0.6000f }, { TP(0, TP_FE_D), 0.3800f },
    { TP(0, TP_FE_S), 0.1000f }, { TP(0, TP_FE_VEL), 1.0f }, { TP(0, TP_AE_D), 0.5000f },
    { TP(0, TP_AE_S), 0.8000f }, { TP(0, TP_AE_R), 0.2000f }, { TP(0, TP_VCA_LEVEL), 1.0f }, { -1, 0 }
};
static const aria_set_t S09[] = {   /* SUB BASS */
    { TP(0, TP_MONO), 1.0f }, { TP(0, TP_O1_OCT), OCT_16 }, { TP(0, TP_O1_TRISAW), 0.0f },
    { TP(0, TP_MX_O1), 0.9000f }, { TP(0, TP_F1_CUT), 0.4500f }, { TP(0, TP_F1_ENV), 0.5500f },
    { TP(0, TP_FE_D), 0.4500f }, { TP(0, TP_FE_S), 0.4000f }, { TP(0, TP_AE_D), 0.6000f },
    { TP(0, TP_AE_S), 0.9000f }, { TP(0, TP_AE_R), 0.2500f }, { TP(0, TP_MX_OVERLOAD), 0.1500f },
    { TP(0, TP_VCA_LEVEL), 0.9390f }, { -1, 0 }
};
static const aria_set_t S10[] = {   /* ACID LINE */
    { TP(0, TP_MONO), 1.0f }, { TP(0, TP_GLIDE), 0.1500f }, { TP(0, TP_F1_CUT), 0.2500f },
    { TP(0, TP_F1_RES), 0.7800f }, { TP(0, TP_F1_ENV), 0.8500f }, { TP(0, TP_FE_D), 0.3500f },
    { TP(0, TP_FE_S), 0.0f }, { TP(0, TP_FE_VEL), 1.0f }, { TP(0, TP_MX_OVERLOAD), 0.5000f },
    { TP(0, TP_AE_S), 0.8000f }, { TP(0, TP_AE_R), 0.2000f }, { GP_DL_MIX, 0.1500f }, { GP_DL_FB, 0.3500f },
    { TP(0, TP_VCA_LEVEL), 0.9920f }, { -1, 0 }
};
static const aria_set_t S11[] = {   /* FAT FIVE */
    { TP(0, TP_UNISON), 1.0f }, { TP(0, TP_O1_OCT), OCT_16 }, { TP(0, TP_O2_OCT), OCT_16 },
    { TP(0, TP_O2_MIX), 0.7000f }, { TP(0, TP_O2_FREQ), 0.5060f }, { TP(0, TP_MX_O2), 0.7000f },
    { TP(0, TP_DETUNE), 0.5000f }, { TP(0, TP_F1_CUT), 0.3000f }, { TP(0, TP_F1_RES), 0.3000f },
    { TP(0, TP_F1_ENV), 0.7000f }, { TP(0, TP_FE_D), 0.4500f }, { TP(0, TP_FE_S), 0.2000f },
    { TP(0, TP_MX_OVERLOAD), 0.4000f }, { TP(0, TP_AE_S), 0.9000f }, { TP(0, TP_AE_R), 0.2500f },
    { TP(0, TP_VCA_LEVEL), 0.5440f }, { -1, 0 }
};
static const aria_set_t S12[] = {   /* PLUCK BASS */
    { TP(0, TP_O1_MIX), 1.0f }, { TP(0, TP_O1_PW), 0.3000f }, { TP(0, TP_F1_CUT), 0.2000f },
    { TP(0, TP_F1_RES), 0.4500f }, { TP(0, TP_F1_ENV), 0.8200f }, { TP(0, TP_FE_D), 0.3200f },
    { TP(0, TP_FE_S), 0.0f }, { TP(0, TP_AE_D), 0.5600f }, { TP(0, TP_AE_S), 0.0f }, { TP(0, TP_AE_R), 0.3000f },
    { TP(0, TP_MX_OVERLOAD), 0.2500f }, { TP(0, TP_VCA_LEVEL), 1.0f }, { -1, 0 }
};
static const aria_set_t S13[] = {   /* RUBBER BASS */
    { TP(0, TP_MONO), 1.0f }, { TP(0, TP_GLIDE), 0.0800f }, { TP(0, TP_O1_MIX), 1.0f }, { TP(0, TP_FM_21), 1.0f },
    { TP(0, TP_FM_AMT), 0.3000f }, { TP(0, TP_O2_OCT), OCT_16 }, { TP(0, TP_O2_TRISAW), 0.0f },
    { TP(0, TP_MX_O2), 0.2000f }, { TP(0, TP_F1_CUT), 0.3000f }, { TP(0, TP_F1_RES), 0.5000f },
    { TP(0, TP_F1_ENV), 0.7500f }, { TP(0, TP_FE_D), 0.4000f }, { TP(0, TP_FE_S), 0.1500f },
    { TP(0, TP_AE_S), 0.8500f }, { TP(0, TP_AE_R), 0.2000f }, { TP(0, TP_VCA_LEVEL), 0.9010f }, { -1, 0 }
};
static const aria_set_t S14[] = {   /* LADDER GROWL */
    { TP(0, TP_MONO), 1.0f }, { TP(0, TP_O1_OCT), OCT_16 }, { TP(0, TP_MX_O2), 0.8000f },
    { TP(0, TP_O2_FREQ), 0.5200f }, { TP(0, TP_F_ORDER), ORD_SERIAL }, { TP(0, TP_F1_CUT), 0.2800f },
    { TP(0, TP_F1_RES), 0.6200f }, { TP(0, TP_F2_CUT), 0.4000f }, { TP(0, TP_F2_RES), 0.4000f },
    { TP(0, TP_F1_ENV), 0.7200f }, { TP(0, TP_F2_ENV), 0.6200f }, { TP(0, TP_FE_D), 0.5000f },
    { TP(0, TP_FE_S), 0.3000f }, { TP(0, TP_L1_AMT), 0.2500f }, { TP(0, TP_L1_RATE), 0.5500f },
    { TP(0, TP_L1_DEST), LD_FILTERS }, { TP(0, TP_MX_OVERLOAD), 0.6000f }, { TP(0, TP_AE_S), 0.9000f },
    { TP(0, TP_AE_R), 0.2000f }, { TP(0, TP_VCA_LEVEL), 0.9160f }, { -1, 0 }
};
static const aria_set_t S15[] = {   /* FM KEYS */
    { TP(0, TP_O1_TRISAW), 0.0f }, { TP(0, TP_O2_TRISAW), 0.0f }, { TP(0, TP_O2_OCT), OCT_4 },
    { TP(0, TP_FM_AMT), 0.5500f }, { TP(0, TP_FM_21), 1.0f }, { TP(0, TP_MX_O2), 0.1500f },
    { TP(0, TP_F1_CUT), 0.5500f }, { TP(0, TP_F1_ENV), 0.7800f }, { TP(0, TP_F1_KB), KB_FULL },
    { TP(0, TP_FE_D), 0.5500f }, { TP(0, TP_FE_S), 0.0f }, { TP(0, TP_FE_R), 0.5000f }, { TP(0, TP_AE_D), 0.7200f },
    { TP(0, TP_AE_S), 0.0f }, { TP(0, TP_AE_R), 0.6000f }, { TP(0, TP_VCA_SPREAD), 0.5000f },
    { GP_DL_MIX, 0.2500f }, { GP_DL_FB, 0.3500f }, { GP_DL_TIME_L, 0.7000f }, { GP_DL_TIME_R, 0.7500f },
    { TP(0, TP_VCA_LEVEL), 0.9350f }, { -1, 0 }
};
static const aria_set_t S16[] = {   /* VINTAGE EP */
    { TP(0, TP_O1_TRISAW), 0.0f }, { TP(0, TP_O2_TRISAW), 0.0f }, { TP(0, TP_O2_OCT), OCT_4 },
    { TP(0, TP_FM_21), 1.0f }, { TP(0, TP_FM_AMT), 0.3500f }, { TP(0, TP_MX_O2), 0.0500f },
    { TP(0, TP_F1_CUT), 0.6000f }, { TP(0, TP_F1_KB), KB_FULL }, { TP(0, TP_F1_ENV), 0.6000f },
    { TP(0, TP_FE_D), 0.6000f }, { TP(0, TP_FE_S), 0.2000f }, { TP(0, TP_AE_D), 0.7500f },
    { TP(0, TP_AE_S), 0.2500f }, { TP(0, TP_AE_R), 0.5000f }, { TP(0, TP_AE_VEL), 1.0f },
    { TP(0, TP_MO_FREQ), 0.5500f }, { TP(0, TP_MO_VCA), 0.2500f }, { TP(0, TP_VCA_SPREAD), 0.6000f },
    { TP(0, TP_PL_AMT), 0.0500f }, { GP_DL_MIX, 0.1500f }, { GP_DL_DIFF, 0.5000f },
    { TP(0, TP_VCA_LEVEL), 0.8380f }, { -1, 0 }
};
static const aria_set_t S17[] = {   /* CLAV FUNK */
    { TP(0, TP_O1_MIX), 1.0f }, { TP(0, TP_O1_PW), 0.2000f }, { TP(0, TP_F1_HP), 1.0f },
    { TP(0, TP_F1_CUT), 0.3800f }, { TP(0, TP_F2_CUT), 0.7000f }, { TP(0, TP_F2_RES), 0.3000f },
    { TP(0, TP_F2_ENV), 0.6500f }, { TP(0, TP_FE_D), 0.4200f }, { TP(0, TP_FE_S), 0.1000f },
    { TP(0, TP_AE_D), 0.5200f }, { TP(0, TP_AE_S), 0.0500f }, { TP(0, TP_AE_R), 0.2500f },
    { TP(0, TP_AE_VEL), 1.0f }, { TP(0, TP_VCA_LEVEL), 1.0f }, { -1, 0 }
};
static const aria_set_t S18[] = {   /* ORGANIC ORGAN */
    { TP(0, TP_O1_MIX), 0.5000f }, { TP(0, TP_O1_TRISAW), 0.0f }, { TP(0, TP_O2_OCT), OCT_4 },
    { TP(0, TP_O2_FREQ), 1.0f }, { TP(0, TP_O2_TRISAW), 0.0f }, { TP(0, TP_MX_O2), 0.4500f },
    { TP(0, TP_MO_AUDIO), 1.0f }, { TP(0, TP_MO_KBTRK), 1.0f }, { TP(0, TP_MO_FREQ), 0.3750f },
    { TP(0, TP_MX_MOD), 0.5000f }, { TP(0, TP_F1_CUT), 0.8500f }, { TP(0, TP_AE_A), 0.2500f },
    { TP(0, TP_AE_D), 0.3000f }, { TP(0, TP_AE_S), 1.0f }, { TP(0, TP_AE_R), 0.3000f },
    { TP(0, TP_PL_AMT), 0.2500f }, { TP(0, TP_PL_RATE), 0.7000f }, { TP(0, TP_VCA_SPREAD), 0.4000f },
    { GP_DL_MIX, 0.1200f }, { GP_DL_DIFF, 0.7000f }, { TP(0, TP_VCA_LEVEL), 0.6730f }, { -1, 0 }
};
static const aria_set_t S19[] = {   /* CHORD KEYS */
    { GP_CH_ON, 1.0f }, { GP_CH_KEY, 1.0f }, { GP_CH_MAP, 0.0f }, { TP(0, TP_O1_TRISAW), 0.6000f },
    { TP(0, TP_MX_O2), 0.5000f }, { TP(0, TP_O2_FREQ), 0.5071f }, { TP(0, TP_F1_CUT), 0.4500f },
    { TP(0, TP_F1_RES), 0.2500f }, { TP(0, TP_F1_ENV), 0.7000f }, { TP(0, TP_F1_KB), KB_HALF },
    { TP(0, TP_FE_A), 0.2000f }, { TP(0, TP_FE_D), 0.6000f }, { TP(0, TP_FE_S), 0.3000f },
    { TP(0, TP_AE_A), 0.2500f }, { TP(0, TP_AE_S), 0.8000f }, { TP(0, TP_AE_R), 0.5000f },
    { TP(0, TP_VCA_SPREAD), 0.7000f }, { TP(0, TP_DETUNE), 0.3500f }, { GP_DL_MIX, 0.3000f }, { GP_DL_FB, 0.4500f },
    { GP_DL_DIFF, 0.5000f }, { TP(0, TP_VCA_LEVEL), 0.9490f }, { -1, 0 }
};
static const aria_set_t S20[] = {   /* CRYSTAL BELLS */
    { TP(0, TP_O1_TRISAW), 0.0f }, { TP(0, TP_O2_TRISAW), 0.0f }, { TP(0, TP_O2_OCT), OCT_2 },
    { TP(0, TP_O2_FREQ), 0.6429f }, { TP(0, TP_FM_12), 1.0f }, { TP(0, TP_FM_21), 1.0f },
    { TP(0, TP_FM_AMT), 0.4500f }, { TP(0, TP_MX_O2), 0.3000f }, { TP(0, TP_F1_CUT), 0.8000f },
    { TP(0, TP_AE_D), 0.8000f }, { TP(0, TP_AE_S), 0.0f }, { TP(0, TP_AE_R), 0.7000f },
    { TP(0, TP_VCA_SPREAD), 0.8000f }, { GP_DL_MIX, 0.4000f }, { GP_DL_FB, 0.5500f }, { GP_DL_DIFF, 0.7000f },
    { TP(0, TP_VCA_LEVEL), 0.9370f }, { -1, 0 }
};
static const aria_set_t S21[] = {   /* RING BELLS */
    { TP(0, TP_O1_TRISAW), 0.0f }, { TP(0, TP_O2_TRISAW), 0.0f }, { TP(0, TP_O2_OCT), OCT_4 },
    { TP(0, TP_O2_FREQ), 0.7143f }, { TP(0, TP_MX_O1), 0.3000f }, { TP(0, TP_MX_RING), 0.8500f },
    { TP(0, TP_F1_CUT), 0.7500f }, { TP(0, TP_AE_D), 0.7000f }, { TP(0, TP_AE_S), 0.0f },
    { TP(0, TP_AE_R), 0.6500f }, { TP(0, TP_VCA_SPREAD), 0.9000f }, { GP_DL_MIX, 0.4000f }, { GP_DL_FB, 0.6000f },
    { GP_DL_DIFF, 0.8000f }, { GP_DL_CHAR, 0.6000f }, { TP(0, TP_VCA_LEVEL), 1.0f }, { -1, 0 }
};
static const aria_set_t S22[] = {   /* SYNC LEAD */
    { TP(0, TP_MONO), 1.0f }, { TP(0, TP_GLIDE), 0.1800f }, { TP(0, TP_O2_SYNC), 1.0f },
    { TP(0, TP_O2_OCT), OCT_4 }, { TP(0, TP_O2_FREQ), 0.6200f }, { TP(0, TP_MX_O1), 0.0f },
    { TP(0, TP_MX_O2), 0.8500f }, { TP(0, TP_L1_DEST), LD_OSC2 }, { TP(0, TP_L1_AMT), 0.5500f },
    { TP(0, TP_L1_RATE), 0.3000f }, { TP(0, TP_L1_WAVE), LW_TRI }, { TP(0, TP_F1_CUT), 0.7000f },
    { TP(0, TP_F1_RES), 0.2000f }, { TP(0, TP_PL_AMT), 0.2500f }, { TP(0, TP_MX_OVERLOAD), 0.3000f },
    { GP_DL_MIX, 0.3000f }, { GP_DL_FB, 0.4000f }, { GP_DL_TIME_L, 0.7200f }, { GP_DL_TIME_R, 0.6600f },
    { TP(0, TP_VCA_LEVEL), 1.0f }, { -1, 0 }
};
static const aria_set_t S23[] = {   /* MOOGY LEAD */
    { TP(0, TP_MONO), 1.0f }, { TP(0, TP_GLIDE), 0.1200f }, { TP(0, TP_O2_OCT), OCT_4 },
    { TP(0, TP_O2_FREQ), 0.5050f }, { TP(0, TP_MX_O2), 0.6000f }, { TP(0, TP_F1_CUT), 0.4500f },
    { TP(0, TP_F1_RES), 0.3500f }, { TP(0, TP_F1_ENV), 0.7000f }, { TP(0, TP_FE_D), 0.5000f },
    { TP(0, TP_FE_S), 0.4000f }, { TP(0, TP_MX_OVERLOAD), 0.3000f }, { TP(0, TP_PL_AMT), 0.2000f },
    { TP(0, TP_PL_RATE), 0.5500f }, { GP_DL_MIX, 0.2500f }, { GP_DL_FB, 0.4000f }, { TP(0, TP_VCA_LEVEL), 0.9900f },
    { -1, 0 }
};
static const aria_set_t S24[] = {   /* SQUARE SOLO */
    { TP(0, TP_MONO), 1.0f }, { TP(0, TP_GLIDE), 0.0600f }, { TP(0, TP_O1_MIX), 1.0f }, { TP(0, TP_O2_MIX), 1.0f },
    { TP(0, TP_O2_PW), 0.4500f }, { TP(0, TP_O2_FREQ), 0.5060f }, { TP(0, TP_MX_O2), 0.6000f },
    { TP(0, TP_F1_CUT), 0.6000f }, { TP(0, TP_F1_RES), 0.2000f }, { TP(0, TP_F1_ENV), 0.5800f },
    { TP(0, TP_PL_AMT), 0.2000f }, { GP_DL_MIX, 0.2000f }, { GP_DL_FB, 0.3500f }, { TP(0, TP_VCA_LEVEL), 0.7870f },
    { -1, 0 }
};
static const aria_set_t S25[] = {   /* SCREAMER */
    { TP(0, TP_UNISON), 1.0f }, { TP(0, TP_O2_FREQ), 0.5080f }, { TP(0, TP_MX_O2), 0.8000f },
    { TP(0, TP_DETUNE), 0.4500f }, { TP(0, TP_F1_CUT), 0.5000f }, { TP(0, TP_F1_RES), 0.7000f },
    { TP(0, TP_F1_ENV), 0.6000f }, { TP(0, TP_FE_A), 0.3000f }, { TP(0, TP_FE_D), 0.6000f },
    { TP(0, TP_FE_S), 0.5000f }, { TP(0, TP_MX_OVERLOAD), 0.7000f }, { GP_DL_MIX, 0.3000f }, { GP_DL_FB, 0.4500f },
    { TP(0, TP_VCA_LEVEL), 0.5390f }, { -1, 0 }
};
static const aria_set_t S26[] = {   /* POLY BRASS */
    { TP(0, TP_O2_FREQ), 0.5070f }, { TP(0, TP_MX_O2), 0.7500f }, { TP(0, TP_F1_CUT), 0.3500f },
    { TP(0, TP_F1_RES), 0.1500f }, { TP(0, TP_F1_ENV), 0.7200f }, { TP(0, TP_F1_KB), KB_HALF },
    { TP(0, TP_FE_A), 0.3800f }, { TP(0, TP_FE_D), 0.5500f }, { TP(0, TP_FE_S), 0.4500f },
    { TP(0, TP_FE_R), 0.4000f }, { TP(0, TP_AE_A), 0.3000f }, { TP(0, TP_AE_S), 0.9000f },
    { TP(0, TP_AE_R), 0.3500f }, { TP(0, TP_VCA_SPREAD), 0.5000f }, { TP(0, TP_DETUNE), 0.4000f },
    { TP(0, TP_PL_AMT), 0.0800f }, { TP(0, TP_FE_VEL), 1.0f }, { GP_DL_MIX, 0.2000f }, { GP_DL_DIFF, 0.7000f },
    { TP(0, TP_VCA_LEVEL), 0.7240f }, { -1, 0 }
};
static const aria_set_t S27[] = {   /* BRASS STABS */
    { TP(0, TP_O2_FREQ), 0.5080f }, { TP(0, TP_MX_O2), 0.8000f }, { TP(0, TP_F1_CUT), 0.3200f },
    { TP(0, TP_F1_RES), 0.2000f }, { TP(0, TP_F1_ENV), 0.7500f }, { TP(0, TP_FE_A), 0.3000f },
    { TP(0, TP_FE_D), 0.4500f }, { TP(0, TP_FE_S), 0.2000f }, { TP(0, TP_AE_A), 0.2200f },
    { TP(0, TP_AE_D), 0.5000f }, { TP(0, TP_AE_S), 0.3000f }, { TP(0, TP_AE_R), 0.3500f },
    { TP(0, TP_VCA_SPREAD), 0.5000f }, { TP(0, TP_DETUNE), 0.3500f }, { TP(0, TP_MX_OVERLOAD), 0.3000f },
    { TP(0, TP_FE_VEL), 1.0f }, { GP_DL_MIX, 0.2000f }, { GP_DL_DIFF, 0.8000f }, { TP(0, TP_VCA_LEVEL), 1.0f },
    { -1, 0 }
};
static const aria_set_t S28[] = {   /* PLUCK ECHO */
    { TP(0, TP_O1_MIX), 1.0f }, { TP(0, TP_O1_PW), 0.4000f }, { TP(0, TP_F1_CUT), 0.3000f },
    { TP(0, TP_F1_RES), 0.5000f }, { TP(0, TP_F1_ENV), 0.8000f }, { TP(0, TP_FE_D), 0.3800f },
    { TP(0, TP_FE_S), 0.0f }, { TP(0, TP_AE_D), 0.5000f }, { TP(0, TP_AE_S), 0.0f }, { TP(0, TP_AE_R), 0.4500f },
    { TP(0, TP_VCA_SPREAD), 0.6000f }, { GP_DL_MIX, 0.4000f }, { GP_DL_FB, 0.5500f }, { GP_DL_TIME_L, 0.6200f },
    { GP_DL_TIME_R, 0.6800f }, { GP_DL_DIFF, 0.3000f }, { TP(0, TP_VCA_LEVEL), 1.0f }, { -1, 0 }
};
static const aria_set_t S29[] = {   /* ARP CHORDS */
    { GP_CH_ON, 1.0f }, { GP_CH_KEY, 1.0f }, { GP_CH_MAP, 0.0f }, { GP_ARP_ON, 1.0f }, { GP_ARP_MODE, ARP_UPDOWN },
    { GP_ARP_OCT, 1.0f }, { GP_ARP_DIV, DIV_16 }, { GP_ARP_GATE, 0.4500f }, { GP_ARP_SWING, 0.2000f },
    { TP(0, TP_O1_MIX), 0.6000f }, { TP(0, TP_MX_O2), 0.5000f }, { TP(0, TP_O2_FREQ), 0.5057f },
    { TP(0, TP_F1_CUT), 0.3800f }, { TP(0, TP_F1_RES), 0.5500f }, { TP(0, TP_F1_ENV), 0.7800f },
    { TP(0, TP_F1_KB), KB_HALF }, { TP(0, TP_FE_D), 0.4000f }, { TP(0, TP_FE_S), 0.0500f },
    { TP(0, TP_AE_D), 0.5000f }, { TP(0, TP_AE_S), 0.3000f }, { TP(0, TP_AE_R), 0.3500f },
    { TP(0, TP_VCA_SPREAD), 0.6000f }, { TP(0, TP_MX_OVERLOAD), 0.2500f }, { GP_DL_MIX, 0.3200f },
    { GP_DL_FB, 0.5000f }, { GP_DL_TIME_L, 0.6000f }, { GP_DL_TIME_R, 0.6600f }, { GP_DL_DIFF, 0.4500f },
    { TP(0, TP_VCA_LEVEL), 1.0f }, { -1, 0 }
};
static const aria_set_t S30[] = {   /* SEQ PULSE */
    { GP_ARP_ON, 1.0f }, { GP_ARP_MODE, ARP_ORDER }, { GP_ARP_OCT, 1.0f }, { GP_ARP_DIV, DIV_16 },
    { GP_ARP_GATE, 0.3000f }, { GP_ARP_STEP1 + 3, 0.0f }, { GP_ARP_STEP1 + 7, 0.0f }, { GP_ARP_STEP1 + 10, 0.0f },
    { GP_ARP_STEP1 + 14, 0.0f }, { TP(0, TP_O1_MIX), 1.0f }, { TP(0, TP_O1_PW), 0.3500f },
    { TP(0, TP_F1_CUT), 0.3500f }, { TP(0, TP_F1_RES), 0.6000f }, { TP(0, TP_F1_ENV), 0.7200f },
    { TP(0, TP_FE_D), 0.3500f }, { TP(0, TP_FE_S), 0.0f }, { TP(0, TP_AE_D), 0.4000f }, { TP(0, TP_AE_S), 0.0f },
    { TP(0, TP_AE_R), 0.3000f }, { TP(0, TP_VCA_SPREAD), 0.7000f }, { TP(0, TP_MX_OVERLOAD), 0.3500f },
    { GP_DL_MIX, 0.3000f }, { GP_DL_FB, 0.4500f }, { GP_DL_TIME_L, 0.5800f }, { GP_DL_TIME_R, 0.6400f },
    { TP(0, TP_VCA_LEVEL), 1.0f }, { -1, 0 }
};
static const aria_set_t S31[] = {   /* TRANCE GATE */
    { TP(0, TP_O2_FREQ), 0.5070f }, { TP(0, TP_MX_O2), 0.7000f }, { TP(0, TP_F1_CUT), 0.5500f },
    { TP(0, TP_F1_RES), 0.3000f }, { TP(0, TP_AE_A), 0.2000f }, { TP(0, TP_AE_S), 1.0f },
    { TP(0, TP_AE_R), 0.4000f }, { TP(0, TP_MO_FREQ), 0.6200f }, { TP(0, TP_MO_WAVE), MOW_SQUARE },
    { TP(0, TP_MO_UNI), 1.0f }, { TP(0, TP_MO_VCA), 1.0f }, { TP(0, TP_MO_KBRESET), 1.0f },
    { TP(0, TP_VCA_SPREAD), 0.6000f }, { TP(0, TP_DETUNE), 0.4000f }, { GP_DL_MIX, 0.3000f }, { GP_DL_FB, 0.5000f },
    { GP_DL_DIFF, 0.4000f }, { TP(0, TP_VCA_LEVEL), 0.7270f }, { -1, 0 }
};
static const aria_set_t S32[] = {   /* UP AND AWAY */
    { GP_ARP_ON, 1.0f }, { GP_ARP_MODE, ARP_UP }, { GP_ARP_OCT, 2.0f }, { GP_ARP_DIV, DIV_16T },
    { GP_ARP_GATE, 0.5000f }, { TP(0, TP_O1_TRISAW), 0.3000f }, { TP(0, TP_O2_OCT), OCT_4 },
    { TP(0, TP_MX_O2), 0.3000f }, { TP(0, TP_F1_CUT), 0.5000f }, { TP(0, TP_F1_RES), 0.3500f },
    { TP(0, TP_F1_ENV), 0.6500f }, { TP(0, TP_FE_D), 0.4200f }, { TP(0, TP_FE_S), 0.1000f },
    { TP(0, TP_AE_D), 0.5000f }, { TP(0, TP_AE_S), 0.2000f }, { TP(0, TP_AE_R), 0.4000f },
    { TP(0, TP_VCA_SPREAD), 0.9000f }, { GP_DL_MIX, 0.3500f }, { GP_DL_FB, 0.5500f }, { GP_DL_DIFF, 0.6000f },
    { GP_DL_MOD, 0.4000f }, { TP(0, TP_VCA_LEVEL), 1.0f }, { -1, 0 }
};
static const aria_set_t S33[] = {   /* PULSING DRONE */
    { TP(0, TP_MO_FREQ), 0.3500f }, { TP(0, TP_MO_VCA), 0.8000f }, { TP(0, TP_MO_FILTER), 0.3500f },
    { TP(0, TP_MO_WAVE), MOW_SQUARE }, { TP(0, TP_MO_UNI), 1.0f }, { TP(0, TP_O2_OCT), OCT_16 },
    { TP(0, TP_MX_O2), 0.6000f }, { TP(0, TP_O2_FREQ), 0.5085f }, { TP(0, TP_F1_CUT), 0.4000f },
    { TP(0, TP_F1_RES), 0.5000f }, { TP(0, TP_F_LINK), 1.0f }, { TP(0, TP_F2_CUT), 0.4500f },
    { TP(0, TP_F_ORDER), ORD_STEREO }, { TP(0, TP_AE_A), 0.4500f }, { TP(0, TP_AE_R), 0.7000f },
    { TP(0, TP_VCA_SPREAD), 0.9000f }, { GP_DL_MIX, 0.3500f }, { GP_DL_FB, 0.6000f }, { GP_DL_DIFF, 0.6000f },
    { TP(0, TP_VCA_LEVEL), 0.9550f }, { -1, 0 }
};
static const aria_set_t S34[] = {   /* WIND TUNNEL */
    { TP(0, TP_MX_O1), 0.0f }, { TP(0, TP_MX_NOISE), 0.8500f }, { TP(0, TP_F1_HP), 1.0f },
    { TP(0, TP_F_LINK), 1.0f }, { TP(0, TP_F1_CUT), 0.4500f }, { TP(0, TP_F2_CUT), 0.5500f },
    { TP(0, TP_F1_RES), 0.6000f }, { TP(0, TP_F2_RES), 0.7000f }, { TP(0, TP_L1_AMT), 0.6000f },
    { TP(0, TP_L1_RATE), 0.2500f }, { TP(0, TP_L1_WAVE), LW_SINE }, { TP(0, TP_L1_DEST), LD_FILTERS },
    { TP(0, TP_L2_AMT), 0.6000f }, { TP(0, TP_L2_RATE), 0.3500f }, { TP(0, TP_L2_DEST), LD_PAN },
    { TP(0, TP_AE_A), 0.8000f }, { TP(0, TP_AE_S), 1.0f }, { TP(0, TP_AE_R), 0.8000f }, { GP_DL_MIX, 0.4000f },
    { GP_DL_FB, 0.6000f }, { GP_DL_DIFF, 0.9000f }, { TP(0, TP_VCA_LEVEL), 1.0f }, { -1, 0 }
};
static const aria_set_t S35[] = {   /* ROBOT TALK */
    { TP(0, TP_MO_AUDIO), 1.0f }, { TP(0, TP_MO_KBTRK), 1.0f }, { TP(0, TP_MO_FREQ), 0.5600f },
    { TP(0, TP_MO_WAVE), MOW_SQUARE }, { TP(0, TP_MO_FILTER), 0.4000f }, { TP(0, TP_MX_RING), 0.5000f },
    { TP(0, TP_MX_O1), 0.5000f }, { TP(0, TP_O2_FREQ), 0.8571f }, { TP(0, TP_F_ORDER), ORD_STEREO },
    { TP(0, TP_F1_CUT), 0.5500f }, { TP(0, TP_F2_CUT), 0.6200f }, { TP(0, TP_F1_RES), 0.6000f },
    { TP(0, TP_F2_RES), 0.6000f }, { TP(0, TP_AE_S), 1.0f }, { TP(0, TP_AE_R), 0.3000f }, { GP_DL_MIX, 0.2500f },
    { TP(0, TP_VCA_LEVEL), 1.0f }, { -1, 0 }
};
static const aria_set_t S36[] = {   /* SPACE SIREN */
    { TP(0, TP_MONO), 1.0f }, { TP(0, TP_GLIDE), 0.4500f }, { TP(0, TP_MX_O1), 0.2000f },
    { TP(0, TP_F1_CUT), 0.5500f }, { TP(0, TP_F1_RES), 0.9500f }, { TP(0, TP_L1_AMT), 0.4500f },
    { TP(0, TP_L1_RATE), 0.4000f }, { TP(0, TP_L1_DEST), LD_F1 }, { TP(0, TP_PL_AMT), 0.3000f },
    { TP(0, TP_AE_A), 0.4000f }, { TP(0, TP_AE_S), 1.0f }, { TP(0, TP_AE_R), 0.6000f }, { GP_DL_MIX, 0.4500f },
    { GP_DL_FB, 0.7000f }, { GP_DL_DIFF, 0.8500f }, { GP_DL_MOD, 0.7000f }, { TP(0, TP_VCA_LEVEL), 1.0f },
    { -1, 0 }
};
static const aria_set_t S37[] = {   /* SPLIT BASS PAD */
    { GP_VC_MODE, VC_SPLIT }, { GP_VC_SPLIT, 0.3780f }, { GP_VC_VOICES_A, 0.0f }, { TP(0, TP_MONO), 1.0f },
    { TP(0, TP_O1_OCT), OCT_16 }, { TP(0, TP_O1_MIX), 1.0f }, { TP(0, TP_MX_O2), 0.6000f },
    { TP(0, TP_F1_CUT), 0.2500f }, { TP(0, TP_F1_RES), 0.4000f }, { TP(0, TP_F1_ENV), 0.8000f },
    { TP(0, TP_FE_D), 0.4000f }, { TP(0, TP_FE_S), 0.1500f }, { TP(0, TP_MX_OVERLOAD), 0.4000f },
    { TP(1, TP_O2_FREQ), 0.5070f }, { TP(1, TP_MX_O2), 0.6000f }, { TP(1, TP_F1_CUT), 0.5000f },
    { TP(1, TP_F1_ENV), 0.6200f }, { TP(1, TP_FE_A), 0.5500f }, { TP(1, TP_FE_D), 0.6500f },
    { TP(1, TP_AE_A), 0.5000f }, { TP(1, TP_AE_R), 0.6000f }, { TP(1, TP_VCA_SPREAD), 0.8000f },
    { TP(1, TP_DETUNE), 0.4000f }, { TP(1, TP_F_ORDER), ORD_STEREO }, { TP(1, TP_F2_CUT), 0.5600f },
    { GP_DL_A, 0.0f }, { GP_DL_MIX, 0.3500f }, { GP_DL_FB, 0.5000f }, { GP_DL_DIFF, 0.6000f },
    { TP(0, TP_VCA_LEVEL), 0.7340f }, { TP(1, TP_VCA_LEVEL), 0.7340f }, { -1, 0 }
};
static const aria_set_t S38[] = {   /* BASS + LEAD */
    { GP_VC_MODE, VC_SPLIT }, { GP_VC_SPLIT, 0.4331f }, { GP_VC_VOICES_A, 0.0f }, { TP(0, TP_MONO), 1.0f },
    { TP(0, TP_O1_OCT), OCT_16 }, { TP(0, TP_F1_CUT), 0.2800f }, { TP(0, TP_F1_RES), 0.5000f },
    { TP(0, TP_F1_ENV), 0.7500f }, { TP(0, TP_FE_D), 0.4000f }, { TP(0, TP_FE_S), 0.2000f },
    { TP(0, TP_MX_OVERLOAD), 0.4000f }, { TP(1, TP_MONO), 1.0f }, { TP(1, TP_GLIDE), 0.1200f },
    { TP(1, TP_O2_OCT), OCT_4 }, { TP(1, TP_O2_FREQ), 0.5050f }, { TP(1, TP_MX_O2), 0.6000f },
    { TP(1, TP_F1_CUT), 0.5000f }, { TP(1, TP_F1_RES), 0.3000f }, { TP(1, TP_F1_ENV), 0.6500f },
    { TP(1, TP_FE_D), 0.5000f }, { TP(1, TP_FE_S), 0.4000f }, { TP(1, TP_PL_AMT), 0.2000f }, { GP_DL_A, 0.0f },
    { GP_DL_MIX, 0.3000f }, { GP_DL_FB, 0.4500f }, { TP(0, TP_VCA_LEVEL), 0.8700f },
    { TP(1, TP_VCA_LEVEL), 0.8700f }, { -1, 0 }
};
static const aria_set_t S39[] = {   /* STACK STRINGS */
    { GP_VC_MODE, VC_STACK }, { GP_VC_VOICES_A, 3.0f }, { TP(0, TP_O2_FREQ), 0.5060f },
    { TP(0, TP_MX_O2), 0.7000f }, { TP(0, TP_F1_CUT), 0.5000f }, { TP(0, TP_F1_ENV), 0.6000f },
    { TP(0, TP_FE_A), 0.6000f }, { TP(0, TP_FE_D), 0.7000f }, { TP(0, TP_FE_S), 0.6000f },
    { TP(0, TP_AE_A), 0.6000f }, { TP(0, TP_AE_S), 1.0f }, { TP(0, TP_AE_R), 0.6500f },
    { TP(0, TP_VCA_SPREAD), 0.7000f }, { TP(0, TP_DETUNE), 0.4000f }, { TP(1, TP_O1_OCT), OCT_4 },
    { TP(1, TP_O1_TRISAW), 0.0f }, { TP(1, TP_O2_OCT), OCT_4 }, { TP(1, TP_O2_TRISAW), 0.0f },
    { TP(1, TP_O2_FREQ), 0.5080f }, { TP(1, TP_MX_O2), 0.6000f }, { TP(1, TP_F1_CUT), 0.7000f },
    { TP(1, TP_AE_A), 0.7000f }, { TP(1, TP_AE_S), 1.0f }, { TP(1, TP_AE_R), 0.7000f },
    { TP(1, TP_VCA_SPREAD), 0.9000f }, { TP(1, TP_PL_AMT), 0.1500f }, { GP_DL_MIX, 0.3500f }, { GP_DL_FB, 0.5500f },
    { GP_DL_DIFF, 0.8000f }, { GP_DL_MOD, 0.5000f }, { TP(0, TP_VCA_LEVEL), 0.6690f },
    { TP(1, TP_VCA_LEVEL), 0.6690f }, { -1, 0 }
};
static const aria_set_t S40[] = {   /* SKYLINE BRASS */
    { TP(0, TP_O2_FREQ), 0.5045f }, { TP(0, TP_MX_O1), 0.7500f }, { TP(0, TP_MX_O2), 0.7000f },
    { TP(0, TP_F1_HP), 1.0f }, { TP(0, TP_F1_CUT), 0.2200f }, { TP(0, TP_F2_CUT), 0.3600f },
    { TP(0, TP_F2_RES), 0.1200f }, { TP(0, TP_F2_ENV), 0.7600f }, { TP(0, TP_F2_KB), KB_HALF },
    { TP(0, TP_FE_A), 0.5600f }, { TP(0, TP_FE_D), 0.7400f }, { TP(0, TP_FE_S), 0.5000f },
    { TP(0, TP_FE_R), 0.6200f }, { TP(0, TP_FE_VEL), 1.0f }, { TP(0, TP_AE_A), 0.4200f },
    { TP(0, TP_AE_D), 0.6000f }, { TP(0, TP_AE_S), 0.9500f }, { TP(0, TP_AE_R), 0.6200f },
    { TP(0, TP_DETUNE), 0.4200f }, { TP(0, TP_VCA_SPREAD), 0.5500f }, { TP(0, TP_PL_AMT), 0.0800f },
    { TP(0, TP_PL_RATE), 0.6000f }, { GP_DL_MIX, 0.3400f }, { GP_DL_FB, 0.7000f }, { GP_DL_DIFF, 0.9200f },
    { GP_DL_TIME_L, 0.8000f }, { GP_DL_TIME_R, 0.8400f }, { GP_DL_MOD, 0.5000f }, { GP_DL_CHAR, 0.4200f },
    { TP(0, TP_VCA_LEVEL), 0.9760f }, { -1, 0 }
};
static const aria_set_t S41[] = {   /* ANTHEM BRASS */
    { TP(0, TP_O2_FREQ), 0.5045f }, { TP(0, TP_MX_O1), 0.7500f }, { TP(0, TP_MX_O2), 0.7000f },
    { TP(0, TP_F1_HP), 1.0f }, { TP(0, TP_F1_CUT), 0.2200f }, { TP(0, TP_F2_CUT), 0.3200f },
    { TP(0, TP_F2_RES), 0.1200f }, { TP(0, TP_F2_ENV), 0.8000f }, { TP(0, TP_F2_KB), KB_HALF },
    { TP(0, TP_FE_A), 0.6000f }, { TP(0, TP_FE_D), 0.7400f }, { TP(0, TP_FE_S), 0.5000f },
    { TP(0, TP_FE_R), 0.6200f }, { TP(0, TP_FE_VEL), 1.0f }, { TP(0, TP_AE_A), 0.4200f },
    { TP(0, TP_AE_D), 0.6000f }, { TP(0, TP_AE_S), 0.9500f }, { TP(0, TP_AE_R), 0.6200f },
    { TP(0, TP_DETUNE), 0.4800f }, { TP(0, TP_VCA_SPREAD), 0.5500f }, { TP(0, TP_PL_AMT), 0.0800f },
    { TP(0, TP_PL_RATE), 0.6000f }, { GP_DL_MIX, 0.4400f }, { GP_DL_FB, 0.8000f }, { GP_DL_DIFF, 0.9500f },
    { GP_DL_TIME_L, 0.8600f }, { GP_DL_TIME_R, 0.9000f }, { GP_DL_MOD, 0.6000f }, { GP_DL_CHAR, 0.4000f },
    { TP(0, TP_O1_OCT), OCT_16 }, { TP(0, TP_MX_OVERLOAD), 0.2000f }, { TP(0, TP_VCA_LEVEL), 1.0f }, { -1, 0 }
};
static const aria_set_t S42[] = {   /* SLOW SWELL BRASS */
    { TP(0, TP_O2_FREQ), 0.5045f }, { TP(0, TP_MX_O1), 0.7500f }, { TP(0, TP_MX_O2), 0.7000f },
    { TP(0, TP_F1_HP), 1.0f }, { TP(0, TP_F1_CUT), 0.2200f }, { TP(0, TP_F2_CUT), 0.3600f },
    { TP(0, TP_F2_RES), 0.1200f }, { TP(0, TP_F2_ENV), 0.7600f }, { TP(0, TP_F2_KB), KB_HALF },
    { TP(0, TP_FE_A), 0.7200f }, { TP(0, TP_FE_D), 0.8000f }, { TP(0, TP_FE_S), 0.6200f },
    { TP(0, TP_FE_R), 0.6200f }, { TP(0, TP_FE_VEL), 1.0f }, { TP(0, TP_AE_A), 0.6200f },
    { TP(0, TP_AE_D), 0.6000f }, { TP(0, TP_AE_S), 0.9500f }, { TP(0, TP_AE_R), 0.7000f },
    { TP(0, TP_DETUNE), 0.4200f }, { TP(0, TP_VCA_SPREAD), 0.5500f }, { TP(0, TP_PL_AMT), 0.0800f },
    { TP(0, TP_PL_RATE), 0.6000f }, { GP_DL_MIX, 0.3400f }, { GP_DL_FB, 0.7000f }, { GP_DL_DIFF, 0.9200f },
    { GP_DL_TIME_L, 0.8000f }, { GP_DL_TIME_R, 0.8400f }, { GP_DL_MOD, 0.5000f }, { GP_DL_CHAR, 0.4200f },
    { TP(0, TP_VCA_LEVEL), 0.9870f }, { -1, 0 }
};
static const aria_set_t S43[] = {   /* HORN SECTION */
    { TP(0, TP_O2_FREQ), 0.5045f }, { TP(0, TP_MX_O1), 0.7500f }, { TP(0, TP_MX_O2), 0.7000f },
    { TP(0, TP_F1_HP), 1.0f }, { TP(0, TP_F1_CUT), 0.2200f }, { TP(0, TP_F2_CUT), 0.4200f },
    { TP(0, TP_F2_RES), 0.2000f }, { TP(0, TP_F2_ENV), 0.7600f }, { TP(0, TP_F2_KB), KB_HALF },
    { TP(0, TP_FE_A), 0.4800f }, { TP(0, TP_FE_D), 0.7400f }, { TP(0, TP_FE_S), 0.4000f },
    { TP(0, TP_FE_R), 0.6200f }, { TP(0, TP_FE_VEL), 1.0f }, { TP(0, TP_AE_A), 0.3600f },
    { TP(0, TP_AE_D), 0.6000f }, { TP(0, TP_AE_S), 0.9500f }, { TP(0, TP_AE_R), 0.6200f },
    { TP(0, TP_DETUNE), 0.4200f }, { TP(0, TP_VCA_SPREAD), 0.5500f }, { TP(0, TP_PL_AMT), 0.0600f },
    { TP(0, TP_PL_RATE), 0.6000f }, { GP_DL_MIX, 0.2400f }, { GP_DL_FB, 0.4500f }, { GP_DL_DIFF, 0.8500f },
    { GP_DL_TIME_L, 0.6000f }, { GP_DL_TIME_R, 0.6400f }, { GP_DL_MOD, 0.4000f }, { TP(0, TP_VCA_LEVEL), 0.9340f },
    { -1, 0 }
};
static const aria_set_t S44[] = {   /* MONOLITH BRASS */
    { TP(0, TP_O2_FREQ), 0.5045f }, { TP(0, TP_MX_O1), 0.7500f }, { TP(0, TP_MX_O2), 0.7000f },
    { TP(0, TP_F1_HP), 1.0f }, { TP(0, TP_F1_CUT), 0.2200f }, { TP(0, TP_F2_CUT), 0.3600f },
    { TP(0, TP_F2_RES), 0.1200f }, { TP(0, TP_F2_ENV), 0.7600f }, { TP(0, TP_F2_KB), KB_HALF },
    { TP(0, TP_FE_A), 0.5600f }, { TP(0, TP_FE_D), 0.7400f }, { TP(0, TP_FE_S), 0.5000f },
    { TP(0, TP_FE_R), 0.6200f }, { TP(0, TP_FE_VEL), 1.0f }, { TP(0, TP_AE_A), 0.4200f },
    { TP(0, TP_AE_D), 0.6000f }, { TP(0, TP_AE_S), 0.9500f }, { TP(0, TP_AE_R), 0.6200f },
    { TP(0, TP_DETUNE), 0.4200f }, { TP(0, TP_VCA_SPREAD), 0.5500f }, { TP(0, TP_PL_AMT), 0.0800f },
    { TP(0, TP_PL_RATE), 0.6000f }, { GP_DL_MIX, 0.4400f }, { GP_DL_FB, 0.8000f }, { GP_DL_DIFF, 0.9500f },
    { GP_DL_TIME_L, 0.8600f }, { GP_DL_TIME_R, 0.9000f }, { GP_DL_MOD, 0.6000f }, { GP_DL_CHAR, 0.4000f },
    { TP(1, TP_O1_OCT), OCT_4 }, { TP(1, TP_O1_MIX), 1.0f }, { TP(1, TP_O1_PW), 0.4000f },
    { TP(1, TP_MX_O1), 0.7000f }, { TP(1, TP_F1_CUT), 0.4200f }, { TP(1, TP_F1_ENV), 0.7000f },
    { TP(1, TP_F1_KB), KB_HALF }, { TP(1, TP_FE_A), 0.6500f }, { TP(1, TP_FE_D), 0.7800f },
    { TP(1, TP_FE_S), 0.4500f }, { TP(1, TP_AE_A), 0.5000f }, { TP(1, TP_AE_S), 0.9000f },
    { TP(1, TP_AE_R), 0.6600f }, { TP(1, TP_VCA_SPREAD), 0.8000f }, { TP(1, TP_PL_AMT), 0.1000f },
    { TP(1, TP_DETUNE), 0.4000f }, { GP_VC_MODE, VC_STACK }, { GP_VC_VOICES_A, 3.0f },
    { TP(0, TP_VCA_LEVEL), 0.7520f }, { TP(1, TP_VCA_LEVEL), 0.7520f }, { -1, 0 }
};
static const aria_set_t S45[] = {   /* DUST HORNS */
    { TP(0, TP_O2_FREQ), 0.5045f }, { TP(0, TP_MX_O1), 0.7500f }, { TP(0, TP_MX_O2), 0.7000f },
    { TP(0, TP_F1_HP), 1.0f }, { TP(0, TP_F1_CUT), 0.2200f }, { TP(0, TP_F2_CUT), 0.3000f },
    { TP(0, TP_F2_RES), 0.1200f }, { TP(0, TP_F2_ENV), 0.7200f }, { TP(0, TP_F2_KB), KB_HALF },
    { TP(0, TP_FE_A), 0.5600f }, { TP(0, TP_FE_D), 0.7400f }, { TP(0, TP_FE_S), 0.5000f },
    { TP(0, TP_FE_R), 0.6200f }, { TP(0, TP_FE_VEL), 1.0f }, { TP(0, TP_AE_A), 0.4200f },
    { TP(0, TP_AE_D), 0.6000f }, { TP(0, TP_AE_S), 0.9500f }, { TP(0, TP_AE_R), 0.6200f },
    { TP(0, TP_DETUNE), 0.4200f }, { TP(0, TP_VCA_SPREAD), 0.5500f }, { TP(0, TP_PL_AMT), 0.0800f },
    { TP(0, TP_PL_RATE), 0.6000f }, { GP_DL_MIX, 0.3400f }, { GP_DL_FB, 0.7000f }, { GP_DL_DIFF, 0.9200f },
    { GP_DL_TIME_L, 0.8000f }, { GP_DL_TIME_R, 0.8400f }, { GP_DL_MOD, 0.5000f }, { GP_DL_CHAR, 0.3400f },
    { TP(0, TP_MX_NOISE), 0.2000f }, { TP(0, TP_L2_AMT), 0.1500f }, { TP(0, TP_L2_RATE), 0.2000f },
    { TP(0, TP_L2_DEST), LD_F2 }, { TP(0, TP_VCA_LEVEL), 0.9180f }, { -1, 0 }
};
static const aria_set_t S46[] = {   /* FIFTH FANFARE */
    { TP(0, TP_O2_FREQ), 1.0f }, { TP(0, TP_MX_O1), 0.7500f }, { TP(0, TP_MX_O2), 0.5000f },
    { TP(0, TP_F1_HP), 1.0f }, { TP(0, TP_F1_CUT), 0.2200f }, { TP(0, TP_F2_CUT), 0.4000f },
    { TP(0, TP_F2_RES), 0.1200f }, { TP(0, TP_F2_ENV), 0.7600f }, { TP(0, TP_F2_KB), KB_HALF },
    { TP(0, TP_FE_A), 0.5000f }, { TP(0, TP_FE_D), 0.7400f }, { TP(0, TP_FE_S), 0.5000f },
    { TP(0, TP_FE_R), 0.6200f }, { TP(0, TP_FE_VEL), 1.0f }, { TP(0, TP_AE_A), 0.3800f },
    { TP(0, TP_AE_D), 0.6000f }, { TP(0, TP_AE_S), 0.9500f }, { TP(0, TP_AE_R), 0.6200f },
    { TP(0, TP_DETUNE), 0.4200f }, { TP(0, TP_VCA_SPREAD), 0.5500f }, { TP(0, TP_PL_AMT), 0.0800f },
    { TP(0, TP_PL_RATE), 0.6000f }, { GP_DL_MIX, 0.3400f }, { GP_DL_FB, 0.7000f }, { GP_DL_DIFF, 0.9200f },
    { GP_DL_TIME_L, 0.8000f }, { GP_DL_TIME_R, 0.8400f }, { GP_DL_MOD, 0.5000f }, { GP_DL_CHAR, 0.4200f },
    { TP(0, TP_VCA_LEVEL), 1.0f }, { -1, 0 }
};
static const aria_set_t S47[] = {   /* NEON RAIN */
    { GP_DL_MIX, 0.4400f }, { GP_DL_FB, 0.8000f }, { GP_DL_DIFF, 0.9500f }, { GP_DL_TIME_L, 0.8600f },
    { GP_DL_TIME_R, 0.9000f }, { GP_DL_MOD, 0.6000f }, { GP_DL_CHAR, 0.4000f }, { TP(0, TP_O2_MIX), 0.6000f },
    { TP(0, TP_O2_PW), 0.4000f }, { TP(0, TP_O2_FREQ), 0.5060f }, { TP(0, TP_MX_O2), 0.6500f },
    { TP(0, TP_F1_CUT), 0.4000f }, { TP(0, TP_F1_RES), 0.3200f }, { TP(0, TP_F1_ENV), 0.6200f },
    { TP(0, TP_F1_KB), KB_HALF }, { TP(0, TP_FE_A), 0.7200f }, { TP(0, TP_FE_D), 0.8200f },
    { TP(0, TP_FE_S), 0.4500f }, { TP(0, TP_FE_R), 0.7800f }, { TP(0, TP_AE_A), 0.6600f }, { TP(0, TP_AE_S), 1.0f },
    { TP(0, TP_AE_R), 0.8000f }, { TP(0, TP_L2_AMT), 0.2200f }, { TP(0, TP_L2_RATE), 0.1800f },
    { TP(0, TP_L2_DEST), LD_FILTERS }, { TP(0, TP_MO_FREQ), 0.2500f }, { TP(0, TP_MO_PWM), 0.3500f },
    { TP(0, TP_DETUNE), 0.4500f }, { TP(0, TP_VCA_SPREAD), 0.8000f }, { TP(0, TP_PL_AMT), 0.0600f },
    { TP(0, TP_VCA_LEVEL), 0.8320f }, { -1, 0 }
};
static const aria_set_t S48[] = {   /* CATHEDRAL */
    { GP_DL_MIX, 0.4400f }, { GP_DL_FB, 0.8000f }, { GP_DL_DIFF, 0.9500f }, { GP_DL_TIME_L, 0.8600f },
    { GP_DL_TIME_R, 0.9000f }, { GP_DL_MOD, 0.6000f }, { GP_DL_CHAR, 0.4000f }, { TP(0, TP_O1_OCT), OCT_16 },
    { TP(0, TP_O2_FREQ), 0.5060f }, { TP(0, TP_MX_O2), 0.6000f }, { TP(0, TP_F1_CUT), 0.3600f },
    { TP(0, TP_F1_ENV), 0.6000f }, { TP(0, TP_FE_A), 0.7800f }, { TP(0, TP_FE_D), 0.8500f },
    { TP(0, TP_FE_S), 0.6000f }, { TP(0, TP_AE_A), 0.7200f }, { TP(0, TP_AE_S), 1.0f }, { TP(0, TP_AE_R), 0.8200f },
    { TP(0, TP_DETUNE), 0.4000f }, { TP(0, TP_VCA_SPREAD), 0.7000f }, { TP(1, TP_O1_OCT), OCT_4 },
    { TP(1, TP_O1_TRISAW), 0.0f }, { TP(1, TP_O2_OCT), OCT_2 }, { TP(1, TP_O2_TRISAW), 0.2000f },
    { TP(1, TP_O2_FREQ), 0.5035f }, { TP(1, TP_MX_O2), 0.4000f }, { TP(1, TP_F1_CUT), 0.6200f },
    { TP(1, TP_AE_A), 0.8000f }, { TP(1, TP_AE_S), 1.0f }, { TP(1, TP_AE_R), 0.8500f },
    { TP(1, TP_VCA_SPREAD), 0.9000f }, { TP(1, TP_PL_AMT), 0.0800f }, { GP_VC_MODE, VC_STACK },
    { GP_VC_VOICES_A, 3.0f }, { TP(0, TP_VCA_LEVEL), 0.7800f }, { TP(1, TP_VCA_LEVEL), 0.7800f }, { -1, 0 }
};
static const aria_set_t S49[] = {   /* ORBITAL DAWN */
    { GP_DL_MIX, 0.4400f }, { GP_DL_FB, 0.8000f }, { GP_DL_DIFF, 0.9500f }, { GP_DL_TIME_L, 0.8600f },
    { GP_DL_TIME_R, 0.9000f }, { GP_DL_MOD, 0.6000f }, { GP_DL_CHAR, 0.4000f }, { TP(0, TP_O2_FREQ), 0.5070f },
    { TP(0, TP_MX_O2), 0.7000f }, { TP(0, TP_F_ORDER), ORD_STEREO }, { TP(0, TP_F1_CUT), 0.2600f },
    { TP(0, TP_F2_CUT), 0.3000f }, { TP(0, TP_F1_RES), 0.5500f }, { TP(0, TP_F2_RES), 0.5500f },
    { TP(0, TP_F1_ENV), 0.8000f }, { TP(0, TP_F2_ENV), 0.7800f }, { TP(0, TP_FE_A), 0.8800f },
    { TP(0, TP_FE_D), 0.9000f }, { TP(0, TP_FE_S), 0.6000f }, { TP(0, TP_FE_R), 0.8500f },
    { TP(0, TP_AE_A), 0.7000f }, { TP(0, TP_AE_S), 1.0f }, { TP(0, TP_AE_R), 0.8500f },
    { TP(0, TP_DETUNE), 0.4500f }, { TP(0, TP_VCA_SPREAD), 0.9000f }, { TP(0, TP_VCA_LEVEL), 0.7310f }, { -1, 0 }
};
static const aria_set_t S50[] = {   /* SILK HORIZON */
    { GP_DL_MIX, 0.3400f }, { GP_DL_FB, 0.7000f }, { GP_DL_DIFF, 0.9200f }, { GP_DL_TIME_L, 0.8000f },
    { GP_DL_TIME_R, 0.8400f }, { GP_DL_MOD, 0.5000f }, { GP_DL_CHAR, 0.4200f }, { TP(0, TP_O1_MIX), 1.0f },
    { TP(0, TP_O1_PW), 0.4500f }, { TP(0, TP_O2_MIX), 1.0f }, { TP(0, TP_O2_PW), 0.5500f },
    { TP(0, TP_O2_FREQ), 0.5055f }, { TP(0, TP_MX_O2), 0.7000f }, { TP(0, TP_MO_FREQ), 0.2200f },
    { TP(0, TP_MO_PWM), 0.5500f }, { TP(0, TP_F1_CUT), 0.5000f }, { TP(0, TP_F1_RES), 0.1500f },
    { TP(0, TP_F1_ENV), 0.5800f }, { TP(0, TP_FE_A), 0.6000f }, { TP(0, TP_FE_S), 0.7000f },
    { TP(0, TP_AE_A), 0.6000f }, { TP(0, TP_AE_S), 1.0f }, { TP(0, TP_AE_R), 0.7500f },
    { TP(0, TP_DETUNE), 0.3500f }, { TP(0, TP_VCA_SPREAD), 0.8000f }, { TP(0, TP_PL_AMT), 0.0600f },
    { TP(0, TP_VCA_LEVEL), 0.6660f }, { -1, 0 }
};
static const aria_set_t S51[] = {   /* DEEP FIELD */
    { GP_DL_MIX, 0.4400f }, { GP_DL_FB, 0.8000f }, { GP_DL_DIFF, 0.9500f }, { GP_DL_TIME_L, 0.8600f },
    { GP_DL_TIME_R, 0.9000f }, { GP_DL_MOD, 0.6000f }, { GP_DL_CHAR, 0.3500f }, { TP(0, TP_O1_OCT), OCT_16 },
    { TP(0, TP_O2_TRISAW), 0.0f }, { TP(0, TP_MX_O2), 0.6000f }, { TP(0, TP_MX_NOISE), 0.1800f },
    { TP(0, TP_F1_CUT), 0.3000f }, { TP(0, TP_F1_RES), 0.4000f }, { TP(0, TP_F1_ENV), 0.6400f },
    { TP(0, TP_FE_A), 0.8000f }, { TP(0, TP_FE_D), 0.8500f }, { TP(0, TP_FE_S), 0.5500f },
    { TP(0, TP_FE_R), 0.8000f }, { TP(0, TP_AE_A), 0.7800f }, { TP(0, TP_AE_S), 1.0f }, { TP(0, TP_AE_R), 0.8500f },
    { TP(0, TP_MX_OVERLOAD), 0.2000f }, { TP(0, TP_L1_WAVE), LW_SH }, { TP(0, TP_L1_RATE), 0.3500f },
    { TP(0, TP_L1_AMT), 0.1200f }, { TP(0, TP_L1_DEST), LD_F1 }, { TP(0, TP_DETUNE), 0.4500f },
    { TP(0, TP_VCA_SPREAD), 0.7000f }, { TP(0, TP_VCA_LEVEL), 0.7920f }, { -1, 0 }
};
static const aria_set_t S52[] = {   /* GOLDEN HOUR */
    { GP_DL_MIX, 0.3400f }, { GP_DL_FB, 0.7000f }, { GP_DL_DIFF, 0.9200f }, { GP_DL_TIME_L, 0.8000f },
    { GP_DL_TIME_R, 0.8400f }, { GP_DL_MOD, 0.5000f }, { GP_DL_CHAR, 0.4200f }, { TP(0, TP_O1_TRISAW), 0.7000f },
    { TP(0, TP_O2_TRISAW), 0.7000f }, { TP(0, TP_O2_FREQ), 0.5070f }, { TP(0, TP_MX_O2), 0.7000f },
    { TP(0, TP_F1_CUT), 0.4400f }, { TP(0, TP_F1_RES), 0.2000f }, { TP(0, TP_F1_ENV), 0.6000f },
    { TP(0, TP_F1_KB), KB_HALF }, { TP(0, TP_FE_A), 0.5500f }, { TP(0, TP_FE_D), 0.7500f },
    { TP(0, TP_FE_S), 0.6000f }, { TP(0, TP_AE_A), 0.5500f }, { TP(0, TP_AE_S), 1.0f }, { TP(0, TP_AE_R), 0.7200f },
    { TP(0, TP_DETUNE), 0.3800f }, { TP(0, TP_VCA_SPREAD), 0.7000f }, { TP(0, TP_PL_AMT), 0.0700f },
    { TP(0, TP_VCA_LEVEL), 0.7720f }, { -1, 0 }
};
static const aria_set_t S53[] = {   /* AURORA */
    { GP_DL_MIX, 0.4400f }, { GP_DL_FB, 0.8000f }, { GP_DL_DIFF, 0.9500f }, { GP_DL_TIME_L, 0.8600f },
    { GP_DL_TIME_R, 0.9000f }, { GP_DL_MOD, 0.6000f }, { GP_DL_CHAR, 0.4000f }, { TP(0, TP_O2_OCT), OCT_4 },
    { TP(0, TP_O2_FREQ), 0.5050f }, { TP(0, TP_MX_O2), 0.5000f }, { TP(0, TP_F_ORDER), ORD_STEREO },
    { TP(0, TP_F1_CUT), 0.4500f }, { TP(0, TP_F2_CUT), 0.5000f }, { TP(0, TP_F1_RES), 0.4000f },
    { TP(0, TP_F2_RES), 0.4000f }, { TP(0, TP_L1_WAVE), LW_TRI }, { TP(0, TP_L1_RATE), 0.2000f },
    { TP(0, TP_L1_AMT), 0.3000f }, { TP(0, TP_L1_DEST), LD_F1 }, { TP(0, TP_L2_RATE), 0.2600f },
    { TP(0, TP_L2_AMT), 0.3000f }, { TP(0, TP_L2_DEST), LD_F2 }, { TP(0, TP_AE_A), 0.7000f },
    { TP(0, TP_AE_S), 1.0f }, { TP(0, TP_AE_R), 0.8500f }, { TP(0, TP_DETUNE), 0.4000f },
    { TP(0, TP_VCA_SPREAD), 1.0f }, { TP(0, TP_VCA_LEVEL), 0.8340f }, { -1, 0 }
};
static const aria_set_t S54[] = {   /* MEMORY OF SNOW */
    { GP_DL_MIX, 0.3400f }, { GP_DL_FB, 0.7000f }, { GP_DL_DIFF, 0.9200f }, { GP_DL_TIME_L, 0.8000f },
    { GP_DL_TIME_R, 0.8400f }, { GP_DL_MOD, 0.5000f }, { GP_DL_CHAR, 0.6200f }, { TP(0, TP_O1_TRISAW), 0.1500f },
    { TP(0, TP_O2_OCT), OCT_4 }, { TP(0, TP_O2_TRISAW), 0.1000f }, { TP(0, TP_O2_FREQ), 0.5035f },
    { TP(0, TP_MX_O2), 0.7000f }, { TP(0, TP_MX_NOISE), 0.1000f }, { TP(0, TP_F1_HP), 1.0f },
    { TP(0, TP_F1_CUT), 0.3000f }, { TP(0, TP_F2_CUT), 0.6200f }, { TP(0, TP_F2_ENV), 0.5500f },
    { TP(0, TP_FE_A), 0.6000f }, { TP(0, TP_AE_A), 0.6600f }, { TP(0, TP_AE_S), 1.0f }, { TP(0, TP_AE_R), 0.8200f },
    { TP(0, TP_VCA_SPREAD), 0.8500f }, { TP(0, TP_PL_AMT), 0.0500f }, { TP(0, TP_MX_O1), 1.0f },
    { TP(0, TP_VCA_LEVEL), 0.9810f }, { -1, 0 }
};
static const aria_set_t S55[] = {   /* VAST HORIZON */
    { GP_DL_MIX, 0.5000f }, { GP_DL_FB, 0.8000f }, { GP_DL_DIFF, 0.9500f }, { GP_DL_TIME_L, 0.8600f },
    { GP_DL_TIME_R, 0.9000f }, { GP_DL_MOD, 0.6000f }, { GP_DL_CHAR, 0.4000f }, { TP(0, TP_O1_OCT), OCT_16 },
    { TP(0, TP_O2_FREQ), 0.5080f }, { TP(0, TP_MX_O2), 0.7500f }, { TP(0, TP_F1_CUT), 0.4800f },
    { TP(0, TP_F1_ENV), 0.5500f }, { TP(0, TP_FE_A), 0.7000f }, { TP(0, TP_AE_A), 0.7500f },
    { TP(0, TP_AE_S), 1.0f }, { TP(0, TP_AE_R), 0.9000f }, { TP(0, TP_DETUNE), 0.6000f },
    { TP(0, TP_VCA_SPREAD), 0.9000f }, { TP(0, TP_VCA_LEVEL), 0.8410f }, { -1, 0 }
};
static const aria_set_t S56[] = {   /* CS STRINGS */
    { TP(0, TP_O2_FREQ), 0.5055f }, { TP(0, TP_MX_O1), 0.7000f }, { TP(0, TP_MX_O2), 0.7000f },
    { TP(0, TP_F1_HP), 1.0f }, { TP(0, TP_F1_CUT), 0.2500f }, { TP(0, TP_F2_CUT), 0.5000f },
    { TP(0, TP_F2_RES), 0.1000f }, { TP(0, TP_F2_ENV), 0.6000f }, { TP(0, TP_F2_KB), KB_HALF },
    { TP(0, TP_FE_A), 0.6200f }, { TP(0, TP_FE_D), 0.7800f }, { TP(0, TP_FE_S), 0.7000f },
    { TP(0, TP_FE_R), 0.7000f }, { TP(0, TP_AE_A), 0.6000f }, { TP(0, TP_AE_D), 0.7000f }, { TP(0, TP_AE_S), 1.0f },
    { TP(0, TP_AE_R), 0.7200f }, { TP(0, TP_DETUNE), 0.5000f }, { TP(0, TP_VCA_SPREAD), 0.7500f },
    { TP(0, TP_PL_AMT), 0.1000f }, { TP(0, TP_PL_RATE), 0.6200f }, { GP_DL_MIX, 0.3400f }, { GP_DL_FB, 0.7000f },
    { GP_DL_DIFF, 0.9200f }, { GP_DL_TIME_L, 0.8000f }, { GP_DL_TIME_R, 0.8400f }, { GP_DL_MOD, 0.7500f },
    { GP_DL_CHAR, 0.4200f }, { TP(0, TP_VCA_LEVEL), 0.9700f }, { -1, 0 }
};
static const aria_set_t S57[] = {   /* ELEGY STRINGS */
    { TP(0, TP_O2_FREQ), 0.5055f }, { TP(0, TP_MX_O1), 0.7000f }, { TP(0, TP_MX_O2), 0.7000f },
    { TP(0, TP_F1_HP), 1.0f }, { TP(0, TP_F1_CUT), 0.2500f }, { TP(0, TP_F2_CUT), 0.4200f },
    { TP(0, TP_F2_RES), 0.1000f }, { TP(0, TP_F2_ENV), 0.6000f }, { TP(0, TP_F2_KB), KB_HALF },
    { TP(0, TP_FE_A), 0.7000f }, { TP(0, TP_FE_D), 0.7800f }, { TP(0, TP_FE_S), 0.7000f },
    { TP(0, TP_FE_R), 0.7000f }, { TP(0, TP_AE_A), 0.7000f }, { TP(0, TP_AE_D), 0.7000f }, { TP(0, TP_AE_S), 1.0f },
    { TP(0, TP_AE_R), 0.8200f }, { TP(0, TP_DETUNE), 0.5000f }, { TP(0, TP_VCA_SPREAD), 0.7500f },
    { TP(0, TP_PL_AMT), 0.1000f }, { TP(0, TP_PL_RATE), 0.6200f }, { GP_DL_MIX, 0.4400f }, { GP_DL_FB, 0.8000f },
    { GP_DL_DIFF, 0.9500f }, { GP_DL_TIME_L, 0.8600f }, { GP_DL_TIME_R, 0.9000f }, { GP_DL_MOD, 0.6000f },
    { GP_DL_CHAR, 0.4000f }, { TP(0, TP_VCA_LEVEL), 0.9920f }, { -1, 0 }
};
static const aria_set_t S58[] = {   /* VIOLA ENSEMBLE */
    { TP(0, TP_O2_FREQ), 0.5055f }, { TP(0, TP_MX_O1), 0.9000f }, { TP(0, TP_MX_O2), 0.9000f },
    { TP(0, TP_F1_HP), 1.0f }, { TP(0, TP_F1_CUT), 0.2500f }, { TP(0, TP_F2_CUT), 0.4600f },
    { TP(0, TP_F2_RES), 0.1000f }, { TP(0, TP_F2_ENV), 0.6000f }, { TP(0, TP_F2_KB), KB_HALF },
    { TP(0, TP_FE_A), 0.6200f }, { TP(0, TP_FE_D), 0.7800f }, { TP(0, TP_FE_S), 0.7000f },
    { TP(0, TP_FE_R), 0.7000f }, { TP(0, TP_AE_A), 0.6000f }, { TP(0, TP_AE_D), 0.7000f }, { TP(0, TP_AE_S), 1.0f },
    { TP(0, TP_AE_R), 0.7200f }, { TP(0, TP_DETUNE), 0.5000f }, { TP(0, TP_VCA_SPREAD), 0.7500f },
    { TP(0, TP_PL_AMT), 0.1000f }, { TP(0, TP_PL_RATE), 0.6200f }, { GP_DL_MIX, 0.4000f }, { GP_DL_FB, 0.3500f },
    { GP_DL_DIFF, 0.6000f }, { GP_DL_TIME_L, 0.2400f }, { GP_DL_TIME_R, 0.2900f }, { GP_DL_MOD, 1.0f },
    { TP(0, TP_O1_MIX), 0.5000f }, { TP(0, TP_O1_PW), 0.3000f }, { TP(0, TP_VCA_LEVEL), 0.9860f }, { -1, 0 }
};
static const aria_set_t S59[] = {   /* TREMOLO STRINGS */
    { TP(0, TP_O2_FREQ), 0.5055f }, { TP(0, TP_MX_O1), 0.7000f }, { TP(0, TP_MX_O2), 0.7000f },
    { TP(0, TP_F1_HP), 1.0f }, { TP(0, TP_F1_CUT), 0.2500f }, { TP(0, TP_F2_CUT), 0.5000f },
    { TP(0, TP_F2_RES), 0.1000f }, { TP(0, TP_F2_ENV), 0.6000f }, { TP(0, TP_F2_KB), KB_HALF },
    { TP(0, TP_FE_A), 0.6200f }, { TP(0, TP_FE_D), 0.7800f }, { TP(0, TP_FE_S), 0.7000f },
    { TP(0, TP_FE_R), 0.7000f }, { TP(0, TP_AE_A), 0.4500f }, { TP(0, TP_AE_D), 0.7000f }, { TP(0, TP_AE_S), 1.0f },
    { TP(0, TP_AE_R), 0.7200f }, { TP(0, TP_DETUNE), 0.5000f }, { TP(0, TP_VCA_SPREAD), 0.7500f },
    { TP(0, TP_PL_AMT), 0.1000f }, { TP(0, TP_PL_RATE), 0.6200f }, { GP_DL_MIX, 0.3400f }, { GP_DL_FB, 0.7000f },
    { GP_DL_DIFF, 0.9200f }, { GP_DL_TIME_L, 0.8000f }, { GP_DL_TIME_R, 0.8400f }, { GP_DL_MOD, 0.5000f },
    { GP_DL_CHAR, 0.4200f }, { TP(0, TP_MO_FREQ), 0.5200f }, { TP(0, TP_MO_VCA), 0.4500f },
    { TP(0, TP_MO_UNI), 1.0f }, { TP(0, TP_VCA_LEVEL), 1.0f }, { -1, 0 }
};
static const aria_set_t S60[] = {   /* HIGH STRINGS */
    { TP(0, TP_O2_FREQ), 0.5055f }, { TP(0, TP_MX_O1), 0.7000f }, { TP(0, TP_MX_O2), 0.7000f },
    { TP(0, TP_F1_HP), 1.0f }, { TP(0, TP_F1_CUT), 0.2500f }, { TP(0, TP_F2_CUT), 0.5800f },
    { TP(0, TP_F2_RES), 0.1000f }, { TP(0, TP_F2_ENV), 0.6000f }, { TP(0, TP_F2_KB), KB_HALF },
    { TP(0, TP_FE_A), 0.6200f }, { TP(0, TP_FE_D), 0.7800f }, { TP(0, TP_FE_S), 0.7000f },
    { TP(0, TP_FE_R), 0.7000f }, { TP(0, TP_AE_A), 0.6000f }, { TP(0, TP_AE_D), 0.7000f }, { TP(0, TP_AE_S), 1.0f },
    { TP(0, TP_AE_R), 0.7200f }, { TP(0, TP_DETUNE), 0.5000f }, { TP(0, TP_VCA_SPREAD), 0.7500f },
    { TP(0, TP_PL_AMT), 0.1200f }, { TP(0, TP_PL_RATE), 0.6200f }, { GP_DL_MIX, 0.3400f }, { GP_DL_FB, 0.7000f },
    { GP_DL_DIFF, 0.9200f }, { GP_DL_TIME_L, 0.8000f }, { GP_DL_TIME_R, 0.8400f }, { GP_DL_MOD, 0.5000f },
    { GP_DL_CHAR, 0.4200f }, { TP(0, TP_O1_OCT), OCT_4 }, { TP(0, TP_O2_OCT), OCT_4 },
    { TP(0, TP_VCA_LEVEL), 0.9240f }, { -1, 0 }
};
static const aria_set_t S61[] = {   /* STRING MACHINE */
    { GP_DL_MIX, 0.4000f }, { GP_DL_FB, 0.2000f }, { GP_DL_DIFF, 0.3000f }, { GP_DL_TIME_L, 0.2400f },
    { GP_DL_TIME_R, 0.2900f }, { GP_DL_MOD, 1.0f }, { TP(0, TP_O2_OCT), OCT_4 }, { TP(0, TP_O2_FREQ), 0.5030f },
    { TP(0, TP_MX_O2), 0.6000f }, { TP(0, TP_F1_CUT), 0.6200f }, { TP(0, TP_AE_A), 0.5000f },
    { TP(0, TP_AE_S), 1.0f }, { TP(0, TP_AE_R), 0.7000f }, { TP(0, TP_DETUNE), 0.3000f },
    { TP(0, TP_VCA_SPREAD), 0.6000f }, { TP(0, TP_VCA_LEVEL), 0.7880f }, { -1, 0 }
};
static const aria_set_t S62[] = {   /* VICTORY LEAD */
    { GP_DL_MIX, 0.3400f }, { GP_DL_FB, 0.7000f }, { GP_DL_DIFF, 0.9200f }, { GP_DL_TIME_L, 0.8000f },
    { GP_DL_TIME_R, 0.8400f }, { GP_DL_MOD, 0.5000f }, { GP_DL_CHAR, 0.4200f }, { TP(0, TP_MONO), 1.0f },
    { TP(0, TP_GLIDE), 0.2200f }, { TP(0, TP_O2_FREQ), 0.5060f }, { TP(0, TP_O2_MIX), 0.4000f },
    { TP(0, TP_MX_O2), 0.9000f }, { TP(0, TP_F1_CUT), 0.4800f }, { TP(0, TP_F1_RES), 0.2200f },
    { TP(0, TP_F1_ENV), 0.6600f }, { TP(0, TP_FE_A), 0.4500f }, { TP(0, TP_FE_D), 0.7000f },
    { TP(0, TP_FE_S), 0.6000f }, { TP(0, TP_FE_VEL), 1.0f }, { TP(0, TP_AE_A), 0.3000f },
    { TP(0, TP_AE_R), 0.6200f }, { TP(0, TP_PL_AMT), 0.1600f }, { TP(0, TP_PL_RATE), 0.6200f },
    { TP(0, TP_DETUNE), 0.3000f }, { TP(0, TP_MX_O1), 1.0f }, { TP(0, TP_MX_OVERLOAD), 0.3000f },
    { TP(0, TP_VCA_LEVEL), 1.0f }, { -1, 0 }
};
static const aria_set_t S63[] = {   /* RIBBON CRY */
    { GP_DL_MIX, 0.4400f }, { GP_DL_FB, 0.8000f }, { GP_DL_DIFF, 0.9500f }, { GP_DL_TIME_L, 0.8600f },
    { GP_DL_TIME_R, 0.9000f }, { GP_DL_MOD, 0.6000f }, { GP_DL_CHAR, 0.4000f }, { TP(0, TP_MONO), 1.0f },
    { TP(0, TP_GLIDE), 0.4200f }, { TP(0, TP_O2_SYNC), 1.0f }, { TP(0, TP_O2_FREQ), 0.7200f },
    { TP(0, TP_MX_O1), 0.5000f }, { TP(0, TP_MX_O2), 1.0f }, { TP(0, TP_F1_CUT), 0.5500f },
    { TP(0, TP_F1_RES), 0.3500f }, { TP(0, TP_L1_WAVE), LW_SINE }, { TP(0, TP_L1_RATE), 0.2500f },
    { TP(0, TP_L1_AMT), 0.2500f }, { TP(0, TP_L1_DEST), LD_OSC2 }, { TP(0, TP_PL_AMT), 0.2000f },
    { TP(0, TP_AE_A), 0.3000f }, { TP(0, TP_AE_R), 0.6000f }, { TP(0, TP_MX_OVERLOAD), 0.3000f },
    { TP(0, TP_VCA_LEVEL), 0.8330f }, { -1, 0 }
};
static const aria_set_t S64[] = {   /* NIGHT SAX */
    { GP_DL_MIX, 0.3000f }, { GP_DL_FB, 0.4500f }, { GP_DL_DIFF, 0.8500f }, { GP_DL_TIME_L, 0.6000f },
    { GP_DL_TIME_R, 0.6400f }, { GP_DL_MOD, 0.4000f }, { TP(0, TP_MONO), 1.0f }, { TP(0, TP_GLIDE), 0.1000f },
    { TP(0, TP_O1_MIX), 0.7500f }, { TP(0, TP_O1_PW), 0.3500f }, { TP(0, TP_O2_MIX), 1.0f },
    { TP(0, TP_O2_PW), 0.4200f }, { TP(0, TP_O2_FREQ), 0.5040f }, { TP(0, TP_MX_O2), 0.4000f },
    { TP(0, TP_MX_NOISE), 0.0800f }, { TP(0, TP_F1_CUT), 0.3800f }, { TP(0, TP_F1_RES), 0.2500f },
    { TP(0, TP_F1_ENV), 0.7000f }, { TP(0, TP_FE_A), 0.4000f }, { TP(0, TP_FE_D), 0.6200f },
    { TP(0, TP_FE_S), 0.5500f }, { TP(0, TP_FE_VEL), 1.0f }, { TP(0, TP_AE_A), 0.3600f },
    { TP(0, TP_AE_R), 0.5500f }, { TP(0, TP_PL_AMT), 0.1400f }, { TP(0, TP_PL_RATE), 0.6000f },
    { TP(0, TP_MX_OVERLOAD), 0.2500f }, { TP(0, TP_VCA_LEVEL), 0.9440f }, { -1, 0 }
};
static const aria_set_t S65[] = {   /* GLASS FLUTE */
    { GP_DL_MIX, 0.3400f }, { GP_DL_FB, 0.7000f }, { GP_DL_DIFF, 0.9200f }, { GP_DL_TIME_L, 0.8000f },
    { GP_DL_TIME_R, 0.8400f }, { GP_DL_MOD, 0.5000f }, { GP_DL_CHAR, 0.4200f }, { TP(0, TP_MONO), 1.0f },
    { TP(0, TP_GLIDE), 0.0800f }, { TP(0, TP_O1_TRISAW), 0.0f }, { TP(0, TP_MX_NOISE), 0.2500f },
    { TP(0, TP_F1_CUT), 0.5500f }, { TP(0, TP_F1_RES), 0.2000f }, { TP(0, TP_F1_ENV), 0.5600f },
    { TP(0, TP_FE_A), 0.4200f }, { TP(0, TP_AE_A), 0.4400f }, { TP(0, TP_AE_R), 0.5000f },
    { TP(0, TP_PL_AMT), 0.1400f }, { TP(0, TP_PL_RATE), 0.6400f }, { TP(0, TP_MX_O1), 1.0f },
    { TP(0, TP_MX_OVERLOAD), 0.2500f }, { TP(0, TP_VCA_LEVEL), 0.9960f }, { -1, 0 }
};
static const aria_set_t S66[] = {   /* ICE OBOE */
    { GP_DL_MIX, 0.2400f }, { GP_DL_FB, 0.4500f }, { GP_DL_DIFF, 0.8500f }, { GP_DL_TIME_L, 0.6000f },
    { GP_DL_TIME_R, 0.6400f }, { GP_DL_MOD, 0.4000f }, { TP(0, TP_MONO), 1.0f }, { TP(0, TP_GLIDE), 0.0600f },
    { TP(0, TP_O1_MIX), 1.0f }, { TP(0, TP_O1_PW), 0.2400f }, { TP(0, TP_F1_HP), 1.0f },
    { TP(0, TP_F1_CUT), 0.3600f }, { TP(0, TP_F1_RES), 0.3500f }, { TP(0, TP_F2_CUT), 0.7000f },
    { TP(0, TP_F2_RES), 0.3000f }, { TP(0, TP_F2_ENV), 0.5600f }, { TP(0, TP_AE_A), 0.3800f },
    { TP(0, TP_AE_R), 0.5000f }, { TP(0, TP_PL_AMT), 0.1400f }, { TP(0, TP_MX_O1), 1.0f },
    { TP(0, TP_MX_OVERLOAD), 0.5000f }, { TP(0, TP_O2_MIX), 1.0f }, { TP(0, TP_O2_PW), 0.2000f },
    { TP(0, TP_O2_FREQ), 0.5040f }, { TP(0, TP_MX_O2), 0.8000f }, { TP(0, TP_VCA_LEVEL), 1.0f }, { -1, 0 }
};
static const aria_set_t S67[] = {   /* DISTANT TRUMPET */
    { TP(0, TP_O2_FREQ), 0.5045f }, { TP(0, TP_MX_O1), 1.0f }, { TP(0, TP_MX_O2), 0.9000f },
    { TP(0, TP_F1_HP), 1.0f }, { TP(0, TP_F1_CUT), 0.2200f }, { TP(0, TP_F2_CUT), 0.4400f },
    { TP(0, TP_F2_RES), 0.1200f }, { TP(0, TP_F2_ENV), 0.7600f }, { TP(0, TP_F2_KB), KB_HALF },
    { TP(0, TP_FE_A), 0.5000f }, { TP(0, TP_FE_D), 0.7400f }, { TP(0, TP_FE_S), 0.5000f },
    { TP(0, TP_FE_R), 0.6200f }, { TP(0, TP_FE_VEL), 1.0f }, { TP(0, TP_AE_A), 0.3600f },
    { TP(0, TP_AE_D), 0.6000f }, { TP(0, TP_AE_S), 0.9500f }, { TP(0, TP_AE_R), 0.6200f },
    { TP(0, TP_DETUNE), 0.4200f }, { TP(0, TP_VCA_SPREAD), 0.5500f }, { TP(0, TP_PL_AMT), 0.1400f },
    { TP(0, TP_PL_RATE), 0.6000f }, { GP_DL_MIX, 0.4400f }, { GP_DL_FB, 0.8000f }, { GP_DL_DIFF, 0.9500f },
    { GP_DL_TIME_L, 0.8600f }, { GP_DL_TIME_R, 0.9000f }, { GP_DL_MOD, 0.6000f }, { GP_DL_CHAR, 0.4000f },
    { TP(0, TP_MONO), 1.0f }, { TP(0, TP_GLIDE), 0.1000f }, { TP(0, TP_MX_OVERLOAD), 0.3500f },
    { TP(0, TP_VCA_LEVEL), 1.0f }, { -1, 0 }
};
static const aria_set_t S68[] = {   /* SYNC SPIRAL */
    { GP_DL_MIX, 0.3400f }, { GP_DL_FB, 0.7000f }, { GP_DL_DIFF, 0.9200f }, { GP_DL_TIME_L, 0.8000f },
    { GP_DL_TIME_R, 0.8400f }, { GP_DL_MOD, 0.5000f }, { GP_DL_CHAR, 0.4200f }, { TP(0, TP_MONO), 1.0f },
    { TP(0, TP_GLIDE), 0.1500f }, { TP(0, TP_O2_SYNC), 1.0f }, { TP(0, TP_O2_FREQ), 0.6000f },
    { TP(0, TP_MX_O1), 0.2000f }, { TP(0, TP_MX_O2), 1.0f }, { TP(0, TP_L1_WAVE), LW_TRI },
    { TP(0, TP_L1_RATE), 0.3500f }, { TP(0, TP_L1_AMT), 0.5000f }, { TP(0, TP_L1_DEST), LD_OSC2 },
    { TP(0, TP_F1_CUT), 0.6000f }, { TP(0, TP_F1_RES), 0.2500f }, { TP(0, TP_PL_AMT), 0.1200f },
    { TP(0, TP_MX_OVERLOAD), 0.4000f }, { TP(0, TP_VCA_LEVEL), 0.9620f }, { -1, 0 }
};
static const aria_set_t S69[] = {   /* EVENING LEAD */
    { GP_DL_MIX, 0.3000f }, { GP_DL_FB, 0.5500f }, { GP_DL_DIFF, 0.4000f }, { GP_DL_TIME_L, 0.7900f },
    { GP_DL_TIME_R, 0.8400f }, { GP_DL_MOD, 0.3000f }, { TP(0, TP_O1_MIX), 1.0f }, { TP(0, TP_O2_MIX), 1.0f },
    { TP(0, TP_O2_PW), 0.4000f }, { TP(0, TP_O2_FREQ), 0.5060f }, { TP(0, TP_MX_O2), 0.6000f },
    { TP(0, TP_F1_CUT), 0.4500f }, { TP(0, TP_F1_RES), 0.3000f }, { TP(0, TP_F1_ENV), 0.6400f },
    { TP(0, TP_FE_D), 0.6000f }, { TP(0, TP_FE_S), 0.5000f }, { TP(0, TP_AE_A), 0.2500f },
    { TP(0, TP_AE_R), 0.5500f }, { TP(0, TP_PL_AMT), 0.1200f }, { TP(0, TP_PL_RATE), 0.5800f },
    { TP(0, TP_VCA_LEVEL), 0.8030f }, { -1, 0 }
};
static const aria_set_t S70[] = {   /* TINE DREAM */
    { GP_DL_MIX, 0.2500f }, { GP_DL_FB, 0.7000f }, { GP_DL_DIFF, 0.9200f }, { GP_DL_TIME_L, 0.8000f },
    { GP_DL_TIME_R, 0.8400f }, { GP_DL_MOD, 0.5000f }, { GP_DL_CHAR, 0.4200f }, { TP(0, TP_O1_TRISAW), 0.0f },
    { TP(0, TP_O2_TRISAW), 0.0f }, { TP(0, TP_O2_OCT), OCT_4 }, { TP(0, TP_FM_21), 1.0f },
    { TP(0, TP_FM_AMT), 0.3800f }, { TP(0, TP_MX_O2), 0.0600f }, { TP(0, TP_F1_CUT), 0.6200f },
    { TP(0, TP_F1_KB), KB_FULL }, { TP(0, TP_F1_ENV), 0.6200f }, { TP(0, TP_FE_D), 0.6200f },
    { TP(0, TP_FE_S), 0.1500f }, { TP(0, TP_AE_D), 0.7800f }, { TP(0, TP_AE_S), 0.1500f },
    { TP(0, TP_AE_R), 0.5800f }, { TP(0, TP_MO_FREQ), 0.5000f }, { TP(0, TP_MO_VCA), 0.2000f },
    { TP(0, TP_VCA_SPREAD), 0.7000f }, { TP(0, TP_VCA_LEVEL), 0.8890f }, { -1, 0 }
};
static const aria_set_t S71[] = {   /* ELECTRIC GRAND */
    { GP_DL_MIX, 0.2400f }, { GP_DL_FB, 0.4500f }, { GP_DL_DIFF, 0.8500f }, { GP_DL_TIME_L, 0.6000f },
    { GP_DL_TIME_R, 0.6400f }, { GP_DL_MOD, 0.4000f }, { TP(0, TP_O1_TRISAW), 0.4000f },
    { TP(0, TP_O2_OCT), OCT_4 }, { TP(0, TP_O2_TRISAW), 0.2000f }, { TP(0, TP_FM_21), 1.0f },
    { TP(0, TP_FM_AMT), 0.2500f }, { TP(0, TP_MX_O2), 0.2000f }, { TP(0, TP_F1_CUT), 0.4500f },
    { TP(0, TP_F1_ENV), 0.7500f }, { TP(0, TP_F1_KB), KB_FULL }, { TP(0, TP_FE_D), 0.6000f },
    { TP(0, TP_FE_S), 0.1000f }, { TP(0, TP_FE_VEL), 1.0f }, { TP(0, TP_AE_D), 0.8000f },
    { TP(0, TP_AE_S), 0.0500f }, { TP(0, TP_AE_R), 0.5200f }, { TP(0, TP_DETUNE), 0.3000f },
    { TP(0, TP_VCA_SPREAD), 0.6000f }, { TP(0, TP_VCA_LEVEL), 1.0f }, { -1, 0 }
};
static const aria_set_t S72[] = {   /* HARPSI ROYAL */
    { GP_DL_MIX, 0.2400f }, { GP_DL_FB, 0.4500f }, { GP_DL_DIFF, 0.8500f }, { GP_DL_TIME_L, 0.6000f },
    { GP_DL_TIME_R, 0.6400f }, { GP_DL_MOD, 0.4000f }, { TP(0, TP_O1_MIX), 1.0f }, { TP(0, TP_O1_PW), 0.1500f },
    { TP(0, TP_O2_OCT), OCT_4 }, { TP(0, TP_O2_MIX), 1.0f }, { TP(0, TP_O2_PW), 0.2500f },
    { TP(0, TP_MX_O2), 0.7000f }, { TP(0, TP_F1_HP), 1.0f }, { TP(0, TP_F1_CUT), 0.3500f },
    { TP(0, TP_F2_CUT), 0.6800f }, { TP(0, TP_F2_ENV), 0.6200f }, { TP(0, TP_FE_D), 0.5500f },
    { TP(0, TP_FE_S), 0.1000f }, { TP(0, TP_AE_D), 0.6600f }, { TP(0, TP_AE_S), 0.0f }, { TP(0, TP_AE_R), 0.5000f },
    { TP(0, TP_VCA_SPREAD), 0.5000f }, { TP(0, TP_MX_O1), 1.0f }, { TP(0, TP_VCA_LEVEL), 1.0f }, { -1, 0 }
};
static const aria_set_t S73[] = {   /* CELESTIAL HARP */
    { GP_DL_MIX, 0.4400f }, { GP_DL_FB, 0.8000f }, { GP_DL_DIFF, 0.9500f }, { GP_DL_TIME_L, 0.8600f },
    { GP_DL_TIME_R, 0.9000f }, { GP_DL_MOD, 0.6000f }, { GP_DL_CHAR, 0.4000f }, { TP(0, TP_O1_TRISAW), 0.3500f },
    { TP(0, TP_O2_OCT), OCT_4 }, { TP(0, TP_O2_TRISAW), 0.0f }, { TP(0, TP_MX_O2), 0.4500f },
    { TP(0, TP_F1_CUT), 0.4000f }, { TP(0, TP_F1_RES), 0.2000f }, { TP(0, TP_F1_ENV), 0.7800f },
    { TP(0, TP_F1_KB), KB_FULL }, { TP(0, TP_FE_D), 0.6000f }, { TP(0, TP_FE_S), 0.0f },
    { TP(0, TP_FE_R), 0.6000f }, { TP(0, TP_AE_D), 0.7800f }, { TP(0, TP_AE_S), 0.0f }, { TP(0, TP_AE_R), 0.7600f },
    { TP(0, TP_VCA_SPREAD), 0.9000f }, { TP(0, TP_MX_O1), 1.0f }, { TP(0, TP_VCA_LEVEL), 1.0f }, { -1, 0 }
};
static const aria_set_t S74[] = {   /* MALLET COAST */
    { GP_DL_MIX, 0.2500f }, { GP_DL_FB, 0.7000f }, { GP_DL_DIFF, 0.9200f }, { GP_DL_TIME_L, 0.8000f },
    { GP_DL_TIME_R, 0.8400f }, { GP_DL_MOD, 0.5000f }, { GP_DL_CHAR, 0.4200f }, { TP(0, TP_O1_TRISAW), 0.0f },
    { TP(0, TP_O2_TRISAW), 0.0f }, { TP(0, TP_O2_OCT), OCT_2 }, { TP(0, TP_FM_21), 1.0f },
    { TP(0, TP_FM_AMT), 0.4000f }, { TP(0, TP_MX_O2), 0.0f }, { TP(0, TP_F1_CUT), 0.7000f },
    { TP(0, TP_AE_D), 0.6200f }, { TP(0, TP_AE_S), 0.0f }, { TP(0, TP_AE_R), 0.5500f },
    { TP(0, TP_VCA_SPREAD), 0.7000f }, { TP(0, TP_VCA_LEVEL), 0.9870f }, { -1, 0 }
};
static const aria_set_t S75[] = {   /* SOFT POLY KEYS */
    { GP_DL_MIX, 0.4000f }, { GP_DL_FB, 0.2000f }, { GP_DL_DIFF, 0.3000f }, { GP_DL_TIME_L, 0.2400f },
    { GP_DL_TIME_R, 0.2900f }, { GP_DL_MOD, 1.0f }, { TP(0, TP_O2_FREQ), 0.5060f }, { TP(0, TP_MX_O2), 0.6000f },
    { TP(0, TP_F1_CUT), 0.3800f }, { TP(0, TP_F1_RES), 0.2000f }, { TP(0, TP_F1_ENV), 0.7200f },
    { TP(0, TP_F1_KB), KB_HALF }, { TP(0, TP_FE_D), 0.6200f }, { TP(0, TP_FE_S), 0.3000f },
    { TP(0, TP_FE_VEL), 1.0f }, { TP(0, TP_AE_D), 0.7000f }, { TP(0, TP_AE_S), 0.6000f },
    { TP(0, TP_AE_R), 0.5500f }, { TP(0, TP_DETUNE), 0.3500f }, { TP(0, TP_VCA_SPREAD), 0.6000f },
    { TP(0, TP_VCA_LEVEL), 0.9540f }, { -1, 0 }
};
static const aria_set_t S76[] = {   /* TEMPLE BELLS */
    { GP_DL_MIX, 0.4400f }, { GP_DL_FB, 0.8000f }, { GP_DL_DIFF, 0.9500f }, { GP_DL_TIME_L, 0.8600f },
    { GP_DL_TIME_R, 0.9000f }, { GP_DL_MOD, 0.6000f }, { GP_DL_CHAR, 0.4000f }, { TP(0, TP_O1_TRISAW), 0.0f },
    { TP(0, TP_O2_TRISAW), 0.0f }, { TP(0, TP_O2_OCT), OCT_4 }, { TP(0, TP_O2_FREQ), 0.7360f },
    { TP(0, TP_MX_O1), 0.4000f }, { TP(0, TP_MX_RING), 1.0f }, { TP(0, TP_F1_CUT), 0.7800f },
    { TP(0, TP_AE_D), 0.8600f }, { TP(0, TP_AE_S), 0.0f }, { TP(0, TP_AE_R), 0.8400f },
    { TP(0, TP_VCA_SPREAD), 0.9000f }, { TP(0, TP_VCA_LEVEL), 1.0f }, { -1, 0 }
};
static const aria_set_t S77[] = {   /* CRYSTAL TOWER */
    { GP_DL_MIX, 0.3400f }, { GP_DL_FB, 0.7000f }, { GP_DL_DIFF, 0.9200f }, { GP_DL_TIME_L, 0.8000f },
    { GP_DL_TIME_R, 0.8400f }, { GP_DL_MOD, 0.5000f }, { GP_DL_CHAR, 0.4200f }, { TP(0, TP_O1_TRISAW), 0.0f },
    { TP(0, TP_O2_TRISAW), 0.0f }, { TP(0, TP_O2_OCT), OCT_2 }, { TP(0, TP_O2_FREQ), 0.5214f },
    { TP(0, TP_FM_12), 1.0f }, { TP(0, TP_FM_21), 1.0f }, { TP(0, TP_FM_AMT), 0.5000f },
    { TP(0, TP_MX_O2), 0.3000f }, { TP(0, TP_F1_CUT), 0.8200f }, { TP(0, TP_AE_D), 0.8400f },
    { TP(0, TP_AE_S), 0.0f }, { TP(0, TP_AE_R), 0.8000f }, { TP(0, TP_VCA_SPREAD), 0.8000f },
    { TP(0, TP_VCA_LEVEL), 0.8770f }, { -1, 0 }
};
static const aria_set_t S78[] = {   /* BRONZE GONG */
    { GP_DL_MIX, 0.4400f }, { GP_DL_FB, 0.8000f }, { GP_DL_DIFF, 0.9500f }, { GP_DL_TIME_L, 0.8600f },
    { GP_DL_TIME_R, 0.9000f }, { GP_DL_MOD, 0.6000f }, { GP_DL_CHAR, 0.4000f }, { TP(0, TP_O1_OCT), OCT_16 },
    { TP(0, TP_O1_TRISAW), 0.2000f }, { TP(0, TP_O2_FREQ), 0.9429f }, { TP(0, TP_MX_O1), 0.5000f },
    { TP(0, TP_MX_RING), 1.0f }, { TP(0, TP_MX_NOISE), 0.0500f }, { TP(0, TP_FM_21), 1.0f },
    { TP(0, TP_FM_AMT), 0.2500f }, { TP(0, TP_F1_CUT), 0.5500f }, { TP(0, TP_F1_ENV), 0.7000f },
    { TP(0, TP_FE_D), 0.8200f }, { TP(0, TP_FE_S), 0.0f }, { TP(0, TP_FE_R), 0.8000f }, { TP(0, TP_AE_A), 0.2000f },
    { TP(0, TP_AE_D), 0.9000f }, { TP(0, TP_AE_S), 0.0f }, { TP(0, TP_AE_R), 0.8800f },
    { TP(0, TP_VCA_SPREAD), 0.6000f }, { TP(0, TP_MX_OVERLOAD), 0.2000f }, { TP(0, TP_VCA_LEVEL), 1.0f }, { -1, 0 }
};
static const aria_set_t S79[] = {   /* MUSIC BOX STARS */
    { GP_DL_MIX, 0.3000f }, { GP_DL_FB, 0.5500f }, { GP_DL_DIFF, 0.4000f }, { GP_DL_TIME_L, 0.7900f },
    { GP_DL_TIME_R, 0.8400f }, { GP_DL_MOD, 0.3000f }, { TP(0, TP_O1_OCT), OCT_4 }, { TP(0, TP_O1_TRISAW), 0.0f },
    { TP(0, TP_O2_OCT), OCT_2 }, { TP(0, TP_O2_TRISAW), 0.0f }, { TP(0, TP_MX_O2), 0.3000f },
    { TP(0, TP_FM_21), 1.0f }, { TP(0, TP_FM_AMT), 0.2000f }, { TP(0, TP_F1_CUT), 0.7500f },
    { TP(0, TP_AE_D), 0.7000f }, { TP(0, TP_AE_S), 0.0f }, { TP(0, TP_AE_R), 0.7000f },
    { TP(0, TP_VCA_SPREAD), 0.9000f }, { TP(0, TP_VCA_LEVEL), 0.9520f }, { -1, 0 }
};
static const aria_set_t S80[] = {   /* GLASS HARMONICA */
    { GP_DL_MIX, 0.4400f }, { GP_DL_FB, 0.8000f }, { GP_DL_DIFF, 0.9500f }, { GP_DL_TIME_L, 0.8600f },
    { GP_DL_TIME_R, 0.9000f }, { GP_DL_MOD, 0.6000f }, { GP_DL_CHAR, 0.4000f }, { TP(0, TP_O1_TRISAW), 0.0f },
    { TP(0, TP_O2_TRISAW), 0.0f }, { TP(0, TP_O2_OCT), OCT_4 }, { TP(0, TP_O2_FREQ), 1.0f },
    { TP(0, TP_MX_O2), 0.3500f }, { TP(0, TP_MX_RING), 0.3000f }, { TP(0, TP_F1_CUT), 0.7000f },
    { TP(0, TP_AE_A), 0.5500f }, { TP(0, TP_AE_D), 0.8000f }, { TP(0, TP_AE_S), 0.4000f },
    { TP(0, TP_AE_R), 0.8000f }, { TP(0, TP_PL_AMT), 0.0800f }, { TP(0, TP_VCA_SPREAD), 0.8000f },
    { TP(0, TP_VCA_LEVEL), 0.9400f }, { -1, 0 }
};
static const aria_set_t S81[] = {   /* TIDAL BASS */
    { TP(0, TP_MONO), 1.0f }, { TP(0, TP_GLIDE), 0.1000f }, { TP(0, TP_O1_OCT), OCT_16 },
    { TP(0, TP_O2_OCT), OCT_16 }, { TP(0, TP_O2_TRISAW), 0.3000f }, { TP(0, TP_O2_FREQ), 0.5030f },
    { TP(0, TP_MX_O2), 0.6000f }, { TP(0, TP_F1_CUT), 0.2600f }, { TP(0, TP_F1_RES), 0.3000f },
    { TP(0, TP_F1_ENV), 0.6800f }, { TP(0, TP_FE_A), 0.4000f }, { TP(0, TP_FE_D), 0.7000f },
    { TP(0, TP_FE_S), 0.4000f }, { TP(0, TP_AE_R), 0.4000f }, { TP(0, TP_MX_OVERLOAD), 0.3000f },
    { GP_DL_MIX, 0.1200f }, { GP_DL_FB, 0.4500f }, { GP_DL_DIFF, 0.8500f }, { GP_DL_TIME_L, 0.6000f },
    { GP_DL_TIME_R, 0.6400f }, { GP_DL_MOD, 0.4000f }, { TP(0, TP_VCA_LEVEL), 0.9910f }, { -1, 0 }
};
static const aria_set_t S82[] = {   /* CS BASS */
    { TP(0, TP_MONO), 1.0f }, { TP(0, TP_O1_OCT), OCT_16 }, { TP(0, TP_O1_MIX), 0.5000f },
    { TP(0, TP_O1_PW), 0.4000f }, { TP(0, TP_F1_HP), 0.0f }, { TP(0, TP_F1_CUT), 0.3000f },
    { TP(0, TP_F2_CUT), 0.7500f }, { TP(0, TP_F2_RES), 0.0f }, { TP(0, TP_F2_ENV), 0.5000f },
    { TP(0, TP_FE_D), 0.5200f }, { TP(0, TP_FE_S), 0.2500f }, { TP(0, TP_FE_VEL), 1.0f },
    { TP(0, TP_AE_R), 0.3000f }, { TP(0, TP_MX_OVERLOAD), 0.5000f }, { TP(0, TP_MX_O1), 1.0f },
    { TP(0, TP_F1_RES), 0.3200f }, { TP(0, TP_F1_ENV), 0.7400f }, { TP(0, TP_VCA_LEVEL), 1.0f }, { -1, 0 }
};
static const aria_set_t S83[] = {   /* PROCESSION BASS */
    { GP_DL_MIX, 0.1400f }, { GP_DL_FB, 0.4500f }, { GP_DL_DIFF, 0.8500f }, { GP_DL_TIME_L, 0.6000f },
    { GP_DL_TIME_R, 0.6400f }, { GP_DL_MOD, 0.4000f }, { TP(0, TP_MONO), 1.0f }, { TP(0, TP_O1_OCT), OCT_16 },
    { TP(0, TP_O2_OCT), OCT_16 }, { TP(0, TP_O2_FREQ), 1.0f }, { TP(0, TP_MX_O2), 0.5000f },
    { TP(0, TP_F1_CUT), 0.3000f }, { TP(0, TP_F1_RES), 0.2500f }, { TP(0, TP_F1_ENV), 0.6600f },
    { TP(0, TP_FE_A), 0.3000f }, { TP(0, TP_FE_D), 0.7200f }, { TP(0, TP_FE_S), 0.4500f },
    { TP(0, TP_AE_A), 0.2000f }, { TP(0, TP_AE_R), 0.5000f }, { TP(0, TP_MX_OVERLOAD), 0.3000f },
    { TP(0, TP_MX_O1), 1.0f }, { TP(0, TP_VCA_LEVEL), 0.9840f }, { -1, 0 }
};
static const aria_set_t S84[] = {   /* DEEP PEDAL */
    { GP_DL_MIX, 0.1200f }, { GP_DL_FB, 0.4500f }, { GP_DL_DIFF, 0.8500f }, { GP_DL_TIME_L, 0.6000f },
    { GP_DL_TIME_R, 0.6400f }, { GP_DL_MOD, 0.4000f }, { TP(0, TP_MONO), 1.0f }, { TP(0, TP_GLIDE), 0.2000f },
    { TP(0, TP_O1_OCT), OCT_16 }, { TP(0, TP_O1_TRISAW), 0.3000f }, { TP(0, TP_MX_O1), 1.0f },
    { TP(0, TP_F1_CUT), 0.4000f }, { TP(0, TP_AE_A), 0.3000f }, { TP(0, TP_AE_R), 0.5500f },
    { TP(0, TP_MX_OVERLOAD), 0.3500f }, { TP(0, TP_VCA_LEVEL), 1.0f }, { -1, 0 }
};
static const aria_set_t S85[] = {   /* DUSK CITY */
    { GP_DL_MIX, 0.4400f }, { GP_DL_FB, 0.8000f }, { GP_DL_DIFF, 0.9500f }, { GP_DL_TIME_L, 0.8600f },
    { GP_DL_TIME_R, 0.9000f }, { GP_DL_MOD, 0.6000f }, { GP_DL_CHAR, 0.4000f }, { TP(0, TP_O1_OCT), OCT_16 },
    { TP(0, TP_MX_O1), 0.6000f }, { TP(0, TP_MX_NOISE), 0.8500f }, { TP(0, TP_F_ORDER), ORD_STEREO },
    { TP(0, TP_F1_CUT), 0.4000f }, { TP(0, TP_F2_CUT), 0.4500f }, { TP(0, TP_F1_RES), 0.6500f },
    { TP(0, TP_F2_RES), 0.6500f }, { TP(0, TP_L1_WAVE), LW_SINE }, { TP(0, TP_L1_RATE), 0.1500f },
    { TP(0, TP_L1_AMT), 0.3500f }, { TP(0, TP_L1_DEST), LD_F1 }, { TP(0, TP_L2_RATE), 0.1900f },
    { TP(0, TP_L2_AMT), 0.3500f }, { TP(0, TP_L2_DEST), LD_F2 }, { TP(0, TP_AE_A), 0.8200f },
    { TP(0, TP_AE_S), 1.0f }, { TP(0, TP_AE_R), 0.8800f }, { TP(0, TP_VCA_SPREAD), 0.8000f },
    { TP(0, TP_MX_OVERLOAD), 0.3000f }, { TP(0, TP_VCA_LEVEL), 1.0f }, { -1, 0 }
};
static const aria_set_t S86[] = {   /* SPACE CARRIER */
    { GP_DL_MIX, 0.4400f }, { GP_DL_FB, 0.8000f }, { GP_DL_DIFF, 0.9500f }, { GP_DL_TIME_L, 0.8600f },
    { GP_DL_TIME_R, 0.9000f }, { GP_DL_MOD, 0.6000f }, { GP_DL_CHAR, 0.4000f }, { TP(0, TP_O1_OCT), OCT_16 },
    { TP(0, TP_O2_OCT), OCT_16 }, { TP(0, TP_O2_FREQ), 0.5015f }, { TP(0, TP_MX_O2), 0.8000f },
    { TP(0, TP_MO_FREQ), 0.2000f }, { TP(0, TP_MO_FILTER), 0.3000f }, { TP(0, TP_MO_UNI), 1.0f },
    { TP(0, TP_F1_CUT), 0.3000f }, { TP(0, TP_F1_RES), 0.5500f }, { TP(0, TP_AE_A), 0.8000f },
    { TP(0, TP_AE_S), 1.0f }, { TP(0, TP_AE_R), 0.9000f }, { TP(0, TP_MX_OVERLOAD), 0.3500f },
    { TP(0, TP_VCA_SPREAD), 0.6000f }, { TP(0, TP_VCA_LEVEL), 0.7640f }, { -1, 0 }
};
static const aria_set_t S87[] = {   /* DISTANT SIGNAL */
    { GP_DL_MIX, 0.3000f }, { GP_DL_FB, 0.5500f }, { GP_DL_DIFF, 0.6000f }, { GP_DL_TIME_L, 0.7900f },
    { GP_DL_TIME_R, 0.8400f }, { GP_DL_MOD, 0.3000f }, { TP(0, TP_O1_TRISAW), 0.0f },
    { TP(0, TP_O2_FREQ), 0.8571f }, { TP(0, TP_MX_O1), 0.3000f }, { TP(0, TP_MX_RING), 0.5000f },
    { TP(0, TP_L1_WAVE), LW_SH }, { TP(0, TP_L1_RATE), 0.5500f }, { TP(0, TP_L1_AMT), 0.5000f },
    { TP(0, TP_L1_DEST), LD_F1 }, { TP(0, TP_F1_CUT), 0.5000f }, { TP(0, TP_F1_RES), 0.7500f },
    { TP(0, TP_AE_A), 0.5000f }, { TP(0, TP_AE_S), 1.0f }, { TP(0, TP_AE_R), 0.8000f },
    { TP(0, TP_VCA_SPREAD), 0.8000f }, { TP(0, TP_VCA_LEVEL), 1.0f }, { -1, 0 }
};
static const aria_set_t S88[] = {   /* OCEAN BREATH */
    { GP_DL_MIX, 0.4400f }, { GP_DL_FB, 0.8000f }, { GP_DL_DIFF, 0.9500f }, { GP_DL_TIME_L, 0.8600f },
    { GP_DL_TIME_R, 0.9000f }, { GP_DL_MOD, 0.6000f }, { GP_DL_CHAR, 0.4000f }, { TP(0, TP_MX_O1), 0.0f },
    { TP(0, TP_MX_NOISE), 1.0f }, { TP(0, TP_F1_CUT), 0.7000f }, { TP(0, TP_F1_RES), 0.3000f },
    { TP(0, TP_L1_WAVE), LW_SINE }, { TP(0, TP_L1_RATE), 0.1200f }, { TP(0, TP_L1_AMT), 0.4500f },
    { TP(0, TP_L1_DEST), LD_F1 }, { TP(0, TP_L2_RATE), 0.1000f }, { TP(0, TP_L2_AMT), 0.1800f },
    { TP(0, TP_L2_DEST), LD_AMP }, { TP(0, TP_MO_FREQ), 0.1000f }, { TP(0, TP_MO_PAN), 0.7000f },
    { TP(0, TP_AE_A), 0.7500f }, { TP(0, TP_AE_S), 1.0f }, { TP(0, TP_AE_R), 0.8500f },
    { TP(0, TP_MX_OVERLOAD), 0.4000f }, { TP(0, TP_VCA_LEVEL), 1.0f }, { -1, 0 }
};
static const aria_set_t S89[] = {   /* THUNDER DRUM */
    { GP_DL_MIX, 0.4400f }, { GP_DL_FB, 0.8000f }, { GP_DL_DIFF, 0.9500f }, { GP_DL_TIME_L, 0.8600f },
    { GP_DL_TIME_R, 0.9000f }, { GP_DL_MOD, 0.6000f }, { GP_DL_CHAR, 0.3500f }, { TP(0, TP_O1_OCT), OCT_16 },
    { TP(0, TP_O1_TRISAW), 0.0f }, { TP(0, TP_MX_NOISE), 0.7000f }, { TP(0, TP_F1_CUT), 0.3000f },
    { TP(0, TP_F1_ENV), 0.7800f }, { TP(0, TP_FE_D), 0.7000f }, { TP(0, TP_FE_S), 0.0f },
    { TP(0, TP_FE_R), 0.7000f }, { TP(0, TP_AE_D), 0.8200f }, { TP(0, TP_AE_S), 0.0f }, { TP(0, TP_AE_R), 0.8000f },
    { TP(0, TP_MX_OVERLOAD), 0.5000f }, { TP(0, TP_VCA_LEVEL), 1.0f }, { -1, 0 }
};
static const aria_set_t S90[] = {   /* STARFIELD */
    { GP_DL_MIX, 0.4400f }, { GP_DL_FB, 0.8000f }, { GP_DL_DIFF, 0.9500f }, { GP_DL_TIME_L, 0.8600f },
    { GP_DL_TIME_R, 0.9000f }, { GP_DL_MOD, 0.6000f }, { GP_DL_CHAR, 0.4000f }, { TP(0, TP_O1_OCT), OCT_4 },
    { TP(0, TP_O1_TRISAW), 0.3000f }, { TP(0, TP_L1_WAVE), LW_SH }, { TP(0, TP_L1_RATE), 0.6000f },
    { TP(0, TP_L1_AMT), 0.4500f }, { TP(0, TP_L1_DEST), LD_F1 }, { TP(0, TP_F1_CUT), 0.5500f },
    { TP(0, TP_F1_RES), 0.6000f }, { TP(0, TP_AE_A), 0.4500f }, { TP(0, TP_AE_S), 1.0f },
    { TP(0, TP_AE_R), 0.8500f }, { TP(0, TP_VCA_SPREAD), 1.0f }, { TP(0, TP_VCA_LEVEL), 0.7930f }, { -1, 0 }
};
static const aria_set_t S91[] = {   /* EPIC SCORE */
    { GP_DL_MIX, 0.4400f }, { GP_DL_FB, 0.8000f }, { GP_DL_DIFF, 0.9500f }, { GP_DL_TIME_L, 0.8600f },
    { GP_DL_TIME_R, 0.9000f }, { GP_DL_MOD, 0.6000f }, { GP_DL_CHAR, 0.4000f }, { TP(0, TP_MONO), 1.0f },
    { TP(0, TP_GLIDE), 0.1000f }, { TP(0, TP_O1_OCT), OCT_16 }, { TP(0, TP_O2_OCT), OCT_16 },
    { TP(0, TP_O2_TRISAW), 0.3000f }, { TP(0, TP_O2_FREQ), 0.5030f }, { TP(0, TP_MX_O2), 0.6000f },
    { TP(0, TP_F1_CUT), 0.2600f }, { TP(0, TP_F1_RES), 0.3000f }, { TP(0, TP_F1_ENV), 0.6800f },
    { TP(0, TP_FE_A), 0.4000f }, { TP(0, TP_FE_D), 0.7000f }, { TP(0, TP_FE_S), 0.4000f },
    { TP(0, TP_AE_R), 0.4000f }, { TP(0, TP_MX_OVERLOAD), 0.3000f }, { TP(1, TP_O2_FREQ), 0.5045f },
    { TP(1, TP_MX_O1), 0.7500f }, { TP(1, TP_MX_O2), 0.7000f }, { TP(1, TP_F1_HP), 1.0f },
    { TP(1, TP_F1_CUT), 0.2200f }, { TP(1, TP_F2_CUT), 0.3600f }, { TP(1, TP_F2_RES), 0.1200f },
    { TP(1, TP_F2_ENV), 0.7600f }, { TP(1, TP_F2_KB), KB_HALF }, { TP(1, TP_FE_A), 0.5600f },
    { TP(1, TP_FE_D), 0.7400f }, { TP(1, TP_FE_S), 0.5000f }, { TP(1, TP_FE_R), 0.6200f },
    { TP(1, TP_FE_VEL), 1.0f }, { TP(1, TP_AE_A), 0.4200f }, { TP(1, TP_AE_D), 0.6000f },
    { TP(1, TP_AE_S), 0.9500f }, { TP(1, TP_AE_R), 0.6200f }, { TP(1, TP_DETUNE), 0.4200f },
    { TP(1, TP_VCA_SPREAD), 0.5500f }, { TP(1, TP_PL_AMT), 0.0800f }, { TP(1, TP_PL_RATE), 0.6000f },
    { GP_VC_MODE, VC_SPLIT }, { GP_VC_SPLIT, 0.3780f }, { GP_VC_VOICES_A, 0.0f }, { GP_DL_A, 0.0f },
    { TP(0, TP_VCA_LEVEL), 0.8190f }, { TP(1, TP_VCA_LEVEL), 0.8190f }, { -1, 0 }
};
static const aria_set_t S92[] = {   /* BRASS + STRINGS */
    { GP_DL_MIX, 0.4400f }, { GP_DL_FB, 0.8000f }, { GP_DL_DIFF, 0.9500f }, { GP_DL_TIME_L, 0.8600f },
    { GP_DL_TIME_R, 0.9000f }, { GP_DL_MOD, 0.6000f }, { GP_DL_CHAR, 0.4000f }, { TP(0, TP_O2_FREQ), 0.5045f },
    { TP(0, TP_MX_O1), 0.7500f }, { TP(0, TP_MX_O2), 0.7000f }, { TP(0, TP_F1_HP), 1.0f },
    { TP(0, TP_F1_CUT), 0.2200f }, { TP(0, TP_F2_CUT), 0.3600f }, { TP(0, TP_F2_RES), 0.1200f },
    { TP(0, TP_F2_ENV), 0.7600f }, { TP(0, TP_F2_KB), KB_HALF }, { TP(0, TP_FE_A), 0.5600f },
    { TP(0, TP_FE_D), 0.7400f }, { TP(0, TP_FE_S), 0.5000f }, { TP(0, TP_FE_R), 0.6200f },
    { TP(0, TP_FE_VEL), 1.0f }, { TP(0, TP_AE_A), 0.4200f }, { TP(0, TP_AE_D), 0.6000f },
    { TP(0, TP_AE_S), 0.9500f }, { TP(0, TP_AE_R), 0.6200f }, { TP(0, TP_DETUNE), 0.4200f },
    { TP(0, TP_VCA_SPREAD), 0.5500f }, { TP(0, TP_PL_AMT), 0.0800f }, { TP(0, TP_PL_RATE), 0.6000f },
    { TP(1, TP_O2_FREQ), 0.5055f }, { TP(1, TP_MX_O1), 0.7000f }, { TP(1, TP_MX_O2), 0.7000f },
    { TP(1, TP_F1_HP), 1.0f }, { TP(1, TP_F1_CUT), 0.2500f }, { TP(1, TP_F2_CUT), 0.5000f },
    { TP(1, TP_F2_RES), 0.1000f }, { TP(1, TP_F2_ENV), 0.6000f }, { TP(1, TP_F2_KB), KB_HALF },
    { TP(1, TP_FE_A), 0.6200f }, { TP(1, TP_FE_D), 0.7800f }, { TP(1, TP_FE_S), 0.7000f },
    { TP(1, TP_FE_R), 0.7000f }, { TP(1, TP_AE_A), 0.6000f }, { TP(1, TP_AE_D), 0.7000f }, { TP(1, TP_AE_S), 1.0f },
    { TP(1, TP_AE_R), 0.7200f }, { TP(1, TP_DETUNE), 0.5000f }, { TP(1, TP_VCA_SPREAD), 0.7500f },
    { TP(1, TP_PL_AMT), 0.1000f }, { TP(1, TP_PL_RATE), 0.6200f }, { GP_VC_MODE, VC_STACK },
    { GP_VC_VOICES_A, 3.0f }, { TP(0, TP_VCA_LEVEL), 0.8780f }, { TP(1, TP_VCA_LEVEL), 0.8780f }, { -1, 0 }
};
static const aria_set_t S93[] = {   /* LOW SPLIT ANTHEM */
    { TP(0, TP_O2_FREQ), 0.5045f }, { TP(0, TP_MX_O1), 0.9000f }, { TP(0, TP_MX_O2), 0.8500f },
    { TP(0, TP_F1_HP), 1.0f }, { TP(0, TP_F1_CUT), 0.2200f }, { TP(0, TP_F2_CUT), 0.3400f },
    { TP(0, TP_F2_RES), 0.1200f }, { TP(0, TP_F2_ENV), 0.7600f }, { TP(0, TP_F2_KB), KB_HALF },
    { TP(0, TP_FE_A), 0.5600f }, { TP(0, TP_FE_D), 0.7400f }, { TP(0, TP_FE_S), 0.5000f },
    { TP(0, TP_FE_R), 0.6200f }, { TP(0, TP_FE_VEL), 1.0f }, { TP(0, TP_AE_A), 0.4200f },
    { TP(0, TP_AE_D), 0.6000f }, { TP(0, TP_AE_S), 0.9500f }, { TP(0, TP_AE_R), 0.6200f },
    { TP(0, TP_DETUNE), 0.4200f }, { TP(0, TP_VCA_SPREAD), 0.5500f }, { TP(0, TP_PL_AMT), 0.0800f },
    { TP(0, TP_PL_RATE), 0.6000f }, { GP_DL_MIX, 0.4400f }, { GP_DL_FB, 0.8000f }, { GP_DL_DIFF, 0.9500f },
    { GP_DL_TIME_L, 0.8600f }, { GP_DL_TIME_R, 0.9000f }, { GP_DL_MOD, 0.6000f }, { GP_DL_CHAR, 0.4000f },
    { TP(0, TP_O1_OCT), OCT_16 }, { GP_LS_SPLIT, 1.0f }, { GP_LS_POINT, 0.3333f }, { TP(0, TP_VCA_LEVEL), 0.9870f },
    { -1, 0 }
};
static const aria_set_t S94[] = {   /* HOLD CATHEDRAL */
    { GP_DL_MIX, 0.4400f }, { GP_DL_FB, 0.8000f }, { GP_DL_DIFF, 0.9500f }, { GP_DL_TIME_L, 0.8600f },
    { GP_DL_TIME_R, 0.9000f }, { GP_DL_MOD, 0.6000f }, { GP_DL_CHAR, 0.4000f }, { TP(0, TP_O2_FREQ), 0.5055f },
    { TP(0, TP_MX_O1), 0.7000f }, { TP(0, TP_MX_O2), 0.7000f }, { TP(0, TP_F1_HP), 1.0f },
    { TP(0, TP_F1_CUT), 0.2500f }, { TP(0, TP_F2_CUT), 0.4400f }, { TP(0, TP_F2_RES), 0.1000f },
    { TP(0, TP_F2_ENV), 0.6000f }, { TP(0, TP_F2_KB), KB_HALF }, { TP(0, TP_FE_A), 0.6200f },
    { TP(0, TP_FE_D), 0.7800f }, { TP(0, TP_FE_S), 0.7000f }, { TP(0, TP_FE_R), 0.7000f },
    { TP(0, TP_AE_A), 0.7000f }, { TP(0, TP_AE_D), 0.7000f }, { TP(0, TP_AE_S), 1.0f }, { TP(0, TP_AE_R), 0.8500f },
    { TP(0, TP_DETUNE), 0.5000f }, { TP(0, TP_VCA_SPREAD), 0.7500f }, { TP(0, TP_PL_AMT), 0.1000f },
    { TP(0, TP_PL_RATE), 0.6200f }, { GP_HOLD, 1.0f }, { TP(0, TP_VCA_LEVEL), 0.9850f }, { -1, 0 }
};
static const aria_set_t S95[] = {   /* LOW DRONE HOLD */
    { GP_DL_MIX, 0.4400f }, { GP_DL_FB, 0.8000f }, { GP_DL_DIFF, 0.9500f }, { GP_DL_TIME_L, 0.8600f },
    { GP_DL_TIME_R, 0.9000f }, { GP_DL_MOD, 0.6000f }, { GP_DL_CHAR, 0.4000f }, { TP(0, TP_O1_OCT), OCT_16 },
    { TP(0, TP_O2_FREQ), 0.5070f }, { TP(0, TP_MX_O2), 0.7000f }, { TP(0, TP_F1_CUT), 0.3600f },
    { TP(0, TP_F1_RES), 0.3500f }, { TP(0, TP_F1_ENV), 0.6000f }, { TP(0, TP_FE_A), 0.7500f },
    { TP(0, TP_AE_A), 0.7000f }, { TP(0, TP_AE_S), 1.0f }, { TP(0, TP_AE_R), 0.8500f },
    { TP(0, TP_DETUNE), 0.5000f }, { TP(0, TP_VCA_SPREAD), 0.8000f }, { GP_LS_SPLIT, 2.0f },
    { GP_LS_POINT, 0.3333f }, { GP_LS_HOLD, 1.0f }, { TP(0, TP_VCA_LEVEL), 0.7530f }, { -1, 0 }
};
static const aria_set_t S96[] = {   /* ECHO SEQUENCE */
    { GP_DL_MIX, 0.3000f }, { GP_DL_FB, 0.5500f }, { GP_DL_DIFF, 0.4000f }, { GP_DL_TIME_L, 0.7900f },
    { GP_DL_TIME_R, 0.8400f }, { GP_DL_MOD, 0.3000f }, { GP_ARP_ON, 1.0f }, { GP_ARP_MODE, ARP_ORDER },
    { GP_ARP_DIV, DIV_8 }, { GP_ARP_GATE, 0.4000f }, { TP(0, TP_O1_MIX), 1.0f }, { TP(0, TP_O1_PW), 0.3500f },
    { TP(0, TP_O2_FREQ), 0.5060f }, { TP(0, TP_MX_O2), 0.7000f }, { TP(0, TP_F1_CUT), 0.3400f },
    { TP(0, TP_F1_RES), 0.4500f }, { TP(0, TP_F1_ENV), 0.7400f }, { TP(0, TP_FE_D), 0.4500f },
    { TP(0, TP_FE_S), 0.0500f }, { TP(0, TP_AE_D), 0.5500f }, { TP(0, TP_AE_S), 0.2000f },
    { TP(0, TP_AE_R), 0.4500f }, { TP(0, TP_VCA_SPREAD), 0.7000f }, { TP(0, TP_MX_O1), 1.0f },
    { TP(0, TP_MX_OVERLOAD), 0.3000f }, { TP(0, TP_VCA_LEVEL), 1.0f }, { -1, 0 }
};
static const aria_set_t S97[] = {   /* STARS IN MOTION */
    { GP_DL_MIX, 0.4400f }, { GP_DL_FB, 0.8000f }, { GP_DL_DIFF, 0.9500f }, { GP_DL_TIME_L, 0.8600f },
    { GP_DL_TIME_R, 0.9000f }, { GP_DL_MOD, 0.6000f }, { GP_DL_CHAR, 0.4000f }, { GP_ARP_ON, 1.0f },
    { GP_ARP_MODE, ARP_UP }, { GP_ARP_OCT, 2.0f }, { GP_ARP_DIV, DIV_16T }, { GP_ARP_GATE, 0.3500f },
    { TP(0, TP_O1_TRISAW), 0.0f }, { TP(0, TP_O2_TRISAW), 0.0f }, { TP(0, TP_O2_OCT), OCT_2 },
    { TP(0, TP_FM_21), 1.0f }, { TP(0, TP_FM_AMT), 0.2500f }, { TP(0, TP_MX_O2), 0.3500f },
    { TP(0, TP_F1_CUT), 0.7200f }, { TP(0, TP_AE_D), 0.6000f }, { TP(0, TP_AE_S), 0.0f },
    { TP(0, TP_AE_R), 0.6000f }, { TP(0, TP_VCA_SPREAD), 1.0f }, { TP(0, TP_MX_O1), 1.0f },
    { TP(0, TP_VCA_LEVEL), 1.0f }, { -1, 0 }
};

const aria_preset_t ARIA_PRESETS[ARIA_NPRESETS] = {
    { "INIT", "INIT", S00 },
    { "WARM STRINGS", "PAD", S01 },
    { "GLASS PAD", "PAD", S02 },
    { "VELVET PAD", "PAD", S03 },
    { "DARK MATTER", "PAD", S04 },
    { "SHIMMER HALO", "PAD", S05 },
    { "SWEEP PAD", "PAD", S06 },
    { "SLOW MOTION", "PAD", S07 },
    { "OVERLOAD BASS", "BASS", S08 },
    { "SUB BASS", "BASS", S09 },
    { "ACID LINE", "BASS", S10 },
    { "FAT FIVE", "BASS", S11 },
    { "PLUCK BASS", "BASS", S12 },
    { "RUBBER BASS", "BASS", S13 },
    { "LADDER GROWL", "BASS", S14 },
    { "FM KEYS", "KEYS", S15 },
    { "VINTAGE EP", "KEYS", S16 },
    { "CLAV FUNK", "KEYS", S17 },
    { "ORGANIC ORGAN", "KEYS", S18 },
    { "CHORD KEYS", "KEYS", S19 },
    { "CRYSTAL BELLS", "KEYS", S20 },
    { "RING BELLS", "KEYS", S21 },
    { "SYNC LEAD", "LEAD", S22 },
    { "MOOGY LEAD", "LEAD", S23 },
    { "SQUARE SOLO", "LEAD", S24 },
    { "SCREAMER", "LEAD", S25 },
    { "POLY BRASS", "BRASS", S26 },
    { "BRASS STABS", "BRASS", S27 },
    { "PLUCK ECHO", "PLUCK", S28 },
    { "ARP CHORDS", "ARP", S29 },
    { "SEQ PULSE", "ARP", S30 },
    { "TRANCE GATE", "ARP", S31 },
    { "UP AND AWAY", "ARP", S32 },
    { "PULSING DRONE", "FX", S33 },
    { "WIND TUNNEL", "FX", S34 },
    { "ROBOT TALK", "FX", S35 },
    { "SPACE SIREN", "FX", S36 },
    { "SPLIT BASS PAD", "SPLIT", S37 },
    { "BASS + LEAD", "SPLIT", S38 },
    { "STACK STRINGS", "SPLIT", S39 },
    { "SKYLINE BRASS", "BRASS", S40 },
    { "ANTHEM BRASS", "BRASS", S41 },
    { "SLOW SWELL BRASS", "BRASS", S42 },
    { "HORN SECTION", "BRASS", S43 },
    { "MONOLITH BRASS", "BRASS", S44 },
    { "DUST HORNS", "BRASS", S45 },
    { "FIFTH FANFARE", "BRASS", S46 },
    { "NEON RAIN", "PAD", S47 },
    { "CATHEDRAL", "PAD", S48 },
    { "ORBITAL DAWN", "PAD", S49 },
    { "SILK HORIZON", "PAD", S50 },
    { "DEEP FIELD", "PAD", S51 },
    { "GOLDEN HOUR", "PAD", S52 },
    { "AURORA", "PAD", S53 },
    { "MEMORY OF SNOW", "PAD", S54 },
    { "VAST HORIZON", "PAD", S55 },
    { "CS STRINGS", "STRINGS", S56 },
    { "ELEGY STRINGS", "STRINGS", S57 },
    { "VIOLA ENSEMBLE", "STRINGS", S58 },
    { "TREMOLO STRINGS", "STRINGS", S59 },
    { "HIGH STRINGS", "STRINGS", S60 },
    { "STRING MACHINE", "STRINGS", S61 },
    { "VICTORY LEAD", "LEAD", S62 },
    { "RIBBON CRY", "LEAD", S63 },
    { "NIGHT SAX", "LEAD", S64 },
    { "GLASS FLUTE", "LEAD", S65 },
    { "ICE OBOE", "LEAD", S66 },
    { "DISTANT TRUMPET", "LEAD", S67 },
    { "SYNC SPIRAL", "LEAD", S68 },
    { "EVENING LEAD", "LEAD", S69 },
    { "TINE DREAM", "KEYS", S70 },
    { "ELECTRIC GRAND", "KEYS", S71 },
    { "HARPSI ROYAL", "KEYS", S72 },
    { "CELESTIAL HARP", "KEYS", S73 },
    { "MALLET COAST", "KEYS", S74 },
    { "SOFT POLY KEYS", "KEYS", S75 },
    { "TEMPLE BELLS", "BELLS", S76 },
    { "CRYSTAL TOWER", "BELLS", S77 },
    { "BRONZE GONG", "BELLS", S78 },
    { "MUSIC BOX STARS", "BELLS", S79 },
    { "GLASS HARMONICA", "BELLS", S80 },
    { "TIDAL BASS", "BASS", S81 },
    { "CS BASS", "BASS", S82 },
    { "PROCESSION BASS", "BASS", S83 },
    { "DEEP PEDAL", "BASS", S84 },
    { "DUSK CITY", "ATMOS", S85 },
    { "SPACE CARRIER", "ATMOS", S86 },
    { "DISTANT SIGNAL", "ATMOS", S87 },
    { "OCEAN BREATH", "ATMOS", S88 },
    { "THUNDER DRUM", "ATMOS", S89 },
    { "STARFIELD", "ATMOS", S90 },
    { "EPIC SCORE", "SPLIT", S91 },
    { "BRASS + STRINGS", "SPLIT", S92 },
    { "LOW SPLIT ANTHEM", "SPLIT", S93 },
    { "HOLD CATHEDRAL", "PAD", S94 },
    { "LOW DRONE HOLD", "ATMOS", S95 },
    { "ECHO SEQUENCE", "ARP", S96 },
    { "STARS IN MOTION", "ARP", S97 },
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
