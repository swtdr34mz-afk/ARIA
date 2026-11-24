/* SPDX-License-Identifier: MPL-2.0
 * Copyright (C) 2026 swtdr34mz (ARIA) */
/* The panel's materials, drawn once and kept as images (at 2x, sharp when the window is scaled up): brushed and
 * anodised aluminium, knurled knobs with spun-aluminium caps, fader caps, screws, the displays' glass. The light
 * comes from the top left. */
#pragma once
#include <juce_graphics/juce_graphics.h>

namespace aria_art {

constexpr float SCALE = 2.0f;            /* the images' pixels per panel unit */

juce::Image panel(int w, int h);         /* the face: black anodised, brushed, lit from above */
juce::Image sidePlate(int w, int h, bool right);
juce::Image knob(float diameter);        /* the knob without its pointer: shadow, knurled skirt, cap */
juce::Image faderCap();                  /* 24 x 14 */
void screw(juce::Graphics &g, juce::Point<float> c, float r, float angle);
void bezel(juce::Graphics &g, juce::Rectangle<float> r);        /* a display's recess and its black glass */
void glass(juce::Graphics &g, juce::Rectangle<float> r);        /* the reflection over a display */
void glowText(juce::Graphics &g, const juce::String &t, juce::Rectangle<float> r, juce::Colour c,
              juce::Justification j, float glow);

} // namespace aria_art
