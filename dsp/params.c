/* SPDX-License-Identifier: MPL-2.0
 * Copyright (C) 2026 swtdr34mz (ARIA) */
/* The parameters' functions (the tables: params_data.c): C that is C++ too (the MPC build compiles it as such). */
#include "aria.h"
#include <math.h>
#include <stdio.h>

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

static void note_name(int n, char *out, int len)
{
    static const char *const N[12] = { "C", "C#", "D", "D#", "E", "F", "F#", "G", "G#", "A", "A#", "B" };
    snprintf(out, (size_t)len, "%s%d", N[n % 12], n / 12 - 1);
}

void aria_value_text(int i, float v, char *out, int len)
{
    const aria_param_t *d = aria_param_info(i);
    if (!d) { snprintf(out, (size_t)len, "?"); return; }
    if (d->kind == PK_CHOICE) {
        int k = (int)lroundf(v); k = k < 0 ? 0 : k >= d->count ? d->count - 1 : k;
        snprintf(out, (size_t)len, "%s", d->choice[k]);
        return;
    }
    if (d->kind == PK_SWITCH) { snprintf(out, (size_t)len, "%s", v > 0.5f ? "ON" : "OFF"); return; }
    if (i >= GP_FIRST) {
        switch (i) {
        case GP_VC_SPLIT: note_name((int)lroundf(v * 127.0f), out, len); return;
        case GP_TEMPO: snprintf(out, (size_t)len, "%d BPM", (int)lroundf(60.0f + 140.0f * v)); return;
        case GP_LS_POINT: note_name(24 + (int)lroundf(v * 72.0f), out, len); return;
        case GP_LS_BALANCE: snprintf(out, (size_t)len, "%+d", (int)lroundf((v - 0.5f) * 100.0f)); return;
        case GP_DL_TIME_L: case GP_DL_TIME_R: {
            float ms = powf(2000.0f, v);
            if (ms < 1000.0f) snprintf(out, (size_t)len, "%.0f ms", ms);
            else snprintf(out, (size_t)len, "%.2f s", ms / 1000.0f);
            return;
        }
        default: break;
        }
    } else {
        switch (i % TP_COUNT) {
        case TP_O1_FREQ: case TP_O2_FREQ: snprintf(out, (size_t)len, "%+.2f st", (v - 0.5f) * 14.0f); return;
        case TP_FE_A: case TP_FE_D: case TP_FE_R: case TP_AE_A: case TP_AE_D: case TP_AE_R: {
            float s = 0.0005f * powf(40000.0f, v);
            if (s < 1.0f) snprintf(out, (size_t)len, s < 0.01f ? "%.1f ms" : "%.0f ms", s * 1000.0f);
            else snprintf(out, (size_t)len, "%.2f s", s);
            return;
        }
        case TP_F1_CUT: case TP_F2_CUT: {
            float hz = 20.0f * exp2f(v * 10.0f);
            if (hz < 1000.0f) snprintf(out, (size_t)len, "%.0f Hz", hz);
            else snprintf(out, (size_t)len, "%.2f kHz", hz / 1000.0f);
            return;
        }
        case TP_L1_RATE: case TP_L2_RATE: snprintf(out, (size_t)len, "%.2f Hz", 0.02f * powf(5000.0f, v)); return;
        case TP_PL_RATE: snprintf(out, (size_t)len, "%.2f Hz", 0.5f * powf(40.0f, v)); return;
        case TP_BEND: snprintf(out, (size_t)len, "%d st", (int)lroundf(v * 12.0f)); return;
        default: break;
        }
    }
    if (d->kind == PK_BIPOLAR) { snprintf(out, (size_t)len, "%+d", (int)lroundf((v - 0.5f) * 200.0f)); return; }
    snprintf(out, (size_t)len, "%d", (int)lroundf(v * 100.0f));
}

void aria_param_short(int p, char *out, int len)
{
    if (p < 0 || p >= ARIA_NPARAM) { snprintf(out, (size_t)len, "?"); return; }
    if (p < GP_FIRST) snprintf(out, (size_t)len, "%s%s", p < TP_COUNT ? "" : "B.", ARIA_TSHORT[p % TP_COUNT]);
    else snprintf(out, (size_t)len, "%s", ARIA_GSHORT[p - GP_FIRST]);
}
