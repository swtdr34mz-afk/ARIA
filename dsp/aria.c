/* SPDX-License-Identifier: MPL-2.0
 * Copyright (C) 2026 swtdr34mz (ARIA) */
/* ARIA's sound engine (see aria.h). The render runs in chunks of CHUNK frames: the knobs, LFOs, glide and filter
 * coefficients move once a chunk, the oscillators, envelopes, filters and the delay every sample. */
#include "aria.h"
#include <math.h>
#include <stdlib.h>
#include <string.h>

#define CHUNK 16
#define PI_F 3.14159265358979f
#define DL_MAX_S 2.2f                    /* the delay's longest time, seconds */
#define NAP 4                            /* the delay's diffusion: allpasses a side */
#define MONO_STACK 16

/* ------------------------------------------------------------------ helpers --- */
static inline float clampf(float x, float lo, float hi) { return x < lo ? lo : x > hi ? hi : x; }
static inline float tanh_f(float x)      /* a cheap tanh (to +-1 by |x| = 3) */
{
    x = clampf(x, -3.0f, 3.0f);
    return x * (27.0f + x * x) / (27.0f + 9.0f * x * x);
}
static inline float tan_f(float x)       /* tan for 0..1.5 (Pade) */
{
    float x2 = x * x;
    return x * (135135.0f - x2 * (17325.0f - x2 * (378.0f - x2))) /
           (135135.0f - x2 * (62370.0f - x2 * (3150.0f - 28.0f * x2)));
}
static inline float sin01(float p)       /* sin(2 pi p), p in 0..1 (a parabola, refined) */
{
    float x = (p < 0.5f ? p : p - 1.0f) * 2.0f;
    float y = 4.0f * x * (1.0f - fabsf(x));
    return y * (0.775f + 0.225f * fabsf(y));
}
static inline float blep(float t, float dt)   /* PolyBLEP: the step's correction at phase t */
{
    if (t < dt) { t /= dt; return t + t - t * t - 1.0f; }
    if (t > 1.0f - dt) { t = (t - 1.0f) / dt; return t * t + t + t + 1.0f; }
    return 0.0f;
}
static inline uint32_t rnd(uint32_t *s) { *s ^= *s << 13; *s ^= *s >> 17; *s ^= *s << 5; return *s; }
static inline float rndf(uint32_t *s) { return (float)(rnd(s) >> 8) * (2.0f / 16777216.0f) - 1.0f; }   /* -1..1 */

/* ---------------------------------------------------------------- the parts --- */
typedef struct { float s[4]; } ladder_t;
enum { ES_IDLE, ES_ATTACK, ES_DECAY, ES_SUSTAIN, ES_RELEASE };
typedef struct { int stage; float lvl, from, t; } env_t;

typedef struct {
    int active, gate, held, key, note, timbre;
    uint32_t age;
    float vel, gain;
    float pitch, target;                 /* semitones (glides to target) */
    float p1, p2, pm;                    /* phases */
    float o1, o2, mo;                    /* the last samples (FM, the mod osc's value) */
    float mo_sh;                         /* the mod osc's noise held (LFO rate) */
    ladder_t f1, f2;
    env_t fe, ae;
    float pan, uni;                      /* pan -1..1; unison offset -1..1 */
    float dt1, dt2, dtc;                 /* its own detune (osc 1, osc 2, cutoff), -1..1 */
    uint32_t seed;
    /* this chunk's */
    float inc1, inc2, incm, k1, k2, cut1, cut2, c_pw, c_amp, c_pan, c_fm, G1, G2;
} voice_t;

typedef struct {                         /* a timbre's LFOs and its mono note stack */
    float l1p, l2p, plp, l1sh, l2sh;
    float l1, l2, pl;                    /* the values this chunk */
    int nstack;
    struct { int key, note; float vel; } stack[MONO_STACK];
    uint32_t panflip;
} timbre_t;

typedef struct {                         /* the diffusion delay */
    float *buf[2];
    int len, w;
    float t[2];                          /* the time now, samples (smoothed) */
    float lp[2], bp[2];                  /* the CHARACTER filter (SVF) */
    float *ap[2][NAP];
    int aplen[2][NAP], apw[2][NAP];
} delay_t;

struct aria {
    float sr;
    float p[ARIA_NPARAM];                /* the parameters */
    float ps[ARIA_NPARAM];               /* smoothed */
    voice_t v[ARIA_VOICES];
    timbre_t tb[ARIA_TIMBRES];
    delay_t dl;
    float hp_s[2];                       /* the low cut */
    uint32_t age, seed;
    int sustain;
    float bend, mw, press, bpm;
    /* the chord memory */
    int8_t chord[8]; int chord_n;
    int8_t map[8][128][8]; uint8_t map_n[8][128];
    int learning; int8_t learn[8]; int learn_n;
    int8_t last[8]; int last_n, last_key;   /* the last key and the notes it played (the screen) */
};

/* ----------------------------------------------------------- the parameters --- */
static int smoothed(int p)               /* knobs move smoothly; choices and switches jump */
{
    const aria_param_t *i = aria_param_info(p);
    return i && (i->kind == PK_KNOB || i->kind == PK_BIPOLAR);
}

void aria_param(aria_t *a, int p, float v)
{
    if (p < 0 || p >= ARIA_NPARAM) return;
    float mx = aria_param_max(p);
    v = clampf(v, 0.0f, mx);
    const aria_param_t *i = aria_param_info(p);
    if (i->kind != PK_KNOB && i->kind != PK_BIPOLAR) v = floorf(v + 0.5f);
    a->p[p] = v;
    if (!smoothed(p)) a->ps[p] = v;
}
float aria_get(const aria_t *a, int p) { return (p >= 0 && p < ARIA_NPARAM) ? a->p[p] : 0.0f; }

void aria_defaults(aria_t *a)
{
    for (int p = 0; p < ARIA_NPARAM; p++) { a->p[p] = a->ps[p] = aria_param_info(p)->def; }
}

/* ------------------------------------------------------------------- delay --- */
static void delay_free(delay_t *d)
{
    for (int c = 0; c < 2; c++) {
        free(d->buf[c]); d->buf[c] = 0;
        for (int i = 0; i < NAP; i++) { free(d->ap[c][i]); d->ap[c][i] = 0; }
    }
}
static void delay_alloc(delay_t *d, float sr)
{
    static const float AP_MS[2][NAP] = { { 4.7f, 7.3f, 11.1f, 16.9f }, { 5.3f, 8.1f, 12.7f, 17.9f } };
    delay_free(d);
    d->len = (int)(DL_MAX_S * sr) + 8;
    d->w = 0;
    for (int c = 0; c < 2; c++) {
        d->buf[c] = calloc((size_t)d->len, sizeof(float));
        d->t[c] = 0.3f * sr;
        d->lp[c] = d->bp[c] = 0;
        for (int i = 0; i < NAP; i++) {
            d->aplen[c][i] = (int)(AP_MS[c][i] * 0.001f * sr) + 1;
            d->ap[c][i] = calloc((size_t)d->aplen[c][i], sizeof(float));
            d->apw[c][i] = 0;
        }
    }
}
static void delay_clear(delay_t *d)
{
    for (int c = 0; c < 2; c++) {
        if (d->buf[c]) memset(d->buf[c], 0, (size_t)d->len * sizeof(float));
        d->lp[c] = d->bp[c] = 0;
        for (int i = 0; i < NAP; i++) if (d->ap[c][i]) memset(d->ap[c][i], 0, (size_t)d->aplen[c][i] * sizeof(float));
    }
}

/* ----------------------------------------------------------------- set up --- */
static void voices_reset(aria_t *a)
{
    for (int i = 0; i < ARIA_VOICES; i++) {
        voice_t *v = &a->v[i];
        uint32_t s = 0x9E3779B9u * (uint32_t)(i + 1);
        memset(v, 0, sizeof *v);
        v->seed = s;
        v->dt1 = rndf(&s); v->dt2 = rndf(&s); v->dtc = rndf(&s);     /* each voice its own character */
        v->p1 = (rndf(&s) + 1) * 0.5f; v->p2 = (rndf(&s) + 1) * 0.5f;
        v->seed = s | 1u;
        v->note = -1;
    }
    for (int t = 0; t < ARIA_TIMBRES; t++) memset(&a->tb[t], 0, sizeof a->tb[t]);
}

aria_t *aria_new(float sr)
{
    aria_t *a = calloc(1, sizeof *a);
    if (!a) return 0;
    a->seed = 0x12345678u;
    a->bpm = 120;
    aria_defaults(a);
    aria_set_rate(a, sr);
    return a;
}
void aria_free(aria_t *a) { if (a) { delay_free(&a->dl); free(a); } }
void aria_set_rate(aria_t *a, float sr)
{
    a->sr = sr > 8000.0f ? sr : 44100.0f;
    voices_reset(a);
    delay_alloc(&a->dl, a->sr);
    a->hp_s[0] = a->hp_s[1] = 0;
}

/* ------------------------------------------------------------ the envelopes --- */
static void env_gate(env_t *e, int on)
{
    if (on) { e->stage = ES_ATTACK; e->from = e->lvl; e->t = 0; }
    else if (e->stage != ES_IDLE) { e->stage = ES_RELEASE; e->from = e->lvl; e->t = 0; }
}
static inline float env_time(float v) { return 0.0005f * powf(40000.0f, v); }   /* 0.5 ms .. 20 s */
/* one sample; inc: the stages' 1/(time*sr); c: the curve 0..1; loop: A-D-R-A while the gate is held */
static inline float env_tick(env_t *e, const float inc[4], float sus, float c, int loop, int gate)
{
    float pa = 1.0f + 2.0f * c, pd = 1.0f + 4.0f * c, u;
    switch (e->stage) {
    case ES_ATTACK:
        e->t += inc[0];
        if (e->t >= 1.0f) { e->lvl = 1.0f; e->stage = ES_DECAY; e->from = 1.0f; e->t = 0; break; }
        u = 1.0f - e->t; e->lvl = e->from + (1.0f - e->from) * (1.0f - powf(u, pa));
        break;
    case ES_DECAY: {
        float target = loop ? 0.0f : sus;
        e->t += inc[1];
        if (e->t >= 1.0f) {
            e->lvl = target;
            if (loop) { e->stage = ES_RELEASE; e->from = 0; e->t = 1; }
            else e->stage = ES_SUSTAIN;
            break;
        }
        u = 1.0f - e->t; e->lvl = target + (e->from - target) * powf(u, pd);
        break;
    }
    case ES_SUSTAIN:
        e->lvl += (sus - e->lvl) * 0.002f;
        break;
    case ES_RELEASE:
        e->t += inc[3];
        if (e->t >= 1.0f) {
            e->lvl = 0;
            if (loop && gate) { e->stage = ES_ATTACK; e->from = 0; e->t = 0; }
            else e->stage = ES_IDLE;
            break;
        }
        u = 1.0f - e->t; e->lvl = e->from * powf(u, pd);
        break;
    default: break;
    }
    return e->lvl;
}

/* ------------------------------------------------------------- the filters --- */
/* a 4-pole ladder, zero-delay feedback with a saturating input; G = g/(1+g); hp: the 4-pole high-pass from the
 * same stages (u - 4y1 + 6y2 - 4y3 + y4) */
static inline float ladder(ladder_t *f, float x, float G, float k, int hp)
{
    float b = 1.0f - G;
    float S = G * G * G * f->s[0] * b + G * G * f->s[1] * b + G * f->s[2] * b + f->s[3] * b;
    float G4 = G * G * G * G;
    float u = (x - k * S) / (1.0f + k * G4);
    u = tanh_f(u);
    float y[4], in = u;
    for (int i = 0; i < 4; i++) {
        float v = (in - f->s[i]) * G;
        y[i] = v + f->s[i];
        f->s[i] = y[i] + v;
        in = y[i];
    }
    if (hp) return (u - 4.0f * y[0] + 6.0f * y[1] - 4.0f * y[2] + y[3]);
    return y[3] * (1.0f + 0.3f * k);
}

/* ----------------------------------------------------------------- the LFOs --- */
static inline float lfo_rate(float v) { return 0.02f * powf(5000.0f, v); }   /* 0.02 .. 100 Hz */
static float lfo_wave(int w, float p, float sh)
{
    switch (w) {
    case LW_SINE: return sin01(p);
    case LW_TRI: return 1.0f - 4.0f * fabsf(p - 0.5f) ;
    case LW_SAW: return 1.0f - 2.0f * p;
    case LW_RAMP: return 2.0f * p - 1.0f;
    case LW_SQUARE: return p < 0.5f ? 1.0f : -1.0f;
    default: return sh;
    }
}

/* ------------------------------------------------------------------ voices --- */
static void pool(const aria_t *a, int t, int *lo, int *hi)
{
    int mode = (int)a->p[GP_VC_MODE];
    if (mode == VC_SINGLE) { *lo = 0; *hi = t == 0 ? ARIA_VOICES : 0; return; }
    int na = (int)a->p[GP_VC_VOICES_A] + 1;
    if (t == 0) { *lo = 0; *hi = na; } else { *lo = na; *hi = ARIA_VOICES; }
}

static void voice_start(aria_t *a, voice_t *v, int t, int key, int note, float vel, int retrig, float uni)
{
    const float *P = &a->p[TP(t, 0)];
    timbre_t *tb = &a->tb[t];
    int fresh = !v->active || v->timbre != t;
    if (!v->active) {
        memset(&v->f1, 0, sizeof v->f1); memset(&v->f2, 0, sizeof v->f2);
        v->fe.lvl = v->ae.lvl = 0;
    }
    v->active = 1; v->gate = 1; v->held = 0;
    v->key = key; v->timbre = t; v->uni = uni;
    if (retrig) v->vel = vel;            /* (legato keeps the first note's velocity: no jump) */
    v->age = ++a->age;
    v->target = (float)note;
    if (fresh || P[TP_GLIDE] < 0.001f || v->note < 0) v->pitch = (float)note;
    v->note = note;
    if (retrig) {
        env_gate(&v->fe, 1); env_gate(&v->ae, 1);
        if (P[TP_MO_KBRESET] > 0.5f) v->pm = 0;
        float sp = a->ps[TP(t, TP_VCA_SPREAD)];
        float side = (tb->panflip++ & 1u) ? -1.0f : 1.0f;
        v->pan = uni != 0.0f ? uni * sp : side * sp;
    }
}

static voice_t *voice_find(aria_t *a, int t)
{
    int lo, hi; pool(a, t, &lo, &hi);
    voice_t *best = 0;
    for (int i = lo; i < hi; i++) if (!a->v[i].active) { if (!best || a->v[i].age < best->age) best = &a->v[i]; }
    if (best) return best;
    for (int i = lo; i < hi; i++) {        /* a released one, the quietest */
        voice_t *v = &a->v[i];
        if (!v->gate && (!best || v->ae.lvl < best->ae.lvl)) best = v;
    }
    if (best) return best;
    for (int i = lo; i < hi; i++) if (!best || a->v[i].age < best->age) best = &a->v[i];   /* the oldest */
    return best;
}

static void mono_play(aria_t *a, int t, int legato)
{
    timbre_t *tb = &a->tb[t];
    const float *P = &a->p[TP(t, 0)];
    int lo, hi; pool(a, t, &lo, &hi);
    if (hi <= lo) return;
    int uni = P[TP_UNISON] > 0.5f;
    int n = uni ? hi - lo : 1;
    if (tb->nstack == 0) {                /* the last key let go */
        for (int i = lo; i < lo + n; i++) if (a->v[i].active && a->v[i].gate) {
            if (a->sustain) a->v[i].held = 1;
            else { a->v[i].gate = 0; env_gate(&a->v[i].fe, 0); env_gate(&a->v[i].ae, 0); }
        }
        return;
    }
    int key = tb->stack[tb->nstack - 1].key, note = tb->stack[tb->nstack - 1].note;
    float vel = tb->stack[tb->nstack - 1].vel;
    for (int i = 0; i < n; i++) {
        voice_t *v = &a->v[lo + i];
        float u = n > 1 ? (2.0f * i / (float)(n - 1) - 1.0f) : 0.0f;
        int retrig = !(legato && v->active && v->gate);
        voice_start(a, v, t, key, note, vel, retrig, u);
    }
}

static void timbre_note_on(aria_t *a, int t, int key, int note, float vel)
{
    const float *P = &a->p[TP(t, 0)];
    timbre_t *tb = &a->tb[t];
    if (P[TP_MONO] > 0.5f || P[TP_UNISON] > 0.5f) {
        int legato = tb->nstack > 0;
        if (tb->nstack == MONO_STACK) { memmove(tb->stack, tb->stack + 1, sizeof tb->stack[0] * (MONO_STACK - 1)); tb->nstack--; }
        tb->stack[tb->nstack].key = key; tb->stack[tb->nstack].note = note; tb->stack[tb->nstack].vel = vel;
        tb->nstack++;
        mono_play(a, t, legato);
        return;
    }
    voice_t *v = voice_find(a, t);
    if (v) voice_start(a, v, t, key, note, vel, 1, 0.0f);
}

static void play_note(aria_t *a, int key, int note, float vel)
{
    if (note < 0 || note > 127) return;
    int mode = (int)a->p[GP_VC_MODE];
    int split = (int)(a->p[GP_VC_SPLIT] * 127.0f + 0.5f);
    if (mode == VC_SINGLE) timbre_note_on(a, 0, key, note, vel);
    else if (mode == VC_SPLIT) timbre_note_on(a, note < split ? 0 : 1, key, note, vel);
    else { timbre_note_on(a, 0, key, note, vel); timbre_note_on(a, 1, key, note, vel); }
}

static void release_key(aria_t *a, int key)
{
    for (int t = 0; t < ARIA_TIMBRES; t++) {      /* mono stacks */
        timbre_t *tb = &a->tb[t];
        int n = 0, was = tb->nstack;
        for (int i = 0; i < tb->nstack; i++) if (tb->stack[i].key != key) tb->stack[n++] = tb->stack[i];
        if (n != was) { tb->nstack = n; mono_play(a, t, 1); }
    }
    for (int i = 0; i < ARIA_VOICES; i++) {
        voice_t *v = &a->v[i];
        const float *P = &a->p[TP(v->timbre, 0)];
        if (!v->active || !v->gate || v->key != key) continue;
        if (P[TP_MONO] > 0.5f || P[TP_UNISON] > 0.5f) continue;   /* (the stack decides) */
        if (a->sustain) { v->held = 1; continue; }
        v->gate = 0; env_gate(&v->fe, 0); env_gate(&v->ae, 0);
    }
}

/* ------------------------------------------------------------- chord memory --- */
static int expand(const aria_t *a, int note, int out[8])
{
    if (a->learning || a->p[GP_CH_ON] < 0.5f) { out[0] = note; return 1; }
    if (a->p[GP_CH_KEY] > 0.5f) {
        int m = (int)a->p[GP_CH_MAP];
        int n = a->map_n[m][note];
        if (!n) { out[0] = note; return 1; }
        for (int i = 0; i < n; i++) out[i] = a->map[m][note][i];
        return n;
    }
    if (!a->chord_n) { out[0] = note; return 1; }
    for (int i = 0; i < a->chord_n; i++) out[i] = note + a->chord[i];
    return a->chord_n;
}

static void learn_note(aria_t *a, int note)
{
    for (int i = 0; i < a->learn_n; i++) if (a->learn[i] == note) return;
    if (a->learn_n < 8) a->learn[a->learn_n++] = (int8_t)note;
}

void aria_chord_learn(aria_t *a)
{
    if (a->learning != 1) { a->learning = 1; a->learn_n = 0; return; }
    if (!a->learn_n) { a->learning = 0; return; }
    for (int i = 1; i < a->learn_n; i++)                  /* low to high */
        for (int j = i; j > 0 && a->learn[j] < a->learn[j - 1]; j--) { int8_t x = a->learn[j]; a->learn[j] = a->learn[j - 1]; a->learn[j - 1] = x; }
    if (a->p[GP_CH_KEY] > 0.5f) { a->learning = 2; return; }
    a->chord_n = a->learn_n;
    for (int i = 0; i < a->learn_n; i++) a->chord[i] = (int8_t)(a->learn[i] - a->learn[0]);
    a->learning = 0;
}
void aria_chord_cancel(aria_t *a) { a->learning = 0; a->learn_n = 0; }
int aria_chord_learning(const aria_t *a) { return a->learning; }
void aria_chord_clear_map(aria_t *a, int m)
{
    if (m < 0 || m > 7) return;
    memset(a->map_n[m], 0, sizeof a->map_n[m]);
}
void aria_chord_set_key(aria_t *a, int m, int key, const int8_t *notes, int n)
{
    if (m < 0 || m > 7 || key < 0 || key > 127) return;
    if (n > 8) n = 8;
    if (n < 0) n = 0;
    a->map_n[m][key] = (uint8_t)n;
    for (int i = 0; i < n; i++) a->map[m][key][i] = notes[i];
}
#define CH_MAGIC 0x31435241u              /* "ARC1" */
int aria_chord_state_size(void) { return 4 + 1 + 8 + 8 * 128 * 9; }
void aria_chord_save(const aria_t *a, uint8_t *o)
{
    uint32_t m = CH_MAGIC; memcpy(o, &m, 4); o += 4;
    *o++ = (uint8_t)a->chord_n;
    memcpy(o, a->chord, 8); o += 8;
    for (int k = 0; k < 8; k++) for (int n = 0; n < 128; n++) { *o++ = a->map_n[k][n]; memcpy(o, a->map[k][n], 8); o += 8; }
}
int aria_chord_load(aria_t *a, const uint8_t *in, int size)
{
    uint32_t m;
    if (size < aria_chord_state_size()) return 0;
    memcpy(&m, in, 4); if (m != CH_MAGIC) return 0;
    in += 4;
    a->chord_n = *in++ > 8 ? 8 : in[-1];
    memcpy(a->chord, in, 8); in += 8;
    for (int k = 0; k < 8; k++) for (int n = 0; n < 128; n++) {
        a->map_n[k][n] = *in > 8 ? 8 : *in; in++;
        memcpy(a->map[k][n], in, 8); in += 8;
    }
    return 1;
}

/* ------------------------------------------------------------------- notes --- */
void aria_note_on(aria_t *a, int id, int note, float vel)
{
    if (note < 0 || note > 127) return;
    vel = clampf(vel, 0.0f, 1.0f);
    if (a->learning == 1) learn_note(a, note);
    else if (a->learning == 2) {              /* KEY mode: this key takes the chord */
        int m = (int)a->p[GP_CH_MAP];
        aria_chord_set_key(a, m, note, a->learn, a->learn_n);
        a->learning = 0;
    }
    release_key(a, id);                      /* (the same key again) */
    int out[8], n = expand(a, note, out);
    for (int i = 0; i < n; i++) play_note(a, id, out[i], vel);
    a->last_n = n; a->last_key = note;
    for (int i = 0; i < n; i++) a->last[i] = (int8_t)out[i];
}
void aria_note_off(aria_t *a, int id) { release_key(a, id); }

void aria_sustain(aria_t *a, int on)
{
    a->sustain = on;
    if (on) return;
    for (int i = 0; i < ARIA_VOICES; i++) {
        voice_t *v = &a->v[i];
        if (v->active && v->held) { v->held = 0; v->gate = 0; env_gate(&v->fe, 0); env_gate(&v->ae, 0); }
    }
}
void aria_bend(aria_t *a, float v) { a->bend = clampf(v, -1, 1); }
void aria_modwheel(aria_t *a, float v) { a->mw = clampf(v, 0, 1); }
void aria_pressure(aria_t *a, float v) { a->press = clampf(v, 0, 1); }
void aria_tempo(aria_t *a, float bpm) { if (bpm > 10) a->bpm = bpm; }
void aria_all_off(aria_t *a)
{
    for (int t = 0; t < ARIA_TIMBRES; t++) a->tb[t].nstack = 0;
    for (int i = 0; i < ARIA_VOICES; i++) {
        voice_t *v = &a->v[i];
        if (v->active && v->gate) { v->gate = 0; v->held = 0; env_gate(&v->fe, 0); env_gate(&v->ae, 0); }
    }
}
void aria_panic(aria_t *a)
{
    voices_reset(a);
    delay_clear(&a->dl);
    a->sustain = 0;
}

int aria_voices_active(const aria_t *a)
{
    int n = 0;
    for (int i = 0; i < ARIA_VOICES; i++) n += a->v[i].active;
    return n;
}
float aria_voice_env(const aria_t *a, int i) { return (i >= 0 && i < ARIA_VOICES) ? a->v[i].ae.lvl : 0.0f; }

/* ------------------------------------------------------------------ render --- */
static const float OCT_SEMI[4] = { -12.0f, 0.0f, 12.0f, 24.0f };
static const float KB_AMT[3] = { 0.0f, 0.5f, 1.0f };

/* the chunk's numbers for a voice: pitch, glide, cutoffs */
static void voice_control(aria_t *a, voice_t *v)
{
    int t = v->timbre;
    const float *P = &a->ps[TP(t, 0)];
    const timbre_t *tb = &a->tb[t];
    float sr = a->sr;
    /* glide */
    if (P[TP_GLIDE] > 0.001f) {
        float tc = 0.002f + 2.0f * P[TP_GLIDE] * P[TP_GLIDE];
        v->pitch += (v->target - v->pitch) * (1.0f - expf(-CHUNK / (tc * sr)));
    } else v->pitch = v->target;

    /* LFOs */
    float l[2] = { tb->l1, tb->l2 }, amt[2] = { P[TP_L1_AMT], P[TP_L2_AMT] };
    int dest[2] = { (int)P[TP_L1_DEST], (int)P[TP_L2_DEST] };
    amt[0] = clampf(amt[0] + a->mw * 0.5f, 0, 1);
    float m_pitch = 0, m_o1 = 0, m_o2 = 0, m_f1 = 0, m_f2 = 0, m_pw = 0, m_amp = 0, m_pan = 0, m_mo = 0, m_fm = 0;
    for (int i = 0; i < 2; i++) {
        float x = l[i] * amt[i];
        switch (dest[i]) {
        case LD_PITCH: m_pitch += x * amt[i] * 12.0f; break;
        case LD_OSC1: m_o1 += x * amt[i] * 12.0f; break;
        case LD_OSC2: m_o2 += x * amt[i] * 12.0f; break;
        case LD_F1: m_f1 += x * 4.0f; break;
        case LD_F2: m_f2 += x * 4.0f; break;
        case LD_FILTERS: m_f1 += x * 4.0f; m_f2 += x * 4.0f; break;
        case LD_PW: m_pw += x * 0.45f; break;
        case LD_AMP: m_amp += (x - amt[i]) * 0.5f; break;      /* (down from full) */
        case LD_PAN: m_pan += x; break;
        case LD_MOFREQ: m_mo += x * 4.0f; break;
        case LD_FM: m_fm += x; break;
        default: break;
        }
    }
    float vib = tb->pl * (P[TP_PL_AMT] * P[TP_PL_AMT] * 2.0f + a->mw * 0.4f);
    float det = P[TP_DETUNE];
    float base = v->pitch + a->bend * P[TP_BEND] * 12.0f + m_pitch + vib + v->uni * det * 0.35f;
    float n1 = base + OCT_SEMI[(int)a->p[TP(t, TP_O1_OCT)]] + (P[TP_O1_FREQ] - 0.5f) * 14.0f + v->dt1 * det * 0.25f + m_o1;
    float n2 = base + OCT_SEMI[(int)a->p[TP(t, TP_O2_OCT)]] + (P[TP_O2_FREQ] - 0.5f) * 14.0f + v->dt2 * det * 0.25f + m_o2;
    float f1 = 440.0f * exp2f((n1 - 69.0f) / 12.0f), f2 = 440.0f * exp2f((n2 - 69.0f) / 12.0f);
    v->inc1 = clampf(f1 / sr, 0, 0.45f);
    v->inc2 = clampf(f2 / sr, 0, 0.45f);
    /* the mod osc */
    float mf;
    if (a->p[TP(t, TP_MO_AUDIO)] > 0.5f) {
        if (a->p[TP(t, TP_MO_KBTRK)] > 0.5f) mf = 440.0f * exp2f((v->pitch - 69.0f) / 12.0f + (P[TP_MO_FREQ] - 0.5f) * 8.0f);
        else mf = 16.0f * exp2f(P[TP_MO_FREQ] * 10.0f);
    } else mf = lfo_rate(P[TP_MO_FREQ]);
    mf *= exp2f(m_mo);
    v->incm = clampf(mf / sr, 0, 0.45f);
    /* cutoffs, in octaves above 20 Hz */
    float c1 = P[TP_F1_CUT], c2 = P[TP_F2_CUT];
    if (a->p[TP(t, TP_F_LINK)] > 0.5f) c1 = c2 + (c1 - 0.5f);
    float fe = v->fe.lvl * (a->p[TP(t, TP_FE_VEL)] > 0.5f ? (0.3f + 0.7f * v->vel) : 1.0f);
    float kb = (v->pitch - 60.0f) / 12.0f;
    float common = v->dtc * det * 0.15f + a->press * 1.5f;
    v->cut1 = c1 * 10.0f + (P[TP_F1_ENV] - 0.5f) * 2.0f * 9.0f * fe + kb * KB_AMT[(int)a->p[TP(t, TP_F1_KB)]] + m_f1 + common;
    v->cut2 = c2 * 10.0f + (P[TP_F2_ENV] - 0.5f) * 2.0f * 9.0f * fe + kb * KB_AMT[(int)a->p[TP(t, TP_F2_KB)]] + m_f2 + common;
    v->k1 = P[TP_F1_RES] * 4.3f;
    v->k2 = P[TP_F2_RES] * 4.3f;
    v->c_pw = m_pw; v->c_amp = m_amp; v->c_pan = m_pan; v->c_fm = m_fm;
}


/* the timbre's LFOs, a chunk on */
static void timbre_control(aria_t *a, int t)
{
    timbre_t *tb = &a->tb[t];
    const float *P = &a->ps[TP(t, 0)];
    float dt = (float)CHUNK / a->sr;
    tb->l1p += lfo_rate(P[TP_L1_RATE]) * dt;
    if (tb->l1p >= 1.0f) { tb->l1p -= floorf(tb->l1p); tb->l1sh = rndf(&a->seed); }
    tb->l2p += lfo_rate(P[TP_L2_RATE]) * dt;
    if (tb->l2p >= 1.0f) { tb->l2p -= floorf(tb->l2p); tb->l2sh = rndf(&a->seed); }
    tb->plp += 0.5f * powf(40.0f, P[TP_PL_RATE]) * dt;
    if (tb->plp >= 1.0f) tb->plp -= floorf(tb->plp);
    tb->l1 = lfo_wave((int)a->p[TP(t, TP_L1_WAVE)], tb->l1p, tb->l1sh);
    tb->l2 = lfo_wave((int)a->p[TP(t, TP_L2_WAVE)], tb->l2p, tb->l2sh);
    float s = P[TP_PL_SHAPE], si = sin01(tb->plp), tr = 1.0f - 4.0f * fabsf(tb->plp - 0.5f), sq = tb->plp < 0.5f ? 1.0f : -1.0f;
    tb->pl = s < 0.5f ? si + (tr - si) * s * 2.0f : tr + (sq - tr) * (s - 0.5f) * 2.0f;
}

static inline float filt_G(float oct, float sr)
{
    float hz = 20.0f * exp2f(clampf(oct, -1.0f, 10.2f));
    float w = PI_F * clampf(hz, 10.0f, 0.45f * sr) / sr;
    float g = tan_f(w);
    return g / (1.0f + g);
}

static inline float osc_wave(float p, float dt, float ts, float pw, float mix)
{
    float saw = 2.0f * p - 1.0f - blep(p, dt);
    float tri = 4.0f * fabsf(p - 0.5f) - 1.0f;
    float q = p - pw; if (q < 0) q += 1.0f;
    float pul = (p < pw ? 1.0f : -1.0f) + blep(p, dt) - blep(q, dt);
    return (tri + (saw - tri) * ts) * (1.0f - mix) + pul * mix;
}

/* a voice's chunk of samples, added to its timbre's bus */
static void voice_render(aria_t *a, voice_t *v, float *bl, float *br, int n)
{
    int t = v->timbre;
    const float *P = &a->ps[TP(t, 0)];
    const float *R = &a->p[TP(t, 0)];
    float sr = a->sr;
    /* this chunk */
    int mo_audio = R[TP_MO_AUDIO] > 0.5f, mo_wave = (int)R[TP_MO_WAVE], mo_uni = R[TP_MO_UNI] > 0.5f;
    int pdest = (int)R[TP_MO_PITCH_DEST], fdest = (int)R[TP_MO_FILTER_DEST];
    float mo_p = P[TP_MO_PITCH] * P[TP_MO_PITCH] * 4.0f, mo_f = P[TP_MO_FILTER] * 5.0f;
    float mo_pwm = P[TP_MO_PWM] * 0.45f, mo_vca = P[TP_MO_VCA], mo_pan = P[TP_MO_PAN];
    int fm21 = R[TP_FM_21] > 0.5f, fm12 = R[TP_FM_12] > 0.5f, sync = R[TP_O2_SYNC] > 0.5f;
    float idx = P[TP_FM_AMT] * P[TP_FM_AMT] * 3.0f + v->c_fm;
    float ts1 = P[TP_O1_TRISAW], ts2 = P[TP_O2_TRISAW], mx1 = P[TP_O1_MIX], mx2 = P[TP_O2_MIX];
    float lo1 = P[TP_MX_O1] * P[TP_MX_O1], lo2 = P[TP_MX_O2] * P[TP_MX_O2], lr = P[TP_MX_RING] * P[TP_MX_RING];
    float lm = P[TP_MX_MOD] * P[TP_MX_MOD], ln = P[TP_MX_NOISE] * P[TP_MX_NOISE];
    float ov = P[TP_MX_OVERLOAD], drive = 1.0f + ov * ov * 9.0f, bias = 0.3f * ov, tb0 = tanh_f(bias);
    float ovn = 1.0f / (0.6f * sqrtf(drive));
    int order = (int)R[TP_F_ORDER], hp1 = R[TP_F1_HP] > 0.5f;
    float k1 = v->k1, k2 = v->k2;
    int audio_f = mo_audio && mo_f > 0.0f;
    float G1 = filt_G(v->cut1, sr), G2 = filt_G(v->cut2, sr);
    float finc[4], ainc[4];
    finc[0] = 1.0f / (env_time(P[TP_FE_A]) * sr); finc[1] = 1.0f / (env_time(P[TP_FE_D]) * sr); finc[3] = 1.0f / (env_time(P[TP_FE_R]) * sr);
    ainc[0] = 1.0f / (env_time(P[TP_AE_A]) * sr); ainc[1] = 1.0f / (env_time(P[TP_AE_D]) * sr); ainc[3] = 1.0f / (env_time(P[TP_AE_R]) * sr);
    finc[2] = ainc[2] = 0;
    int floop = R[TP_FE_LOOP] > 0.5f, aloop = R[TP_AE_LOOP] > 0.5f;
    float velg = R[TP_AE_VEL] > 0.5f ? 0.2f + 0.8f * v->vel : 1.0f;
    float level = P[TP_VCA_LEVEL] * P[TP_VCA_LEVEL] * 1.2f * velg * (1.0f + v->c_amp);
    float pan0 = (P[TP_VCA_PAN] - 0.5f) * 2.0f + v->pan + v->c_pan;
    float pw1b = P[TP_O1_PW] + v->c_pw, pw2b = P[TP_O2_PW] + v->c_pw;
    float i1b = v->inc1, i2b = v->inc2, im = v->incm;

    for (int s = 0; s < n; s++) {
        /* the mod osc */
        v->pm += im;
        int mwrap = v->pm >= 1.0f;
        if (mwrap) v->pm -= 1.0f;
        float m;
        switch (mo_wave) {
        case MOW_SINE: m = sin01(v->pm); break;
        case MOW_SAW: m = 2.0f * v->pm - 1.0f - (mo_audio ? blep(v->pm, im) : 0.0f); break;
        case MOW_RAMP: m = 1.0f - 2.0f * v->pm + (mo_audio ? blep(v->pm, im) : 0.0f); break;
        case MOW_SQUARE: m = v->pm < 0.5f ? 1.0f : -1.0f; break;
        default:
            if (mo_audio) m = rndf(&v->seed);
            else { if (mwrap) v->mo_sh = rndf(&v->seed); m = v->mo_sh; }
            break;
        }
        float mv = mo_uni ? (m + 1.0f) * 0.5f : m;
        v->mo = m;
        /* the oscillators */
        float i1 = i1b, i2 = i2b;
        if (mo_p > 0.0f) {
            float mul = exp2f(mv * mo_p);
            if (pdest != DEST1_OSC2) i1 *= mul;
            if (pdest != DEST1_OSC1) i2 *= mul;
        }
        if (fm21) i1 *= 1.0f + idx * v->o2;
        if (fm12) i2 *= 1.0f + idx * v->o1;
        i1 = clampf(i1, 0.0f, 0.45f); i2 = clampf(i2, 0.0f, 0.45f);
        float pw1 = clampf(pw1b + mo_pwm * mv, 0.03f, 0.97f), pw2 = clampf(pw2b + mo_pwm * mv, 0.03f, 0.97f);
        v->p1 += i1;
        int wrap1 = v->p1 >= 1.0f;
        if (wrap1) v->p1 -= 1.0f;
        float o1 = osc_wave(v->p1, i1 > 1e-6f ? i1 : 1e-6f, ts1, pw1, mx1);
        v->p2 += i2;
        if (sync && wrap1 && i1 > 1e-6f) v->p2 = v->p1 / i1 * i2;
        if (v->p2 >= 1.0f) v->p2 -= floorf(v->p2);
        float o2 = osc_wave(v->p2, i2 > 1e-6f ? i2 : 1e-6f, ts2, pw2, mx2);
        v->o1 = o1; v->o2 = o2;
        /* the mixer */
        float x = o1 * lo1 + o2 * lo2 + o1 * o2 * lr + m * lm + rndf(&v->seed) * ln;
        if (ov > 0.0f) x = (tanh_f(x * drive * 0.6f + bias) - tb0) * ovn;
        x *= 0.7f;
        /* the filters */
        float g1 = G1, g2 = G2;
        if (audio_f) {
            if (fdest != DESTF_F2) g1 = filt_G(v->cut1 + mo_f * mv, sr);
            if (fdest != DESTF_F1) g2 = filt_G(v->cut2 + mo_f * mv, sr);
        }
        float yl, yr;
        if (order == ORD_SERIAL) { yl = ladder(&v->f2, ladder(&v->f1, x, g1, k1, hp1), g2, k2, 0); yr = yl; }
        else if (order == ORD_STEREO) { yl = ladder(&v->f1, x, g1, k1, hp1); yr = ladder(&v->f2, x, g2, k2, 0); }
        else { yl = 0.6f * (ladder(&v->f1, x, g1, k1, hp1) + ladder(&v->f2, x, g2, k2, 0)); yr = yl; }
        /* the envelopes and the VCA */
        env_tick(&v->fe, finc, P[TP_FE_S], P[TP_FE_CURVE], floop, v->gate);
        float ae = env_tick(&v->ae, ainc, P[TP_AE_S], P[TP_AE_CURVE], aloop, v->gate);
        float amp = ae * level;
        if (mo_vca > 0.0f) amp *= 1.0f - mo_vca * (mo_uni ? 1.0f - mv : 0.5f * (1.0f - mv));
        float pan = clampf(pan0 + mo_pan * mv, -1.0f, 1.0f);
        float gl = sqrtf(0.5f * (1.0f - pan)), gr = sqrtf(0.5f * (1.0f + pan));
        bl[s] += yl * amp * gl;
        br[s] += yr * amp * gr;
    }
    if (v->ae.stage == ES_IDLE) { v->active = 0; v->gate = 0; v->held = 0; }
}

/* the diffusion delay: one chunk, wet out of the send in (in place) */
static void delay_render(aria_t *a, float *l, float *r, int n)
{
    delay_t *d = &a->dl;
    const float *P = a->ps;
    float sr = a->sr;
    float tl = 0.001f * powf(2000.0f, P[GP_DL_TIME_L]) * sr;
    float tr = a->p[GP_DL_LINK] > 0.5f ? tl : 0.001f * powf(2000.0f, P[GP_DL_TIME_R]) * sr;
    float tgt[2] = { clampf(tl, 4.0f, (float)d->len - 4.0f), clampf(tr, 4.0f, (float)d->len - 4.0f) };
    float fb = clampf(P[GP_DL_FB] * 1.0f, 0.0f, 0.99f);
    float c = P[GP_DL_CHAR], hz; int hpm = c > 0.5f;
    if (hpm) hz = 20.0f * exp2f((c - 0.5f) * 2.0f * 6.3f);
    else hz = 200.0f * exp2f(c * 2.0f * 6.6f);
    float g = tan_f(PI_F * clampf(hz, 10.0f, 0.45f * sr) / sr), K = 1.414f;
    float hd = 1.0f / (1.0f + K * g + g * g);
    int flat = fabsf(c - 0.5f) < 0.01f;
    float apg = P[GP_DL_DIFF] * 0.7f;
    float *io[2] = { l, r };
    for (int s = 0; s < n; s++) {
        float out[2];
        for (int ch = 0; ch < 2; ch++) {
            d->t[ch] += (tgt[ch] - d->t[ch]) * 0.0004f;
            float pos = (float)d->w - d->t[ch];
            if (pos < 0) pos += (float)d->len;
            int i0 = (int)pos; float fr = pos - (float)i0;
            int i1 = i0 + 1 >= d->len ? 0 : i0 + 1;
            float x = d->buf[ch][i0] + (d->buf[ch][i1] - d->buf[ch][i0]) * fr;
            if (!flat) {                    /* CHARACTER: low-pass below noon, high-pass above */
                float hp = (x - (K + g) * d->lp[ch] - d->bp[ch]) * hd;
                float bp = g * hp + d->lp[ch];
                d->lp[ch] = g * hp + bp;
                float lp = g * bp + d->bp[ch];
                d->bp[ch] = g * bp + lp;
                x = hpm ? hp : lp;
            }
            if (apg > 0.0f) for (int k = 0; k < NAP; k++) {
                float *b = d->ap[ch][k]; int w = d->apw[ch][k];
                float dd = b[w], y = -apg * x + dd;
                b[w] = x + apg * y;
                d->apw[ch][k] = w + 1 >= d->aplen[ch][k] ? 0 : w + 1;
                x = y;
            }
            x = tanh_f(x);
            d->buf[ch][d->w] = io[ch][s] + fb * x;
            out[ch] = x;
        }
        if (++d->w >= d->len) d->w = 0;
        l[s] = out[0]; r[s] = out[1];
    }
}

void aria_render(aria_t *a, float *outl, float *outr, int n)
{
    float bus[ARIA_TIMBRES][2][CHUNK], wl[CHUNK], wr[CHUNK];
    while (n > 0) {
        int m = n < CHUNK ? n : CHUNK;
        /* the knobs move */
        for (int p = 0; p < ARIA_NPARAM; p++) a->ps[p] += (a->p[p] - a->ps[p]) * 0.12f;
        for (int t = 0; t < ARIA_TIMBRES; t++) timbre_control(a, t);
        memset(bus, 0, sizeof bus);
        for (int i = 0; i < ARIA_VOICES; i++) {
            voice_t *v = &a->v[i];
            if (!v->active) continue;
            voice_control(a, v);
            voice_render(a, v, bus[v->timbre][0], bus[v->timbre][1], m);
        }
        /* the delay and the output */
        float sa = a->p[GP_DL_A] > 0.5f ? 1.0f : 0.0f, sb = a->p[GP_DL_B] > 0.5f ? 1.0f : 0.0f;
        float mix = a->ps[GP_DL_MIX];
        for (int s = 0; s < m; s++) {
            wl[s] = bus[0][0][s] * sa + bus[1][0][s] * sb;
            wr[s] = bus[0][1][s] * sa + bus[1][1][s] * sb;
        }
        float dl[CHUNK], dr[CHUNK];
        memcpy(dl, wl, sizeof dl); memcpy(dr, wr, sizeof dr);
        delay_render(a, dl, dr, m);
        float vol = a->ps[GP_OUT_VOL] * a->ps[GP_OUT_VOL] * 2.0f * 0.3f;
        float hpa = 1.0f - expf(-2.0f * PI_F * 10.0f * exp2f(a->ps[GP_OUT_LOWCUT] * 6.0f) / a->sr);
        for (int s = 0; s < m; s++) {
            float dryl = bus[0][0][s] + bus[1][0][s] - wl[s], dryr = bus[0][1][s] + bus[1][1][s] - wr[s];
            float L = dryl + wl[s] * (1.0f - mix) + dl[s] * mix;
            float Rr = dryr + wr[s] * (1.0f - mix) + dr[s] * mix;
            a->hp_s[0] += (L - a->hp_s[0]) * hpa; L -= a->hp_s[0];
            a->hp_s[1] += (Rr - a->hp_s[1]) * hpa; Rr -= a->hp_s[1];
            L *= vol; Rr *= vol;
            outl[s] = 1.5f * tanh_f(L / 1.5f);
            outr[s] = 1.5f * tanh_f(Rr / 1.5f);
        }
        outl += m; outr += m; n -= m;
    }
}

int aria_chord_get_key(const aria_t *a, int m, int key, int8_t out[8])
{
    if (m < 0 || m > 7 || key < 0 || key > 127) return 0;
    int n = a->map_n[m][key];
    for (int i = 0; i < n; i++) out[i] = a->map[m][key][i];
    return n;
}
int aria_last_played(const aria_t *a, int8_t out[8], int *key)
{
    for (int i = 0; i < a->last_n; i++) out[i] = a->last[i];
    if (key) *key = a->last_n ? a->last_key : -1;
    return a->last_n;
}
int aria_chord_learned(const aria_t *a, int8_t out[8])
{
    if (a->learning) { for (int i = 0; i < a->learn_n; i++) out[i] = a->learn[i]; return a->learn_n; }
    for (int i = 0; i < a->chord_n; i++) out[i] = (int8_t)(60 + a->chord[i]);
    return a->chord_n;
}
