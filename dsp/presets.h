/* SPDX-License-Identifier: MPL-2.0
 * Copyright (C) 2026 swtdr34mz (ARIA) */
#pragma once
#include "aria.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct { int p; float v; } aria_set_t;          /* a parameter and its value; p -1 ends a list */
typedef struct { const char *name, *category; const aria_set_t *set; } aria_preset_t;
#define ARIA_NPRESETS 40
extern const aria_preset_t ARIA_PRESETS[ARIA_NPRESETS];
void aria_preset_values(int i, float *out);             /* every parameter's value in factory sound i (ARIA_NPARAM) */
void aria_factory_keymaps(aria_t *a);                   /* key maps 1-4: the chords of C major and C minor */

#ifdef __cplusplus
}
#endif
