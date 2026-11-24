/* ARIA: helpers for tools/muse2aria.py (loaded with ctypes): the parameters' defaults, and a sound's loudness (the
 * loudest 100 ms of a chord, as tools/levels.c measures the factory sounds).
 *   cc -O2 -shared -fPIC -Idsp -o /tmp/libmuse.so tools/muse_measure.c dsp/aria.c dsp/params.c dsp/params_data.c -lm */
#include "aria.h"
#include <math.h>
#include <stdlib.h>

int mm_nparam(void) { return ARIA_NPARAM; }
float mm_default(int p) { return aria_param_info(p)->def; }
float mm_max(int p) { return aria_param_max(p); }

float mm_measure(const float *v, int mono, float *peak)
{
    const int SR = 48000;
    aria_t *a = aria_new((float)SR);
    for (int p = 0; p < ARIA_NPARAM; p++) aria_param(a, p, v[p]);
    float tmp[512], tmp2[512];
    for (int k = 0; k < 60; k++) aria_render(a, tmp, tmp2, 512);
    int notes[4] = { 48, 55, 60, 64 }, n = mono ? 1 : 4;
    if (mono) notes[0] = 48;
    for (int k = 0; k < n; k++) aria_note_on(a, notes[k], notes[k], 0.8f);
    int len = (8 * SR / 256) * 256;     /* (8 s: slow attacks reach their top) */
    float *l = malloc(sizeof(float) * len), *r = malloc(sizeof(float) * len);
    for (int s = 0; s < len; s += 256) aria_render(a, l + s, r + s, 256);
    double best = 0; *peak = 0;
    const int win = SR / 10;
    for (int s0 = 0; s0 + win <= len; s0 += win / 4) {
        double e = 0;
        for (int s = s0; s < s0 + win; s++) e += 0.5 * (l[s] * l[s] + r[s] * r[s]);
        if (e > best) best = e;
    }
    for (int s = 0; s < len; s++) { float x = fmaxf(fabsf(l[s]), fabsf(r[s])); if (!(x == x)) x = 99; *peak = fmaxf(*peak, x); }
    free(l); free(r); aria_free(a);
    return (float)sqrt(best / win);
}
