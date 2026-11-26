/* SPDX-License-Identifier: MPL-2.0
 * Copyright (C) 2026 swtdr34mz (ARIA) */
/* ARIA's panel: black metal, knurled knobs with an aluminium cap, faders, backlit rubber buttons, an OLED display.
 * The panel is drawn at 1680 x 720 and scaled to the window. Timbre controls show the timbre chosen with A / B. */
#pragma once
#include "PluginProcessor.h"

juce::String ariaValueText(int i, float v);

namespace aria_ui {

struct Fonts {
    juce::Typeface::Ptr semi, bold, mono;
    static Fonts &get();
    static void release();
    juce::Font label(float h) const;
    juce::Font heading(float h) const;
    juce::Font oled(float h) const;
};

class Panel;

/* the editors' shared fonts and drawn images: they live while an editor is open (a SharedResourcePointer in each)
 * and go when the last one closes. Never function statics: those would be freed while the host unloads the DLL,
 * under Windows' loader lock, where freeing fonts and images can hang the host (FL Studio froze on close). */
struct UiResources {
    UiResources() = default;
    ~UiResources();
};

/* a control on the panel: a parameter of the timbre being edited (timbre) or a global one */
class Control : public juce::Component
{
public:
    Control(Panel &p, int param, bool timbre, juce::String label);
    int index() const;
    float value() const;                     /* in the engine's units */
    void setValue(float v);                  /* (begin/end gesture around it) */
    void begin();
    void end();
    bool changed();                          /* its value moved since it was last drawn */
    juce::String label;
protected:
    /* a right-click: the host's own menu for this parameter (MIDI learn, automation, ...); true when it was one, and
     * then the value stays as it is (the drag and the release that follow are ignored too) */
    bool hostMenu(const juce::MouseEvent &e);
    bool menuClick = false;
    Panel &panel;
    int rel;
    bool isTimbre;
    float drawn = -100.0f;
    int drawnIndex = -1;
};

class Knob : public Control
{
public:
    Knob(Panel &p, int param, bool timbre, juce::String label, bool big = false);
    void paint(juce::Graphics &) override;
    void mouseDown(const juce::MouseEvent &) override;
    void mouseDrag(const juce::MouseEvent &) override;
    void mouseUp(const juce::MouseEvent &) override;
    void mouseDoubleClick(const juce::MouseEvent &) override;
    void mouseWheelMove(const juce::MouseEvent &, const juce::MouseWheelDetails &) override;
    bool big;
    bool newRow = false;                     /* (the panel: this knob starts a new row) */
private:
    float startV = 0;
};

class Fader : public Control
{
public:
    Fader(Panel &p, int param, bool timbre, juce::String label);
    void paint(juce::Graphics &) override;
    void mouseDown(const juce::MouseEvent &) override;
    void mouseDrag(const juce::MouseEvent &) override;
    void mouseUp(const juce::MouseEvent &) override;
    void mouseDoubleClick(const juce::MouseEvent &) override;
private:
    float startV = 0;
};

/* a backlit button: a switch, one value of a choice (radio), a choice stepped through (cycle), or an action */
class Button : public Control
{
public:
    enum Kind { SWITCH, RADIO, CYCLE, ACTION };
    Button(Panel &p, Kind k, int param, bool timbre, juce::String label, int radioValue = 0);
    std::function<void()> onPress;           /* ACTION */
    std::function<bool()> lit;               /* ACTION: lit? */
    std::function<bool()> blink;             /* any kind: blinking (LEARN) */
    juce::Colour role{ 0xffffb02e };         /* the colour it lights in (its section's) */
    void paint(juce::Graphics &) override;
    void mouseDown(const juce::MouseEvent &) override;
    bool isLit() const;
    bool small = false;                      /* a key-map dot */
    Kind kind;
    int radioValue;
};

/* a choice shown as a column of LEDs with their names (LOW SPLIT: OFF, -1 OCT ...); a click on a line picks it */
class LedList : public Control
{
public:
    LedList(Panel &p, int param, bool timbre);
    void paint(juce::Graphics &) override;
    void mouseDown(const juce::MouseEvent &) override;
    juce::Colour role{ 0xffffb02e };
    static constexpr int LINE = 19;
};

/* a step of the arp pattern: a light key, its LED on when the step plays, the key lit when the arp is on it */
class StepKey : public Control
{
public:
    StepKey(Panel &p, int step);
    void paint(juce::Graphics &) override;
    void mouseDown(const juce::MouseEvent &) override;
    int step;
};

class Display : public juce::Component
{
public:
    explicit Display(Panel &p) : panel(p) {}
    void paint(juce::Graphics &) override;
private:
    Panel &panel;
};

/* the chord memory's screen: the chord the last key played (its name and notes), the key map's keys on a keyboard */
class ChordScreen : public juce::Component
{
public:
    explicit ChordScreen(Panel &p) : panel(p) {}
    void paint(juce::Graphics &) override;
private:
    Panel &panel;
};

/* the mixer's screen: the sound's waveform, live */
class Scope : public juce::Component
{
public:
    explicit Scope(Panel &p) : panel(p) {}
    void paint(juce::Graphics &) override;
private:
    Panel &panel;
};

juce::String chordName(const int *notes, int n);
void paintOled(juce::Graphics &g, juce::Rectangle<float> r);

struct Module {
    juce::String title;
    int x, y, w, h;
    std::vector<Knob *> knobs;
    std::vector<Fader *> faders;
    std::vector<Button *> buttons;
    bool accent = false;
    juce::Colour col;                        /* the section's colour (its buttons, its heading's rule) */
};
juce::Colour sectionColour(const juce::String &title);

class Panel : public juce::Component, private juce::Timer
{
public:
    explicit Panel(AriaProcessor &p);
    void paint(juce::Graphics &) override;
    void touched(int index);                 /* a control moved: the display shows it */
    AriaProcessor &proc;
    juce::Colour accent{ 0xffff5b2e };
    int lastTouched = -1;
    juce::uint32 touchedAt = 0;
    int learning = 0;
    juce::uint32 blinkPhase = 0;
    static constexpr int W = 1800, H = 900, BAND_Y = 668, BAND_H = 176;
private:
    void timerCallback() override;
    std::vector<std::unique_ptr<Control>> controls;
    std::vector<Module> modules;
    Display display{ *this };
    ChordScreen chordScreen{ *this };
    Scope scope{ *this };
    juce::Image tag, face, sideL, sideR;
    void paintKnob(juce::Graphics &, Knob &);
    void paintFader(juce::Graphics &, Fader &);
    std::vector<StepKey *> steps;
    int shownStep = -2;
    float shownLen = -1.0f;
public:
    int arpStep = -1;
private:
    Knob *knob(Module &m, int param, bool timbre, const char *label, bool big = false);
    Fader *fader(Module &m, int param, bool timbre, const char *label);
    Button *button(Module &m, Button::Kind k, int param, bool timbre, const char *label, int radio = 0);
    void build();
    void layoutModule(Module &m);
    int shownEdit = -1;
};

} // namespace aria_ui

class AriaEditor : public juce::AudioProcessorEditor
{
public:
    explicit AriaEditor(AriaProcessor &);
    void resized() override;
    void paint(juce::Graphics &g) override { g.fillAll(juce::Colour(0xff0a0a0b)); }
private:
    AriaProcessor &proc;
    juce::SharedResourcePointer<aria_ui::UiResources> resources;   /* (before the panel: freed after it) */
    aria_ui::Panel panel;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(AriaEditor)
};
