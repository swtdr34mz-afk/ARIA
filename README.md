# ARIA

An 8-voice, bi-timbral analogue-modelling polysynth by **swtdr34mz**: two oscillators with a continuous wave mix, a
per-voice mod oscillator, two 4-pole ladder filters (series, stereo or parallel), a chord memory with a KEY mode and
8 key maps, and a diffusion delay. Planned as a VST3 / standalone plugin (Cubase, Windows) and an MPC plugin.

**Status:** the sound engine (`dsp/`) is written and tested. Still to come: the arpeggiator, the factory presets, the
plugin itself (panel, VST3, standalone) and the MPC version.

## Auf Deutsch, kurz

ARIA ist ein 8-stimmiger, bi-timbraler Synth: zwei Oszillatoren mit Wave-Mix (Dreieck–Säge, Pulsbreite, Mix), FM und
Sync, ein Mod-Oszillator pro Stimme, Mixer mit Ringmodulator und Overload, zwei Ladder-Filter (seriell, stereo,
parallel, LINK), zwei Hüllkurven mit Kurve, Loop und Velocity, LFOs, Timbre A/B mit Split und Stack, Unison und Mono,
die Chord Memory mit KEY-Modus (ein eigener Akkord pro Taste, 8 Keymaps) und das Diffusion Delay.

## The engine

- `dsp/aria.h` — the interface: one `aria_t` per plugin instance (no globals), floats in, stereo floats out.
- `dsp/aria.c` — the voices, filters, envelopes, LFOs, voice allocation, chord memory and delay.
- `dsp/params.c` — the parameters: names, kinds, defaults.

Test it (Linux or macOS, x86):

```
cd tests && mkdir -p out
cc -O2 -o demo demo.c ../dsp/aria.c ../dsp/params.c -lm && ./demo
```

It checks the chord memory and the voice allocation, renders five demo sounds to `tests/out/*.wav` and measures the
CPU.

Unofficial and independent: ARIA is its own instrument, not made, endorsed or supported by any synthesizer maker.

License: MPL-2.0.
