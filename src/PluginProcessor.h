/* SPDX-License-Identifier: MPL-2.0
 * Copyright (C) 2026 swtdr34mz (ARIA) */
/* ARIA as a plugin (VST3, AU on the Mac) and a standalone app: the engine (dsp/aria.h), its parameters for the host
 * (automation, saved with the project), the factory sounds as the host's programs, MIDI in, and the chord memory's
 * key maps saved with the project. */
#pragma once
#include <juce_audio_utils/juce_audio_utils.h>
#include <atomic>
#include "aria.h"
#include "presets.h"

class AriaProcessor : public juce::AudioProcessor
{
public:
    AriaProcessor();
    ~AriaProcessor() override;

    void prepareToPlay(double sampleRate, int samplesPerBlock) override;
    void releaseResources() override {}
    bool isBusesLayoutSupported(const BusesLayout &layouts) const override;
    void processBlock(juce::AudioBuffer<float> &, juce::MidiBuffer &) override;

    juce::AudioProcessorEditor *createEditor() override;
    bool hasEditor() const override { return true; }

    const juce::String getName() const override { return "ARIA"; }
    bool acceptsMidi() const override { return true; }
    bool producesMidi() const override { return false; }
    bool isMidiEffect() const override { return false; }
    double getTailLengthSeconds() const override { return 6.0; }

    int getNumPrograms() override { return ARIA_NPRESETS; }
    int getCurrentProgram() override { return program; }
    void setCurrentProgram(int index) override;
    const juce::String getProgramName(int index) override;
    void changeProgramName(int, const juce::String &) override {}

    void getStateInformation(juce::MemoryBlock &destData) override;
    void setStateInformation(const void *data, int sizeInBytes) override;

    juce::AudioProcessorValueTreeState apvts;
    static juce::String paramId(int i) { return "p" + juce::String(i); }
    juce::RangedAudioParameter *param(int i) const { return params[(size_t)i]; }

    /* from the editor (the audio thread picks them up) */
    std::atomic<int> learnRequest{0};         /* LEARN pressed */
    std::atomic<int> cancelRequest{0};
    std::atomic<int> learning{0};             /* the chord memory's state, for the editor */
    std::atomic<int> editTimbre{0};           /* the panel shows timbre A (0) or B (1) */
    std::atomic<float> uiScale{0.8f};
    std::atomic<int> activeVoices{0};
    juce::MidiKeyboardState keyboardState;    /* (the standalone's keys, from the computer keyboard) */

private:
    aria_t *engine = nullptr;
    std::vector<juce::RangedAudioParameter *> params;
    std::vector<std::atomic<float> *> raw;
    float sent[ARIA_NPARAM];
    int program = 0;
    juce::CriticalSection chordLock;          /* the key maps: the audio thread and the state */
    static juce::AudioProcessorValueTreeState::ParameterLayout createLayout();
    void handleMidi(const juce::MidiMessage &m);
    void sendParams(bool all);

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(AriaProcessor)
};
