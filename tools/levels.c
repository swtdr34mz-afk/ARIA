/* ARIA: evens out the factory sounds' loudness. Plays each (a chord, or a note for basses and leads), measures it
 * and writes tools/levels.json (each sound's VCA LEVEL, A and B) for tools/presets.py; and reports the peaks.
 *   cc -O2 -Idsp -o /tmp/levels tools/levels.c dsp/aria.c dsp/params.c dsp/presets.c -lm && /tmp/levels */
#include "aria.h"
#include "presets.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define SR 48000
static float measure(int i, float *peak)
{
    aria_t *a = aria_new(SR);
    float v[ARIA_NPARAM];
    aria_preset_values(i, v);
    for (int p = 0; p < ARIA_NPARAM; p++) aria_param(a, p, v[p]);
    aria_factory_keymaps(a);
    float tmp[512], tmp2[512];
    for (int k = 0; k < 60; k++) aria_render(a, tmp, tmp2, 512);       /* (the knobs settle) */
    const char *cat = ARIA_PRESETS[i].category;
    int notes[4] = { 48, 55, 60, 64 }, n = 4;
    if (!strcmp(cat, "BASS")) { notes[0] = 36; n = 1; }
    if (!strcmp(cat, "LEAD")) { notes[0] = 60; n = 1; }
    if (!strcmp(cat, "SPLIT")) { notes[0] = 40; notes[1] = 60; notes[2] = 64; notes[3] = 67; }
    if (v[GP_CH_ON] > 0.5f) { notes[0] = 60; n = 1; }
    for (int k = 0; k < n; k++) aria_note_on(a, notes[k], notes[k], 0.8f);
    int len = 2 * SR;
    float *l = malloc(sizeof(float) * len), *r = malloc(sizeof(float) * len);
    for (int s = 0; s < len; s += 256) aria_render(a, l + s, r + s, 256);
    /* the loudest 100 ms (so plucks and pads compare) */
    double best = 0; *peak = 0;
    const int win = SR / 10;
    for (int s0 = 0; s0 + win <= len; s0 += win / 4) {
        double e = 0;
        for (int s = s0; s < s0 + win; s++) e += 0.5 * (l[s] * l[s] + r[s] * r[s]);
        if (e > best) best = e;
    }
    for (int s = 0; s < len; s++) *peak = fmaxf(*peak, fmaxf(fabsf(l[s]), fabsf(r[s])));
    free(l); free(r); aria_free(a);
    return (float)sqrt(best / win);
}

int main(void)
{
    const float target = 0.15f;            /* the loudest 100 ms at about -16.5 dBFS RMS */
    FILE *f = fopen("tools/levels.json", "w");
    fprintf(f, "{\n");
    for (int i = 0; i < ARIA_NPRESETS; i++) {
        float v[ARIA_NPARAM], pk;
        aria_preset_values(i, v);
        float rms = measure(i, &pk);
        float g = rms > 1e-5f ? sqrtf(target / rms) : 1.0f;
        float la = fminf(1.0f, fmaxf(0.25f, v[TP(0, TP_VCA_LEVEL)] * g));
        int b = v[GP_VC_MODE] != VC_SINGLE;
        float lb = fminf(1.0f, fmaxf(0.25f, v[TP(1, TP_VCA_LEVEL)] * g));
        printf("%-16s %-6s rms %.3f peak %.2f  level A %.2f -> %.2f\n", ARIA_PRESETS[i].name, ARIA_PRESETS[i].category, rms, pk, v[TP(0, TP_VCA_LEVEL)], la);
        if (b) fprintf(f, "  \"%s\": [%.3f, %.3f]%s\n", ARIA_PRESETS[i].name, la, lb, i < ARIA_NPRESETS - 1 ? "," : "");
        else fprintf(f, "  \"%s\": [%.3f, null]%s\n", ARIA_PRESETS[i].name, la, i < ARIA_NPRESETS - 1 ? "," : "");
    }
    fprintf(f, "}\n");
    fclose(f);
    return 0;
}
