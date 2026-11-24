/* SPDX-License-Identifier: MPL-2.0
 * Copyright (C) 2026 swtdr34mz (ARIA) */
#include "PluginProcessor.h"
#include "PluginEditor.h"
#include <cmath>

/* ------------------------------------------------------------------------------- parameters --- */
static juce::String paramName(int i)
{
    const aria_param_t *d = aria_param_info(i);
    juce::String n(d->name);
    if (i < GP_FIRST) n = juce::String(i < TP_COUNT ? "A " : "B ") + n;
    return n;
}

static juce::String noteName(int n)
{
    static const char *const N[12] = { "C", "C#", "D", "D#", "E", "F", "F#", "G", "G#", "A", "A#", "B" };
    return juce::String(N[n % 12]) + juce::String(n / 12 - 1);
}

/* a knob's value as the panel and the host show it */
juce::String ariaValueText(int i, float v)
{
    int p = i < GP_FIRST ? i % TP_COUNT : i;
    const aria_param_t *d = aria_param_info(i);
    if (d->kind == PK_CHOICE) return d->choice[juce::jlimit(0, d->count - 1, (int)std::lround(v))];
    if (d->kind == PK_SWITCH) return v > 0.5f ? "ON" : "OFF";
    if (i >= GP_FIRST) {
        switch (p) {
        case GP_VC_SPLIT: return noteName((int)std::lround(v * 127.0f));
        case GP_DL_TIME_L: case GP_DL_TIME_R: {
            float ms = 1.0f * std::pow(2000.0f, v);
            return ms < 1000.0f ? juce::String(ms, 0) + " ms" : juce::String(ms / 1000.0f, 2) + " s";
        }
        default: break;
        }
    } else {
        switch (p) {
        case TP_O1_FREQ: case TP_O2_FREQ: {
            float s = (v - 0.5f) * 14.0f;
            return (s >= 0 ? "+" : "") + juce::String(s, 2) + " st";
        }
        case TP_FE_A: case TP_FE_D: case TP_FE_R: case TP_AE_A: case TP_AE_D: case TP_AE_R: {
            float s = 0.0005f * std::pow(40000.0f, v);
            return s < 1.0f ? juce::String(s * 1000.0f, s < 0.01f ? 1 : 0) + " ms" : juce::String(s, 2) + " s";
        }
        case TP_F1_CUT: case TP_F2_CUT: {
            float hz = 20.0f * std::exp2(v * 10.0f);
            return hz < 1000.0f ? juce::String(hz, 0) + " Hz" : juce::String(hz / 1000.0f, 2) + " kHz";
        }
        case TP_L1_RATE: case TP_L2_RATE: return juce::String(0.02f * std::pow(5000.0f, v), 2) + " Hz";
        case TP_PL_RATE: return juce::String(0.5f * std::pow(40.0f, v), 2) + " Hz";
        case TP_BEND: return juce::String((int)std::lround(v * 12.0f)) + " st";
        default: break;
        }
    }
    if (d->kind == PK_BIPOLAR) {
        int x = (int)std::lround((v - 0.5f) * 200.0f);
        return (x > 0 ? "+" : "") + juce::String(x);
    }
    return juce::String((int)std::lround(v * 100.0f));
}

juce::AudioProcessorValueTreeState::ParameterLayout AriaProcessor::createLayout()
{
    juce::AudioProcessorValueTreeState::ParameterLayout layout;
    float def[ARIA_NPARAM];
    aria_preset_values(0, def);
    for (int i = 0; i < ARIA_NPARAM; i++) {
        const aria_param_t *d = aria_param_info(i);
        juce::ParameterID id{ paramId(i), 1 };
        if (d->kind == PK_CHOICE) {
            juce::StringArray ch;
            for (int k = 0; k < d->count; k++) ch.add(d->choice[k]);
            layout.add(std::make_unique<juce::AudioParameterChoice>(id, paramName(i), ch, (int)def[i]));
        } else if (d->kind == PK_SWITCH) {
            /* (a choice OFF/ON, not a bool: it saves and restores exactly) */
            layout.add(std::make_unique<juce::AudioParameterChoice>(id, paramName(i), juce::StringArray{ "OFF", "ON" }, def[i] > 0.5f ? 1 : 0));
        } else {
            layout.add(std::make_unique<juce::AudioParameterFloat>(
                id, paramName(i), juce::NormalisableRange<float>(0.0f, 1.0f), def[i],
                juce::AudioParameterFloatAttributes().withStringFromValueFunction([i](float v, int) { return ariaValueText(i, v); })));
        }
    }
    return layout;
}

AriaProcessor::AriaProcessor()
    : AudioProcessor(BusesProperties().withOutput("Output", juce::AudioChannelSet::stereo(), true)),
      apvts(*this, nullptr, "ARIA", createLayout())
{
    for (int i = 0; i < ARIA_NPARAM; i++) {
        params.push_back(apvts.getParameter(paramId(i)));
        raw.push_back(apvts.getRawParameterValue(paramId(i)));
        sent[i] = -1.0f;
    }
    engine = aria_new(44100.0f);
    aria_factory_keymaps(engine);
}

AriaProcessor::~AriaProcessor() { aria_free(engine); }

bool AriaProcessor::isBusesLayoutSupported(const BusesLayout &layouts) const
{
    return layouts.getMainOutputChannelSet() == juce::AudioChannelSet::stereo() ||
           layouts.getMainOutputChannelSet() == juce::AudioChannelSet::mono();
}

void AriaProcessor::prepareToPlay(double sampleRate, int)
{
    const juce::ScopedLock sl(chordLock);
    aria_set_rate(engine, (float)sampleRate);
    for (int i = 0; i < ARIA_NPARAM; i++) sent[i] = -1.0f;
    sendParams(true);
    keyboardState.reset();
}

void AriaProcessor::sendParams(bool all)
{
    for (int i = 0; i < ARIA_NPARAM; i++) {
        float v = raw[(size_t)i]->load();
        if (all || v != sent[i]) { aria_param(engine, i, v); sent[i] = v; }
    }
}

/* ------------------------------------------------------------------------------------ MIDI --- */
void AriaProcessor::handleMidi(const juce::MidiMessage &m)
{
    if (m.isNoteOn()) aria_note_on(engine, m.getNoteNumber(), m.getNoteNumber(), m.getFloatVelocity());
    else if (m.isNoteOff()) aria_note_off(engine, m.getNoteNumber());
    else if (m.isPitchWheel()) aria_bend(engine, (float)(m.getPitchWheelValue() - 8192) / 8192.0f);
    else if (m.isChannelPressure()) aria_pressure(engine, (float)m.getChannelPressureValue() / 127.0f);
    else if (m.isAftertouch()) aria_pressure(engine, (float)m.getAfterTouchValue() / 127.0f);
    else if (m.isController()) {
        int cc = m.getControllerNumber(), v = m.getControllerValue();
        if (cc == 1) aria_modwheel(engine, (float)v / 127.0f);
        else if (cc == 64) aria_sustain(engine, v >= 64);
        else if (cc == 120) aria_panic(engine);
        else if (cc == 123) aria_all_off(engine);
    } else if (m.isAllNotesOff()) aria_all_off(engine);
}

void AriaProcessor::processBlock(juce::AudioBuffer<float> &buffer, juce::MidiBuffer &midi)
{
    juce::ScopedNoDenormals noDenormals;
    const juce::ScopedTryLock sl(chordLock);
    int n = buffer.getNumSamples();
    if (!sl.isLocked()) { buffer.clear(); return; }      /* (the state is being loaded) */
    keyboardState.processNextMidiBuffer(midi, 0, n, true);
    sendParams(false);
    {   /* the host's tempo and position: the arpeggiator's grid */
        float bpm = 120.0f; double ppq = 0.0; bool playing = false;
        if (auto *ph = getPlayHead())
            if (auto pos = ph->getPosition()) {
                if (auto b = pos->getBpm()) bpm = (float)*b;
                if (auto q = pos->getPpqPosition()) ppq = *q;
                playing = pos->getIsPlaying();
            }
        aria_transport(engine, bpm, ppq, playing ? 1 : 0);
    }
    if (learnRequest.exchange(0)) aria_chord_learn(engine);
    if (cancelRequest.exchange(0)) aria_chord_cancel(engine);

    float *l = buffer.getWritePointer(0);
    float *r = buffer.getNumChannels() > 1 ? buffer.getWritePointer(1) : nullptr;
    std::vector<float> tmp;
    if (!r) { tmp.resize((size_t)n); r = tmp.data(); }
    int pos = 0;
    for (const auto meta : midi) {
        int at = juce::jlimit(0, n, meta.samplePosition);
        if (at > pos) { aria_render(engine, l + pos, r + pos, at - pos); pos = at; }
        handleMidi(meta.getMessage());
    }
    if (pos < n) aria_render(engine, l + pos, r + pos, n - pos);
    if (buffer.getNumChannels() == 1)
        for (int i = 0; i < n; i++) l[i] = 0.5f * (l[i] + r[i]);
    for (int c = 2; c < buffer.getNumChannels(); c++) buffer.clear(c, 0, n);
    learning.store(aria_chord_learning(engine));
    activeVoices.store(aria_voices_active(engine));
    /* the screens */
    auto pack = [](const int8_t *n, int c) {
        uint64_t v = 0;
        for (int i = 0; i < c && i < 8; i++) v |= (uint64_t)(uint8_t)(n[i] + 1) << (8 * i);   /* (+1: 0 is none) */
        return v;
    };
    int8_t notes[16];
    int c = aria_held_notes(engine, notes);          /* every key held; with none, the last held stay shown */
    if (c) { lastNotes.store(pack(notes, c)); lastNotes2.store(c > 8 ? pack(notes + 8, c - 8) : 0); }
    c = aria_chord_learned(engine, notes);
    learnNotes.store(pack(notes, c));
    int map = (int)std::lround(raw[GP_CH_MAP]->load());
    uint64_t m[2] = { 0, 0 };
    for (int k = 0; k < 128; k++) if (aria_chord_get_key(engine, map, k, notes)) m[k >> 6] |= 1ull << (k & 63);
    keyMask[0].store(m[0]); keyMask[1].store(m[1]);
    int w = scopeW.load();
    for (int i = 0; i < n; i++) { scope[w] = 0.5f * (l[i] + (buffer.getNumChannels() > 1 ? r[i] : l[i])); w = (w + 1) & (SCOPE - 1); }
    scopeW.store(w);
}

int AriaProcessor::unpack(uint64_t v, int out[8])
{
    int c = 0;
    for (int i = 0; i < 8; i++) { int b = (int)((v >> (8 * i)) & 0xff); if (b) out[c++] = b - 1; }
    return c;
}

/* -------------------------------------------------------------------------------- programs --- */
void AriaProcessor::setCurrentProgram(int index)
{
    if (index < 0 || index >= ARIA_NPRESETS) return;
    program = index;
    float v[ARIA_NPARAM];
    aria_preset_values(index, v);
    for (int i = 0; i < ARIA_NPARAM; i++) {
        auto *p = params[(size_t)i];
        p->setValueNotifyingHost(p->convertTo0to1(v[i]));
    }
    updateHostDisplay(ChangeDetails().withProgramChanged(true));
}

const juce::String AriaProcessor::getProgramName(int index)
{
    return index >= 0 && index < ARIA_NPRESETS ? juce::String(ARIA_PRESETS[index].name) : juce::String();
}

/* ----------------------------------------------------------------------------------- state --- */
void AriaProcessor::getStateInformation(juce::MemoryBlock &dest)
{
    auto state = apvts.copyState();
    std::unique_ptr<juce::XmlElement> xml(state.createXml());
    xml->setAttribute("program", program);
    xml->setAttribute("editTimbre", editTimbre.load());
    xml->setAttribute("uiScale", (double)uiScale.load());
    {
        const juce::ScopedLock sl(chordLock);
        std::vector<uint8_t> blob((size_t)aria_chord_state_size());
        aria_chord_save(engine, blob.data());
        xml->setAttribute("chords", juce::Base64::toBase64(blob.data(), blob.size()));
    }
    copyXmlToBinary(*xml, dest);
}

void AriaProcessor::setStateInformation(const void *data, int size)
{
    std::unique_ptr<juce::XmlElement> xml(getXmlFromBinary(data, size));
    if (!xml || !xml->hasTagName(apvts.state.getType())) return;
    program = juce::jlimit(0, ARIA_NPRESETS - 1, xml->getIntAttribute("program", 0));
    editTimbre.store(juce::jlimit(0, 1, xml->getIntAttribute("editTimbre", 0)));
    uiScale.store((float)juce::jlimit(0.5, 1.5, xml->getDoubleAttribute("uiScale", 0.8)));
    juce::MemoryOutputStream blob;
    if (juce::Base64::convertFromBase64(blob, xml->getStringAttribute("chords"))) {
        const juce::ScopedLock sl(chordLock);
        aria_chord_load(engine, (const uint8_t *)blob.getData(), (int)blob.getDataSize());
    }
    apvts.replaceState(juce::ValueTree::fromXml(*xml));
}

juce::AudioProcessorEditor *AriaProcessor::createEditor() { return new AriaEditor(*this); }

juce::AudioProcessor *JUCE_CALLTYPE createPluginFilter() { return new AriaProcessor(); }
