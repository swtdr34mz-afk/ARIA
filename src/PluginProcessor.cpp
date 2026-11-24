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

/* a knob's value as the panel and the host show it (dsp/params.c, shared with the MPC plugin) */
juce::String ariaValueText(int i, float v)
{
    char t[32];
    aria_value_text(i, v, t, (int)sizeof t);
    return juce::String(t);
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
    loadUserPresets();
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
        float bpm = 60.0f + 140.0f * raw[GP_TEMPO]->load(); double ppq = 0.0; bool playing = false;   /* (TEMPO: without a host) */
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
    arpStep.store(aria_arp_step(engine));
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
static bool keptOnProgramChange(int i) { return i == GP_TEMPO || i == GP_QUALITY; }   /* (settings, not sound) */

void AriaProcessor::setCurrentProgram(int index)
{
    if (index < 0 || index >= getNumPrograms()) return;
    program = index;
    std::vector<float> v((size_t)ARIA_NPARAM);
    if (index < ARIA_NPRESETS) aria_preset_values(index, v.data());
    else v = userPresets[(size_t)(index - ARIA_NPRESETS)].v;
    for (int i = 0; i < ARIA_NPARAM; i++) {
        if (keptOnProgramChange(i)) continue;
        auto *p = params[(size_t)i];
        p->setValueNotifyingHost(p->convertTo0to1(v[(size_t)i]));
    }
    updateHostDisplay(ChangeDetails().withProgramChanged(true));
}

const juce::String AriaProcessor::getProgramName(int index)
{
    if (index >= 0 && index < ARIA_NPRESETS) return ARIA_PRESETS[index].name;
    if (index >= ARIA_NPRESETS && index < getNumPrograms()) return userPresets[(size_t)(index - ARIA_NPRESETS)].name;
    return {};
}

juce::File AriaProcessor::presetFolder()
{
    return juce::File::getSpecialLocation(juce::File::userDocumentsDirectory).getChildFile("ARIA").getChildFile("Presets");
}

void AriaProcessor::loadUserPresets()
{
    userPresets.clear();
    auto files = presetFolder().findChildFiles(juce::File::findFiles, false, "*.aria");
    files.sort();
    float def[ARIA_NPARAM];
    aria_preset_values(0, def);
    for (auto &f : files)
        if (auto xml = juce::XmlDocument::parse(f)) {
            if (!xml->hasTagName("ARIAPRESET")) continue;
            UserPreset u;
            u.name = xml->getStringAttribute("name", f.getFileNameWithoutExtension());
            u.v.resize((size_t)ARIA_NPARAM);
            for (int i = 0; i < ARIA_NPARAM; i++)     /* (a parameter the file lacks: INIT's) */
                u.v[(size_t)i] = juce::jlimit(0.0f, aria_param_max(i), (float)xml->getDoubleAttribute("p" + juce::String(i), def[i]));
            userPresets.push_back(std::move(u));
        }
}

int AriaProcessor::savePreset(const juce::String &name)
{
    auto clean = name.trim().toUpperCase().substring(0, 20);
    if (clean.isEmpty()) return -1;
    juce::XmlElement xml("ARIAPRESET");
    xml.setAttribute("name", clean);
    xml.setAttribute("version", 1);
    for (int i = 0; i < ARIA_NPARAM; i++)
        xml.setAttribute("p" + juce::String(i), (double)params[(size_t)i]->convertFrom0to1(params[(size_t)i]->getValue()));
    auto folder = presetFolder();
    folder.createDirectory();
    if (!xml.writeTo(folder.getChildFile(juce::File::createLegalFileName(clean) + ".aria"))) return -1;
    loadUserPresets();
    for (size_t i = 0; i < userPresets.size(); i++)
        if (userPresets[i].name == clean) {
            program = ARIA_NPRESETS + (int)i;
            updateHostDisplay(ChangeDetails().withProgramChanged(true));
            return program;
        }
    return -1;
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
    program = juce::jlimit(0, getNumPrograms() - 1, xml->getIntAttribute("program", 0));
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
