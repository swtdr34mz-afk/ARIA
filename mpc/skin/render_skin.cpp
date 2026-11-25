/* SPDX-License-Identifier: MPL-2.0
 * Copyright (C) 2026 swtdr34mz (ARIA) */
/* ARIA's MPC skin, drawn with the VST3's own art (src/Art.cpp: the brushed panel, the knobs, the fader caps; the
 * panel's fonts, section colours and lit buttons): each page laid out as the VST's sections side by side, then cut
 * up: its background (the panel, the sections' plates and headings, the maker's tag), and every control at every
 * value it takes (a knob's or fader's film strip; a switch's two looks; a choice of up to four as a button each,
 * longer ones a button that steps; the arp's 16 step keys). manifest.json says what each picture is, where it goes
 * and which MPC parameter it shows, for make_skin.py. With --preview, a picture of each page as the MPC shows it,
 * with the function keys under it, instead.
 *
 *   OSC     OSCILLATOR 1, OSCILLATOR 2, MOD OSC;  again: ENV (FILTER ENV, AMP ENV)          (function key 1)
 *   MIXER   MIXER, VCA, OUTPUT                                                         (2)
 *   FILTER  LADDER FILTERS;  again: VOICES                                             (3)
 *   LFO     LFO 1, LFO 2, PITCH LFO                                                    (4)
 *   FX      DIFFUSION DELAY                                                            (5)
 *   DIV     LOW SPLIT, CHORD MEMORY, PERFORM;  again: ARP (ARPEGGIATOR: knobs and buttons above, the pattern in 2 x 8 below)   (6)
 *
 * A page is laid out at 2/3 of the MPC's 1280 x 628 and drawn at 1.5x: the VST's knobs and buttons, touch-sized.
 *
 *   aria_skin_render <out folder> [--preview] [factory sound]                                                      */
#include "../../src/PluginEditor.h"
#include "../../src/Art.h"
#include "../../dsp/presets.h"
#include "BinaryData.h"
#include <vector>

using juce::Colour;
using juce::Rectangle;
namespace {
constexpr int TW = 1280, TH = 628;       /* a page on the MPC */
constexpr float S = 1.5f, W = TW / S, H = TH / S;
constexpr int P_INIT = -2, P_LEARN = -3;  /* the MPC plugin's own: INIT and the chord memory's LEARN */

/* the MPC plugin's parameter numbers (mpc/plugin/src/AriaMpc.cpp): VOLUME first, the engine's in order, INIT, LEARN */
int mpcIndex(int p)
{
    if (p == P_INIT) return ARIA_NPARAM;
    if (p == P_LEARN) return ARIA_NPARAM + 1;
    return p == GP_OUT_VOL ? 0 : p < GP_OUT_VOL ? p + 1 : p;
}

enum Kind { KNOB, FADER, SWITCH, CHOICE, STEPS };   /* CHOICE: a button a choice (up to 4), else one that steps */
struct Ctl { int p; Kind k; const char *label; };
struct Sec { const char *title; float w; int cols; std::vector<Ctl> c; };   /* title "": the one before goes on */
struct Page { const char *name; int key, sub; std::vector<Sec> s; std::vector<std::vector<int>> q; };
constexpr int A = 0;
#define T(x) TP(A, TP_##x)

const char *KEYS[6] = { "OSC", "MIXER", "FILTER", "LFO", "FX", "DIV" };
const char *SUBS[6] = { "ENV", "", "VOICES", "", "", "ARP" };

std::vector<Page> pages()
{
    return {
        { "OSC", 0, 0, {
            { "OSCILLATOR 1", 1.0f, 2, { { T(O1_FREQ), KNOB, "FREQ" }, { T(O1_TRISAW), KNOB, "TRI-SAW" }, { T(O1_PW), KNOB, "WIDTH" },
                                        { T(O1_MIX), KNOB, "WAVE MIX" }, { T(FM_AMT), KNOB, "FM AMT" },
                                        { T(O1_OCT), CHOICE, "" }, { T(FM_21), SWITCH, "FM 2>1" } } },
            { "OSCILLATOR 2", 1.0f, 2, { { T(O2_FREQ), KNOB, "FREQ" }, { T(O2_TRISAW), KNOB, "TRI-SAW" }, { T(O2_PW), KNOB, "WIDTH" },
                                        { T(O2_MIX), KNOB, "WAVE MIX" },
                                        { T(O2_OCT), CHOICE, "" }, { T(O2_SYNC), SWITCH, "SYNC" }, { T(FM_12), SWITCH, "FM 1>2" } } },
            { "MOD OSC", 1.45f, 3, { { T(MO_FREQ), KNOB, "FREQ" }, { T(MO_PITCH), KNOB, "PITCH" }, { T(MO_FILTER), KNOB, "FILTER" },
                                    { T(MO_PWM), KNOB, "PWM" }, { T(MO_VCA), KNOB, "VCA" }, { T(MO_PAN), KNOB, "PAN" },
                                    { T(MO_WAVE), CHOICE, "" }, { T(MO_AUDIO), SWITCH, "AUDIO" }, { T(MO_KBTRK), SWITCH, "KB" },
                                    { T(MO_KBRESET), SWITCH, "RESET" }, { T(MO_UNI), SWITCH, "UNI" },
                                    { T(MO_PITCH_DEST), CHOICE, "P>" }, { T(MO_FILTER_DEST), CHOICE, "F>" } } } },
          { { T(O1_FREQ), T(O1_TRISAW), T(O1_PW), T(O1_MIX) }, { T(O2_FREQ), T(O2_TRISAW), T(O2_PW), T(O2_MIX) },
            { T(MO_FREQ), T(MO_PITCH), T(MO_FILTER), T(MO_PWM) }, { T(MO_VCA), T(MO_PAN), T(FM_AMT), -1 } } },
        { "MIXER", 1, 0, {
            { "MIXER", 1.7f, 6, { { T(MX_O1), FADER, "OSC 1" }, { T(MX_RING), FADER, "RING" }, { T(MX_O2), FADER, "OSC 2" },
                                 { T(MX_MOD), FADER, "MOD" }, { T(MX_NOISE), FADER, "NOISE" }, { T(MX_OVERLOAD), FADER, "DRIVE" } } },
            { "VCA", 1.0f, 2, { { T(VCA_LEVEL), KNOB, "LEVEL" }, { T(VCA_PAN), KNOB, "PAN" }, { T(VCA_SPREAD), KNOB, "SPREAD" } } },
            { "OUTPUT", 1.0f, 1, { { GP_OUT_VOL, KNOB, "VOLUME" }, { GP_OUT_LOWCUT, KNOB, "LOW CUT" }, { GP_QUALITY, CHOICE, "" },
                                  { P_INIT, SWITCH, "INIT" } } } },
          { { T(MX_O1), T(MX_RING), T(MX_O2), T(MX_MOD) }, { T(MX_NOISE), T(MX_OVERLOAD), -1, -1 },
            { T(VCA_LEVEL), T(VCA_PAN), T(VCA_SPREAD), -1 }, { GP_OUT_VOL, GP_OUT_LOWCUT, -1, -1 } } },
        { "FILTER", 2, 0, {
            { "LADDER FILTERS", 1.0f, 3, { { T(F1_CUT), KNOB, "CUTOFF 1" }, { T(F1_RES), KNOB, "RES 1" }, { T(F1_ENV), KNOB, "ENV 1" },
                                          { T(F2_CUT), KNOB, "CUTOFF 2" }, { T(F2_RES), KNOB, "RES 2" }, { T(F2_ENV), KNOB, "ENV 2" },
                                          { T(F1_HP), SWITCH, "HP 1" }, { T(F_LINK), SWITCH, "LINK" }, { T(F_ORDER), CHOICE, "" },
                                          { T(F1_KB), CHOICE, "KB1" }, { T(F2_KB), CHOICE, "KB2" } } } },
          { { T(F1_CUT), T(F1_RES), T(F1_ENV), -1 }, { T(F2_CUT), T(F2_RES), T(F2_ENV), -1 } } },
        { "VOICES", 2, 1, {
            { "VOICES", 1.0f, 3, { { T(DETUNE), KNOB, "DETUNE" }, { GP_VC_SPLIT, KNOB, "SPLIT" }, { GP_VC_VOICES_A, KNOB, "A VOICES" },
                                  { GP_VC_MODE, CHOICE, "" }, { T(UNISON), SWITCH, "UNI" }, { T(MONO), SWITCH, "MONO" } } } },
          { { T(DETUNE), GP_VC_SPLIT, GP_VC_VOICES_A, -1 } } },
        { "ENV", 0, 1, {
            { "FILTER ENV", 1.0f, 4, { { T(FE_A), FADER, "A" }, { T(FE_D), FADER, "D" }, { T(FE_S), FADER, "S" }, { T(FE_R), FADER, "R" },
                                      { T(FE_CURVE), KNOB, "CURVE" }, { T(FE_LOOP), SWITCH, "LOOP" }, { T(FE_VEL), SWITCH, "VEL" } } },
            { "AMP ENV", 1.0f, 4, { { T(AE_A), FADER, "A" }, { T(AE_D), FADER, "D" }, { T(AE_S), FADER, "S" }, { T(AE_R), FADER, "R" },
                                   { T(AE_CURVE), KNOB, "CURVE" }, { T(AE_LOOP), SWITCH, "LOOP" }, { T(AE_VEL), SWITCH, "VEL" } } } },
          { { T(FE_A), T(FE_D), T(FE_S), T(FE_R) }, { T(AE_A), T(AE_D), T(AE_S), T(AE_R) }, { T(FE_CURVE), T(AE_CURVE), -1, -1 } } },
        { "LFO", 3, 0, {
            { "LFO 1", 1.0f, 2, { { T(L1_RATE), KNOB, "RATE" }, { T(L1_AMT), KNOB, "AMT" }, { T(L1_WAVE), CHOICE, "" }, { T(L1_DEST), CHOICE, "" } } },
            { "LFO 2", 1.0f, 2, { { T(L2_RATE), KNOB, "RATE" }, { T(L2_AMT), KNOB, "AMT" }, { T(L2_WAVE), CHOICE, "" }, { T(L2_DEST), CHOICE, "" } } },
            { "PITCH LFO", 1.0f, 3, { { T(PL_RATE), KNOB, "RATE" }, { T(PL_SHAPE), KNOB, "SHAPE" }, { T(PL_AMT), KNOB, "AMT" } } } },
          { { T(L1_RATE), T(L1_AMT), T(L2_RATE), T(L2_AMT) }, { T(PL_RATE), T(PL_SHAPE), T(PL_AMT), -1 } } },
        { "FX", 4, 0, {
            { "DIFFUSION DELAY", 1.0f, 4, { { GP_DL_TIME_L, KNOB, "TIME L" }, { GP_DL_TIME_R, KNOB, "TIME R" }, { GP_DL_FB, KNOB, "FDBK" },
                                           { GP_DL_MIX, KNOB, "MIX" }, { GP_DL_CHAR, KNOB, "CHAR" }, { GP_DL_DIFF, KNOB, "DIFF" },
                                           { GP_DL_MOD, KNOB, "CHORUS" }, { GP_DL_LINK, SWITCH, "LINK" }, { GP_DL_A, SWITCH, "A" },
                                           { GP_DL_B, SWITCH, "B" } } } },
          { { GP_DL_TIME_L, GP_DL_TIME_R, GP_DL_FB, GP_DL_MIX }, { GP_DL_CHAR, GP_DL_DIFF, GP_DL_MOD, -1 } } },
        { "DIV", 5, 0, {
            { "LOW SPLIT", 1.0f, 3, { { GP_LS_SPLIT, KNOB, "SPLIT" }, { GP_LS_POINT, KNOB, "POINT" }, { GP_LS_BALANCE, KNOB, "BALANCE" },
                                     { GP_LS_HOLD, SWITCH, "LOW HOLD" } } },
            { "CHORD MEMORY", 0.85f, 1, { { GP_CH_MAP, KNOB, "KEY MAP" }, { GP_CH_ON, SWITCH, "ON" }, { GP_CH_KEY, SWITCH, "KEY" },
                                        { P_LEARN, SWITCH, "LEARN" } } },
            { "PERFORM", 0.85f, 2, { { T(GLIDE), KNOB, "GLIDE" }, { T(BEND), KNOB, "BEND" }, { GP_HOLD, SWITCH, "HOLD" } } } },
          { { GP_LS_SPLIT, GP_LS_POINT, GP_LS_BALANCE, -1 }, { GP_CH_MAP, T(GLIDE), T(BEND), -1 } } },
        { "ARP", 5, 1, {
            { "ARPEGGIATOR", 3.25f, 3, { { GP_ARP_GATE, KNOB, "GATE" }, { GP_ARP_SWING, KNOB, "SWING" }, { GP_TEMPO, KNOB, "TEMPO" },
                                        { GP_ARP_ON, SWITCH, "ON" }, { GP_ARP_LATCH, SWITCH, "LATCH" }, { GP_ARP_DIV, CHOICE, "" },
                                        { GP_ARP_MODE, CHOICE, "" }, { GP_ARP_OCT, CHOICE, "" },
                                        { GP_ARP_STEP1, STEPS, "" }, { GP_ARP_LEN, CHOICE, "LENGTH" } } } },   /* (and its pattern) */
          { { GP_ARP_GATE, GP_ARP_SWING, GP_TEMPO, -1 } } },
    };
}

float V[ARIA_NPARAM + 2];                /* the values drawn (INIT and LEARN at the end) */
float &val(int p) { return V[p == P_INIT ? ARIA_NPARAM : p == P_LEARN ? ARIA_NPARAM + 1 : p]; }
juce::String vtext(int p) { return p < 0 ? juce::String() : ariaValueText(p, V[p]); }
int nchoices(int p) { if (p < 0) return 0; auto *i = aria_param_info(p); return i->kind == PK_CHOICE ? i->count : 0; }
bool isSwitch(int p) { return p < 0 || aria_param_info(p)->kind == PK_SWITCH; }
float pmax(int p) { return p < 0 ? 1.0f : aria_param_max(p); }

/* ------------------------------------------------------------------ the controls' looks (as the panel's) --- */
void drawKnob(juce::Graphics &g, Rectangle<float> r, int p, const char *label, float d)
{
    float cx = r.getCentreX(), top = r.getY(), ring = d + 14.0f, cy = top + ring * 0.5f;
    bool bip = aria_param_info(p)->kind == PK_BIPOLAR, step = nchoices(p) > 0;
    int n = step ? nchoices(p) - 1 : 10;
    for (int i = 0; i <= n; i++) {
        float a = -0.75f * 3.14159265f + 1.5f * 3.14159265f * (float)i / (float)n;
        bool major = step || i == 0 || i == n || (bip && i == 5);
        float r0 = ring * 0.5f - (major ? 5.0f : 3.5f), r1 = ring * 0.5f;
        g.setColour(major ? Colour(0xffd9dadc) : Colour(0xff8a8d91));
        g.drawLine(cx + std::sin(a) * r0, cy - std::cos(a) * r0, cx + std::sin(a) * r1, cy - std::cos(a) * r1, major ? 1.4f : 1.0f);
    }
    float size = d + 26.0f;
    g.drawImage(aria_art::knob(d), Rectangle<float>(cx - size * 0.5f, cy - size * 0.5f + 2.0f, size, size), juce::RectanglePlacement::stretchToFit);
    float v = V[p] / pmax(p), R = d * 0.5f, rc = R * 0.6f;
    float a = -0.75f * 3.14159265f + 1.5f * 3.14159265f * v, sx = std::sin(a), sy = -std::cos(a);
    g.setColour(Colour(0xb3000000));
    g.drawLine(cx + sx * (rc + 1.5f) + 0.6f, cy + sy * (rc + 1.5f) + 0.8f, cx + sx * (R - 1.0f) + 0.6f, cy + sy * (R - 1.0f) + 0.8f, 2.4f);
    g.setColour(Colour(0xfff7f7f5));
    g.drawLine(cx + sx * (rc + 1.5f), cy + sy * (rc + 1.5f), cx + sx * (R - 1.0f), cy + sy * (R - 1.0f), 2.0f);
    auto &F = aria_ui::Fonts::get();
    g.setColour(Colour(0xffd4d5d7)); g.setFont(F.label(11.5f));
    g.drawText(label, Rectangle<float>(r.getX(), top + ring + 3, r.getWidth(), 14), juce::Justification::centredTop);
    g.setColour(Colour(0xff8c8e92)); g.setFont(F.oled(10.0f));
    g.drawText(vtext(p), Rectangle<float>(r.getX(), top + ring + 17, r.getWidth(), 12), juce::Justification::centredTop);
}

void drawFader(juce::Graphics &g, Rectangle<float> r, int p, const char *label, float len)
{
    float cx = r.getCentreX(), top = r.getY() + 6.0f;
    Rectangle<float> slot(cx - 2.5f, top, 5.0f, len);
    g.setColour(Colour(0x40ffffff)); g.fillRoundedRectangle(slot.translated(0, 1.0f), 2.5f);
    g.setColour(Colour(0xff050506)); g.fillRoundedRectangle(slot, 2.5f);
    for (int i = 0; i <= 10; i++) {
        float y = top + 7.0f + (len - 14.0f) * (float)i / 10.0f;
        bool major = i == 0 || i == 5 || i == 10;
        g.setColour(major ? Colour(0xffc9cacc) : Colour(0xff76797d));
        g.drawLine(cx - (major ? 14.0f : 12.0f), y, cx - 8.0f, y, 1.0f);
        g.drawLine(cx + 8.0f, y, cx + (major ? 14.0f : 12.0f), y, 1.0f);
    }
    float y = top + 7.0f + (len - 14.0f) * (1.0f - V[p]) - 7.0f;
    g.drawImage(aria_art::faderCap(), Rectangle<float>(cx - 16.0f, y - 4.0f, 32.0f, 26.0f), juce::RectanglePlacement::stretchToFit);
    auto &F = aria_ui::Fonts::get();
    g.setColour(Colour(0xffd4d5d7)); g.setFont(F.label(11.5f));
    g.drawText(label, Rectangle<float>(r.getX(), top + len + 6, r.getWidth(), 14), juce::Justification::centredTop);
    g.setColour(Colour(0xff8c8e92)); g.setFont(F.oled(10.0f));
    g.drawText(vtext(p), Rectangle<float>(r.getX(), top + len + 20, r.getWidth(), 12), juce::Justification::centredTop);
}

void drawButton(juce::Graphics &g, Rectangle<float> cell, Colour ac, bool on, const juce::String &label, bool white)
{
    Rectangle<float> b(cell.getCentreX() - 17.0f, cell.getY() + 6.0f, 34.0f, 16.0f);
    float rad = 3.5f;
    g.setColour(Colour(0xff050506)); g.fillRoundedRectangle(b.expanded(1.6f), rad + 1.4f);
    if (on) {
        for (int i = 4; i >= 1; i--) { g.setColour(ac.withAlpha(0.10f + 0.05f * (float)(4 - i))); g.fillRoundedRectangle(b.expanded((float)i * 0.9f + 0.6f), rad + (float)i); }
        juce::ColourGradient body(Colour(0xffffffff).interpolatedWith(ac, 0.35f), b.getCentreX(), b.getCentreY(), ac.darker(0.15f), b.getRight(), b.getBottom(), true);
        g.setGradientFill(body); g.fillRoundedRectangle(b, rad);
        g.setColour(Colour(0x80ffffff)); g.drawLine(b.getX() + 2, b.getY() + 1.0f, b.getRight() - 2, b.getY() + 1.0f, 0.8f);
    } else {
        juce::ColourGradient body(Colour(0xff3a3b3f).interpolatedWith(ac, 0.22f), 0, b.getY(), Colour(0xff1c1d20).interpolatedWith(ac, 0.16f), 0, b.getBottom(), false);
        g.setGradientFill(body); g.fillRoundedRectangle(b, rad);
        g.setColour(Colour(0x40ffffff)); g.drawLine(b.getX() + 2, b.getY() + 0.8f, b.getRight() - 2, b.getY() + 0.8f, 0.7f);
    }
    g.setColour(Colour(0xff020202)); g.drawRoundedRectangle(b, rad, 0.8f);
    g.setColour(on || white ? Colour(0xffe9e9ea) : Colour(0xffa4a6aa));
    g.setFont(aria_ui::Fonts::get().label(10.5f));
    g.drawFittedText(label, (int)cell.getX(), (int)b.getBottom() + 4, (int)cell.getWidth(), 13, juce::Justification::centredTop, 1, 0.7f);
}

void drawStep(juce::Graphics &g, Rectangle<float> r, int i, Colour ac)
{
    bool on = V[GP_ARP_STEP1 + i] > 0.5f;
    Rectangle<float> k(r.getX() + 2.5f, r.getY() + 14, r.getWidth() - 5.0f, r.getHeight() - 16.0f);
    g.setColour(Colour(0xff050506)); g.fillRoundedRectangle(k.expanded(1.5f), 4.0f);
    juce::ColourGradient body(on ? Colour(0xfff4f1ea) : Colour(0xff9b988f), 0, k.getY(), on ? Colour(0xffd6d2c8) : Colour(0xff6e6b64), 0, k.getBottom(), false);
    g.setGradientFill(body); g.fillRoundedRectangle(k, 3.0f);
    g.setColour(Colour(0xff3a3a3a)); g.setFont(aria_ui::Fonts::get().label(10.0f));
    g.drawText(juce::String(i + 1), k.withTrimmedTop(k.getHeight() - 22.0f), juce::Justification::centred);
    Rectangle<float> led(k.getCentreX() - 7, r.getY() + 3, 14, 5);
    g.setColour(on ? ac : ac.withMultipliedBrightness(0.25f)); g.fillRoundedRectangle(led, 2.0f);
}

/* ------------------------------------------------------------------------------------ the layout --- */
struct Item {
    enum IK { IKNOB, IFADER, ISWITCH, IRADIO, ICYCLE, ISTEP } k;
    int p, choice = 0;                   /* IRADIO: its choice; ISTEP: the step */
    juce::String label;
    const char *klabel = "";
    Rectangle<float> r;                  /* in the page's units (x 1.5 on the MPC) */
    Colour col;
    float d = 44.0f, len = 170.0f;
    bool white = false;
};
struct Layout {
    std::vector<Item> items;
    struct Plate { Rectangle<float> r; juce::String title; Colour col; };
    std::vector<Plate> plates;
};

Layout layout(const Page &pg)
{
    Layout L;
    float total = 0;
    for (auto &s : pg.s) total += s.w;
    float x = 8.0f, avail = W - 16.0f;
    const float ptop = 22.0f, pbot = H - 22.0f;   /* (the bottom strip: the maker's tag) */
    for (size_t si = 0; si < pg.s.size(); si++) {
        auto &s = pg.s[si];
        float w = avail * s.w / total;
        bool joined = juce::String(s.title).isEmpty();
        const char *title = s.title;
        for (size_t j = si; joined && j > 0 && juce::String(title).isEmpty(); j--) title = pg.s[j - 1].title;
        Colour col = aria_ui::sectionColour(title);
        if (!joined) {
            float pw = w;
            for (size_t j = si + 1; j < pg.s.size() && juce::String(pg.s[j].title).isEmpty(); j++) pw += avail * pg.s[j].w / total;
            L.plates.push_back({ Rectangle<float>(x + 4.0f, ptop, pw - 8.0f, pbot - ptop), s.title, col });
        }
        Rectangle<float> plate(x + 4.0f, ptop, w - 8.0f, pbot - ptop);
        size_t first = L.items.size();
        std::vector<const Ctl *> kn, fd, bt, st;
        for (auto &c : s.c) (c.k == KNOB ? kn : c.k == FADER ? fd : c.k == STEPS ? st : bt).push_back(&c);
        /* the buttons: a choice of up to four as a button each (together on a row), longer ones a button that steps */
        struct Cell { Item it; int group; };
        std::vector<Cell> cells;
        for (auto *c : bt) {
            int gi = (int)(c - s.c.data());
            if (c->k == SWITCH) {
                Item it{ Item::ISWITCH, c->p }; it.label = c->label; it.col = col;
                if (c->p == P_INIT) it.col = Colour(0xff9a9ca0);
                cells.push_back({ it, gi });
                continue;
            }
            int n = nchoices(c->p);
            if (n <= 4) {
                auto *info = aria_param_info(c->p);
                for (int k = 0; k < n; k++) {
                    Item it{ Item::IRADIO, c->p, k };
                    it.label = info->choice[k];
                    if (juce::String(c->label).isNotEmpty()) it.label = juce::String(c->label) + " " + it.label;
                    it.col = col;
                    cells.push_back({ it, gi });
                }
            } else {
                Item it{ Item::ICYCLE, c->p }; it.klabel = c->label; it.col = col; it.white = true;
                cells.push_back({ it, gi });
            }
        }
        const float cw = 56.0f, bh = 42.0f;
        if (juce::String(s.title) == "OSCILLATOR 1") {   /* the four knobs and the octave buttons as OSCILLATOR 2's (same size,
                                                          columns, rows); under them FM AMT (left column) and FM 2>1 (right) */
            const float d = 56.0f, rh = d + 48.0f, y0 = plate.getY() + 8.0f, kw = (w - 20.0f) / 2.0f, ww = std::min(84.0f, kw - 2.0f);
            size_t n4 = 0;
            float by = y0 + rh * 2.0f + 10.0f;                 /* (OSCILLATOR 2's first button row) */
            for (size_t i = 0; i < kn.size(); i++) {
                Item it{ Item::IKNOB, kn[i]->p }; it.klabel = kn[i]->label; it.col = col; it.d = d;
                float cx, y;
                if (juce::String(kn[i]->label) == "FM AMT") { cx = x + 10 + kw * 0.5f; y = by + 40.0f; }
                else { cx = x + 10 + kw * ((float)(n4 % 2) + 0.5f); y = y0 + 4 + rh * (float)(n4 / 2); n4++; }
                it.r = Rectangle<float>(cx - ww * 0.5f, y, ww, d + 46.0f);
                L.items.push_back(it);
            }
            std::vector<Item> oct;
            for (auto &cl : cells) {
                Item it = cl.it;
                if (it.k == Item::ISWITCH) { it.r = Rectangle<float>(x + 10 + kw * 1.5f - cw * 0.5f, by + 40.0f + 4.0f + d * 0.5f - 8.0f, cw, 40); L.items.push_back(it); }
                else oct.push_back(it);
            }
            float x0 = x + (w - cw * (float)oct.size()) * 0.5f;
            for (size_t i = 0; i < oct.size(); i++) { oct[i].r = Rectangle<float>(x0 + cw * (float)i, by, cw, 40); L.items.push_back(oct[i]); }
            x += w;
            continue;
        }
        if (juce::String(s.title) == "DIFFUSION DELAY") {   /* as on the VST: five knobs in a row, CHORUS and MIX centred
                                                             below, the buttons under them */
            const float d = 68.0f, sp = 140.0f, cx0 = x + w * 0.5f, y0 = plate.getY() + 22.0f;
            static const char *ROW1[5] = { "TIME L", "TIME R", "FDBK", "CHAR", "DIFF" };
            for (auto *k : kn) {
                juce::String l(k->label);
                float cx = cx0, y = y0;
                int i1 = -1;
                for (int i = 0; i < 5; i++) if (l == ROW1[i]) i1 = i;
                if (i1 >= 0) cx = cx0 + sp * (float)(i1 - 2);
                else { y = y0 + 150.0f; cx = cx0 + (l == "CHORUS" ? -0.5f : 0.5f) * sp; }
                Item it{ Item::IKNOB, k->p }; it.klabel = k->label; it.d = d; it.col = col;
                it.r = Rectangle<float>(cx - (d + 34.0f) * 0.5f, y, d + 34.0f, d + 46.0f);
                L.items.push_back(it);
            }
            float by = y0 + 150.0f + d + 46.0f + 22.0f, x0 = x + (w - cw * (float)cells.size()) * 0.5f;
            for (size_t i = 0; i < cells.size(); i++) {
                Item it = cells[i].it;
                it.r = Rectangle<float>(x0 + cw * (float)i, by, cw, 40);
                L.items.push_back(it);
            }
            x += w;
            continue;
        }
        if (juce::String(s.title) == "LADDER FILTERS") {   /* as on the VST: the cutoffs large outside, the resonances small
                                                            between them, the envelope amounts small below, the buttons under */
            const float big = 92.0f, small = 58.0f, cx0 = x + w * 0.5f, y0 = plate.getY() + 22.0f;
            auto knobAt = [&](int p, const char *lab, float d, float cx, float y) {
                Item it{ Item::IKNOB, p }; it.klabel = lab; it.d = d; it.col = col;
                it.r = Rectangle<float>(cx - (d + 34.0f) * 0.5f, y, d + 34.0f, d + 46.0f);
                L.items.push_back(it);
            };
            for (auto *k : kn) {
                juce::String l(k->label);
                if (l == "CUTOFF 1") knobAt(k->p, k->label, big, cx0 - 235.0f, y0);
                else if (l == "CUTOFF 2") knobAt(k->p, k->label, big, cx0 + 235.0f, y0);
                else if (l == "RES 1") knobAt(k->p, k->label, small, cx0 - 78.0f, y0 + 6.0f);
                else if (l == "RES 2") knobAt(k->p, k->label, small, cx0 + 78.0f, y0 + 6.0f);
                else if (l == "ENV 1") knobAt(k->p, k->label, small, cx0 - 78.0f, y0 + 148.0f);
                else knobAt(k->p, k->label, small, cx0 + 78.0f, y0 + 148.0f);
            }
            float by = y0 + 148.0f + small + 46.0f + 22.0f, x0 = x + (w - cw * (float)cells.size()) * 0.5f;
            for (size_t i = 0; i < cells.size(); i++) {
                Item it = cells[i].it;
                it.r = Rectangle<float>(x0 + cw * (float)i, by, cw, 40);
                L.items.push_back(it);
            }
            x += w;
            continue;
        }
        if (!st.empty()) {   /* the arpeggiator: knobs and buttons side by side above, the 16 step keys spread out below */
            const float kd = 56.0f, kcw = 96.0f;
            float bw = 5 * cw, gw = kcw * (float)kn.size() + 40.0f + bw;
            float gx = x + (w - gw) * 0.5f, gy = plate.getY() + 12.0f;
            for (size_t i = 0; i < kn.size(); i++) {
                Item it{ Item::IKNOB, kn[i]->p }; it.klabel = kn[i]->label; it.d = kd; it.col = col;
                float cx = gx + kcw * ((float)i + 0.5f);
                it.r = Rectangle<float>(cx - 42.0f, gy, 84.0f, kd + 46.0f);
                L.items.push_back(it);
            }
            std::vector<Cell> rest;                       /* LENGTH goes beside the steps */
            for (auto &cl : cells) if (cl.it.p == GP_ARP_LEN) { Item it = cl.it; it.k = Item::ICYCLE; it.klabel = "LENGTH"; it.white = true; it.label = "";
                    it.r = Rectangle<float>(0, 0, 0, 0); rest.push_back({ it, cl.group }); }
            float bx = gx + kcw * (float)kn.size() + 40.0f, by = gy + 12.0f;
            int col4 = 0, row = 0, lastg = -1;
            for (auto &cl : cells) {
                if (cl.it.p == GP_ARP_LEN) continue;
                if (cl.it.k == Item::IRADIO && cl.group != lastg && col4 > 0) { row++; col4 = 0; }   /* a choice on a row of its own */
                if (col4 >= 4) { row++; col4 = 0; }
                Item it = cl.it;
                it.r = Rectangle<float>(bx + cw * (float)col4, by + bh * (float)row, cw, 40);
                L.items.push_back(it);
                col4++; lastg = cl.group;
            }
            /* the steps: 2 rows of 8, square keys big enough for a finger on a small screen, LENGTH beside them */
            const float key = 74.0f, pitch = 82.0f, gap = 16.0f;
            float gwid = pitch * 8.0f + gap - (pitch - key - 5.0f), sx = x + (w - gwid) * 0.5f, sy = plate.getY() + 150.0f;
            for (int i = 0; i < 16; i++) {
                int row = i / 8, col = i % 8;
                Item it{ Item::ISTEP, GP_ARP_STEP1 + i, i };
                it.r = Rectangle<float>(sx + pitch * (float)col + gap * (float)(col / 4), sy + 100.0f * (float)row, key + 5.0f, key + 16.0f);
                it.col = Colour(0xff3ddc84);
                L.items.push_back(it);
            }
            for (auto &cl : rest) {                    /* LENGTH: at the end of the buttons' first row */
                Item it = cl.it; it.col = col;
                it.r = Rectangle<float>(bx + cw * 4.0f, by, cw + 12.0f, 40);
                L.items.push_back(it);
            }
            x += w;
            continue;
        }
        int per = std::max(1, (int)((w - 16.0f) / cw));
        std::vector<std::vector<size_t>> brows;
        for (size_t k = 0; k < cells.size();) {
            if (brows.empty() || (int)brows.back().size() >= per) brows.push_back({});
            size_t e = k; while (e < cells.size() && cells[e].group == cells[k].group) e++;
            if ((int)(brows.back().size() + (e - k)) > per && !brows.back().empty() && (int)(e - k) <= per) brows.push_back({});
            for (; k < e; k++) { if ((int)brows.back().size() >= per) brows.push_back({}); brows.back().push_back(k); }
        }
        int cols = fd.empty() ? s.cols : std::min(s.cols, (int)kn.size());
        int krows = kn.empty() ? 0 : ((int)kn.size() + cols - 1) / cols;
        float d = fd.empty() ? (krows <= 2 ? 56.0f : 44.0f) : 44.0f, rh = d + 48.0f;
        float len = kn.empty() ? 250.0f : 170.0f;
        float fH = fd.empty() ? 0.0f : len + 46.0f, kH = rh * (float)krows, bH = bh * (float)brows.size();
        float tot = fH + kH + (bH > 0 ? bH + 10.0f : 0.0f);
        float y = juce::String(s.title) == "OSCILLATOR 2" ? plate.getY() + 8.0f   /* (level with OSCILLATOR 1) */
                                                           : plate.getY() + std::max(8.0f, (plate.getHeight() - tot) * 0.5f);
        if (!fd.empty()) {
            int n = (int)fd.size();
            float fw = (w - 20.0f) / (float)n;
            for (int i = 0; i < n; i++) {
                Item it{ Item::IFADER, fd[(size_t)i]->p }; it.klabel = fd[(size_t)i]->label; it.len = len; it.col = col;
                float cx = x + 10 + fw * ((float)i + 0.5f), ww = std::min(68.0f, fw - 2.0f);
                it.r = Rectangle<float>(cx - ww * 0.5f, y, ww, len + 40.0f);
                L.items.push_back(it);
            }
            y += fH;
        }
        if (!kn.empty()) {
            float kw = (w - 20.0f) / (float)cols;
            for (size_t i = 0; i < kn.size(); i++) {
                Item it{ Item::IKNOB, kn[i]->p }; it.klabel = kn[i]->label; it.d = d; it.col = col;
                float cx = x + 10 + kw * ((float)(i % (size_t)cols) + 0.5f), ww = std::min(84.0f, kw - 2.0f);
                it.r = Rectangle<float>(cx - ww * 0.5f, y + 4 + rh * (float)(i / (size_t)cols), ww, d + 14.0f + 32.0f);
                L.items.push_back(it);
            }
            y += kH;
        }
        y += 10.0f;
        for (auto &row : brows) {
            float x0 = x + (w - cw * (float)row.size()) * 0.5f;
            for (size_t i = 0; i < row.size(); i++) {
                Item it = cells[row[i]].it;
                it.r = Rectangle<float>(x0 + cw * (float)i, y, cw, 40);
                L.items.push_back(it);
            }
            y += bh;
        }
        /* a knob made larger and centred under the others (VCA's SPREAD, OSCILLATOR 1's FM AMT): what follows moves down */
        for (size_t i = first; i < L.items.size(); i++) {
            auto &it = L.items[i];
            juce::String l(it.klabel);
            if (it.k != Item::IKNOB || !(juce::String(s.title) == "VCA" && l == "SPREAD"))
                continue;
            const float grow = 26.0f;
            float top = it.r.getY();
            for (size_t j = first; j < L.items.size(); j++)
                if (j != i && L.items[j].r.getY() > top + 1.0f) L.items[j].r.translate(0, grow);
            it.d += grow;
            it.r = Rectangle<float>(x + w * 0.5f - (it.d + 34.0f) * 0.5f, top + 6.0f, it.d + 34.0f, it.d + 46.0f);
        }
        /* VOICES: the buttons further apart and further from the knobs */
        if (juce::String(s.title) == "VOICES") {
            std::vector<Item *> b;
            for (size_t i = first; i < L.items.size(); i++) if (L.items[i].k != Item::IKNOB) b.push_back(&L.items[i]);
            const float sp = 92.0f;
            float x0 = x + (w - sp * (float)b.size()) * 0.5f + (sp - cw) * 0.5f;
            for (size_t i = 0; i < b.size(); i++) b[i]->r = Rectangle<float>(x0 + sp * (float)i, b[i]->r.getY() + 34.0f, cw + 8.0f, 40);
            for (size_t i = first; i < L.items.size(); i++) if (L.items[i].k == Item::IKNOB) L.items[i].r.translate(0, -16.0f);
        }
        x += w;
    }
    return L;
}

/* the page without its controls: the panel, the plates and headings, the tag */
void drawBackground(juce::Graphics &g, const Layout &L, const juce::Image &tag)
{
    g.drawImage(aria_art::panel((int)W, (int)H), Rectangle<float>(0, 0, W, H), juce::RectanglePlacement::stretchToFit);
    auto &F = aria_ui::Fonts::get();
    for (auto &pl : L.plates) {
        auto r = pl.r;
        g.setColour(Colour(0x2a000000)); g.fillRoundedRectangle(r, 4.0f);
        g.setColour(Colour(0x40000000)); g.drawRoundedRectangle(r, 4.0f, 1.0f);
        g.setColour(Colour(0x12ffffff)); g.drawLine(r.getX() + 4, r.getBottom() + 0.5f, r.getRight() - 4, r.getBottom() + 0.5f, 1.0f);
        auto hf = F.heading(11.5f);
        float tw = juce::GlyphArrangement::getStringWidth(hf, pl.title), cx = r.getCentreX() , hy = 11.0f;
        g.setColour(pl.col.withAlpha(0.55f));
        g.drawLine(r.getX() + 8, hy, cx - tw * 0.5f - 8, hy, 1.2f);
        g.drawLine(cx + tw * 0.5f + 8, hy, r.getRight() - 8, hy, 1.2f);
        g.setColour(Colour(0xffeceded)); g.setFont(hf);
        g.drawText(pl.title, Rectangle<float>(cx - tw * 0.5f - 4, hy - 8, tw + 8, 16), juce::Justification::centred);
    }
    /* the bottom strip: ARIA left, the maker's tag right */
    g.setColour(Colour(0xff7c7e82)); g.setFont(F.heading(10.0f));
    g.drawText("ARIA", Rectangle<float>(14, H - 18, 120, 14), juce::Justification::centredLeft);
    if (tag.isValid()) {
        float th = 11.0f, tw = th * (float)tag.getWidth() / (float)tag.getHeight();
        g.setOpacity(0.92f);
        g.drawImage(tag, Rectangle<float>(W - 14.0f - tw, H - 16.0f, tw, th), juce::RectanglePlacement::centred);
        g.setOpacity(1.0f);
    }
}

void drawItem(juce::Graphics &g, const Item &it)
{
    switch (it.k) {
    case Item::IKNOB: drawKnob(g, it.r, it.p, it.klabel, it.d); break;
    case Item::IFADER: drawFader(g, it.r, it.p, it.klabel, it.len); break;
    case Item::ISWITCH: drawButton(g, it.r, it.col, val(it.p) > 0.5f, it.label, false); break;
    case Item::IRADIO: drawButton(g, it.r, it.col, (int)std::lround(V[it.p]) == it.choice, it.label, false); break;
    case Item::ICYCLE: {
        juce::String t = vtext(it.p);
        if (juce::String(it.klabel).isNotEmpty()) t = juce::String(it.klabel) + " " + t;
        drawButton(g, it.r, it.col, false, t, true);
        break;
    }
    case Item::ISTEP: drawStep(g, it.r, it.choice, it.col); break;
    }
}

/* ---------------------------------------------------------------------------- cutting a page up --- */
Rectangle<int> toMpc(Rectangle<float> r)     /* (edges rounded: neighbours meet without overlapping) */
{
    int x0 = juce::jlimit(0, TW, (int)std::lround(r.getX() * S)), y0 = juce::jlimit(0, TH, (int)std::lround(r.getY() * S));
    int x1 = juce::jlimit(0, TW, (int)std::lround(r.getRight() * S)), y1 = juce::jlimit(0, TH, (int)std::lround(r.getBottom() * S));
    return { x0, y0, x1 - x0, y1 - y0 };
}

/* a control at a value, on its piece of the background */
juce::Image piece(const juce::Image &bg, const Item &it, Rectangle<int> R)
{
    juce::Image im(juce::Image::ARGB, R.getWidth(), R.getHeight(), true);
    juce::Graphics g(im);
    g.drawImageAt(bg.getClippedImage(R), 0, 0);
    g.addTransform(juce::AffineTransform::scale(S).translated((float)-R.getX(), (float)-R.getY()));
    drawItem(g, it);
    return im;
}

bool writePng(const juce::Image &img, const juce::File &f)
{
    f.deleteFile();
    juce::FileOutputStream out(f);
    return out.openedOk() && juce::PNGImageFormat().writeImageToStream(img, out);
}

juce::var rectVar(Rectangle<int> r) { return juce::Array<juce::var>{ r.getX(), r.getY(), r.getWidth(), r.getHeight() }; }

/* a film strip's frames: one a value, at most 128, the strip at most 9800 pixels tall (as the PROPHET 67's) */
int framesOf(int p, int h)
{
    int n = nchoices(p);
    if (n > 0) return n;
    return juce::jmax(2, juce::jmin(128, 9800 / juce::jmax(1, h)));
}

juce::var cutPage(const Page &pg, const juce::File &dir, const juce::Image &tag)
{
    Layout L = layout(pg);
    juce::String name = juce::String(pg.name).toLowerCase();
    juce::Image bg(juce::Image::ARGB, TW, TH, true);
    {
        juce::Graphics g(bg);
        g.addTransform(juce::AffineTransform::scale(S));
        drawBackground(g, L, tag);
    }
    int n = 0;
    auto save = [&](const juce::Image &img, const juce::String &what) {
        juce::String f = name + "_" + juce::String(n++).paddedLeft('0', 3) + "_" + what + ".png";
        writePng(img, dir.getChildFile(f));
        return f;
    };
    juce::String bgName = name + "_panel.png";
    writePng(bg, dir.getChildFile(bgName));

    juce::Array<juce::var> list;
    std::vector<bool> radioDone(L.items.size(), false);
    for (size_t i = 0; i < L.items.size(); i++) {
        auto it = L.items[i];
        int p = it.p, mpc = mpcIndex(p);
        float keep = val(p);
        auto *o = new juce::DynamicObject();
        o->setProperty("mpc", mpc);
        juce::Array<juce::var> names;
        if (nchoices(p) > 0)
            for (int k = 0; k < nchoices(p); k++) names.add(juce::String(aria_param_info(p)->choice[k]));
        else if (isSwitch(p)) names = { "OFF", "ON" };
        o->setProperty("names", names);
        auto R = toMpc(it.r);
        if (it.k == Item::IKNOB || it.k == Item::IFADER) {
            int frames = framesOf(p, R.getHeight()), steps = nchoices(p) > 0 ? nchoices(p) : 128;
            juce::Image strip(juce::Image::ARGB, R.getWidth(), R.getHeight() * frames, true);
            {
                juce::Graphics g(strip);
                for (int f = 0; f < frames; f++) {
                    V[p] = nchoices(p) > 0 ? (float)f : (float)f / (float)(frames - 1);
                    g.drawImageAt(piece(bg, it, R), 0, f * R.getHeight());
                }
            }
            o->setProperty("kind", "knob");
            o->setProperty("fader", it.k == Item::IFADER);
            o->setProperty("bipolar", aria_param_info(p)->kind == PK_BIPOLAR);
            o->setProperty("steps", steps);
            o->setProperty("frames", frames);
            o->setProperty("rect", rectVar(R));
            o->setProperty("strip", save(strip, (it.k == Item::IFADER ? "fader_p" : "knob_p") + juce::String(mpc)));
        } else if (it.k == Item::IRADIO) {
            if (radioDone[i]) { delete o; continue; }
            /* a choice's buttons: one control, a row each (as the PROPHET 67's LED lists) */
            o->setProperty("kind", "leds");
            o->setProperty("steps", nchoices(p));
            juce::Array<juce::var> rows;
            for (size_t j = i; j < L.items.size(); j++) {
                auto &jt = L.items[j];
                if (jt.k != Item::IRADIO || jt.p != p) continue;
                radioDone[j] = true;
                auto RJ = toMpc(jt.r);
                auto *ro = new juce::DynamicObject();
                ro->setProperty("rect", rectVar(RJ));
                V[p] = (float)jt.choice;
                ro->setProperty("on", save(piece(bg, jt, RJ), "radio_p" + juce::String(mpc) + "_" + juce::String(jt.choice) + "_on"));
                V[p] = (float)((jt.choice + 1) % nchoices(p));
                ro->setProperty("off", save(piece(bg, jt, RJ), "radio_p" + juce::String(mpc) + "_" + juce::String(jt.choice) + "_off"));
                rows.add(juce::var(ro));
            }
            o->setProperty("rows", rows);
        } else {
            /* a switch, a step key, a choice that steps: a look a value, a tap the next */
            int steps = it.k == Item::ICYCLE ? nchoices(p) : 2;
            o->setProperty("kind", "button");
            o->setProperty("steps", steps);
            o->setProperty("rect", rectVar(R));
            juce::Array<juce::var> rs;
            for (int v = 0; v < steps; v++) {
                val(p) = (float)v;
                auto *ro = new juce::DynamicObject();
                ro->setProperty("a", v);
                ro->setProperty("b", v);
                ro->setProperty("next", (v + 1) % steps);
                ro->setProperty("image", save(piece(bg, it, R), "button_p" + juce::String(mpc) + "_" + juce::String(v)));
                rs.add(juce::var(ro));
            }
            o->setProperty("ranges", rs);
        }
        val(p) = keep;
        list.add(juce::var(o));
    }
    auto *po = new juce::DynamicObject();
    po->setProperty("name", juce::String(pg.name));
    po->setProperty("key", pg.key);
    po->setProperty("sub", pg.sub);
    po->setProperty("background", bgName);
    po->setProperty("controls", list);
    juce::Array<juce::var> q;
    for (auto &grp : pg.q) {
        juce::Array<juce::var> gi;
        for (int p : grp) gi.add(p < 0 ? -1 : mpcIndex(p));
        q.add(gi);
    }
    po->setProperty("qlinks", q);
    std::printf("%-7s %d controls, %d pictures\n", pg.name, list.size(), n + 1);
    return juce::var(po);
}

/* the MPC's function keys under the screen (the MPC draws these itself: for the preview only) */
void drawKeys(juce::Graphics &g, const Page &pg, float y)
{
    g.setColour(Colour(0xff0b0b0c)); g.fillRect(0.0f, y, (float)TW, 72.0f);
    float kw = (float)TW / 6.0f;
    auto &F = aria_ui::Fonts::get();
    for (int k = 0; k < 6; k++) {
        Rectangle<float> r(kw * (float)k + 6, y + 10, kw - 12, 52);
        bool on = k == pg.key;
        g.setColour(on ? Colour(0xffff5b2e) : Colour(0xff26272b)); g.fillRoundedRectangle(r, 6.0f);
        juce::String t = on && pg.sub ? SUBS[k] : KEYS[k];
        g.setColour(on ? Colour(0xff1a0a05) : Colour(0xffc9cacc)); g.setFont(F.heading(19.0f));
        g.drawText(t, r.withTrimmedBottom(14), juce::Justification::centred);
        if (juce::String(SUBS[k]).isNotEmpty()) {
            juce::String alt = juce::String::fromUTF8("2\xc3\x97: ") + (on && pg.sub ? KEYS[k] : SUBS[k]);
            g.setColour(on ? Colour(0xff4a1a0a) : Colour(0xff8c8e92)); g.setFont(F.label(13.0f));
            g.drawText(alt, r.withTrimmedTop(30), juce::Justification::centred);
        }
    }
}
} // namespace

int main(int argc, char **argv)
{
    if (argc < 2) {
        std::fprintf(stderr, "usage: aria_skin_render <out folder> [--preview] [factory sound]\n");
        return 2;
    }
    juce::ScopedJuceInitialiser_GUI init;
    juce::SharedResourcePointer<aria_ui::UiResources> res;
    juce::File dir = juce::File::getCurrentWorkingDirectory().getChildFile(argv[1]);
    dir.createDirectory();
    bool preview = argc > 2 && juce::String(argv[2]) == "--preview";
    int prog = argc > (preview ? 3 : 2) ? atoi(argv[preview ? 3 : 2]) : 0;
    aria_preset_values(prog, V);
    V[ARIA_NPARAM] = V[ARIA_NPARAM + 1] = 0;
    auto tag = juce::ImageCache::getFromMemory(BinaryData::swtdr34mztag_png, BinaryData::swtdr34mztag_pngSize);

    if (preview) {
        int i = 0;
        for (auto &pg : pages()) {
            Layout L = layout(pg);
            juce::Image img(juce::Image::ARGB, TW, TH + 72, true);
            juce::Graphics g(img);
            {
                juce::Graphics::ScopedSaveState ss(g);
                g.addTransform(juce::AffineTransform::scale(S));
                drawBackground(g, L, tag);
                for (auto &it : L.items) drawItem(g, it);
            }
            drawKeys(g, pg, (float)TH);
            writePng(img, dir.getChildFile(juce::String(++i) + "-" + pg.name + ".png"));
        }
        return 0;
    }
    juce::Array<juce::var> list;
    for (auto &pg : pages()) list.add(cutPage(pg, dir, tag));
    auto *root = new juce::DynamicObject();
    root->setProperty("pages", list);
    /* Program (Track) mode: the knobs as the MPC's own program knobs, Q-Link 1 bottom left */
    juce::Array<juce::var> prog16;
    for (int p : std::initializer_list<int>{ (int)GP_OUT_VOL, T(GLIDE), T(PL_AMT), T(VCA_LEVEL), T(AE_A), T(AE_D), T(AE_S), T(AE_R),
                   T(FE_A), T(FE_D), T(FE_S), T(FE_R), T(F1_CUT), T(F1_RES), T(F1_ENV), T(MX_OVERLOAD) })
        prog16.add(mpcIndex(p));
    root->setProperty("program", prog16);
    dir.getChildFile("manifest.json").replaceWithText(juce::JSON::toString(juce::var(root)));
    return 0;
}
