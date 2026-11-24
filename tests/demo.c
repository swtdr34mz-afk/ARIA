/* ARIA: the engine's tests, a CPU check and the demo sounds (WAV files). cc -O2 demo.c ../dsp/aria.c ../dsp/params.c -lm */
#include "../dsp/aria.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <xmmintrin.h>

#define SR 48000
static int fails;
#define CHECK(c, msg) do { if (!(c)) { printf("FAIL: %s\n", msg); fails++; } else printf("ok:   %s\n", msg); } while (0)

typedef struct { float t, dur; int note; float vel; } ev_t;

static void wav(const char *path, const float *l, const float *r, int n)
{
    FILE *f = fopen(path, "wb");
    int bytes = n * 4;
    fwrite("RIFF", 1, 4, f); int x = 36 + bytes; fwrite(&x, 4, 1, f); fwrite("WAVEfmt ", 1, 8, f);
    x = 16; fwrite(&x, 4, 1, f); short s = 1; fwrite(&s, 2, 1, f); s = 2; fwrite(&s, 2, 1, f);
    x = SR; fwrite(&x, 4, 1, f); x = SR * 4; fwrite(&x, 4, 1, f); s = 4; fwrite(&s, 2, 1, f); s = 16; fwrite(&s, 2, 1, f);
    fwrite("data", 1, 4, f); fwrite(&bytes, 4, 1, f);
    for (int i = 0; i < n; i++) {
        short o[2] = { (short)lrintf(fmaxf(-1, fminf(1, l[i])) * 32000), (short)lrintf(fmaxf(-1, fminf(1, r[i])) * 32000) };
        fwrite(o, 2, 2, f);
    }
    fclose(f);
}

/* plays the events, returns the peak; writes the wav if path */
static float play(aria_t *a, const ev_t *ev, int nev, float secs, const char *path, int *bad)
{
    int n = (int)(secs * SR);
    float *l = malloc(sizeof(float) * n), *r = malloc(sizeof(float) * n);
    int on[64] = { 0 };
    float peak = 0;
    for (int pos = 0; pos < n; pos += 64) {
        float t = (float)pos / SR;
        for (int e = 0; e < nev; e++) {
            if (!on[e] && t >= ev[e].t) { aria_note_on(a, e, ev[e].note, ev[e].vel); on[e] = 1; }
            if (on[e] == 1 && t >= ev[e].t + ev[e].dur) { aria_note_off(a, e); on[e] = 2; }
        }
        int m = n - pos < 64 ? n - pos : 64;
        aria_render(a, l + pos, r + pos, m);
    }
    for (int i = 0; i < n; i++) {
        if (!isfinite(l[i]) || !isfinite(r[i])) { (*bad)++; l[i] = r[i] = 0; }
        peak = fmaxf(peak, fmaxf(fabsf(l[i]), fabsf(r[i])));
    }
    if (path) {                              /* (to -1 dBFS for listening) */
        float g = peak > 0 ? 0.89f / peak : 1;
        for (int i = 0; i < n; i++) { l[i] *= g; r[i] *= g; }
        wav(path, l, r, n);
    }
    free(l); free(r);
    return peak;
}

#define SET(p, v) aria_param(a, (p), (v))
#define A(p) TP(0, p)
#define Bt(p) TP(1, p)

int main(void)
{
    _mm_setcsr(_mm_getcsr() | 0x8040);       /* flush denormals (the plugin does the same) */
    aria_t *a = aria_new(SR);
    int bad = 0;

    /* ---- chord memory ---- */
    SET(GP_CH_ON, 1);
    aria_chord_learn(a);
    aria_note_on(a, 1, 60, 1); aria_note_on(a, 2, 63, 1); aria_note_on(a, 3, 67, 1);
    aria_note_off(a, 1); aria_note_off(a, 2); aria_note_off(a, 3);
    aria_chord_learn(a);
    CHECK(aria_chord_learning(a) == 0, "chord: learned (C minor)");
    aria_panic(a);
    aria_note_on(a, 10, 62, 1);
    float tmp[256], tmp2[256]; aria_render(a, tmp, tmp2, 256);
    CHECK(aria_voices_active(a) == 3, "chord: one key plays three voices");
    aria_note_off(a, 10); aria_panic(a);
    SET(GP_CH_KEY, 1); SET(GP_CH_MAP, 2);
    aria_chord_learn(a);
    aria_note_on(a, 1, 50, 1); aria_note_on(a, 2, 57, 1); aria_note_on(a, 3, 60, 1); aria_note_on(a, 4, 64, 1);
    aria_chord_learn(a);
    CHECK(aria_chord_learning(a) == 2, "chord KEY: waits for the key");
    aria_all_off(a); aria_panic(a);
    aria_note_on(a, 5, 48, 1);
    CHECK(aria_chord_learning(a) == 0, "chord KEY: key 48 took the chord");
    aria_panic(a);
    aria_note_on(a, 6, 48, 1); aria_render(a, tmp, tmp2, 256);
    CHECK(aria_voices_active(a) == 4, "chord KEY: key 48 plays its 4 notes");
    aria_panic(a);
    aria_note_on(a, 7, 49, 1); aria_render(a, tmp, tmp2, 256);
    CHECK(aria_voices_active(a) == 1, "chord KEY: a key without a chord plays its note");
    aria_panic(a);
    SET(GP_CH_MAP, 3); aria_note_on(a, 8, 48, 1); aria_render(a, tmp, tmp2, 256);
    CHECK(aria_voices_active(a) == 1, "chord KEY: another key map is empty");
    {
        int sz = aria_chord_state_size(); uint8_t *blob = malloc(sz);
        aria_chord_save(a, blob);
        aria_t *b = aria_new(SR);
        CHECK(aria_chord_load(b, blob, sz), "chord: state saved and loaded");
        aria_param(b, GP_CH_ON, 1); aria_param(b, GP_CH_KEY, 1); aria_param(b, GP_CH_MAP, 2);
        aria_note_on(b, 1, 48, 1); aria_render(b, tmp, tmp2, 256);
        CHECK(aria_voices_active(b) == 4, "chord: the loaded key map plays");
        aria_free(b); free(blob);
    }
    aria_panic(a); aria_defaults(a);

    /* ---- the keys held, for the chord screen ---- */
    {
        int8_t hn[16];
        aria_note_on(a, 60, 60, 1); aria_note_on(a, 64, 64, 1); aria_note_on(a, 67, 67, 1);
        CHECK(aria_held_notes(a, hn) == 3 && hn[0] == 60 && hn[2] == 67, "held: three keys, three notes");
        aria_note_off(a, 64);
        CHECK(aria_held_notes(a, hn) == 2, "held: one let go, two left");
        aria_panic(a);
        aria_param(a, GP_CH_ON, 1); aria_param(a, GP_CH_KEY, 1); aria_param(a, GP_CH_MAP, 2);
        aria_note_on(a, 48, 48, 1); aria_note_on(a, 30, 30, 1);
        CHECK(aria_held_notes(a, hn) == 5, "held: a key's chord (4) and a plain key (1)");
        aria_panic(a); aria_defaults(a);
    }

    /* ---- voices ---- */
    for (int i = 0; i < 10; i++) aria_note_on(a, 20 + i, 48 + i, 1);
    aria_render(a, tmp, tmp2, 256);
    CHECK(aria_voices_active(a) == 8, "8 voices, no more");
    aria_panic(a);
    SET(GP_VC_MODE, VC_SPLIT); SET(GP_VC_VOICES_A, 3);   /* A: 4 voices below C4, B: 4 above */
    for (int i = 0; i < 6; i++) aria_note_on(a, 40 + i, 40 + i, 1);
    aria_render(a, tmp, tmp2, 256);
    CHECK(aria_voices_active(a) == 4, "split: timbre A keeps to its 4 voices");
    aria_panic(a);
    SET(GP_VC_MODE, VC_STACK);
    aria_note_on(a, 1, 60, 1); aria_render(a, tmp, tmp2, 256);
    CHECK(aria_voices_active(a) == 2, "stack: one key, a voice of A and one of B");
    aria_panic(a); aria_defaults(a);
    SET(A(TP_UNISON), 1);
    aria_note_on(a, 1, 60, 1); aria_render(a, tmp, tmp2, 256);
    CHECK(aria_voices_active(a) == 8, "unison: one key, all voices");
    aria_panic(a); aria_defaults(a);

    /* ---- the demos ---- */
    /* 1. a wide pad: two detuned saws, the filters in stereo, slow envelopes, pan spread, the diffusion delay */
    {
        SET(A(TP_O1_TRISAW), 1); SET(A(TP_O2_TRISAW), 1); SET(A(TP_O2_FREQ), 0.5f + 0.08f / 14.0f); SET(A(TP_MX_O2), 0.75f);
        SET(A(TP_O2_OCT), OCT_16); SET(A(TP_MX_O2), 0.55f);
        SET(A(TP_F_ORDER), ORD_STEREO); SET(A(TP_F1_CUT), 0.52f); SET(A(TP_F2_CUT), 0.58f); SET(A(TP_F1_RES), 0.35f); SET(A(TP_F2_RES), 0.35f);
        SET(A(TP_F1_ENV), 0.68f); SET(A(TP_F2_ENV), 0.64f);
        SET(A(TP_FE_A), 0.62f); SET(A(TP_FE_D), 0.7f); SET(A(TP_FE_S), 0.4f); SET(A(TP_FE_R), 0.6f);
        SET(A(TP_AE_A), 0.58f); SET(A(TP_AE_S), 0.9f); SET(A(TP_AE_R), 0.62f);
        SET(A(TP_VCA_SPREAD), 0.8f); SET(A(TP_DETUNE), 0.45f);
        SET(A(TP_L2_AMT), 0.25f); SET(A(TP_L2_RATE), 0.25f); SET(A(TP_L2_DEST), LD_FILTERS);
        SET(GP_DL_MIX, 0.38f); SET(GP_DL_FB, 0.55f); SET(GP_DL_DIFF, 0.7f); SET(GP_DL_CHAR, 0.38f);
        ev_t ev[] = { { 0.0f, 3.6f, 50, .8f }, { 0.0f, 3.6f, 57, .8f }, { 0.0f, 3.6f, 60, .8f }, { 0.0f, 3.6f, 64, .8f },
                      { 4.0f, 3.6f, 48, .8f }, { 4.0f, 3.6f, 55, .8f }, { 4.0f, 3.6f, 59, .8f }, { 4.0f, 3.6f, 62, .8f },
                      { 8.0f, 4.0f, 45, .8f }, { 8.0f, 4.0f, 52, .8f }, { 8.0f, 4.0f, 55, .8f }, { 8.0f, 4.0f, 60, .8f }, { 8.0f, 4.0f, 64, .8f } };
        float pk = play(a, ev, sizeof ev / sizeof ev[0], 16, "out/ARIA-01-pad.wav", &bad);
        CHECK(pk > 0.01f && pk < 1.6f, "demo 1 pad: sound, no blow-up");
        aria_panic(a); aria_defaults(a);
    }
    /* 2. a bass: mono, square 16' and saw, OVERLOAD, a resonant pluck through both ladders in series, glide */
    {
        SET(A(TP_MONO), 1); SET(A(TP_GLIDE), 0.12f);
        SET(A(TP_O1_OCT), OCT_16); SET(A(TP_O1_MIX), 1); SET(A(TP_O1_PW), 0.5f);
        SET(A(TP_O2_TRISAW), 1); SET(A(TP_MX_O2), 0.7f); SET(A(TP_O2_FREQ), 0.5f + 0.06f / 14.0f);
        SET(A(TP_MX_OVERLOAD), 0.55f);
        SET(A(TP_F1_CUT), 0.22f); SET(A(TP_F1_RES), 0.55f); SET(A(TP_F1_ENV), 0.86f);
        SET(A(TP_F2_CUT), 0.6f); SET(A(TP_F_ORDER), ORD_SERIAL);
        SET(A(TP_FE_D), 0.38f); SET(A(TP_FE_S), 0.1f); SET(A(TP_FE_VEL), 1);
        SET(A(TP_AE_D), 0.5f); SET(A(TP_AE_S), 0.8f); SET(A(TP_AE_R), 0.2f);
        int seq[] = { 36, 36, 48, 36, 39, 36, 43, 41, 36, 36, 48, 36, 34, 34, 46, 38 };
        ev_t ev[32];
        for (int i = 0; i < 16; i++) { ev[i].t = i * 0.2f; ev[i].dur = (i % 4 == 3) ? 0.26f : 0.14f; ev[i].note = seq[i]; ev[i].vel = (i % 4 == 0) ? 1.0f : 0.6f; }
        for (int i = 0; i < 16; i++) { ev[16 + i] = ev[i]; ev[16 + i].t += 3.2f; }
        float pk = play(a, ev, 32, 7, "out/ARIA-02-bass.wav", &bad);
        CHECK(pk > 0.01f && pk < 1.6f, "demo 2 bass: sound, no blow-up");
        aria_panic(a); aria_defaults(a);
    }
    /* 3. FM keys: triangles, osc 2 a fifth above modulating osc 1, a bell-like decay */
    {
        SET(A(TP_O1_TRISAW), 0); SET(A(TP_O2_TRISAW), 0); SET(A(TP_O2_OCT), OCT_4); SET(A(TP_O2_FREQ), 0.5f + 7.0f / 14.0f * 0.0f);
        SET(A(TP_FM_AMT), 0.55f); SET(A(TP_FM_21), 1); SET(A(TP_MX_O2), 0.15f);
        SET(A(TP_F1_CUT), 0.55f); SET(A(TP_F1_ENV), 0.78f); SET(A(TP_F1_KB), KB_FULL);
        SET(A(TP_FE_D), 0.55f); SET(A(TP_FE_S), 0.0f); SET(A(TP_FE_R), 0.5f);
        SET(A(TP_AE_D), 0.72f); SET(A(TP_AE_S), 0.0f); SET(A(TP_AE_R), 0.6f);
        SET(A(TP_VCA_SPREAD), 0.5f);
        SET(GP_DL_MIX, 0.25f); SET(GP_DL_FB, 0.35f); SET(GP_DL_TIME_L, 0.7f); SET(GP_DL_TIME_R, 0.75f);
        int mel[] = { 72, 76, 79, 84, 83, 79, 76, 74, 72, 67, 71, 74 };
        ev_t ev[16];
        for (int i = 0; i < 12; i++) { ev[i].t = i * 0.3f; ev[i].dur = 0.25f; ev[i].note = mel[i]; ev[i].vel = 0.7f + 0.3f * (i % 2); }
        ev[12] = (ev_t){ 0.0f, 3.4f, 48, .6f }; ev[13] = (ev_t){ 1.8f, 1.6f, 55, .6f };
        float pk = play(a, ev, 14, 6, "out/ARIA-03-fm-keys.wav", &bad);
        CHECK(pk > 0.01f && pk < 1.6f, "demo 3 FM keys: sound, no blow-up");
        aria_panic(a); aria_defaults(a);
    }
    /* 4. the chord memory in KEY mode: white keys play the diatonic 7th and 9th chords of C, one finger */
    {
        static const int8_t CH[7][5] = { { 48, 55, 59, 62, 64 }, { 50, 57, 60, 64, 65 }, { 52, 59, 62, 64, 67 }, { 53, 60, 64, 67, 69 },
                                         { 55, 62, 65, 69, 71 }, { 57, 64, 67, 71, 72 }, { 59, 62, 65, 69, 72 } };
        static const int WK[7] = { 60, 62, 64, 65, 67, 69, 71 };
        for (int i = 0; i < 7; i++) aria_chord_set_key(a, 0, WK[i], CH[i], 5);
        SET(GP_CH_ON, 1); SET(GP_CH_KEY, 1); SET(GP_CH_MAP, 0);
        SET(A(TP_O1_TRISAW), 0.6f); SET(A(TP_O2_TRISAW), 1); SET(A(TP_MX_O2), 0.5f); SET(A(TP_O2_FREQ), 0.5f + 0.1f / 14.0f);
        SET(A(TP_F1_CUT), 0.45f); SET(A(TP_F1_RES), 0.25f); SET(A(TP_F1_ENV), 0.7f); SET(A(TP_F1_KB), KB_HALF);
        SET(A(TP_FE_A), 0.2f); SET(A(TP_FE_D), 0.6f); SET(A(TP_FE_S), 0.3f);
        SET(A(TP_AE_A), 0.25f); SET(A(TP_AE_S), 0.8f); SET(A(TP_AE_R), 0.5f);
        SET(A(TP_VCA_SPREAD), 0.7f); SET(A(TP_DETUNE), 0.35f);
        SET(GP_DL_MIX, 0.3f); SET(GP_DL_FB, 0.45f); SET(GP_DL_DIFF, 0.5f);
        int prog[] = { 69, 65, 60, 67, 69, 64, 65, 67 };
        ev_t ev[8];
        for (int i = 0; i < 8; i++) { ev[i].t = i * 1.0f; ev[i].dur = 0.85f; ev[i].note = prog[i]; ev[i].vel = 0.8f; }
        float pk = play(a, ev, 8, 10, "out/ARIA-04-chord-memory.wav", &bad);
        CHECK(pk > 0.01f && pk < 1.6f, "demo 4 chord memory: sound, no blow-up");
        aria_panic(a); aria_defaults(a);
    }
    /* 5. a sync lead: osc 2 synced to 1 and swept by the filter envelope's LFO... here LFO 1 on osc 2, glide, delay */
    {
        SET(A(TP_MONO), 1); SET(A(TP_GLIDE), 0.18f);
        SET(A(TP_O2_SYNC), 1); SET(A(TP_O2_OCT), OCT_4); SET(A(TP_O2_FREQ), 0.62f); SET(A(TP_MX_O1), 0.0f); SET(A(TP_MX_O2), 0.85f);
        SET(A(TP_L1_DEST), LD_OSC2); SET(A(TP_L1_AMT), 0.55f); SET(A(TP_L1_RATE), 0.3f); SET(A(TP_L1_WAVE), LW_TRI);
        SET(A(TP_F1_CUT), 0.7f); SET(A(TP_F1_RES), 0.2f);
        SET(A(TP_PL_AMT), 0.25f); SET(A(TP_MX_OVERLOAD), 0.3f);
        SET(GP_DL_MIX, 0.3f); SET(GP_DL_FB, 0.4f); SET(GP_DL_TIME_L, 0.72f); SET(GP_DL_TIME_R, 0.66f);
        int mel[] = { 64, 67, 69, 72, 71, 67, 64, 62 };
        ev_t ev[8];
        for (int i = 0; i < 8; i++) { ev[i].t = i * 0.45f; ev[i].dur = 0.42f; ev[i].note = mel[i]; ev[i].vel = 0.9f; }
        ev[7].dur = 1.5f;
        float pk = play(a, ev, 8, 7, "out/ARIA-05-sync-lead.wav", &bad);
        CHECK(pk > 0.01f && pk < 1.6f, "demo 5 sync lead: sound, no blow-up");
        aria_panic(a); aria_defaults(a);
    }
    CHECK(bad == 0, "no NaN or Inf in any demo");

    /* ---- CPU: 8 voices, stereo filters, the mod osc at audio rate into the filters, the delay ---- */
    {
        SET(A(TP_F_ORDER), ORD_STEREO); SET(A(TP_F1_RES), 0.5f); SET(A(TP_MX_O2), 0.5f);
        SET(A(TP_MO_AUDIO), 1); SET(A(TP_MO_FILTER), 0.3f); SET(A(TP_MO_PITCH), 0.1f);
        SET(GP_DL_MIX, 0.3f); SET(GP_DL_DIFF, 0.5f);
        for (int i = 0; i < 8; i++) aria_note_on(a, i, 48 + i * 3, 1);
        int secs = 20, n = secs * SR;
        float l[256], r[256];
        clock_t c0 = clock();
        for (int i = 0; i < n; i += 256) aria_render(a, l, r, 256);
        double used = (double)(clock() - c0) / CLOCKS_PER_SEC;
        printf("CPU:  %d s of audio (8 voices, worst case) in %.2f s: %.1f%% of one core\n", secs, used, 100.0 * used / secs);
    }
    aria_free(a);
    printf(fails ? "\n%d FAILED\n" : "\nall passed\n", fails);
    return fails != 0;
}
