// SPDX-License-Identifier: MPL-2.0
// Copyright (C) 2026 swtdr34mz (ARIA)
//
// ARIA for the MPC: the same engine as the VST3 (dsp/), hosted by the MPC Plugin SDK's framework (mpc_plugin.h).
// mpc/build.py copies the engine beside this file (its C as C++) and generates params_data.cpp before the SDK builds it.
//
// Parameters (their ids never change; new ones only at the end):
//   0        VOLUME (the engine's OUTPUT VOLUME; the host nudges parameter 0 on every prepare: harmless, it glides)
//   1..      every other parameter of the engine, in the engine's order (dsp/aria.h), ids "p<engine index>"
//   then     INIT (a press: timbres A and B back to the INIT sound; arp, chord memory, delay, volume stay) and
//            LEARN (a press: the chord memory's LEARN, as on the VST's panel)
//
// The chord memory's learned chord and its 8 key maps go with the project (the framework's extra state).
// MIDI in: notes and velocity, pitch bend, channel and poly pressure, CC 1 mod wheel, CC 64 sustain, CC 120 all sound
// off, CC 121 reset controllers, CC 123 all notes off. Tempo and position: the MPC's (the arpeggiator's grid).

#include "mpc_plugin.h"

extern "C" {
#include "aria.h"
#include "presets.h"
}

#include <atomic>
#include <cmath>
#include <cstring>
#include <vector>

namespace {

/* the MPC's index of an engine parameter, and back */
inline int mpcIndex(int p) { return p == GP_OUT_VOL ? 0 : p < GP_OUT_VOL ? p + 1 : p; }
inline int engineIndex(int i) { return i == 0 ? GP_OUT_VOL : i <= GP_OUT_VOL ? i - 1 : i; }
constexpr int kInit = ARIA_NPARAM, kLearn = ARIA_NPARAM + 1, kNumParams = ARIA_NPARAM + 2;

class AriaMpc : public mpc::Plugin
{
public:
    AriaMpc() : mpc::Plugin(makeInfo())
    {
        for (int i = 0; i < ARIA_NPARAM; i++) {
            int p = engineIndex(i);
            const aria_param_t *d = aria_param_info(p);
            char name[32];
            aria_param_short(p, name, (int)sizeof name);
            std::string id = p == GP_OUT_VOL ? std::string("vol") : "p" + std::to_string(p);
            float def = defaultOf(p);
            if (d->kind == PK_CHOICE) {
                std::vector<std::string> ch;
                for (int k = 0; k < d->count; k++) ch.push_back(d->choice[k]);
                addParam(mpc::choiceParam(id.c_str(), name, ch, (int)def));
            } else if (d->kind == PK_SWITCH) {
                addParam(mpc::boolParam(id.c_str(), name, def > 0.5f));
            } else {
                addParam(mpc::floatParam(id.c_str(), name, 0.0f, 1.0f, def, "", 2));
            }
        }
        addParam(mpc::boolParam("init", "Init", false));
        addParam(mpc::boolParam("learn", "Chord Learn", false));

        /* the factory sounds as the MPC's programs (QUALITY, TEMPO and VOLUME as they start) */
        for (int k = 0; k < ARIA_NPRESETS; k++) {
            float v[ARIA_NPARAM];
            aria_preset_values(k, v);
            std::vector<float> pv((size_t)kNumParams, 0.0f);
            for (int p = 0; p < ARIA_NPARAM; p++) pv[(size_t)mpcIndex(p)] = keptOnProgram(p) ? defaultOf(p) : v[p];
            addProgram(ARIA_PRESETS[k].name, pv);
        }
        engine = aria_new(44100.0f);    /* (the MPC's rate; made here, on the UI thread, never in process) */
        aria_factory_keymaps(engine);
        rate = 44100.0;
        for (int p = 0; p < ARIA_NPARAM; p++) sent[p] = -1.0f;
    }
    ~AriaMpc() override { aria_free(engine); }

    void parameterChanged(int index, float plain) override
    {
        if (plain < 0.5f || busy) return;
        if (index == kLearn) {           /* LEARN: the audio thread takes it; the key springs back */
            learnRequest.store(true);
            busy = true; setAndTell(kLearn, 0.0f); busy = false;
        } else if (index == kInit) {     /* INIT: both timbres' sound as the INIT program's */
            busy = true;
            float v[ARIA_NPARAM];
            aria_preset_values(0, v);
            for (int p = 0; p < GP_FIRST; p++) setAndTell(mpcIndex(p), v[p]);
            setAndTell(kInit, 0.0f);
            busy = false;
        }
    }

    void prepare(double sampleRate, int) override
    {
        if (sampleRate != rate) {        /* (only if the rate really changes: the MPC's is 44.1 kHz) */
            aria_set_rate(engine, (float)sampleRate);
            rate = sampleRate;
        }
        for (int p = 0; p < ARIA_NPARAM; p++) sent[p] = -1.0f;
    }

    void reset() override              /* resume, or a program / state loaded: the voices let go at once */
    {
        aria_panic(engine);
        if (stagedReady.exchange(false)) aria_chord_load(engine, staged.data(), (int)staged.size());
        for (int p = 0; p < ARIA_NPARAM; p++) sent[p] = -1.0f;
    }

    void process(mpc::ProcessContext &ctx) override
    {
        const int n = ctx.numFrames;
        float *L = ctx.outputs[0], *R = ctx.numOutputs > 1 ? ctx.outputs[1] : ctx.outputs[0];
        for (int p = 0; p < ARIA_NPARAM; p++) {
            float v = get(mpcIndex(p));
            if (v != sent[p]) { aria_param(engine, p, v); sent[p] = v; }
        }
        if (learnRequest.exchange(false)) aria_chord_learn(engine);
        const auto &t = ctx.transport;
        aria_transport(engine, t.valid && t.tempoValid ? (float)t.bpm : 120.0f, t.valid && t.ppqValid ? t.ppqPos : 0.0,
                       t.valid && t.playing && t.ppqValid ? 1 : 0);
        int done = 0;
        for (int i = 0; i < ctx.numMidiEvents; i++) {
            const mpc::MidiEvent &e = ctx.midi[i];
            int at = mpc::clamp(e.sampleOffset, 0, n);
            if (at > done) { render(L, R, done, at - done); done = at; }
            midi(e);
        }
        if (n > done) render(L, R, done, n - done);
    }

    bool formatValue(int index, float plain, std::string &out) override
    {
        if (index >= ARIA_NPARAM) return false;
        char t[32];
        aria_value_text(engineIndex(index), plain, t, (int)sizeof t);
        out = t;
        return true;
    }

    void saveExtraState(std::vector<uint8_t> &out) override
    {
        out.resize((size_t)aria_chord_state_size());
        aria_chord_save(engine, out.data());
    }
    void loadExtraState(const uint8_t *data, size_t size) override
    {
        staged.assign(data, data + size);  /* (swapped in by reset(), on the audio thread) */
        stagedReady.store(true);
    }

private:
    static float defaultOf(int p)
    {
        if (p == GP_QUALITY) {
#if defined(__arm__)                     /* the 32-bit MPCs (X, Live, Live II, One, Key, Force): ECO to start with */
            return (float)Q_ECO;
#else
            return (float)Q_HIGH;
#endif
        }
        return aria_param_info(p)->def;
    }
    static bool keptOnProgram(int p) { return p == GP_QUALITY || p == GP_TEMPO || p == GP_OUT_VOL; }

    static mpc::PluginInfo makeInfo()
    {
        mpc::PluginInfo i;
        i.name = "ARIA";
        i.vendor = "swtdr34mz";
        i.uniqueId = MPC_FOURCC('A', 'r', 'i', 'a');
        i.version = 1000;
        i.isInstrument = true;
        i.numInputs = 0;
        i.numOutputs = 2;
        i.tailSamples = 44100 * 6;    /* (the delay's tail, about) */
        return i;
    }

    void setAndTell(int i, float plain) { set(i, plain); notifyHostParameterChanged(i); }

    void render(float *L, float *R, int at, int count)
    {
        if (R == L) {                    /* (a mono output: the two sides averaged) */
            float tmp[256];
            while (count > 0) {
                int m = count < 256 ? count : 256;
                aria_render(engine, L + at, tmp, m);
                for (int s = 0; s < m; s++) L[at + s] = 0.5f * (L[at + s] + tmp[s]);
                at += m; count -= m;
            }
            return;
        }
        aria_render(engine, L + at, R + at, count);
    }

    void midi(const mpc::MidiEvent &e)
    {
        const uint8_t st = e.status();
        if (e.isNoteOn()) aria_note_on(engine, e.note(), e.note(), (float)e.velocity() / 127.0f);
        else if (e.isNoteOff()) aria_note_off(engine, e.note());
        else if (e.isPitchBend()) aria_bend(engine, (float)e.pitchBend() / 8192.0f);
        else if (st == 0xA0) aria_pressure(engine, (float)e.data[2] / 127.0f);          /* poly pressure */
        else if (e.isAftertouch()) aria_pressure(engine, (float)e.data[1] / 127.0f);    /* channel pressure */
        else if (e.isController()) {
            int cc = e.controller(), v = e.controllerValue();
            if (cc == 1) aria_modwheel(engine, (float)v / 127.0f);
            else if (cc == 64) aria_sustain(engine, v >= 64);
            else if (cc == 120) aria_panic(engine);
            else if (cc == 123) aria_all_off(engine);
            else if (cc == 121) { aria_bend(engine, 0); aria_modwheel(engine, 0); aria_sustain(engine, 0); aria_pressure(engine, 0); }
        }
    }

    aria_t *engine = nullptr;
    double rate = 0.0;
    float sent[ARIA_NPARAM];
    bool busy = false;
    std::atomic<bool> learnRequest{false}, stagedReady{false};
    std::vector<uint8_t> staged;
};

} // namespace

MPC_PLUGIN_ENTRY(AriaMpc)
