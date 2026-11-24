/* SPDX-License-Identifier: MPL-2.0
 * Copyright (C) 2026 swtdr34mz (ARIA) */
#include "Art.h"
#include <cmath>
#include <map>
#include <vector>

using juce::Colour;
using juce::Image;

namespace {
constexpr float PI = 3.14159265f;
constexpr float LIGHT = -2.36f;          /* the light's direction (radians, 0 = up, clockwise): the top left */

struct Rng {
    juce::uint32 s;
    explicit Rng(juce::uint32 seed) : s(seed | 1u) {}
    float next() { s ^= s << 13; s ^= s >> 17; s ^= s << 5; return (float)(s >> 8) / 16777216.0f; }   /* 0..1 */
    float bi() { return next() * 2.0f - 1.0f; }
};

inline juce::uint8 clamp8(float v) { return (juce::uint8)juce::jlimit(0, 255, (int)std::lround(v)); }

/* a brushed metal field: v(x, y) = base + streaks along the brushing + grain; dir 0 horizontal, 1 vertical */
void brushed(Image &img, std::function<float(float, float)> base, float streak, float grain, int dir, juce::uint32 seed)
{
    const int w = img.getWidth(), h = img.getHeight();
    Image::BitmapData bd(img, Image::BitmapData::writeOnly);
    Rng rng(seed);
    const int lines = dir == 0 ? h : w, len = dir == 0 ? w : h;
    std::vector<float> n((size_t)len + 64), sm((size_t)len);
    for (int L = 0; L < lines; L++) {
        float row = rng.bi() * 0.8f;
        for (auto &v : n) v = rng.bi();
        float acc = 0; const int box = 48;        /* (long streaks: the noise blurred along the brushing) */
        for (int i = 0; i < box; i++) acc += n[(size_t)i];
        for (int i = 0; i < len; i++) { sm[(size_t)i] = acc / (float)box; acc += n[(size_t)(i + box)] - n[(size_t)i]; }
        for (int i = 0; i < len; i++) {
            int x = dir == 0 ? i : L, y = dir == 0 ? L : i;
            float v = base((float)x / (float)w, (float)y / (float)h) + row * streak + sm[(size_t)i] * streak * 3.0f + rng.bi() * grain;
            bd.setPixelColour(x, y, Colour(clamp8(v), clamp8(v + 0.6f), clamp8(v + 1.6f)));
        }
    }
}
} // namespace

namespace aria_art {

Image panel(int w, int h)
{
    Image img(Image::RGB, (int)(w * SCALE), (int)(h * SCALE), false);
    brushed(img, [](float x, float y) {
        float v = 36.0f - 12.0f * y;                                        /* lit from above */
        float dx = (x - 0.38f) / 0.55f, dy = y / 0.42f;
        v += 7.0f * std::exp(-(dx * dx + dy * dy));                         /* the light's pool */
        float ex = std::min(x, 1.0f - x) * 14.0f, ey = std::min(y, 1.0f - y) * 9.0f;
        v *= 0.82f + 0.18f * std::min(1.0f, std::min(ex, ey));              /* darker to the edges */
        return v;
    }, 0.55f, 0.9f, 0, 0xA51Au);
    juce::Graphics g(img);
    g.addTransform(juce::AffineTransform::scale(SCALE));
    g.setColour(Colour(0x40ffffff)); g.drawHorizontalLine(0, 0, (float)w);          /* the face's front edge */
    g.setColour(Colour(0x18ffffff)); g.drawHorizontalLine(1, 0, (float)w);
    g.setColour(Colour(0x80000000)); g.drawHorizontalLine(h - 1, 0, (float)w);
    return img;
}

Image sidePlate(int w, int h, bool right)
{
    Image img(Image::RGB, (int)(w * SCALE), (int)(h * SCALE), false);
    brushed(img, [right](float x, float) {
        float t = right ? 1.0f - x : x;                                     /* 0 at the outside */
        return 14.0f + 26.0f * std::pow(std::sin(t * PI * 0.92f), 1.6f) - (t > 0.93f ? 10.0f : 0.0f);
    }, 0.7f, 1.0f, 1, right ? 0xBEEFu : 0xF00Du);
    juce::Graphics g(img);
    g.addTransform(juce::AffineTransform::scale(SCALE));
    float in = right ? 0.5f : (float)w - 0.5f, out = right ? (float)w - 0.5f : 0.5f;
    g.setColour(Colour(0x55ffffff)); g.drawVerticalLine((int)(right ? 1 : w - 2), 0, (float)h);   /* the chamfer */
    g.setColour(Colour(0xcc000000)); g.drawLine(in, 0, in, (float)h, 1.0f);
    g.setColour(Colour(0x30ffffff)); g.drawLine(out, 0, out, (float)h, 1.0f);
    g.setColour(Colour(0x50ffffff)); g.drawHorizontalLine(0, 0, (float)w);
    return img;
}

/* the drawn images kept: plain pointers, freed by releaseCaches when the last editor closes (never function statics,
 * which would be freed while the host unloads the DLL; see aria_ui::UiResources) */
static std::map<int, Image> *knobCache = nullptr;
static Image *faderImg = nullptr;
void releaseCaches()
{
    delete knobCache; knobCache = nullptr;
    delete faderImg; faderImg = nullptr;
}

Image knob(float d)
{
    if (knobCache == nullptr) knobCache = new std::map<int, Image>();
    auto &cache = *knobCache;
    int key = (int)std::lround(d * 10.0f);
    if (auto it = cache.find(key); it != cache.end()) return it->second;
    float size = d + 26.0f;
    Image img(Image::ARGB, (int)(size * SCALE), (int)(size * SCALE), true);
    juce::Graphics g(img);
    g.addTransform(juce::AffineTransform::scale(SCALE));
    float cx = size * 0.5f, cy = size * 0.5f - 2.0f, R = d * 0.5f;
    juce::Path disc; disc.addEllipse(cx - R, cy - R, d, d);
    /* the shadow on the panel */
    juce::DropShadow(Colour(0xb0000000), 9, { 0, 5 }).drawForPath(g, disc);
    juce::DropShadow(Colour(0x90000000), 3, { 0, 2 }).drawForPath(g, disc);
    /* the skirt: black anodised, knurled; each ridge catches the light by its angle */
    juce::ColourGradient sk(Colour(0xff222326), cx, cy, Colour(0xff0a0a0b), cx + R, cy, true);
    g.setGradientFill(sk); g.fillPath(disc);
    const int N = (int)(d * 1.5f);
    for (int i = 0; i < N; i++) {
        float a = 2.0f * PI * ((float)i + 0.5f) / (float)N;
        float L = 0.5f + 0.5f * std::cos(a - LIGHT);
        float b = 14.0f + 70.0f * L * L * L;
        g.setColour(Colour(clamp8(b), clamp8(b + 1), clamp8(b + 3)));
        float r0 = R * 0.74f, r1 = R - 0.7f;
        g.drawLine(cx + std::sin(a) * r0, cy - std::cos(a) * r0, cx + std::sin(a) * r1, cy - std::cos(a) * r1, 1.0f);
        g.setColour(Colour(0xff060607));
        float a2 = a + PI / (float)N;
        g.drawLine(cx + std::sin(a2) * r0, cy - std::cos(a2) * r0, cx + std::sin(a2) * r1, cy - std::cos(a2) * r1, 0.8f);
    }
    /* the skirt's rim: lit top left */
    juce::ColourGradient rim(Colour(0x90ffffff), cx - R * 0.7f, cy - R * 0.7f, Colour(0x00ffffff), cx + R * 0.3f, cy + R * 0.3f, false);
    g.setGradientFill(rim); g.drawEllipse(cx - R + 0.6f, cy - R + 0.6f, d - 1.2f, d - 1.2f, 1.0f);
    /* the shoulder under the cap */
    float rc = R * (d > 44.0f ? 0.64f : 0.6f);
    g.setColour(Colour(0xff050506)); g.fillEllipse(cx - rc - 1.8f, cy - rc - 1.2f, 2 * rc + 3.6f, 2 * rc + 3.6f);
    /* the cap: spun aluminium, concentric grain and the two bright wedges spun metal throws back */
    juce::Rectangle<float> cap(cx - rc, cy - rc, 2 * rc, 2 * rc);
    juce::ColourGradient cb(Colour(0xffc9ccce), cap.getX(), cap.getY(), Colour(0xff8e9295), cap.getRight(), cap.getBottom(), false);
    g.setGradientFill(cb); g.fillEllipse(cap);
    Rng rng((juce::uint32)key * 7919u);
    for (float r = rc; r > 0.5f; r -= 0.3f) {
        float v = rng.bi();
        g.setColour(v > 0 ? Colour(0xffffffff).withAlpha(0.06f * v) : Colour(0xff000000).withAlpha(-0.07f * v));
        g.drawEllipse(cx - r, cy - r, 2 * r, 2 * r, 0.3f);
    }
    for (int i = 0; i < 180; i++) {
        float a0 = 2.0f * PI * (float)i / 180.0f, a1 = a0 + 2.0f * PI / 180.0f + 0.004f;
        float c = std::cos(a0 + PI / 180.0f - LIGHT), s = std::sin(a0 + PI / 180.0f - LIGHT);
        float hi = std::pow(std::abs(c), 7.0f), lo = std::pow(std::abs(s), 5.0f);
        juce::Path w; w.addPieSegment(cap, a0, a1, 0.0f);
        if (hi > 0.01f) { g.setColour(Colour(0xffffffff).withAlpha(0.62f * hi)); g.fillPath(w); }
        if (lo > 0.01f) { g.setColour(Colour(0xff000000).withAlpha(0.22f * lo)); g.fillPath(w); }
    }
    /* the cap's chamfer and a soft dimple */
    juce::ColourGradient ch(Colour(0xe0ffffff), cap.getX(), cap.getY(), Colour(0x90000000), cap.getRight(), cap.getBottom(), false);
    g.setGradientFill(ch); g.drawEllipse(cap.reduced(0.4f), 0.9f);
    juce::ColourGradient dm(Colour(0x22000000), cx, cy, Colour(0x00000000), cx + rc * 0.45f, cy, true);
    g.setGradientFill(dm); g.fillEllipse(cap.reduced(rc * 0.55f));
    cache[key] = img;
    return img;
}

Image faderCap()
{
    if (faderImg != nullptr) return *faderImg;
    faderImg = new Image([] {
        Image im(Image::ARGB, (int)(32 * SCALE), (int)(26 * SCALE), true);
        juce::Graphics g(im);
        g.addTransform(juce::AffineTransform::scale(SCALE));
        juce::Rectangle<float> c(4, 4, 24, 14);
        juce::Path p; p.addRoundedRectangle(c, 2.0f);
        juce::DropShadow(Colour(0xc0000000), 5, { 0, 4 }).drawForPath(g, p);
        juce::ColourGradient body(Colour(0xff3c3d41), 0, c.getY(), Colour(0xff111214), 0, c.getBottom(), false);
        body.addColour(0.45, Colour(0xff222326));
        g.setGradientFill(body); g.fillPath(p);
        g.setColour(Colour(0x70ffffff)); g.drawLine(c.getX() + 1.5f, c.getY() + 0.6f, c.getRight() - 1.5f, c.getY() + 0.6f, 0.8f);
        juce::ColourGradient gl(Colour(0x30ffffff), 0, c.getY(), Colour(0x00ffffff), 0, c.getCentreY(), false);
        g.setGradientFill(gl); g.fillRoundedRectangle(c.withHeight(7).reduced(1, 0.5f), 1.5f);
        g.setColour(Colour(0xff050505)); g.drawRoundedRectangle(c, 2.0f, 0.8f);
        g.setColour(Colour(0xff000000)); g.fillRect(c.getX() + 1.5f, c.getCentreY() - 0.2f, c.getWidth() - 3.0f, 2.2f);
        g.setColour(Colour(0xfff4f4f4)); g.fillRect(c.getX() + 1.5f, c.getCentreY() - 1.2f, c.getWidth() - 3.0f, 1.6f);
        return im;
    }());
    return *faderImg;
}

void screw(juce::Graphics &g, juce::Point<float> c, float r, float angle)
{
    juce::Rectangle<float> e(c.x - r, c.y - r, 2 * r, 2 * r);
    g.setColour(Colour(0xa0000000)); g.fillEllipse(e.expanded(1.2f).translated(0, 0.8f));
    juce::ColourGradient m(Colour(0xffb9bcbf), e.getX() + r * 0.5f, e.getY() + r * 0.4f, Colour(0xff2a2b2e), e.getRight(), e.getBottom(), true);
    g.setGradientFill(m); g.fillEllipse(e);
    g.setColour(Colour(0xff0b0b0c)); g.drawEllipse(e, 0.7f);
    for (float a : { angle, angle + PI * 0.5f }) {
        float dx = std::cos(a) * r * 0.62f, dy = std::sin(a) * r * 0.62f;
        g.setColour(Colour(0xff111112)); g.drawLine(c.x - dx, c.y - dy, c.x + dx, c.y + dy, r * 0.32f);
        g.setColour(Colour(0x40ffffff)); g.drawLine(c.x - dx, c.y - dy + 0.5f, c.x + dx, c.y + dy + 0.5f, 0.4f);
    }
}

void bezel(juce::Graphics &g, juce::Rectangle<float> r)
{
    auto o = r.expanded(3.0f);
    g.setColour(Colour(0x50ffffff)); g.fillRoundedRectangle(o.translated(0, 1.0f), 6.0f);   /* the lip catching the light */
    juce::ColourGradient rec(Colour(0xff000000), 0, o.getY(), Colour(0xff1a1b1e), 0, o.getBottom(), false);
    g.setGradientFill(rec); g.fillRoundedRectangle(o, 6.0f);
    juce::ColourGradient gl(Colour(0xff040506), r.getX(), r.getY(), Colour(0xff0d1013), r.getRight(), r.getBottom(), false);
    g.setGradientFill(gl); g.fillRoundedRectangle(r, 4.0f);
    g.setColour(Colour(0xff000000)); g.drawRoundedRectangle(r, 4.0f, 1.2f);
}

void glass(juce::Graphics &g, juce::Rectangle<float> r)
{
    juce::Graphics::ScopedSaveState ss(g);
    juce::Path clip; clip.addRoundedRectangle(r, 4.0f);
    g.reduceClipRegion(clip);
    juce::Path refl;
    refl.startNewSubPath(r.getX(), r.getY());
    refl.lineTo(r.getX() + r.getWidth() * 0.62f, r.getY());
    refl.lineTo(r.getX() + r.getWidth() * 0.38f, r.getBottom());
    refl.lineTo(r.getX(), r.getBottom());
    refl.closeSubPath();
    juce::ColourGradient rg(Colour(0x14ffffff), r.getX(), r.getY(), Colour(0x02ffffff), r.getX() + r.getWidth() * 0.5f, r.getBottom(), false);
    g.setGradientFill(rg); g.fillPath(refl);
    g.setColour(Colour(0x0affffff));
    for (float y = r.getY() + 1.0f; y < r.getBottom(); y += 2.0f) g.drawHorizontalLine((int)y, r.getX(), r.getRight());   /* (the pixel rows) */
    g.setColour(Colour(0x22ffffff)); g.drawHorizontalLine((int)r.getY() + 1, r.getX() + 4, r.getRight() - 4);
}

void glowText(juce::Graphics &g, const juce::String &t, juce::Rectangle<float> r, Colour c, juce::Justification j, float glow)
{
    if (glow > 0.0f) {
        g.setColour(c.withAlpha(0.10f * glow));
        for (auto o : { juce::Point<float>(-1.2f, 0), { 1.2f, 0 }, { 0, -1.2f }, { 0, 1.2f }, { -0.8f, -0.8f }, { 0.8f, 0.8f } })
            g.drawText(t, r.translated(o.x, o.y), j, false);
    }
    g.setColour(c);
    g.drawText(t, r, j, false);
}

} // namespace aria_art
