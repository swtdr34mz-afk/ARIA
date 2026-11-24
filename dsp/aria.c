/* SPDX-License-Identifier: MPL-2.0
 * Copyright (C) 2026 swtdr34mz (ARIA) */
/* ARIA's sound engine (see aria.h). The render runs in chunks of CHUNK frames: the knobs, LFOs, glide and filter
 * coefficients move once a chunk, the oscillators, envelopes, filters and the delay every sample. */
#include "aria.h"
#include <math.h>
#include <stdlib.h>
#include <string.h>

#define CHUNK 16                         /* frames at the output rate */
#define MAXOS 16                         /* oversampling, at most */
#define PI_F 3.14159265358979f
#define DL_MAX_S 2.2f                    /* the delay's longest time, seconds */
#define NPRE 4                           /* the delay's diffusion: allpasses before the line, a side */
#define NLOOP 8                          /* and in its feedback, a side (24 lines in all) */
#define MONO_STACK 16
#define ARP_MAX 32                       /* notes the arpeggiator holds */
#define ARP_KEY 0x7FFF0                  /* the key its notes play under */

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
typedef struct { float y[4], ty[4], fb; } ladder_t;      /* a transistor ladder: the stages and their tanh */
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
    float dr1, dr2;                      /* the oscillators' drift (a slow random walk) */
    float mx_fb, dc_x, dc_y;             /* the mixer: OVERLOAD's feedback, the DC blocker */
    uint32_t seed;
    /* this chunk's */
    float inc1, inc2, incm, k1, k2, cut1, cut2, c_pw, c_amp, c_pan, c_fm;
} voice_t;

typedef struct {                         /* a timbre's LFOs and its mono note stack */
    float l1p, l2p, plp, l1sh, l2sh;
    float l1, l2, pl;                    /* the values this chunk */
    int nstack;
    struct { int key, note; float vel; } stack[MONO_STACK];
    uint32_t panflip;
} timbre_t;

typedef struct { float *b; int len, w; float base; } line_t;   /* an allpass's delay line */
typedef struct {                         /* the diffusion delay */
    float *buf[2];
    int len, w;
    float t[2];                          /* the time now, samples (smoothed) */
    float lp[2], bp[2];                  /* the CHARACTER filter (SVF) */
    float bw[2], hp[2];                  /* the loop's bandwidth (a vintage rack's) */
    line_t pre[2][NPRE], loop[2][NLOOP];
    float lfo[6];                        /* the chorus: six LFOs, round robin over the lines */
} delay_t;

typedef struct { float x[10], y[10]; } hb_t;   /* a half-band decimator (polyphase IIR), 2 to 1 */

struct aria {
    float sr;
    float p[ARIA_NPARAM];                /* the parameters */
    float ps[ARIA_NPARAM];               /* smoothed */
    voice_t v[ARIA_VOICES];
    timbre_t tb[ARIA_TIMBRES];
    delay_t dl;
    float hp_s[2];                       /* the low cut */
    int os;                              /* oversampling now: 1, 2, 4 */
    float vsr;                           /* the voices' rate (sr * os) */
    hb_t dec[4][ARIA_TIMBRES][2];        /* the cascade: [0] 2x to 1x .. [3] 16x to 8x */
    /* a chunk's voices between their two halves: the mixer's output, the filters' coefficients, the VCA and pan */
    struct { float x[CHUNK * MAXOS], g1[CHUNK * MAXOS], g2[CHUNK * MAXOS], amp[CHUNK * MAXOS], gl[CHUNK * MAXOS],
             gr[CHUNK * MAXOS]; } vs[ARIA_VOICES];
    uint32_t age, seed;
    int sustain;
    float bend, mw, press, bpm;
    /* the chord memory */
    int8_t chord[8]; int chord_n;
    int8_t map[8][128][8]; uint8_t map_n[8][128];
    int learning; int8_t learn[8]; int learn_n;
    int8_t last[8]; int last_n, last_key;   /* the last key and the notes it played (the screen) */
    int8_t held[128][9];                 /* each key held (id < 128): its count and the notes it plays */
    /* the arpeggiator */
    struct { int key, note; float vel; } arp[ARP_MAX];   /* the notes it holds, in the order played */
    int arp_n, arp_keys, arp_pos, arp_dir, arp_fresh, arp_cur, arp_on, arp_step;
    long arp_count;                      /* steps since the arp started (the pattern's position) */
    long arp_slot;
    double ppq, arp_off;                 /* where the clock is (quarter notes); the playing note's end */
    int host_play;
    int offline;                         /* the host renders (not in real time) */
    /* LOW SPLIT (after the PROPHET 67's) and HOLD, on the keys before everything else */
    int8_t ls_mapped[128], ls_note[128];  /* each key (by id): the note it plays (-1 none), the note it is */
    uint8_t ls_held[128], ls_latched[128];
    int ls_root;
    uint8_t h_down[128], h_latched[128];  /* HOLD: keys down, keys let go but held */
    float note_gain;                     /* the level of the notes being started (LOW SPLIT's BALANCE) */
    float obuf[2][CHUNK]; int opos;      /* the chunk rendered, and how much of it is given out */
};

static void hold_released(aria_t *a);
static void lowhold_released(aria_t *a);

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
    float was = a->p[p];
    a->p[p] = v;
    if (p == GP_HOLD && was > 0.5f && v < 0.5f) hold_released(a);
    if (p == GP_LS_HOLD && was > 0.5f && v < 0.5f) lowhold_released(a);
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
        for (int i = 0; i < NPRE; i++) { free(d->pre[c][i].b); d->pre[c][i].b = 0; }
        for (int i = 0; i < NLOOP; i++) { free(d->loop[c][i].b); d->loop[c][i].b = 0; }
    }
}
static void line_alloc(line_t *l, float ms, float sr)
{
    l->base = ms * 0.001f * sr;
    l->len = (int)(l->base + 0.003f * sr) + 4;          /* (room for the chorus) */
    l->b = (float *)calloc((size_t)l->len, sizeof(float));
    l->w = 0;
}
static void delay_alloc(delay_t *d, float sr)
{
    static const float PRE_MS[2][NPRE] = { { 1.3f, 2.9f, 4.1f, 6.7f }, { 1.7f, 3.1f, 4.7f, 7.3f } };
    static const float LOOP_MS[2][NLOOP] = { { 7.9f, 11.3f, 13.7f, 17.9f, 21.1f, 25.3f, 29.9f, 33.7f },
                                             { 8.3f, 10.9f, 14.3f, 18.7f, 22.3f, 24.1f, 31.1f, 35.9f } };
    delay_free(d);
    memset(d, 0, sizeof *d);
    d->len = (int)(DL_MAX_S * sr) + 8;
    for (int c = 0; c < 2; c++) {
        d->buf[c] = (float *)calloc((size_t)d->len, sizeof(float));
        d->t[c] = 0.3f * sr;
        for (int i = 0; i < NPRE; i++) line_alloc(&d->pre[c][i], PRE_MS[c][i], sr);
        for (int i = 0; i < NLOOP; i++) line_alloc(&d->loop[c][i], LOOP_MS[c][i], sr);
    }
    for (int i = 0; i < 6; i++) d->lfo[i] = (float)i / 6.0f;
}
static void delay_clear(delay_t *d)
{
    for (int c = 0; c < 2; c++) {
        if (d->buf[c]) memset(d->buf[c], 0, (size_t)d->len * sizeof(float));
        d->lp[c] = d->bp[c] = d->bw[c] = d->hp[c] = 0;
        for (int i = 0; i < NPRE; i++) if (d->pre[c][i].b) memset(d->pre[c][i].b, 0, (size_t)d->pre[c][i].len * sizeof(float));
        for (int i = 0; i < NLOOP; i++) if (d->loop[c][i].b) memset(d->loop[c][i].b, 0, (size_t)d->loop[c][i].len * sizeof(float));
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
    aria_t *a = (aria_t *)calloc(1, sizeof *a);
    if (!a) return 0;
    a->seed = 0x12345678u;
    a->bpm = 120;
    a->arp_cur = -1; a->arp_slot = -1;
    a->opos = CHUNK;
    memset(a->ls_mapped, -1, sizeof a->ls_mapped); a->ls_root = -1; a->note_gain = 1.0f;
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
    a->os = 0;                           /* (set at the next render) */
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
/* The transistor ladder after Huovilainen: four one-pole stages, each transistor pair's tanh, the feedback of the
 * last stage (averaged over two samples: the half-sample delay) with part of the input added back (the bass the
 * ladder loses to resonance, partly restored), run at the oversampled rate. g = 1 - exp(-2 pi fc / fs); k 0..1.4,
 * self-oscillating from about 1 (the knob: 0 to 1.4). hp: the four-pole high-pass from the same stages. */
static inline float ladder(ladder_t *f, float x, float g, float k, int hp)
{
    float fb = 0.5f * (f->y[3] + f->fb);
    f->fb = f->y[3];
    float u = tanh_f(x * (1.0f + 2.0f * k) - 4.0f * k * fb);
    f->y[0] += g * (u - f->ty[0]);       f->ty[0] = tanh_f(f->y[0]);
    f->y[1] += g * (f->ty[0] - f->ty[1]); f->ty[1] = tanh_f(f->y[1]);
    f->y[2] += g * (f->ty[1] - f->ty[2]); f->ty[2] = tanh_f(f->y[2]);
    f->y[3] += g * (f->ty[2] - f->ty[3]); f->ty[3] = tanh_f(f->y[3]);
    if (hp) return u - 4.0f * f->y[0] + 6.0f * f->y[1] - 4.0f * f->y[2] + f->y[3];
    return f->y[3] * (1.0f + 0.6f * k);  /* (resonance and self-oscillation up to the oscillators' level) */
}
static inline float ladder_g(float oct, float sr)       /* oct: the cutoff in octaves above 20 Hz */
{
    float hz = 20.0f * exp2f(clampf(oct, -1.0f, 10.2f));
    return 1.0f - expf(-2.0f * PI_F * clampf(hz, 5.0f, 0.45f * sr) / sr);
}

/* four voices' ladders at once (the same sums as ladder() above, lane by lane: the same sound, a quarter of the
 * work). SSE on x86 (every x64 CPU has it), NEON on ARM (the MPC), else plain C the compiler may vectorise. */
#if !defined(ARIA_NO_SIMD) && (defined(__SSE2__) || defined(_M_X64) || (defined(_M_IX86_FP) && _M_IX86_FP >= 2))
#include <emmintrin.h>
typedef __m128 v4;
#define V_SET(x) _mm_set1_ps(x)
#define V_LOAD(p) _mm_loadu_ps(p)
#define V_STORE(p, v) _mm_storeu_ps(p, v)
#define V_ADD _mm_add_ps
#define V_SUB _mm_sub_ps
#define V_MUL _mm_mul_ps
#define V_DIV _mm_div_ps
#define V_MIN _mm_min_ps
#define V_MAX _mm_max_ps
static inline v4 V_SEL(v4 m, v4 a, v4 b) { return _mm_or_ps(_mm_and_ps(m, a), _mm_andnot_ps(m, b)); }
static inline v4 V_MASK(const int *f) { return _mm_castsi128_ps(_mm_set_epi32(-(f[3] != 0), -(f[2] != 0), -(f[1] != 0), -(f[0] != 0))); }
#elif !defined(ARIA_NO_SIMD) && (defined(__ARM_NEON) || defined(__ARM_NEON__))
#include <arm_neon.h>
typedef float32x4_t v4;
#define V_SET(x) vdupq_n_f32(x)
#define V_LOAD(p) vld1q_f32(p)
#define V_STORE(p, v) vst1q_f32(p, v)
#define V_ADD vaddq_f32
#define V_SUB vsubq_f32
#define V_MUL vmulq_f32
#define V_MIN vminq_f32
#define V_MAX vmaxq_f32
#if defined(__aarch64__)
#define V_DIV vdivq_f32
#else
static inline v4 V_DIV(v4 a, v4 b)        /* (32-bit ARM has no vector divide: the estimate, refined twice) */
{
    v4 r = vrecpeq_f32(b);
    r = vmulq_f32(vrecpsq_f32(b, r), r);
    r = vmulq_f32(vrecpsq_f32(b, r), r);
    return vmulq_f32(a, r);
}
#endif
static inline v4 V_SEL(v4 m, v4 a, v4 b) { return vbslq_f32(vreinterpretq_u32_f32(m), a, b); }
static inline v4 V_MASK(const int *f)
{
    uint32_t u[4] = { f[0] ? 0xFFFFFFFFu : 0u, f[1] ? 0xFFFFFFFFu : 0u, f[2] ? 0xFFFFFFFFu : 0u, f[3] ? 0xFFFFFFFFu : 0u };
    return vreinterpretq_f32_u32(vld1q_u32(u));
}
#else
typedef struct { float f[4]; } v4;
static inline v4 V_SET(float x) { v4 r = { { x, x, x, x } }; return r; }
static inline v4 V_LOAD(const float *p) { v4 r = { { p[0], p[1], p[2], p[3] } }; return r; }
#define V_STORE(p, v) memcpy((p), (v).f, sizeof(float) * 4)
#define V_OP(name, e) static inline v4 name(v4 a, v4 b) { v4 r; for (int i = 0; i < 4; i++) { float x = a.f[i], y = b.f[i]; r.f[i] = (e); } return r; }
V_OP(V_ADD, x + y) V_OP(V_SUB, x - y) V_OP(V_MUL, x * y) V_OP(V_DIV, x / y) V_OP(V_MIN, x < y ? x : y) V_OP(V_MAX, x > y ? x : y)
static inline v4 V_SEL(v4 m, v4 a, v4 b) { v4 r; for (int i = 0; i < 4; i++) r.f[i] = m.f[i] != 0.0f ? a.f[i] : b.f[i]; return r; }
static inline v4 V_MASK(const int *f) { v4 r; for (int i = 0; i < 4; i++) r.f[i] = f[i] ? 1.0f : 0.0f; return r; }
#endif

static inline v4 tanh_v(v4 x)              /* tanh_f, four at once */
{
    x = V_MIN(V_MAX(x, V_SET(-3.0f)), V_SET(3.0f));
    v4 x2 = V_MUL(x, x);
    return V_DIV(V_MUL(x, V_ADD(V_SET(27.0f), x2)), V_ADD(V_SET(27.0f), V_MUL(V_MUL(V_SET(9.0f), x), x)));
}
typedef struct { v4 y0, y1, y2, y3, t0, t1, t2, t3, fb; } ladder4_t;
static void ladder4_load(ladder4_t *L, ladder_t *const f[4])
{
    float b[9][4];
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) { b[j][i] = f[i]->y[j]; b[4 + j][i] = f[i]->ty[j]; }
        b[8][i] = f[i]->fb;
    }
    L->y0 = V_LOAD(b[0]); L->y1 = V_LOAD(b[1]); L->y2 = V_LOAD(b[2]); L->y3 = V_LOAD(b[3]);
    L->t0 = V_LOAD(b[4]); L->t1 = V_LOAD(b[5]); L->t2 = V_LOAD(b[6]); L->t3 = V_LOAD(b[7]); L->fb = V_LOAD(b[8]);
}
static void ladder4_save(const ladder4_t *L, ladder_t *const f[4])
{
    float b[9][4];
    V_STORE(b[0], L->y0); V_STORE(b[1], L->y1); V_STORE(b[2], L->y2); V_STORE(b[3], L->y3);
    V_STORE(b[4], L->t0); V_STORE(b[5], L->t1); V_STORE(b[6], L->t2); V_STORE(b[7], L->t3); V_STORE(b[8], L->fb);
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) { f[i]->y[j] = b[j][i]; f[i]->ty[j] = b[4 + j][i]; }
        f[i]->fb = b[8][i];
    }
}
/* kin: 1 + 2k, k4: 4k, kout: 1 + 0.6k (as ladder() works them out) */
static inline v4 ladder4(ladder4_t *f, v4 x, v4 g, v4 kin, v4 k4, v4 kout, v4 hp)
{
    v4 fb = V_MUL(V_SET(0.5f), V_ADD(f->y3, f->fb));
    f->fb = f->y3;
    v4 u = tanh_v(V_SUB(V_MUL(x, kin), V_MUL(k4, fb)));
    f->y0 = V_ADD(f->y0, V_MUL(g, V_SUB(u, f->t0)));     f->t0 = tanh_v(f->y0);
    f->y1 = V_ADD(f->y1, V_MUL(g, V_SUB(f->t0, f->t1))); f->t1 = tanh_v(f->y1);
    f->y2 = V_ADD(f->y2, V_MUL(g, V_SUB(f->t1, f->t2))); f->t2 = tanh_v(f->y2);
    f->y3 = V_ADD(f->y3, V_MUL(g, V_SUB(f->t2, f->t3))); f->t3 = tanh_v(f->y3);
    v4 hpo = V_ADD(V_SUB(V_ADD(V_SUB(u, V_MUL(V_SET(4.0f), f->y0)), V_MUL(V_SET(6.0f), f->y1)), V_MUL(V_SET(4.0f), f->y2)), f->y3);
    return V_SEL(hp, hpo, V_MUL(f->y3, kout));
}

/* the half-band decimators: two in, one out (Laurent de Soras' polyphase IIR), one design a stage of the cascade,
 * each as short as its place allows while keeping 20 kHz flat and the images 100 dB down at 44.1 kHz:
 *   2x to 1x  10 coefficients (-106 dB)    4x to 2x  5 (-100 dB)    8x to 4x  5 (-121 dB)    16x to 8x  4 (-107 dB) */
static const float HB_2[10] = { 0.035572986325f, 0.132792135946f, 0.268223042129f, 0.415490782908f, 0.554392021337f,
                                0.674172987728f, 0.772192382352f, 0.850958174092f, 0.915586680728f, 0.972370292771f };
static const float HB_4[5] = { 0.043751628948f, 0.165966743828f, 0.346406252508f, 0.569034401011f, 0.837719989175f };
static const float HB_8[5] = { 0.034100817257f, 0.133628508836f, 0.293150164894f, 0.512380466010f, 0.808321839639f };
static const float HB_16[4] = { 0.046107119410f, 0.182583157163f, 0.410854348052f, 0.757068335712f };
static const float *const HB_C[4] = { HB_2, HB_4, HB_8, HB_16 };
static const int HB_N[4] = { 10, 5, 5, 4 };
static inline float hb_down(hb_t *h, const float *c, int nc, float in0, float in1)
{
    float a = in1, b = in0;
    int i = 0;
    for (; i + 1 < nc; i += 2) {
        float t = (a - h->y[i]) * c[i] + h->x[i]; h->x[i] = a; h->y[i] = t; a = t;
        t = (b - h->y[i + 1]) * c[i + 1] + h->x[i + 1]; h->x[i + 1] = b; h->y[i + 1] = t; b = t;
    }
    if (i < nc) { float t = (a - h->y[i]) * c[i] + h->x[i]; h->x[i] = a; h->y[i] = t; a = t; }
    return 0.5f * (a + b);
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
        v->mx_fb = v->dc_x = v->dc_y = 0;
        v->fe.lvl = v->ae.lvl = 0;
    }
    v->active = 1; v->gate = 1; v->held = 0;
    v->key = key; v->timbre = t; v->uni = uni;
    if (retrig || fresh) { v->vel = vel; v->gain = a->note_gain; }   /* (legato keeps the first note's: no jump) */
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

/* ------------------------------------------------------------ arpeggiator --- */
static const double DIV_Q[6] = { 1.0, 0.5, 1.0 / 3.0, 0.25, 1.0 / 6.0, 0.125 };   /* a step, in quarter notes */

static void arp_stop_note(aria_t *a)
{
    if (a->arp_cur >= 0) { release_key(a, ARP_KEY); a->arp_cur = -1; }
}
static void arp_clear(aria_t *a)
{
    arp_stop_note(a);
    a->arp_n = 0; a->arp_keys = 0;
}
static void arp_add(aria_t *a, int key, const int *notes, int n, float vel)
{
    if (a->p[GP_ARP_LATCH] > 0.5f && a->arp_keys == 0) { a->arp_n = 0; }      /* LATCH: a new chord after letting go */
    if (a->arp_n == 0) {
        a->arp_fresh = 1;
        a->arp_count = 0;
        if (!a->host_play) { a->ppq = 0; a->arp_slot = -1; }                  /* (on its own: from this key) */
    }
    a->arp_keys++;
    for (int i = 0; i < n; i++) {
        int dup = 0;
        for (int k = 0; k < a->arp_n; k++) if (a->arp[k].note == notes[i]) { dup = 1; break; }
        if (dup || a->arp_n >= ARP_MAX) continue;
        a->arp[a->arp_n].key = key; a->arp[a->arp_n].note = notes[i]; a->arp[a->arp_n].vel = vel;
        a->arp_n++;
    }
}
static void arp_remove(aria_t *a, int key)
{
    int had = 0;
    for (int k = 0; k < a->arp_n; k++) if (a->arp[k].key == key) { had = 1; break; }
    if (!had) return;
    if (a->arp_keys > 0) a->arp_keys--;
    if (a->p[GP_ARP_LATCH] > 0.5f) return;   /* (kept until the next chord) */
    int m = 0;
    for (int k = 0; k < a->arp_n; k++) if (a->arp[k].key != key) a->arp[m++] = a->arp[k];
    a->arp_n = m;
    if (!m) arp_stop_note(a);
}

/* the next note: its index in the held notes over the octaves (L of them) */
static int arp_next(aria_t *a, int L)
{
    int mode = (int)a->p[GP_ARP_MODE];
    if (a->arp_fresh) {
        a->arp_fresh = 0; a->arp_dir = 1;
        a->arp_pos = mode == ARP_DOWN ? L - 1 : mode == ARP_RANDOM ? (int)(rnd(&a->seed) % (uint32_t)L) : 0;
        return a->arp_pos;
    }
    switch (mode) {
    case ARP_DOWN: a->arp_pos = (a->arp_pos - 1 + L) % L; break;
    case ARP_UPDOWN:
        if (L == 1) { a->arp_pos = 0; break; }
        a->arp_pos += a->arp_dir;
        if (a->arp_pos >= L) { a->arp_pos = L - 2; a->arp_dir = -1; }
        if (a->arp_pos < 0) { a->arp_pos = 1; a->arp_dir = 1; }
        break;
    case ARP_RANDOM: {
        int p = (int)(rnd(&a->seed) % (uint32_t)L);
        if (L > 1 && p == a->arp_pos) p = (p + 1) % L;
        a->arp_pos = p;
        break;
    }
    default: a->arp_pos = (a->arp_pos + 1) % L; break;
    }
    return a->arp_pos;
}

/* a chunk of m frames: a step that falls in it plays, a note whose gate ends stops; the clock moves on */
static void arp_chunk(aria_t *a, int m)
{
    int on = a->p[GP_ARP_ON] > 0.5f;
    if (on != a->arp_on) { if (!on) arp_clear(a); a->arp_on = on; a->arp_slot = -1; }
    double step = DIV_Q[(int)a->p[GP_ARP_DIV]];
    double end = a->ppq + (double)m * a->bpm / 60.0 / a->sr;
    if (on && a->arp_n) {
        double sw = a->ps[GP_ARP_SWING] * 0.5;           /* the off-beat later by up to half a step */
        double pair = floor(a->ppq / (2.0 * step)), in = a->ppq - pair * 2.0 * step;
        long slot = (long)pair * 2 + (in >= step * (1.0 + sw) ? 1 : 0);
        double start = pair * 2.0 * step + ((slot & 1) ? step * (1.0 + sw) : 0.0);
        if (slot != a->arp_slot) {
            a->arp_slot = slot;
            arp_stop_note(a);
            /* the pattern: on the host's grid its step is the slot's, on its own it counts from the first key */
            int len = (int)a->p[GP_ARP_LEN] + 1;
            long c = a->host_play ? slot : a->arp_count++;
            a->arp_step = (int)(((c % len) + len) % len);
            if (a->p[GP_ARP_STEP1 + a->arp_step] < 0.5f) goto rest;    /* a rest: no note, the next one waits */
            /* the notes low to high (ORDER: as played), over the octaves */
            int idx[ARP_MAX], n = a->arp_n;
            for (int i = 0; i < n; i++) idx[i] = i;
            if ((int)a->p[GP_ARP_MODE] != ARP_ORDER)
                for (int i = 1; i < n; i++)
                    for (int j = i; j > 0 && a->arp[idx[j]].note < a->arp[idx[j - 1]].note; j--) { int t = idx[j]; idx[j] = idx[j - 1]; idx[j - 1] = t; }
            int oct = (int)a->p[GP_ARP_OCT] + 1, L = n * oct;
            int k = arp_next(a, L);
            int note = a->arp[idx[k % n]].note + 12 * (k / n);
            if (note <= 127) {
                a->note_gain = 1.0f;
                play_note(a, ARP_KEY, note, a->arp[idx[k % n]].vel);
                a->arp_cur = note;
            }
            float gate = a->ps[GP_ARP_GATE];
            double slen = step * ((slot & 1) ? 1.0 - sw : 1.0 + sw);
            a->arp_off = gate > 0.98f ? 1e30 : start + slen * (0.05 + 0.93 * gate);
        }
    rest:;
    }
    if (a->arp_cur >= 0 && (end >= a->arp_off || !a->arp_n || !on)) arp_stop_note(a);
    a->ppq = end;
}

void aria_transport(aria_t *a, float bpm, double ppq, int playing)
{
    if (bpm > 10.0f) a->bpm = bpm;
    if (playing) a->ppq = ppq;
    else if (a->host_play) { arp_stop_note(a); a->arp_slot = -1; }      /* (stopped: the arp's own clock again) */
    a->host_play = playing;
}
int aria_arp_note(const aria_t *a) { return a->arp_cur; }
void aria_offline(aria_t *a, int on) { a->offline = on; }
int aria_arp_step(const aria_t *a) { return (a->arp_on && a->arp_n) ? a->arp_step : -1; }

/* ------------------------------------------------------------------- notes --- */
static void core_note_on(aria_t *a, int id, int note, float vel)
{
    if (note < 0 || note > 127) return;
    vel = clampf(vel, 0.0f, 1.0f);
    if (a->learning == 1) learn_note(a, note);
    else if (a->learning == 2) {              /* KEY mode: this key takes the chord */
        int m = (int)a->p[GP_CH_MAP];
        aria_chord_set_key(a, m, note, a->learn, a->learn_n);
        a->learning = 0;
    }
    int out[8], n = expand(a, note, out);
    if (a->p[GP_ARP_ON] > 0.5f && !a->learning) arp_add(a, id, out, n, vel);
    else {
        release_key(a, id);                  /* (the same key again) */
        for (int i = 0; i < n; i++) play_note(a, id, out[i], vel);
    }
    a->last_n = n; a->last_key = note;
    if (id >= 0 && id < 128) { a->held[id][0] = (int8_t)n; for (int i = 0; i < n; i++) a->held[id][1 + i] = (int8_t)out[i]; }
    for (int i = 0; i < n; i++) a->last[i] = (int8_t)out[i];
}
static void core_note_off(aria_t *a, int id)
{
    if (id >= 0 && id < 128) a->held[id][0] = 0;
    if (a->arp_n) arp_remove(a, id);
    release_key(a, id);
}

/* ---------------------------------------------------------- LOW SPLIT, HOLD --- */
/* LOW SPLIT (the PROPHET 67's, src/KeySplit.h there): below POINT the keys go down SPLIT octaves (and to SCALE L);
 * above it to SCALE R, rooted on KEY, or (ROOT) on the note held on the left, or (WHITE) the scale laid on the white
 * keys from the point up; BALANCE between the two sides; LOW HOLD keeps the left side's notes until a new one is
 * played there. Then the chord memory, the arp and the voices as before. */
static const struct { int n; int st[7]; } SCALES[11] = {
    { 7, { 0, 2, 4, 5, 7, 9, 11 } }, { 7, { 0, 2, 3, 5, 7, 8, 10 } }, { 7, { 0, 2, 3, 5, 7, 9, 10 } },
    { 7, { 0, 1, 3, 5, 7, 8, 10 } }, { 7, { 0, 2, 4, 6, 7, 9, 11 } }, { 7, { 0, 2, 4, 5, 7, 9, 10 } },
    { 7, { 0, 2, 3, 5, 7, 8, 11 } }, { 5, { 0, 2, 4, 7, 9 } }, { 5, { 0, 3, 5, 7, 10 } }, { 6, { 0, 3, 5, 6, 7, 10 } },
    { 7, { 0, 1, 4, 5, 7, 8, 11 } } };
static int is_white(int n) { int c = n % 12; return c == 0 || c == 2 || c == 4 || c == 5 || c == 7 || c == 9 || c == 11; }
static int ls_snap(int n, int root, int scale)          /* to the scale's note at or below n (scale 1..11) */
{
    int rel = ((n - root) % 12 + 12) % 12, best = 0;
    for (int i = 0; i < SCALES[scale - 1].n; i++) if (SCALES[scale - 1].st[i] <= rel) best = SCALES[scale - 1].st[i];
    return n - rel + best;
}
static int ls_split(const aria_t *a) { return (int)a->p[GP_LS_SPLIT]; }
static int ls_point(const aria_t *a) { return 24 + (int)lroundf(a->p[GP_LS_POINT] * 72.0f); }
static int ls_left(const aria_t *a, int note) { return ls_split(a) > 0 && note < ls_point(a); }
static int ls_map(const aria_t *a, int note)
{
    int split = ls_split(a), sl = (int)a->p[GP_LS_SCALE_L], sr = (int)a->p[GP_LS_SCALE_R];
    int key = (int)a->p[GP_LS_KEY], follow = (int)a->p[GP_LS_FOLLOW];
    if (ls_left(a, note)) {
        int n = note - 12 * split;
        return sl ? ls_snap(n, key, sl) : n;
    }
    if (!sr) return note;
    int base = split > 0 && follow != LSF_KEY && a->ls_root >= 0 ? a->ls_root : key;
    if (split > 0 && follow == LSF_WHITE) {             /* the scale on the white keys from the split point up */
        if (!is_white(note)) return -1;
        int first = ls_point(a), d = 0, n = SCALES[sr - 1].n;
        while (!is_white(first)) first++;
        for (int j = first; j < note; j++) d += is_white(j);
        int b0 = first - (((first - base) % 12 + 12) % 12);
        if (first - b0 > 6) b0 += 12;
        return b0 + 12 * (d / n) + SCALES[sr - 1].st[d % n];
    }
    return ls_snap(note, base, sr);
}
static float ls_gain(const aria_t *a, int note)
{
    float b = (a->p[GP_LS_BALANCE] - 0.5f) * 2.0f;
    if (!ls_split(a)) return 1.0f;
    return ls_left(a, note) ? (b > 0.0f ? 1.0f - b : 1.0f) : (b < 0.0f ? 1.0f + b : 1.0f);
}
static void ls_root_update(aria_t *a)
{
    for (int n = 0; n < 128; n++)
        if (a->ls_mapped[n] >= 0 && ls_left(a, a->ls_note[n])) { a->ls_root = a->ls_mapped[n] % 12; return; }
}
static void ls_stop(aria_t *a, int id)
{
    if (a->ls_mapped[id] >= 0) core_note_off(a, id);
    a->ls_mapped[id] = -1;
    a->ls_latched[id] = 0;
}
static void ls_note_on(aria_t *a, int id, int note, float vel)   /* id: the key (0..127) */
{
    int lowhold = a->p[GP_LS_HOLD] > 0.5f;
    if (lowhold && ls_left(a, note)) {                /* a new note on the left, none held there: the held ones go */
        int any_held = 0, any_latched = 0;
        for (int n = 0; n < 128; n++) { any_held |= a->ls_held[n] && ls_left(a, a->ls_note[n]); any_latched |= a->ls_latched[n]; }
        if (!any_held && any_latched)
            for (int n = 0; n < 128; n++) if (a->ls_latched[n]) ls_stop(a, n);
    }
    if (a->ls_mapped[id] >= 0) ls_stop(a, id);
    a->ls_held[id] = 1;
    a->ls_note[id] = (int8_t)note;
    int m = ls_map(a, note);
    if (m < 0 || m > 127) return;
    a->ls_mapped[id] = (int8_t)m;
    a->note_gain = ls_gain(a, note);
    core_note_on(a, id, m, vel);
    a->note_gain = 1.0f;
    if (ls_left(a, note)) {
        if (lowhold) a->ls_latched[id] = 1;
        ls_root_update(a);
    }
}
static void ls_note_off(aria_t *a, int id)
{
    a->ls_held[id] = 0;
    if (a->ls_latched[id] && a->p[GP_LS_HOLD] > 0.5f) return;   /* LOW HOLD: it stays */
    ls_stop(a, id);
    ls_root_update(a);
}

/* HOLD: the keys let go keep sounding; once every key is up, the next key lets the held ones go first (a new chord
 * replaces the old); HOLD off lets go of what only it held */
void aria_note_on(aria_t *a, int id, int note, float vel)
{
    if (note < 0 || note > 127) return;
    if (id < 0 || id > 127) { core_note_on(a, id, note, vel); return; }   /* (not a key: as it is) */
    if (a->p[GP_HOLD] > 0.5f) {
        int any_down = 0, any_held = 0;
        for (int k = 0; k < 128; k++) { any_down |= a->h_down[k]; any_held |= a->h_latched[k]; }
        if (!any_down && any_held)
            for (int k = 0; k < 128; k++) if (a->h_latched[k]) { a->h_latched[k] = 0; ls_note_off(a, k); }
        a->h_latched[id] = 0;
    }
    a->h_down[id] = 1;
    ls_note_on(a, id, note, vel);
}
void aria_note_off(aria_t *a, int id)
{
    if (id < 0 || id > 127) { core_note_off(a, id); return; }
    a->h_down[id] = 0;
    if (a->p[GP_HOLD] > 0.5f) { a->h_latched[id] = 1; return; }
    ls_note_off(a, id);
}
static void hold_released(aria_t *a)                   /* HOLD off: what only it held lets go */
{
    for (int k = 0; k < 128; k++) if (a->h_latched[k]) { a->h_latched[k] = 0; if (!a->h_down[k]) ls_note_off(a, k); }
}
static void lowhold_released(aria_t *a)                /* LOW HOLD off: likewise */
{
    for (int k = 0; k < 128; k++) if (a->ls_latched[k] && !a->ls_held[k]) { ls_stop(a, k); }
    ls_root_update(a);
}

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
    memset(a->held, 0, sizeof a->held);
    memset(a->ls_mapped, -1, sizeof a->ls_mapped); memset(a->ls_held, 0, sizeof a->ls_held); memset(a->ls_latched, 0, sizeof a->ls_latched);
    memset(a->h_down, 0, sizeof a->h_down); memset(a->h_latched, 0, sizeof a->h_latched);
    arp_clear(a);
    for (int t = 0; t < ARIA_TIMBRES; t++) a->tb[t].nstack = 0;
    for (int i = 0; i < ARIA_VOICES; i++) {
        voice_t *v = &a->v[i];
        if (v->active && v->gate) { v->gate = 0; v->held = 0; env_gate(&v->fe, 0); env_gate(&v->ae, 0); }
    }
}
void aria_panic(aria_t *a)
{
    memset(a->held, 0, sizeof a->held);
    memset(a->ls_mapped, -1, sizeof a->ls_mapped); memset(a->ls_held, 0, sizeof a->ls_held); memset(a->ls_latched, 0, sizeof a->ls_latched);
    memset(a->h_down, 0, sizeof a->h_down); memset(a->h_latched, 0, sizeof a->h_latched); a->ls_root = -1;
    a->arp_n = a->arp_keys = 0; a->arp_cur = -1; a->arp_slot = -1;
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
    float sr = a->vsr;
    /* glide */
    if (P[TP_GLIDE] > 0.001f) {
        float tc = 0.002f + 2.0f * P[TP_GLIDE] * P[TP_GLIDE];
        v->pitch += (v->target - v->pitch) * (1.0f - expf(-(float)CHUNK / (tc * a->sr)));
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
    /* drift: a slow random walk each oscillator, as a discrete VCO wanders */
    v->dr1 = v->dr1 * 0.9985f + rndf(&v->seed) * 0.004f;
    v->dr2 = v->dr2 * 0.9985f + rndf(&v->seed) * 0.004f;
    float drift = 0.25f + det * 0.6f;
    float n1 = base + v->dr1 * drift + OCT_SEMI[(int)a->p[TP(t, TP_O1_OCT)]] + (P[TP_O1_FREQ] - 0.5f) * 14.0f + v->dt1 * det * 0.25f + m_o1;
    float n2 = base + v->dr2 * drift + OCT_SEMI[(int)a->p[TP(t, TP_O2_OCT)]] + (P[TP_O2_FREQ] - 0.5f) * 14.0f + v->dt2 * det * 0.25f + m_o2;
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
    v->k1 = P[TP_F1_RES] * 1.4f;               /* (self-oscillates from about 0.72) */
    v->k2 = P[TP_F2_RES] * 1.4f;
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

static inline float osc_wave(float p, float dt, float ts, float pw, float mix)
{
    float ts_part = 0.0f, pul = 0.0f;
    if (mix < 1.0f) {                    /* (only the shapes the mix hears) */
        float tri = 4.0f * fabsf(p - 0.5f) - 1.0f;
        if (ts > 0.0f) {
            float saw = 2.0f * p - 1.0f - blep(p, dt);
            ts_part = tri + (saw - tri) * ts;
        } else ts_part = tri;
    }
    if (mix > 0.0f) {
        float q = p - pw; if (q < 0) q += 1.0f;
        pul = (p < pw ? 1.0f : -1.0f) + blep(p, dt) - blep(q, dt);
    }
    return ts_part * (1.0f - mix) + pul * mix;
}

/* a voice's chunk, its first half: the mod osc, the oscillators, the mixer, the envelopes and the VCA, into the
 * voice's scratch (a->vs); the filters follow four voices at a time (voices_filter) */
static void voice_render(aria_t *a, voice_t *v, int n)
{
    float *X = a->vs[v - a->v].x, *G1s = a->vs[v - a->v].g1, *G2s = a->vs[v - a->v].g2;
    float *AMP = a->vs[v - a->v].amp, *GL = a->vs[v - a->v].gl, *GR = a->vs[v - a->v].gr;
    int t = v->timbre;
    const float *P = &a->ps[TP(t, 0)];
    const float *R = &a->p[TP(t, 0)];
    float sr = a->vsr;
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
    /* the CP3-style mixer: a summing amp that clips softly when driven, harder on its negative side (the even
     * harmonics), a DC offset (OVERLOAD's bias) for more asymmetry, and its own output fed back a little */
    float ov = P[TP_MX_OVERLOAD], drive = 1.0f + ov * ov * 7.0f, bias = 0.35f * ov;
    float ovn = 1.15f / powf(drive, 0.6f), ofb = 0.25f * ov;
    float cb = bias > 0.0f ? tanh_f(bias) : 0.0f;
    float dca = 1.0f - 2.0f * PI_F * 12.0f / sr;
    int audio_f = mo_audio && mo_f > 0.0f;
    float G1 = ladder_g(v->cut1, sr), G2 = ladder_g(v->cut2, sr);
    float finc[4], ainc[4];
    /* the envelopes tick once a frame of the output rate (oversampled or not: the same curves, and no float
     * precision lost to tiny steps at 16x) */
    const float esr = a->sr;
    const int os = a->os;
    finc[0] = 1.0f / (env_time(P[TP_FE_A]) * esr); finc[1] = 1.0f / (env_time(P[TP_FE_D]) * esr); finc[3] = 1.0f / (env_time(P[TP_FE_R]) * esr);
    ainc[0] = 1.0f / (env_time(P[TP_AE_A]) * esr); ainc[1] = 1.0f / (env_time(P[TP_AE_D]) * esr); ainc[3] = 1.0f / (env_time(P[TP_AE_R]) * esr);
    finc[2] = ainc[2] = 0;
    int floop = R[TP_FE_LOOP] > 0.5f, aloop = R[TP_AE_LOOP] > 0.5f;
    float velg = R[TP_AE_VEL] > 0.5f ? 0.2f + 0.8f * v->vel : 1.0f;
    float level = P[TP_VCA_LEVEL] * P[TP_VCA_LEVEL] * 1.2f * velg * (1.0f + v->c_amp) * v->gain;
    float pan0 = (P[TP_VCA_PAN] - 0.5f) * 2.0f + v->pan + v->c_pan;
    float pw1b = P[TP_O1_PW] + v->c_pw, pw2b = P[TP_O2_PW] + v->c_pw;
    float i1b = v->inc1, i2b = v->inc2, im = v->incm;
    float gl = 0.70710678f, gr = 0.70710678f;
    int ph = 0;                              /* (s % os) */

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
        float x = o1 * lo1 + o2 * lo2 + o1 * o2 * lr + m * lm;
        if (ln > 0.0f) x += rndf(&v->seed) * ln;
        float u = x * drive + bias + v->mx_fb * ofb;
        float y = (u > 0.0f ? tanh_f(u) : 0.8f * tanh_f(u * 1.25f)) - cb;
        v->mx_fb = y;
        y *= ovn;
        v->dc_y = y - v->dc_x + dca * v->dc_y; v->dc_x = y;   /* (the bias's DC out) */
        x = v->dc_y;
        /* the filters' coefficients (they run in voices_filter) */
        float g1 = G1, g2 = G2;
        if (audio_f) {
            if (fdest != DESTF_F2) g1 = ladder_g(v->cut1 + mo_f * mv, sr);
            if (fdest != DESTF_F1) g2 = ladder_g(v->cut2 + mo_f * mv, sr);
        }
        X[s] = x; G1s[s] = g1; G2s[s] = g2;
        /* the envelopes and the VCA */
        if (ph == 0) {
            env_tick(&v->fe, finc, P[TP_FE_S], P[TP_FE_CURVE], floop, v->gate);
            env_tick(&v->ae, ainc, P[TP_AE_S], P[TP_AE_CURVE], aloop, v->gate);
        }
        float ae = v->ae.lvl;
        float amp = ae * level;
        if (mo_vca > 0.0f) amp *= 1.0f - mo_vca * (mo_uni ? 1.0f - mv : 0.5f * (1.0f - mv));
        if (ph == 0 && (mo_pan != 0.0f || s == 0)) {         /* (the pan: once a frame of the output rate) */
            float pan = clampf(pan0 + mo_pan * mv, -1.0f, 1.0f);
            gl = sqrtf(0.5f * (1.0f - pan)); gr = sqrtf(0.5f * (1.0f + pan));
        }
        AMP[s] = amp; GL[s] = gl; GR[s] = gr;
        if (++ph == os) ph = 0;
    }
    if (v->ae.stage == ES_IDLE) { v->active = 0; v->gate = 0; v->held = 0; }
}

/* a chunk's voices, their second half: both filters, four voices at a time, then the VCA and the pan onto the
 * timbre's bus (as voice_render did them one by one, to the bit) */
static void voices_filter(aria_t *a, voice_t *const *vv, int nv, float bus[][2][CHUNK * MAXOS], int n)
{
    for (int b0 = 0; b0 < nv; b0 += 4) {
        int nl = nv - b0 < 4 ? nv - b0 : 4;
        ladder_t dummy[2][4];
        ladder_t *f1[4], *f2[4];
        voice_t *lane[4];
        int hp[4], ser[4], ste[4];
        float kin1[4], k41[4], ko1[4], kin2[4], k42[4], ko2[4];
        const float *X[4], *G1[4], *G2[4];
        static const float zero[CHUNK * MAXOS] = { 0 };
        memset(dummy, 0, sizeof dummy);
        for (int i = 0; i < 4; i++) {
            if (i < nl) {
                voice_t *v = vv[b0 + i];
                const float *R = &a->p[TP(v->timbre, 0)];
                int k = (int)(v - a->v);
                lane[i] = v; f1[i] = &v->f1; f2[i] = &v->f2;
                int order = (int)R[TP_F_ORDER];
                hp[i] = R[TP_F1_HP] > 0.5f; ser[i] = order == ORD_SERIAL; ste[i] = order == ORD_STEREO;
                kin1[i] = 1.0f + 2.0f * v->k1; k41[i] = 4.0f * v->k1; ko1[i] = 1.0f + 0.6f * v->k1;
                kin2[i] = 1.0f + 2.0f * v->k2; k42[i] = 4.0f * v->k2; ko2[i] = 1.0f + 0.6f * v->k2;
                X[i] = a->vs[k].x; G1[i] = a->vs[k].g1; G2[i] = a->vs[k].g2;
            } else {
                lane[i] = NULL; f1[i] = &dummy[0][i]; f2[i] = &dummy[1][i];
                hp[i] = 0; ser[i] = 1; ste[i] = 0;
                kin1[i] = k41[i] = ko1[i] = kin2[i] = k42[i] = ko2[i] = 0.0f;
                X[i] = G1[i] = G2[i] = zero;
            }
        }
        ladder4_t L1, L2;
        ladder4_load(&L1, f1); ladder4_load(&L2, f2);
        v4 Kin1 = V_LOAD(kin1), K41 = V_LOAD(k41), Ko1 = V_LOAD(ko1), Kin2 = V_LOAD(kin2), K42 = V_LOAD(k42), Ko2 = V_LOAD(ko2);
        static const int none[4] = { 0, 0, 0, 0 };
        v4 Hp = V_MASK(hp), Ser = V_MASK(ser), Ste = V_MASK(ste), No = V_MASK(none);
        float yl[4][CHUNK * MAXOS], yr[4][CHUNK * MAXOS];
        for (int s = 0; s < n; s++) {
            float xs[4] = { X[0][s], X[1][s], X[2][s], X[3][s] };
            float g1[4] = { G1[0][s], G1[1][s], G1[2][s], G1[3][s] }, g2[4] = { G2[0][s], G2[1][s], G2[2][s], G2[3][s] };
            v4 x = V_LOAD(xs);
            v4 o1 = ladder4(&L1, x, V_LOAD(g1), Kin1, K41, Ko1, Hp);
            v4 o2 = ladder4(&L2, V_SEL(Ser, o1, x), V_LOAD(g2), Kin2, K42, Ko2, No);
            v4 par = V_MUL(V_SET(0.65f), V_ADD(o1, o2));
            v4 l = V_SEL(Ser, o2, V_SEL(Ste, o1, par)), r = V_SEL(Ser, o2, V_SEL(Ste, o2, par));
            float lo[4], ro[4];
            V_STORE(lo, l); V_STORE(ro, r);
            for (int i = 0; i < 4; i++) { yl[i][s] = lo[i]; yr[i][s] = ro[i]; }
        }
        ladder4_save(&L1, f1); ladder4_save(&L2, f2);
        for (int i = 0; i < nl; i++) {
            voice_t *v = lane[i];
            int k = (int)(v - a->v);
            float *bl = bus[v->timbre][0], *br = bus[v->timbre][1];
            const float *AMP = a->vs[k].amp, *GL = a->vs[k].gl, *GR = a->vs[k].gr;
            for (int s = 0; s < n; s++) {
                bl[s] += yl[i][s] * AMP[s] * GL[s];
                br[s] += yr[i][s] * AMP[s] * GR[s];
            }
        }
    }
}

/* the diffusion delay: one chunk, wet out of the send in (in place).
 *   input  -> 4 allpasses a side (DIFFUSION smears the attack) -> the delay line
 *   loop   <- the line -> CHARACTER (low-pass below noon, high-pass above) -> 8 allpasses a side, their lengths
 *             swept by six slow LFOs round robin (CHORUS) -> a vintage rack's bandwidth -> soft clip -> FEEDBACK,
 *             a little of each side into the other
 * 24 delay lines in the diffusion network, as in the golden-era rack units it is after. */
static inline float ap_tick(line_t *l, float x, float g, float mod)
{
    float pos = (float)l->w - (l->base + mod);
    if (pos < 0) pos += (float)l->len;
    int i0 = (int)pos; float fr = pos - (float)i0;
    int i1 = i0 + 1 >= l->len ? 0 : i0 + 1;
    float d = l->b[i0] + (l->b[i1] - l->b[i0]) * fr;
    float y = -g * x + d;
    l->b[l->w] = x + g * y;
    if (++l->w >= l->len) l->w = 0;
    return y;
}

static void delay_render(aria_t *a, float *l, float *r, int n)
{
    delay_t *d = &a->dl;
    const float *P = a->ps;
    float sr = a->sr;
    float tl = 0.001f * powf(2000.0f, P[GP_DL_TIME_L]) * sr;
    float tr = a->p[GP_DL_LINK] > 0.5f ? tl : 0.001f * powf(2000.0f, P[GP_DL_TIME_R]) * sr;
    float tgt[2] = { clampf(tl, 4.0f, (float)d->len - 4.0f), clampf(tr, 4.0f, (float)d->len - 4.0f) };
    float fb = clampf(P[GP_DL_FB], 0.0f, 1.0f) * 0.98f, cross = 0.18f;
    float c = P[GP_DL_CHAR], hz; int hpm = c > 0.5f;
    if (hpm) hz = 20.0f * exp2f((c - 0.5f) * 2.0f * 6.3f);
    else hz = 200.0f * exp2f(c * 2.0f * 6.6f);
    float g = tan_f(PI_F * clampf(hz, 10.0f, 0.45f * sr) / sr), K = 1.414f;
    float hd = 1.0f / (1.0f + K * g + g * g);
    int flat = fabsf(c - 0.5f) < 0.01f;
    float diff = P[GP_DL_DIFF], gpre = diff * 0.62f, gloop = diff * 0.72f;
    float depth = P[GP_DL_MOD] * 0.0016f * sr;
    float bwa = 1.0f - expf(-2.0f * PI_F * 11000.0f / sr), hpa = 1.0f - expf(-2.0f * PI_F * 35.0f / sr);
    static const float LFO_HZ[6] = { 0.13f, 0.21f, 0.29f, 0.37f, 0.53f, 0.71f };
    float *io[2] = { l, r };
    for (int s = 0; s < n; s++) {
        float mods[6];
        for (int k = 0; k < 6; k++) {
            d->lfo[k] += LFO_HZ[k] / sr;
            if (d->lfo[k] >= 1.0f) d->lfo[k] -= 1.0f;
            mods[k] = depth > 0.0f ? (sin01(d->lfo[k]) + 1.0f) * 0.5f * depth : 0.0f;
        }
        float x[2];
        for (int ch = 0; ch < 2; ch++) {
            d->t[ch] += (tgt[ch] - d->t[ch]) * 0.0004f;
            float pos = (float)d->w - d->t[ch];
            if (pos < 0) pos += (float)d->len;
            int i0 = (int)pos; float fr = pos - (float)i0;
            int i1 = i0 + 1 >= d->len ? 0 : i0 + 1;
            float v = d->buf[ch][i0] + (d->buf[ch][i1] - d->buf[ch][i0]) * fr;
            if (!flat) {
                float hp = (v - (K + g) * d->lp[ch] - d->bp[ch]) * hd;
                float bp = g * hp + d->lp[ch];
                d->lp[ch] = g * hp + bp;
                float lp = g * bp + d->bp[ch];
                d->bp[ch] = g * bp + lp;
                v = hpm ? hp : lp;
            }
            if (gloop > 0.0f)
                for (int k = 0; k < NLOOP; k++) v = ap_tick(&d->loop[ch][k], v, gloop, mods[(k + ch * 3) % 6]);
            d->bw[ch] += (v - d->bw[ch]) * bwa; v = d->bw[ch];
            d->hp[ch] += (v - d->hp[ch]) * hpa; v -= d->hp[ch];
            x[ch] = tanh_f(v);
        }
        for (int ch = 0; ch < 2; ch++) {
            float in = io[ch][s];
            if (gpre > 0.0f) for (int k = 0; k < NPRE; k++) in = ap_tick(&d->pre[ch][k], in, gpre, 0.0f);
            d->buf[ch][d->w] = in + fb * ((1.0f - cross) * x[ch] + cross * x[1 - ch]);
        }
        if (++d->w >= d->len) d->w = 0;
        l[s] = x[0]; r[s] = x[1];
    }
}

/* one chunk of CHUNK frames: everything moves on a fixed grid, so the sound does not depend on the host's block
 * sizes (a note lands on the next grid point: within 16 frames) */
static void render_chunk(aria_t *a, float *outl, float *outr)
{
    float bus[ARIA_TIMBRES][2][CHUNK * MAXOS], ob[ARIA_TIMBRES][2][CHUNK], wl[CHUNK], wr[CHUNK];
    {
        const int m = CHUNK;
        /* the oversampling (a change: the voices' state starts over at the new rate) */
        static const int OS[NQ] = { 1, 2, 4, 16, 4 };
        int q = (int)a->p[GP_QUALITY], os = OS[q < 0 ? 0 : q >= NQ ? NQ - 1 : q];
        if (q == Q_RENDER && a->offline) os = 16;  /* (RENDER 16X: 4x while playing, 16x when the host renders) */
        if (os != a->os) {
            a->os = os; a->vsr = a->sr * (float)os;
            memset(a->dec, 0, sizeof a->dec);
        }
        int mi = m * os;
        /* the knobs move */
        for (int p = 0; p < ARIA_NPARAM; p++) a->ps[p] += (a->p[p] - a->ps[p]) * 0.12f;
        arp_chunk(a, m);
        for (int t = 0; t < ARIA_TIMBRES; t++) timbre_control(a, t);
        memset(bus, 0, sizeof bus);
        voice_t *vv[ARIA_VOICES];
        int nv = 0;
        for (int i = 0; i < ARIA_VOICES; i++) {
            voice_t *v = &a->v[i];
            if (!v->active) continue;
            voice_control(a, v);
            voice_render(a, v, mi);
            vv[nv++] = v;
        }
        voices_filter(a, vv, nv, bus, mi);
        /* back to the output rate */
        for (int t = 0; t < ARIA_TIMBRES; t++)
            for (int c = 0; c < 2; c++) {
                float *b = bus[t][c];
                int n = mi, st = 0;
                while ((1 << st) < os) st++;           /* the stages: from the top of the cascade down */
                for (int k = st - 1; k >= 1; k--) {
                    n /= 2;
                    for (int s = 0; s < n; s++) b[s] = hb_down(&a->dec[k][t][c], HB_C[k], HB_N[k], b[2 * s], b[2 * s + 1]);
                }
                if (os >= 2) for (int s = 0; s < m; s++) ob[t][c][s] = hb_down(&a->dec[0][t][c], HB_C[0], HB_N[0], b[2 * s], b[2 * s + 1]);
                else memcpy(ob[t][c], b, sizeof(float) * (size_t)m);
            }
        /* the delay and the output */
        float sa = a->p[GP_DL_A] > 0.5f ? 1.0f : 0.0f, sb = a->p[GP_DL_B] > 0.5f ? 1.0f : 0.0f;
        float mix = a->ps[GP_DL_MIX];
        for (int s = 0; s < m; s++) {
            wl[s] = ob[0][0][s] * sa + ob[1][0][s] * sb;
            wr[s] = ob[0][1][s] * sa + ob[1][1][s] * sb;
        }
        float dl[CHUNK], dr[CHUNK];
        memcpy(dl, wl, sizeof dl); memcpy(dr, wr, sizeof dr);
        delay_render(a, dl, dr, m);
        float vol = a->ps[GP_OUT_VOL] * a->ps[GP_OUT_VOL] * 2.0f * 0.5f;
        float hpa = 1.0f - expf(-2.0f * PI_F * 10.0f * exp2f(a->ps[GP_OUT_LOWCUT] * 6.0f) / a->sr);
        for (int s = 0; s < m; s++) {
            float dryl = ob[0][0][s] + ob[1][0][s] - wl[s], dryr = ob[0][1][s] + ob[1][1][s] - wr[s];
            float L = dryl + wl[s] * (1.0f - mix) + dl[s] * mix;
            float Rr = dryr + wr[s] * (1.0f - mix) + dr[s] * mix;
            a->hp_s[0] += (L - a->hp_s[0]) * hpa; L -= a->hp_s[0];
            a->hp_s[1] += (Rr - a->hp_s[1]) * hpa; Rr -= a->hp_s[1];
            L *= vol; Rr *= vol;
            outl[s] = 1.5f * tanh_f(L / 1.5f);
            outr[s] = 1.5f * tanh_f(Rr / 1.5f);
        }
    }
}

void aria_render(aria_t *a, float *outl, float *outr, int n)
{
    while (n > 0) {
        if (a->opos >= CHUNK) { render_chunk(a, a->obuf[0], a->obuf[1]); a->opos = 0; }
        int m = CHUNK - a->opos;
        if (m > n) m = n;
        memcpy(outl, a->obuf[0] + a->opos, sizeof(float) * (size_t)m);
        memcpy(outr, a->obuf[1] + a->opos, sizeof(float) * (size_t)m);
        a->opos += m; outl += m; outr += m; n -= m;
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

int aria_held_notes(const aria_t *a, int8_t out[16])
{
    int n = 0;
    for (int note = 0; note < 128 && n < 16; note++)
        for (int k = 0; k < 128; k++) {
            int c = a->held[k][0], hit = 0;
            for (int i = 0; i < c; i++) if (a->held[k][1 + i] == note) { hit = 1; break; }
            if (hit) { out[n++] = (int8_t)note; break; }
        }
    return n;
}
