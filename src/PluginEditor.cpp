/* SPDX-License-Identifier: MPL-2.0
 * Copyright (C) 2026 swtdr34mz (ARIA) */
#include "PluginEditor.h"
#include "BinaryData.h"
#include "Art.h"
#include <cmath>

using namespace aria_ui;
using juce::Colour;

namespace {
const Colour C_PANEL_TOP(0xff232427), C_PANEL(0xff1b1c1e), C_PANEL_BOT(0xff18191b);
const Colour C_LINE(0xff2c2d30), C_HEAD_LINE(0xff3a3b3f), C_TEXT(0xffcfd0d2), C_DIM(0xff8c8e92), C_WHITE(0xffe9e9ea);
const Colour C_TICK(0xff6c6f73), C_OLED(0xffdff3ff);
constexpr float PI = 3.14159265f;
}

/* ------------------------------------------------------------------------------------ fonts --- */
Fonts &Fonts::get()
{
    static Fonts f = [] {
        Fonts x;
        x.semi = juce::Typeface::createSystemTypefaceFor(BinaryData::FiraSansSemiBold_ttf, BinaryData::FiraSansSemiBold_ttfSize);
        x.bold = juce::Typeface::createSystemTypefaceFor(BinaryData::FiraSansBold_ttf, BinaryData::FiraSansBold_ttfSize);
        x.mono = juce::Typeface::createSystemTypefaceFor(BinaryData::AnonymousProBold_ttf, BinaryData::AnonymousProBold_ttfSize);
        return x;
    }();
    return f;
}
juce::Font Fonts::label(float h) const { return juce::Font(juce::FontOptions(semi).withHeight(h)).withExtraKerningFactor(0.06f); }
juce::Font Fonts::heading(float h) const { return juce::Font(juce::FontOptions(semi).withHeight(h)).withExtraKerningFactor(0.28f); }
juce::Font Fonts::oled(float h) const { return juce::Font(juce::FontOptions(mono).withHeight(h)).withExtraKerningFactor(0.08f); }

/* ---------------------------------------------------------------------------------- control --- */
Control::Control(Panel &p, int param, bool timbre, juce::String l) : label(std::move(l)), panel(p), rel(param), isTimbre(timbre) {}
int Control::index() const { return isTimbre ? TP(panel.proc.editTimbre.load(), rel) : rel; }
float Control::value() const
{
    auto *p = panel.proc.param(index());
    return p->convertFrom0to1(p->getValue());
}
void Control::setValue(float v)
{
    auto *p = panel.proc.param(index());
    p->setValueNotifyingHost(p->convertTo0to1(juce::jlimit(0.0f, aria_param_max(index()), v)));
    panel.touched(index());
}
void Control::begin() { panel.proc.param(index())->beginChangeGesture(); panel.touched(index()); }
void Control::end() { panel.proc.param(index())->endChangeGesture(); }
bool Control::changed()
{
    float v = value();
    if (v == drawn && index() == drawnIndex) return false;
    drawn = v; drawnIndex = index();
    return true;
}

/* ------------------------------------------------------------------------------------- knob --- */
Knob::Knob(Panel &p, int param, bool timbre, juce::String l, bool b) : Control(p, param, timbre, std::move(l)), big(b) {}

/* the knob's body, scale and label are the panel's (Panel::paintKnob); the knob draws its pointer */
void Knob::paint(juce::Graphics &g)
{
    float w = (float)getWidth();
    float d = big ? 50.0f : 36.0f, R = d * 0.5f, ring = d + 14.0f;
    float cx = w * 0.5f, cy = ring * 0.5f + 1.0f;
    float a = -0.75f * PI + 1.5f * PI * (value() / aria_param_max(index()));
    float rc = R * (big ? 0.64f : 0.6f);
    float sx = std::sin(a), sy = -std::cos(a);
    g.setColour(juce::Colours::black.withAlpha(0.7f));
    g.drawLine(cx + sx * (rc + 1.5f) + 0.6f, cy + sy * (rc + 1.5f) + 0.8f, cx + sx * (R - 1.0f) + 0.6f, cy + sy * (R - 1.0f) + 0.8f, 2.4f);
    g.setColour(Colour(0xfff7f7f5));
    g.drawLine(cx + sx * (rc + 1.5f), cy + sy * (rc + 1.5f), cx + sx * (R - 1.0f), cy + sy * (R - 1.0f), 2.0f);
    g.setColour(Colour(0x55000000));                 /* (an engraved dot on the cap's edge) */
    g.fillEllipse(cx + sx * (rc - 3.2f) - 1.2f, cy + sy * (rc - 3.2f) - 1.2f, 2.4f, 2.4f);
}

void Knob::mouseDown(const juce::MouseEvent &) { startV = value(); begin(); }
void Knob::mouseDrag(const juce::MouseEvent &e)
{
    float range = aria_param_max(index());
    float sens = e.mods.isShiftDown() ? 900.0f : 220.0f;
    setValue(startV - (float)e.getDistanceFromDragStartY() / sens * range);
}
void Knob::mouseUp(const juce::MouseEvent &) { end(); }
void Knob::mouseDoubleClick(const juce::MouseEvent &)
{
    auto *p = panel.proc.param(index());
    begin(); p->setValueNotifyingHost(p->getDefaultValue()); end();
}
void Knob::mouseWheelMove(const juce::MouseEvent &, const juce::MouseWheelDetails &w)
{
    begin(); setValue(value() + w.deltaY * 0.05f * aria_param_max(index())); end();
}

/* ------------------------------------------------------------------------------------ fader --- */
Fader::Fader(Panel &p, int param, bool timbre, juce::String l) : Control(p, param, timbre, std::move(l)) {}
/* the slot, scale and label are the panel's (Panel::paintFader); the fader draws its cap */
void Fader::paint(juce::Graphics &g)
{
    float cx = (float)getWidth() * 0.5f, top = 4.0f, len = 110.0f;
    float y = top + 7.0f + (len - 14.0f) * (1.0f - value()) - 7.0f;
    auto img = aria_art::faderCap();
    g.drawImage(img, juce::Rectangle<float>(cx - 16.0f, y - 4.0f, 32.0f, 26.0f), juce::RectanglePlacement::stretchToFit);
}

void Fader::mouseDown(const juce::MouseEvent &) { startV = value(); begin(); }
void Fader::mouseDrag(const juce::MouseEvent &e)
{
    float sens = e.mods.isShiftDown() ? 400.0f : 96.0f;
    setValue(startV - (float)e.getDistanceFromDragStartY() / sens);
}
void Fader::mouseUp(const juce::MouseEvent &) { end(); }
void Fader::mouseDoubleClick(const juce::MouseEvent &)
{
    auto *p = panel.proc.param(index());
    begin(); p->setValueNotifyingHost(p->getDefaultValue()); end();
}

/* ----------------------------------------------------------------------------------- button --- */
Button::Button(Panel &p, Kind k, int param, bool timbre, juce::String l, int rv)
    : Control(p, param, timbre, std::move(l)), kind(k), radioValue(rv) {}

bool Button::isLit() const
{
    switch (kind) {
    case SWITCH: return value() > 0.5f;
    case RADIO: return (int)std::lround(value()) == radioValue;
    case CYCLE: return false;
    default: return lit && lit();
    }
}

void Button::paint(juce::Graphics &g)
{
    bool on = isLit();
    if (blink && blink()) on = (panel.blinkPhase / 8) % 2 == 0;
    auto ac = panel.accent;
    juce::Rectangle<float> b = small ? juce::Rectangle<float>(((float)getWidth() - 16.0f) * 0.5f, 2.0f, 16.0f, 16.0f)
                                     : juce::Rectangle<float>(((float)getWidth() - 34.0f) * 0.5f, 6.0f, 34.0f, 16.0f);
    float rad = small ? 8.0f : 3.5f;
    /* the well it sits in, its shadow */
    g.setColour(Colour(0xff050506)); g.fillRoundedRectangle(b.expanded(1.6f), rad + 1.4f);
    g.setColour(Colour(0x22ffffff)); g.drawLine(b.getX() - 1, b.getBottom() + 1.9f, b.getRight() + 1, b.getBottom() + 1.9f, 0.8f);
    if (on) {
        int reach = small ? 2 : 4;                   /* the glow, soft, inside the button's bounds */
        for (int i = reach; i >= 1; i--) {
            g.setColour(ac.withAlpha(0.10f + 0.05f * (float)(reach - i)));
            g.fillRoundedRectangle(b.expanded((float)i * 0.9f + 0.6f), rad + (float)i);
        }
        juce::ColourGradient body(Colour(0xffffffff).interpolatedWith(ac, 0.35f), b.getCentreX(), b.getCentreY(), ac.darker(0.15f), b.getRight(), b.getBottom(), true);
        g.setGradientFill(body);
        g.fillRoundedRectangle(b, rad);
        g.setColour(Colour(0x80ffffff)); g.drawLine(b.getX() + 2, b.getY() + 1.0f, b.getRight() - 2, b.getY() + 1.0f, 0.8f);
    } else {
        juce::ColourGradient body(Colour(0xff3a3b3f), 0, b.getY(), Colour(0xff1e1f22), 0, b.getBottom(), false);
        g.setGradientFill(body);
        g.fillRoundedRectangle(b, rad);
        juce::ColourGradient sheen(Colour(0x1effffff), 0, b.getY(), Colour(0x00ffffff), 0, b.getCentreY(), false);
        g.setGradientFill(sheen); g.fillRoundedRectangle(b.reduced(1.0f, 0.5f), rad);
        g.setColour(Colour(0x40ffffff)); g.drawLine(b.getX() + 2, b.getY() + 0.8f, b.getRight() - 2, b.getY() + 0.8f, 0.7f);
    }
    g.setColour(Colour(0xff020202));
    g.drawRoundedRectangle(b, rad, 0.8f);
    juce::String t = label;
    if (kind == CYCLE) t = (label.isEmpty() ? "" : label + " ") + ariaValueText(index(), value());
    if (small) {
        g.setColour(on ? Colour(0xff151515) : C_DIM);
        g.setFont(Fonts::get().oled(10.0f));
        g.drawText(label, b, juce::Justification::centred);
        return;
    }
    g.setColour(on || kind == CYCLE ? C_WHITE : Colour(0xffa4a6aa));
    g.setFont(Fonts::get().label(10.5f));
    g.drawFittedText(t, 0, (int)b.getBottom() + 4, getWidth(), 13, juce::Justification::centredTop, 1, 0.7f);
}

void Button::mouseDown(const juce::MouseEvent &e)
{
    switch (kind) {
    case SWITCH: begin(); setValue(value() > 0.5f ? 0.0f : 1.0f); end(); break;
    case RADIO: begin(); setValue((float)radioValue); end(); break;
    case CYCLE: {
        int n = (int)aria_param_max(index()) + 1, v = (int)std::lround(value());
        v = e.mods.isRightButtonDown() || e.mods.isShiftDown() ? (v + n - 1) % n : (v + 1) % n;
        begin(); setValue((float)v); end();
        break;
    }
    default: if (onPress) onPress(); break;
    }
    repaint();
}

/* ---------------------------------------------------------------------------------- step key --- */
StepKey::StepKey(Panel &p, int st) : Control(p, GP_ARP_STEP1 + st, false, juce::String(st + 1)), step(st) {}

void StepKey::paint(juce::Graphics &g)
{
    auto &P = panel.proc;
    int len = (int)std::lround(P.param(GP_ARP_LEN)->convertFrom0to1(P.param(GP_ARP_LEN)->getValue())) + 1;
    bool on = value() > 0.5f, inLen = step < len, now = panel.arpStep == step;
    auto ac = panel.accent;
    /* the LED above */
    juce::Rectangle<float> led((float)getWidth() * 0.5f - 6.0f, 2.0f, 12.0f, 4.0f);
    if (on && inLen) {
        g.setColour(ac.withAlpha(0.25f)); g.fillRoundedRectangle(led.expanded(2.5f, 2.0f), 3.0f);
        g.setColour(ac.brighter(0.3f)); g.fillRoundedRectangle(led, 2.0f);
    } else { g.setColour(Colour(0xff2a1510)); g.fillRoundedRectangle(led, 2.0f); }
    /* the key: a light keycap in its well */
    juce::Rectangle<float> k(3.0f, 11.0f, (float)getWidth() - 6.0f, (float)getHeight() - 14.0f);   /* (taller than wide) */
    g.setColour(Colour(0xff050506)); g.fillRoundedRectangle(k.expanded(2.0f), 3.2f);
    juce::Path cap; cap.addRoundedRectangle(k, 2.2f);
    juce::DropShadow(Colour(0x90000000), 3, { 0, 2 }).drawForPath(g, cap);
    Colour top = inLen ? Colour(0xffeeece6) : Colour(0xff8d8c88), bot = inLen ? Colour(0xffc4c2bb) : Colour(0xff6a6966);
    juce::ColourGradient cg(top, 0, k.getY(), bot, 0, k.getBottom(), false);
    g.setGradientFill(cg); g.fillPath(cap);
    juce::ColourGradient sheen(Colour(0x60ffffff), 0, k.getY(), Colour(0x00ffffff), 0, k.getY() + 10.0f, false);
    g.setGradientFill(sheen); g.fillRoundedRectangle(k.reduced(1.5f, 1.0f).withHeight(12.0f), 2.0f);
    if (now) {                                       /* the arp is on this step */
        g.setColour(ac.withAlpha(0.55f)); g.fillPath(cap);
        g.setColour(ac.brighter(0.5f)); g.strokePath(cap, juce::PathStrokeType(1.4f));
    }
    g.setColour(Colour(0xff1a1a1a)); g.strokePath(cap, juce::PathStrokeType(0.8f));
    g.setColour(inLen ? Colour(0xff3a3a3a) : Colour(0xff3c3c3a));
    g.setFont(Fonts::get().label(11.0f));
    g.drawText(label, k.withTrimmedTop(k.getHeight() * 0.45f), juce::Justification::centred);
}

void StepKey::mouseDown(const juce::MouseEvent &)
{
    begin(); setValue(value() > 0.5f ? 0.0f : 1.0f); end();
    repaint();
}

/* ---------------------------------------------------------------------------------- display --- */
void aria_ui::paintOled(juce::Graphics &g, juce::Rectangle<float> r)
{
    aria_art::bezel(g, r.reduced(3.0f));
}

void Display::paint(juce::Graphics &g)
{
    auto r = getLocalBounds().toFloat();
    paintOled(g, r);

    auto &P = panel.proc;
    int prog = P.getCurrentProgram();
    const juce::String dot = juce::String::fromUTF8("  \xc2\xb7  ");
    juce::String top = "TIMBRE " + juce::String(P.editTimbre.load() ? "B" : "A") + dot +
                       juce::String(prog + 1).paddedLeft('0', 3) + " " + (prog < ARIA_NPRESETS ? juce::String(ARIA_PRESETS[prog].category) : juce::String("USER")) +
                       dot + "VOICES " + juce::String(P.activeVoices.load());
    juce::String line;
    if (panel.learning == 1) { top = "CHORD LEARN"; line = "PLAY CHORD, PRESS LEARN"; }
    else if (panel.learning == 2) { top = "CHORD LEARN" + dot + "KEY MAP " + juce::String((int)std::lround(P.param(GP_CH_MAP)->convertFrom0to1(P.param(GP_CH_MAP)->getValue())) + 1); line = "PRESS THE KEY FOR IT"; }
    else if (panel.lastTouched >= 0 && juce::Time::getMillisecondCounter() - panel.touchedAt < 2500) {
        auto *p = P.param(panel.lastTouched);
        line = p->getName(40) + "  " + ariaValueText(panel.lastTouched, p->convertFrom0to1(p->getValue()));
    } else line = P.getProgramName(prog);
    g.setFont(Fonts::get().oled(10.0f));
    g.setColour(Colour(0xff6f7a80));
    g.drawText(top, r.reduced(12, 6).removeFromTop(12), juce::Justification::left);
    auto lr = r.reduced(12, 6).withTrimmedTop(15);
    g.setFont(Fonts::get().oled(17.0f));
    g.setColour(C_OLED.withAlpha(0.18f));
    g.drawFittedText(line.toUpperCase(), lr.translated(0, 0.6f).toNearestInt(), juce::Justification::left, 1, 0.7f);
    g.setColour(C_OLED);
    g.drawFittedText(line.toUpperCase(), lr.toNearestInt(), juce::Justification::left, 1, 0.7f);
    aria_art::glass(g, r.reduced(3.0f));
}

/* ----------------------------------------------------------------------------- chord screen --- */
static juce::String noteName(int n)
{
    static const char *const N[12] = { "C", "C#", "D", "Eb", "E", "F", "F#", "G", "Ab", "A", "Bb", "B" };
    return juce::String(N[((n % 12) + 12) % 12]) + juce::String(n / 12 - 1);
}
static const char *const PC[12] = { "C", "C#", "D", "Eb", "E", "F", "F#", "G", "Ab", "A", "Bb", "B" };

juce::String aria_ui::chordName(const int *notes, int n)
{
    if (n <= 0) return {};
    int lo = 127, set = 0;
    for (int i = 0; i < n; i++) lo = std::min(lo, notes[i]);
    for (int i = 0; i < n; i++) set |= 1 << (notes[i] % 12);
    if (n == 1) return PC[notes[0] % 12];
    static const struct { int mask; const char *name; } T[] = {
        { 0x091, "" }, { 0x089, "m" }, { 0x049, "dim" }, { 0x111, "+" }, { 0x085, "sus2" }, { 0x0A1, "sus4" },
        { 0x291, "6" }, { 0x289, "m6" }, { 0x491, "7" }, { 0x891, "maj7" }, { 0x489, "m7" }, { 0x449, "m7b5" },
        { 0x249, "dim7" }, { 0x889, "mMaj7" }, { 0x095, "add9" }, { 0x08D, "madd9" }, { 0x495, "9" }, { 0x895, "maj9" },
        { 0x48D, "m9" }, { 0x4A1, "7sus4" }, { 0x081, "5" }, { 0x011, "(no5)" }, { 0x411, "7" }, { 0x811, "maj7" },
        { 0x409, "m7" }, { 0x00D, "m(add9)" }, { 0x885, "maj7sus2" }, { 0x485, "7sus2" }, { 0x295, "6/9" },
        { 0x4A9, "m11" }, { 0x8D1, "maj7#11" }, { 0x4B5, "11" }, { 0x695, "13" }, { 0x491 | 0x002, "7b9" }, { 0x4D1, "7#11" } };
    int order[12], no = 0;                            /* the bass as the root first, then the others */
    order[no++] = lo % 12;
    for (int pc = 0; pc < 12; pc++) if (((set >> pc) & 1) && pc != lo % 12) order[no++] = pc;
    for (int o = 0; o < no; o++) {
        int root = order[o], rel = 0;
        for (int pc = 0; pc < 12; pc++) if ((set >> pc) & 1) rel |= 1 << ((pc - root + 12) % 12);
        for (auto &t : T)
            if (t.mask == rel) {
                juce::String s = juce::String(PC[root]) + t.name;
                if (root != lo % 12) s += juce::String("/") + PC[lo % 12];
                return s;
            }
    }
    return juce::String(PC[lo % 12]) + "?";
}

void ChordScreen::paint(juce::Graphics &g)
{
    auto r = getLocalBounds().toFloat();
    paintOled(g, r);
    auto &P = panel.proc;
    auto &F = Fonts::get();
    auto par = [&](int i) { return P.param(i)->convertFrom0to1(P.param(i)->getValue()); };
    bool on = par(GP_CH_ON) > 0.5f, keyMode = par(GP_CH_KEY) > 0.5f;
    int map = (int)std::lround(par(GP_CH_MAP));
    int notes[16], n;
    juce::String top, big, small;
    if (panel.learning) {
        n = AriaProcessor::unpack(P.learnNotes.load(), notes);
        top = panel.learning == 1 ? "LEARN: PLAY THE CHORD" : "LEARN: PRESS ITS KEY";
    } else {
        n = AriaProcessor::unpack(P.lastNotes.load(), notes);
        n += AriaProcessor::unpack(P.lastNotes2.load(), notes + n);
        top = !on ? juce::String("PLAYED (MEMORY OFF)") : keyMode ? "KEY MAP " + juce::String(map + 1) : juce::String("CHORD");
        if (on && !keyMode && n <= 1) { n = AriaProcessor::unpack(P.learnNotes.load(), notes); }
    }
    big = n ? chordName(notes, n) : juce::String("-");
    for (int i = 0; i < n; i++) small += (i ? " " : "") + noteName(notes[i]);
    auto in = r.reduced(10, 6);
    g.setFont(F.oled(10.0f));
    g.setColour(Colour(0xff6f7a80));
    g.drawText(top, in.removeFromTop(12), juce::Justification::left);
    g.setColour(C_OLED);
    g.setFont(F.oled(22.0f));
    g.drawFittedText(big, in.removeFromTop(26).toNearestInt(), juce::Justification::left, 1, 0.6f);
    g.setFont(F.oled(10.0f));
    g.setColour(C_OLED.withAlpha(0.7f));
    g.drawFittedText(small, in.removeFromTop(14).toNearestInt(), juce::Justification::left, 1, 0.6f);
    /* the keyboard: C2..B5; keys with a chord in this map a dot, the notes sounding lit */
    auto kb = in.withTrimmedTop(4);
    const int first = 36, octaves = 4, whites = octaves * 7;
    float ww = kb.getWidth() / (float)whites;
    uint64_t m0 = P.keyMask[0].load(), m1 = P.keyMask[1].load();
    auto has = [&](int k) { return keyMode && (((k < 64 ? m0 >> k : m1 >> (k - 64)) & 1ull) != 0); };
    auto lit = [&](int k) { for (int i = 0; i < n; i++) if (notes[i] == k) return true; return false; };
    static const int WO[7] = { 0, 2, 4, 5, 7, 9, 11 };
    auto ac = panel.accent;
    for (int w = 0; w < whites; w++) {
        int k = first + (w / 7) * 12 + WO[w % 7];
        juce::Rectangle<float> key(kb.getX() + (float)w * ww + 0.5f, kb.getY(), ww - 1.0f, kb.getHeight());
        g.setColour(lit(k) ? ac : C_OLED.withAlpha(0.16f));
        g.fillRect(key);
        if (has(k)) { g.setColour(lit(k) ? Colour(0xff111111) : C_OLED); g.fillEllipse(key.getCentreX() - 1.5f, key.getBottom() - 5.0f, 3.0f, 3.0f); }
    }
    for (int w = 0; w < whites; w++) {
        int d = w % 7;
        if (d == 2 || d == 6) continue;
        int k = first + (w / 7) * 12 + WO[d] + 1;
        juce::Rectangle<float> key(kb.getX() + (float)(w + 1) * ww - ww * 0.32f, kb.getY(), ww * 0.64f, kb.getHeight() * 0.58f);
        g.setColour(Colour(0xff07090a));
        g.fillRect(key.expanded(0.8f, 0));
        g.setColour(lit(k) ? ac : C_OLED.withAlpha(0.32f));
        g.fillRect(key);
        if (has(k)) { g.setColour(C_OLED); g.fillEllipse(key.getCentreX() - 1.5f, key.getBottom() - 5.0f, 3.0f, 3.0f); }
    }
    aria_art::glass(g, r.reduced(3.0f));
}

/* ------------------------------------------------------------------------------------ scope --- */
void Scope::paint(juce::Graphics &g)
{
    auto r = getLocalBounds().toFloat();
    paintOled(g, r);
    auto in = r.reduced(8, 8);
    g.setColour(C_OLED.withAlpha(0.07f));
    for (int i = 1; i < 4; i++) g.drawHorizontalLine((int)(in.getY() + in.getHeight() * (float)i / 4.0f), in.getX(), in.getRight());
    for (int i = 1; i < 8; i++) g.drawVerticalLine((int)(in.getX() + in.getWidth() * (float)i / 8.0f), in.getY(), in.getBottom());
    g.setFont(Fonts::get().oled(9.0f));
    g.setColour(Colour(0xff6f7a80));
    g.drawText("OUT", in.removeFromTop(10).toNearestInt(), juce::Justification::left);
    auto &P = panel.proc;
    const int S = AriaProcessor::SCOPE, len = 640;
    int w = P.scopeW.load();
    /* a rising zero crossing, so the wave stands still */
    int start = (w - len - 1 + S) & (S - 1);
    for (int back = 0; back < 1200; back++) {
        int i = (w - len - 2 - back + 2 * S) & (S - 1), j = (i + 1) & (S - 1);
        if (P.scope[i] <= 0.0f && P.scope[j] > 0.0f) { start = j; break; }
    }
    float peak = 0.05f;
    for (int k = 0; k < len; k++) peak = std::max(peak, std::abs(P.scope[(start + k) & (S - 1)]));
    float gain = 0.9f / std::max(peak, 0.25f);
    juce::Path p;
    for (int k = 0; k < len; k++) {
        float x = in.getX() + in.getWidth() * (float)k / (float)(len - 1);
        float y = in.getCentreY() - juce::jlimit(-1.0f, 1.0f, P.scope[(start + k) & (S - 1)] * gain) * in.getHeight() * 0.5f;
        if (k == 0) p.startNewSubPath(x, y); else p.lineTo(x, y);
    }
    g.setColour(C_OLED.withAlpha(0.15f));
    g.strokePath(p, juce::PathStrokeType(3.5f));
    g.setColour(C_OLED);
    g.strokePath(p, juce::PathStrokeType(1.3f));
    aria_art::glass(g, r.reduced(3.0f));
}

/* ------------------------------------------------------------------------------------ panel --- */
Panel::Panel(AriaProcessor &p) : proc(p)
{
    setSize(W, H);
    build();
    addAndMakeVisible(display);
    display.setBounds(W - 34 - 30 - 300 - 84, 22, 300, 48);
    for (int i = 0; i < 2; i++) {                /* the patch: back, on */
        auto *b = new Button(*this, Button::ACTION, 0, false, i ? "NEXT" : "PREV");
        b->onPress = [this, i] {
            { int n = proc.getNumPrograms(); proc.setCurrentProgram((proc.getCurrentProgram() + (i ? 1 : n - 1)) % n); }
            lastTouched = -1;
        };
        controls.emplace_back(b); addAndMakeVisible(b);
        b->setBounds(W - 34 - 30 - 84 + i * 42, 28, 42, 38);
    }
    tag = juce::ImageCache::getFromMemory(BinaryData::swtdr34mztag_png, BinaryData::swtdr34mztag_pngSize);
    startTimerHz(30);
}

void Panel::touched(int i) { lastTouched = i; touchedAt = juce::Time::getMillisecondCounter(); display.repaint(); }

Knob *Panel::knob(Module &m, int param, bool timbre, const char *label, bool big)
{
    auto *k = new Knob(*this, param, timbre, label, big);
    controls.emplace_back(k); addAndMakeVisible(k); m.knobs.push_back(k);
    return k;
}
Fader *Panel::fader(Module &m, int param, bool timbre, const char *label)
{
    auto *f = new Fader(*this, param, timbre, label);
    controls.emplace_back(f); addAndMakeVisible(f); m.faders.push_back(f);
    return f;
}
Button *Panel::button(Module &m, Button::Kind k, int param, bool timbre, const char *label, int radio)
{
    auto *b = new Button(*this, k, param, timbre, label, radio);
    controls.emplace_back(b); addAndMakeVisible(b); m.buttons.push_back(b);
    return b;
}

void Panel::build()
{
    using B = Button;
    const int X0 = 64, ROW1 = 104, H1 = 300, ROW2 = 420, H2 = 232;
    auto add = [&](const char *title, int x, int y, int w, int h) -> Module & {
        modules.push_back(Module{ title, x, y, w, h, {}, {}, {}, false });
        return modules.back();
    };
    modules.reserve(20);
    int x = X0;
    { auto &m = add("MOD OSC", x, ROW1, 230, H1); x += 230;
      knob(m, TP_MO_FREQ, true, "FREQ"); knob(m, TP_MO_PITCH, true, "PITCH"); knob(m, TP_MO_FILTER, true, "FILTER");
      knob(m, TP_MO_PWM, true, "PWM"); knob(m, TP_MO_VCA, true, "VCA"); knob(m, TP_MO_PAN, true, "PAN");
      button(m, B::SWITCH, TP_MO_AUDIO, true, "AUDIO"); button(m, B::SWITCH, TP_MO_KBTRK, true, "KB");
      button(m, B::SWITCH, TP_MO_KBRESET, true, "RESET"); button(m, B::SWITCH, TP_MO_UNI, true, "UNI");
      button(m, B::CYCLE, TP_MO_WAVE, true, ""); button(m, B::CYCLE, TP_MO_PITCH_DEST, true, "P>");
      button(m, B::CYCLE, TP_MO_FILTER_DEST, true, "F>"); }
    { auto &m = add("OSCILLATOR 1", x, ROW1, 200, H1); x += 200;
      knob(m, TP_O1_FREQ, true, "FREQ"); knob(m, TP_O1_TRISAW, true, "TRI-SAW"); knob(m, TP_O1_PW, true, "WIDTH");
      knob(m, TP_O1_MIX, true, "WAVE MIX"); knob(m, TP_FM_AMT, true, "FM AMT");
      button(m, B::CYCLE, TP_O1_OCT, true, ""); button(m, B::SWITCH, TP_FM_21, true, "FM 2>1"); }
    { auto &m = add("OSCILLATOR 2", x, ROW1, 200, H1); x += 200;
      knob(m, TP_O2_FREQ, true, "FREQ"); knob(m, TP_O2_TRISAW, true, "TRI-SAW"); knob(m, TP_O2_PW, true, "WIDTH");
      knob(m, TP_O2_MIX, true, "WAVE MIX");
      button(m, B::CYCLE, TP_O2_OCT, true, ""); button(m, B::SWITCH, TP_O2_SYNC, true, "SYNC");
      button(m, B::SWITCH, TP_FM_12, true, "FM 1>2"); }
    { auto &m = add("MIXER", x, ROW1, 250, H1); x += 250;
      fader(m, TP_MX_O1, true, "OSC 1"); fader(m, TP_MX_RING, true, "RING"); fader(m, TP_MX_O2, true, "OSC 2");
      fader(m, TP_MX_MOD, true, "MOD"); fader(m, TP_MX_NOISE, true, "NOISE"); fader(m, TP_MX_OVERLOAD, true, "DRIVE"); }
    { auto &m = add("LADDER FILTERS", x, ROW1, 330, H1); x += 330;
      knob(m, TP_F1_CUT, true, "CUTOFF 1", true); knob(m, TP_F1_RES, true, "RES 1"); knob(m, TP_F2_RES, true, "RES 2");
      knob(m, TP_F2_CUT, true, "CUTOFF 2", true); knob(m, TP_F1_ENV, true, "ENV 1")->newRow = true; knob(m, TP_F2_ENV, true, "ENV 2");
      button(m, B::SWITCH, TP_F1_HP, true, "HP 1"); button(m, B::SWITCH, TP_F_LINK, true, "LINK");
      button(m, B::RADIO, TP_F_ORDER, true, "SER", ORD_SERIAL); button(m, B::RADIO, TP_F_ORDER, true, "STR", ORD_STEREO);
      button(m, B::RADIO, TP_F_ORDER, true, "PAR", ORD_PARALLEL);
      button(m, B::CYCLE, TP_F1_KB, true, "KB1"); button(m, B::CYCLE, TP_F2_KB, true, "KB2"); }
    { auto &m = add("FILTER ENV", x, ROW1, 231, H1); x += 231;
      fader(m, TP_FE_A, true, "A"); fader(m, TP_FE_D, true, "D"); fader(m, TP_FE_S, true, "S"); fader(m, TP_FE_R, true, "R");
      knob(m, TP_FE_CURVE, true, "CURVE");
      button(m, B::SWITCH, TP_FE_LOOP, true, "LOOP"); button(m, B::SWITCH, TP_FE_VEL, true, "VEL"); }
    { auto &m = add("AMP ENV", x, ROW1, 231, H1); x += 231;
      fader(m, TP_AE_A, true, "A"); fader(m, TP_AE_D, true, "D"); fader(m, TP_AE_S, true, "S"); fader(m, TP_AE_R, true, "R");
      knob(m, TP_AE_CURVE, true, "CURVE");
      button(m, B::SWITCH, TP_AE_LOOP, true, "LOOP"); button(m, B::SWITCH, TP_AE_VEL, true, "VEL"); }

    x = X0;
    { auto &m = add("LFO 1", x, ROW2, 124, H2); x += 124;
      knob(m, TP_L1_RATE, true, "RATE"); knob(m, TP_L1_AMT, true, "AMT");
      button(m, B::CYCLE, TP_L1_WAVE, true, ""); button(m, B::CYCLE, TP_L1_DEST, true, ">"); }
    { auto &m = add("LFO 2", x, ROW2, 124, H2); x += 124;
      knob(m, TP_L2_RATE, true, "RATE"); knob(m, TP_L2_AMT, true, "AMT");
      button(m, B::CYCLE, TP_L2_WAVE, true, ""); button(m, B::CYCLE, TP_L2_DEST, true, ">"); }
    { auto &m = add("PITCH LFO", x, ROW2, 130, H2); x += 130;
      knob(m, TP_PL_RATE, true, "RATE"); knob(m, TP_PL_SHAPE, true, "SHAPE"); knob(m, TP_PL_AMT, true, "AMT"); }
    { auto &m = add("PERFORM", x, ROW2, 100, H2); x += 100;
      knob(m, TP_GLIDE, true, "GLIDE"); knob(m, TP_BEND, true, "BEND"); }
    { auto &m = add("ARPEGGIATOR", x, ROW2, 198, H2); x += 198;
      knob(m, GP_ARP_GATE, false, "GATE"); knob(m, GP_ARP_SWING, false, "SWING"); knob(m, GP_TEMPO, false, "TEMPO");
      button(m, B::SWITCH, GP_ARP_ON, false, "ON"); button(m, B::SWITCH, GP_ARP_LATCH, false, "LATCH");
      button(m, B::CYCLE, GP_ARP_DIV, false, ""); button(m, B::CYCLE, GP_ARP_MODE, false, "");
      button(m, B::CYCLE, GP_ARP_OCT, false, ""); }
    { auto &m = add("CHORD MEMORY", x, ROW2, 200, H2); x += 200; m.accent = true;
      button(m, B::SWITCH, GP_CH_ON, false, "ON"); button(m, B::SWITCH, GP_CH_KEY, false, "KEY");
      auto *l = button(m, B::ACTION, GP_CH_ON, false, "LEARN");
      l->onPress = [this] { proc.learnRequest.store(1); };
      l->lit = [this] { return learning != 0; };
      l->blink = [this] { return learning != 0; };
      for (int i = 0; i < 8; i++) {
          auto *b = button(m, B::RADIO, GP_CH_MAP, false, "", i);
          b->label = juce::String(i + 1);
          b->small = true;
      } }
    { auto &m = add("VOICES", x, ROW2, 242, H2); x += 242;
      knob(m, TP_DETUNE, true, "DETUNE"); knob(m, GP_VC_SPLIT, false, "SPLIT");
      button(m, B::ACTION, 0, false, "EDIT A")->onPress = [this] { proc.editTimbre.store(0); repaint(); };
      m.buttons.back()->lit = [this] { return proc.editTimbre.load() == 0; };
      button(m, B::ACTION, 0, false, "EDIT B")->onPress = [this] { proc.editTimbre.store(1); repaint(); };
      m.buttons.back()->lit = [this] { return proc.editTimbre.load() == 1; };
      button(m, B::RADIO, GP_VC_MODE, false, "SINGLE", VC_SINGLE); button(m, B::RADIO, GP_VC_MODE, false, "SPLIT", VC_SPLIT);
      button(m, B::RADIO, GP_VC_MODE, false, "STACK", VC_STACK); button(m, B::CYCLE, GP_VC_VOICES_A, false, "A");
      button(m, B::SWITCH, TP_UNISON, true, "UNI"); button(m, B::SWITCH, TP_MONO, true, "MONO"); }
    { auto &m = add("VCA", x, ROW2, 130, H2); x += 130;
      knob(m, TP_VCA_LEVEL, true, "LEVEL"); knob(m, TP_VCA_PAN, true, "PAN"); knob(m, TP_VCA_SPREAD, true, "SPREAD"); }
    { auto &m = add("DIFFUSION DELAY", x, ROW2, 304, H2); x += 304;
      knob(m, GP_DL_TIME_L, false, "TIME L"); knob(m, GP_DL_TIME_R, false, "TIME R"); knob(m, GP_DL_FB, false, "FDBK");
      knob(m, GP_DL_CHAR, false, "CHAR"); knob(m, GP_DL_DIFF, false, "DIFF"); knob(m, GP_DL_MOD, false, "CHORUS");
      knob(m, GP_DL_MIX, false, "MIX");
      button(m, B::SWITCH, GP_DL_LINK, false, "LINK"); button(m, B::SWITCH, GP_DL_A, false, "A");
      button(m, B::SWITCH, GP_DL_B, false, "B"); }
    { auto &m = add("OUTPUT", x, ROW2, 120, H2);
      knob(m, GP_OUT_VOL, false, "VOLUME", true); knob(m, GP_OUT_LOWCUT, false, "LOW CUT"); }
    for (auto &m : modules) layoutModule(m);
    {   /* SAVE: the sound as a preset of the user's own */
        auto *sv = new Button(*this, Button::ACTION, 0, false, "SAVE");
        sv->onPress = [this] {
            auto *w = new juce::AlertWindow("SAVE PRESET", "A name for this sound:", juce::MessageBoxIconType::NoIcon, this);
            w->addTextEditor("name", proc.getProgramName(proc.getCurrentProgram()));
            w->addButton("SAVE", 1, juce::KeyPress(juce::KeyPress::returnKey));
            w->addButton("CANCEL", 0, juce::KeyPress(juce::KeyPress::escapeKey));
            w->enterModalState(true, juce::ModalCallbackFunction::create([this, w](int r) {
                if (r == 1 && proc.savePreset(w->getTextEditorContents("name")) < 0)
                    juce::AlertWindow::showMessageBoxAsync(juce::MessageBoxIconType::WarningIcon, "SAVE PRESET", "Could not save it.");
                lastTouched = -1;
            }), true);
        };
        controls.emplace_back(sv); addAndMakeVisible(sv);
        sv->setBounds(W - 34 - 30 - 300 - 84 - 84 - 72, 28, 64, 38);
    }
    {   /* QUALITY: the oversampling, by the display */
        auto *q = new Button(*this, Button::CYCLE, GP_QUALITY, false, "");
        controls.emplace_back(q); addAndMakeVisible(q);
        q->setBounds(W - 34 - 30 - 300 - 84 - 84, 28, 64, 38);
    }
    {   /* the ARP PATTERN: 16 steps in four groups, its LENGTH and ALL ON either side */
        const int kw = 40, gap = 3, group = 12, total = 16 * kw + 15 * gap + 3 * group;
        int x0 = (W - total) / 2, y0 = 680;
        for (int i = 0; i < 16; i++) {
            auto *k = new StepKey(*this, i);
            controls.emplace_back(k); addAndMakeVisible(k); steps.push_back(k);
            k->setBounds(x0 + i * (kw + gap) + (i / 4) * group, y0, kw, 58);
        }
        auto *len = new Button(*this, Button::CYCLE, GP_ARP_LEN, false, "LENGTH");
        controls.emplace_back(len); addAndMakeVisible(len);
        len->setBounds(x0 - 96, y0 + 12, 76, 38);
        auto *all = new Button(*this, Button::ACTION, 0, false, "ALL ON");
        all->onPress = [this] {
            for (int i = 0; i < 16; i++) { auto *p = proc.param(GP_ARP_STEP1 + i); p->beginChangeGesture(); p->setValueNotifyingHost(1.0f); p->endChangeGesture(); }
        };
        controls.emplace_back(all); addAndMakeVisible(all);
        all->setBounds(x0 + total + 20, y0 + 12, 60, 38);
        patternX = x0 - 110; patternW = total + 220;
    }
    for (auto &m : modules) {
        if (m.title == "MIXER") { addAndMakeVisible(scope); scope.setBounds(m.x + 10, m.y + 182, m.w - 20, m.h - 188); }
        if (m.title == "CHORD MEMORY") { addAndMakeVisible(chordScreen); chordScreen.setBounds(m.x + 10, m.y + 26, m.w - 20, 116); }
    }
}

/* knobs flow from the top, faders under them, buttons from the bottom; each row centred */
void Panel::layoutModule(Module &m)
{
    const int pad = 10, inner = m.w - 2 * pad;
    int y = m.y + 28;
    auto breaks = [](juce::Component *c) { auto *k = dynamic_cast<Knob *>(c); return k != nullptr && k->newRow; };
    auto flow = [&](auto &items, auto wOf, auto hOf, int gapX, int gapY, int yTop) {
        size_t i = 0;
        int yy = yTop;
        while (i < items.size()) {
            size_t j = i; int rowW = 0, rowH = 0;
            while (j < items.size() && rowW + wOf(items[j]) + (j > i ? gapX : 0) <= inner && !(j > i && breaks(items[j]))) { rowW += wOf(items[j]) + (j > i ? gapX : 0); rowH = std::max(rowH, hOf(items[j])); j++; }
            if (j == i) j = i + 1;
            int xx = m.x + pad + (inner - rowW) / 2;
            for (size_t k = i; k < j; k++) { items[k]->setBounds(xx, yy, wOf(items[k]), hOf(items[k])); xx += wOf(items[k]) + gapX; }
            yy += rowH + gapY;
            i = j;
        }
        return yy;
    };
    auto kw = [](Knob *k) { return k->big ? 70 : 50; };
    auto kh = [](Knob *k) { return k->big ? 84 : 68; };
    auto fw = [](Fader *) { return 32; };
    auto fh = [](Fader *) { return 134; };
    if (!m.faders.empty()) y = flow(m.faders, fw, fh, 2, 8, y) + 2;
    if (!m.knobs.empty()) y = flow(m.knobs, kw, kh, 4, 6, y);
    /* buttons: count the rows first, then lay them from the bottom */
    auto bw = [](Button *b) { return b->small ? 20 : (b->kind == Button::CYCLE ? 58 : 42); };
    auto bh = [](Button *b) { return b->small ? 20 : 38; };
    if (!m.buttons.empty()) {
        std::vector<Button *> big, dots;
        for (auto *b : m.buttons) (b->small ? dots : big).push_back(b);
        int hb = 0;
        { int rowW = 0, rows = big.empty() ? 0 : 1; for (auto *b : big) { if (rowW + bw(b) > inner) { rows++; rowW = 0; } rowW += bw(b) + 4; } hb = rows * 42; }
        if (!dots.empty()) hb += 36;
        int yb = m.y + m.h - hb - 6;
        int after = flow(big, bw, bh, 4, 4, yb);
        if (!dots.empty()) flow(dots, bw, bh, 2, 4, after + 2);
    }
}

/* a knob's fixed parts: its scale, its body (the shadow, the skirt, the cap) and its label */
void Panel::paintKnob(juce::Graphics &g, Knob &k)
{
    auto b = k.getBounds().toFloat();
    float d = k.big ? 50.0f : 36.0f, ring = d + 14.0f;
    float cx = b.getCentreX(), cy = b.getY() + ring * 0.5f + 1.0f;
    bool bip = aria_param_info(k.index())->kind == PK_BIPOLAR;
    for (int i = 0; i <= 10; i++) {
        float a = -0.75f * PI + 1.5f * PI * (float)i / 10.0f;
        bool major = i == 0 || i == 10 || (bip && i == 5);
        float r0 = ring * 0.5f - (major ? 5.0f : 3.5f), r1 = ring * 0.5f;
        g.setColour(major ? Colour(0xffd9dadc) : Colour(0xff8a8d91));
        g.drawLine(cx + std::sin(a) * r0, cy - std::cos(a) * r0, cx + std::sin(a) * r1, cy - std::cos(a) * r1, major ? 1.4f : 1.0f);
    }
    auto img = aria_art::knob(d);
    float size = d + 26.0f;
    g.drawImage(img, juce::Rectangle<float>(cx - size * 0.5f, cy - size * 0.5f + 2.0f, size, size), juce::RectanglePlacement::stretchToFit);
    g.setColour(Colour(0xffd4d5d7));
    g.setFont(Fonts::get().label(11.5f));
    g.drawFittedText(k.label, (int)b.getX() - 6, (int)(b.getY() + ring + 4), (int)b.getWidth() + 12, 14, juce::Justification::centredTop, 1, 0.8f);
}

/* a fader's fixed parts: the slot cut in the panel, the scale, the label */
void Panel::paintFader(juce::Graphics &g, Fader &f)
{
    auto b = f.getBounds().toFloat();
    float cx = b.getCentreX(), top = b.getY() + 4.0f, len = 110.0f;
    juce::Rectangle<float> slot(cx - 2.5f, top, 5.0f, len);
    g.setColour(Colour(0x40ffffff)); g.fillRoundedRectangle(slot.translated(0, 1.0f), 2.5f);
    juce::ColourGradient sg(Colour(0xff000000), slot.getX(), 0, Colour(0xff141517), slot.getRight(), 0, false);
    g.setGradientFill(sg); g.fillRoundedRectangle(slot, 2.5f);
    for (int i = 0; i <= 10; i++) {
        float y = top + 7.0f + (len - 14.0f) * (float)i / 10.0f;
        bool major = i == 0 || i == 5 || i == 10;
        g.setColour(major ? Colour(0xffc9cacc) : Colour(0xff76797d));
        g.drawLine(cx - (major ? 14.0f : 12.0f), y, cx - 8.0f, y, 1.0f);
        g.drawLine(cx + 8.0f, y, cx + (major ? 14.0f : 12.0f), y, 1.0f);
    }
    g.setColour(Colour(0xffd4d5d7));
    g.setFont(Fonts::get().label(11.5f));
    g.drawFittedText(f.label, (int)b.getX() - 4, (int)(top + len + 6), (int)b.getWidth() + 8, 14, juce::Justification::centredTop, 1, 0.8f);
}

void Panel::paint(juce::Graphics &g)
{
    using namespace aria_art;
    if (!face.isValid()) {
        face = aria_art::panel(W - 68, H);
        sideL = sidePlate(34, H, false);
        sideR = sidePlate(34, H, true);
    }
    g.drawImage(sideL, juce::Rectangle<float>(0, 0, 34, (float)H), juce::RectanglePlacement::stretchToFit);
    g.drawImage(sideR, juce::Rectangle<float>((float)W - 34, 0, 34, (float)H), juce::RectanglePlacement::stretchToFit);
    g.drawImage(face, juce::Rectangle<float>(34, 0, (float)W - 68, (float)H), juce::RectanglePlacement::stretchToFit);
    for (float y : { 26.0f, (float)H - 26.0f }) {
        screw(g, { 17.0f, y }, 5.0f, 0.4f + y * 0.01f);
        screw(g, { (float)W - 17.0f, y }, 5.0f, 1.1f - y * 0.013f);
    }
    /* the header */
    auto &F = Fonts::get();
    auto brand = F.heading(36.0f).withExtraKerningFactor(0.42f);
    g.setColour(Colour(0x66000000));
    g.setFont(brand);
    g.drawText("ARIA", 64, 23, 220, 44, juce::Justification::left);
    g.setColour(Colour(0xfff2f2f0));
    g.drawText("ARIA", 64, 22, 220, 44, juce::Justification::left);
    float aw = juce::GlyphArrangement::getStringWidth(brand, "ARIA");
    juce::ColourGradient dot(accent.brighter(0.4f), 64.0f + aw - 3.0f, 56.0f, accent.darker(0.3f), 64.0f + aw + 4.0f, 63.0f, false);
    g.setGradientFill(dot);
    g.fillEllipse(64.0f + aw - 4.0f, 56.0f, 7.0f, 7.0f);
    g.setColour(Colour(0xff9a9ca0));
    g.setFont(F.heading(11.0f));
    g.drawText(juce::String::fromUTF8("8-VOICE  \xc2\xb7  BI-TIMBRAL  \xc2\xb7  DUAL LADDER  \xc2\xb7  ANALOG MODELLING"), 64 + (int)aw + 26, 40, 520, 20, juce::Justification::left);
    auto groove = [&](float y) {                     /* a line cut in the panel: dark, lit below */
        g.setColour(Colour(0xcc000000)); g.drawLine(64.0f, y, (float)W - 64.0f, y, 1.0f);
        g.setColour(Colour(0x1effffff)); g.drawLine(64.0f, y + 1.0f, (float)W - 64.0f, y + 1.0f, 1.0f);
    };
    groove(86.0f);
    /* the modules: headings and separators */
    int lastRowY = -1;
    for (auto &m : modules) {
        if (m.y == lastRowY) {
            g.setColour(Colour(0xaa000000)); g.drawLine((float)m.x, (float)m.y + 2, (float)m.x, (float)(m.y + m.h), 1.0f);
            g.setColour(Colour(0x14ffffff)); g.drawLine((float)m.x + 1, (float)m.y + 2, (float)m.x + 1, (float)(m.y + m.h), 1.0f);
        }
        lastRowY = m.y;
        auto hf = F.heading(11.5f);
        float tw = juce::GlyphArrangement::getStringWidth(hf, m.title);
        float cx = (float)m.x + (float)m.w * 0.5f, hy = (float)m.y + 9.0f;
        g.setColour(Colour(0xff4a4c50));
        g.drawLine((float)m.x + 12, hy, cx - tw * 0.5f - 8, hy, 1.0f);
        g.drawLine(cx + tw * 0.5f + 8, hy, (float)(m.x + m.w) - 12, hy, 1.0f);
        g.setColour(m.accent ? accent : Colour(0xffeceded));
        g.setFont(hf);
        g.drawText(m.title, juce::Rectangle<float>(cx - tw * 0.5f - 4, hy - 8, tw + 8, 16), juce::Justification::centred);
        if (m.accent) {
            g.setColour(Colour(0xff9a9ca0));
            g.setFont(F.heading(10.0f));
            g.drawText("KEY MAP", m.x, m.y + m.h - 4, m.w, 14, juce::Justification::centred);
        }
        for (auto *f : m.faders) paintFader(g, *f);
        for (auto *k : m.knobs) paintKnob(g, *k);
    }
    {   /* the ARP PATTERN strip's heading */
        auto hf = F.heading(11.5f);
        juce::String t("ARP PATTERN");
        float tw = juce::GlyphArrangement::getStringWidth(hf, t), cx = (float)W * 0.5f, hy = 672.0f;
        g.setColour(Colour(0xff4a4c50));
        g.drawLine((float)patternX, hy, cx - tw * 0.5f - 8, hy, 1.0f);
        g.drawLine(cx + tw * 0.5f + 8, hy, (float)(patternX + patternW), hy, 1.0f);
        g.setColour(Colour(0xffeceded)); g.setFont(hf);
        g.drawText(t, juce::Rectangle<float>(cx - tw * 0.5f - 4, hy - 8, tw + 8, 16), juce::Justification::centred);
    }
    groove((float)H - 46.0f);
    g.setColour(Colour(0xff7c7e82));
    g.setFont(F.heading(11.0f));
    g.drawText(juce::String::fromUTF8("VST3  \xc2\xb7  STANDALONE  \xc2\xb7  MPC"), 64, H - 38, 400, 20, juce::Justification::left);
    if (tag.isValid()) {
        float th = 14.0f, tw = th * (float)tag.getWidth() / (float)tag.getHeight();
        g.setOpacity(0.92f);
        g.drawImage(tag, juce::Rectangle<float>((float)W - 64.0f - tw, (float)H - 35.0f, tw, th), juce::RectanglePlacement::centred);
        g.setOpacity(1.0f);
    }
}

void Panel::timerCallback()
{
    blinkPhase++;
    int l = proc.learning.load();
    bool all = l != learning || proc.editTimbre.load() != shownEdit;
    learning = l;
    shownEdit = proc.editTimbre.load();
    arpStep = proc.arpStep.load();
    if (arpStep != shownStep) { shownStep = arpStep; for (auto *k : steps) k->repaint(); }
    if (proc.param(GP_ARP_LEN)->getValue() != shownLen) { shownLen = proc.param(GP_ARP_LEN)->getValue(); for (auto *k : steps) k->repaint(); }
    for (auto &c : controls)
        if (c->changed() || all || (learning && dynamic_cast<Button *>(c.get()))) c->repaint();
    display.repaint();
    chordScreen.repaint();
    scope.repaint();
}

/* ----------------------------------------------------------------------------------- editor --- */
AriaEditor::AriaEditor(AriaProcessor &p) : AudioProcessorEditor(&p), proc(p), panel(p)
{
    addAndMakeVisible(panel);
    setResizable(true, true);
    getConstrainer()->setFixedAspectRatio((double)Panel::W / Panel::H);
    setResizeLimits(Panel::W / 2, Panel::H / 2, Panel::W * 3 / 2, Panel::H * 3 / 2);
    float s = proc.uiScale.load();
    setSize((int)std::lround(Panel::W * s), (int)std::lround(Panel::H * s));
}

void AriaEditor::resized()
{
    float s = (float)getWidth() / (float)Panel::W;
    panel.setTransform(juce::AffineTransform::scale(s));
    proc.uiScale.store(s);
}
