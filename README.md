# ARIA

An 8-voice, bi-timbral analogue-modelling polysynth by **swtdr34mz**: two oscillators with a continuous wave mix, a
per-voice mod oscillator, two 4-pole ladder filters (series, stereo or parallel), a chord memory with a KEY mode and
8 key maps, and a diffusion delay. Planned as a VST3 / standalone plugin (Cubase, Windows) and an MPC plugin.

**Status:** the engine, the arpeggiator, the plugin (VST3 and standalone for Windows, with the panel) and the MPC
version (`mpc/`) are here.

## Download

[Actions](https://github.com/swtdr34mz-afk/ARIA/actions) → the newest green run → **Artifacts** at the bottom:
`ARIA-Windows` (a GitHub sign-in is needed for artifacts). `ARIA-demo-sounds` has the engine's demo sounds.

## On the MPC

ARIA also runs on Akai MPC standalone hardware (MPC XL, Live III, and the 32-bit MPCs), built with Akai's MPC Plugin
SDK from the same engine: `mpc/` (its README: `mpc/plugin/README.md`). Download: Actions → the newest run →
Artifacts → **ARIA-MPC**; copy its folder into `VST2 Plugins` on a drive of the MPC (MPC OS 3.10.1 or later,
Preferences → Enhancements (Beta) → Enable Enhancements and External Plugin Support).

## Installieren (Windows, Cubase)

1. Zip entpacken.
2. Den Ordner `ARIA.vst3` nach `C:\Program Files\Common Files\VST3\` kopieren.
3. Cubase starten (oder *Studio → VST-PlugIn-Manager → Aktualisieren*). ARIA steht unter den Instrumenten,
   Hersteller swtdr34mz.
4. `ARIA.exe` ist die Standalone-App. Audio- und MIDI-Gerät stellt man unter *Options* ein. Windows SmartScreen kann
   beim ersten Start warnen (die App ist nicht signiert): *Weitere Informationen → Trotzdem ausführen*.

**Linux (Bitwig, Reaper, Ardour, ...):** Die `.tar.gz` entpacken. `ARIA.vst3` nach `~/.vst3/` kopieren, `ARIA.lv2` nach `~/.lv2/`. Die App `ARIA` startet auch ohne DAW. 64 Bit, gebaut auf Ubuntu 22.04.

## Bedienung

- **Regler:** ziehen (Shift: fein), Doppelklick: Grundwert, Mausrad.
- **EDIT A / EDIT B** (VOICES): welches Timbre das Panel zeigt. **SINGLE / SPLIT / STACK**: wie die Tasten die Timbres
  spielen; **SPLIT** ist der Splitpunkt, **A** wie viele der 8 Stimmen Timbre A bekommt.
- **Taster mit Wert** (Oktave, Wellenform, LFO-Ziel …): klicken weiter, Shift-Klick zurück.
- **Rechtsklick** auf einen Regler, Taster, Step oder eine LED-Auswahl: das Menü der DAW für diesen Parameter (MIDI Learn, Automation …); der Wert bleibt, wie er ist.
- **CHORD MEMORY:** ON schaltet sie ein, KEY den Modus „ein Akkord pro Taste“. LEARN drücken, Akkord spielen, LEARN
  noch einmal: ohne KEY ist das der Akkord (verschoben auf jede Taste), mit KEY drückst du danach die Taste, auf der er
  liegen soll. Die Punkte 1–8 sind die Key Maps; 1–4 kommen fertig mit: C-Dur-Septakkorde, C-Dur-Dreiklänge,
  c-Moll-Septakkorde, c-Moll-Nonenakkorde auf den weißen Tasten. Alles wird mit dem Projekt gespeichert.
- **ARPEGGIATOR:** ON, LATCH (die Noten bleiben nach dem Loslassen, bis ein neuer Akkord kommt), Rate (1/4 bis 1/32,
  auch triolisch), Richtung (UP, DOWN, UP-DN, ORDER = wie gespielt, RANDOM), 1–4 Oktaven, GATE (Notenlänge), SWING.
  Läuft im Takt von Cubase bzw. der MPC, sobald der Host spielt; sonst ab der ersten Taste. Mit der Chord Memory
  zusammen wird jeder Akkord einer Taste arpeggiert (Preset ARP CHORDS).
- **QUALITY** (PRESET-Feld unten rechts): ECO 1X, HIGH 2X (Standard), ULTRA 4X, MAX 16X, RENDER 16X – das Oversampling
  der Stimmen. RENDER 16X spielt live mit 4X und rechnet beim Exportieren/Bouncen mit 16X. CPU (8 Stimmen, ein Kern,
  44,1 kHz): ECO ~2 %, HIGH ~3 %, ULTRA ~5 %, MAX 16X ~17 % (die Filter rechnen vier Stimmen auf einmal). Aliasing bei hohen Tönen: ULTRA −51 dB, 16X −59 dB.
- **LOW SPLIT:** Der SPLIT-Regler rastet auf OFF, -1, -2 und -3 OCT (die LEDs daneben zeigen die Stufe, ein Klick auf
  eine Zeile wählt sie direkt). Alles links vom Punkt (POINT) klingt so viele Oktaven tiefer. BALANCE regelt die
  Lautstärke links gegen rechts, LOW HOLD hält die linke Hand nach dem Loslassen.
- **PLAY:** HOLD hält die Noten nach dem Loslassen; ein neuer Akkord ersetzt den alten, HOLD aus lässt alles los.
  **ALL OFF** beendet alle Noten sofort.
- **Patches:** PREV / NEXT unter dem Display (oder die Programme des Hosts): 98 Werkssounds in Kategorien (PAD, STRINGS, BRASS, BASS, KEYS, BELLS, LEAD, PLUCK, ARP, ATMOS, FX, SPLIT); ab Nr. 41 die Cinema-Sammlung: große Filmmusik-Sounds im Stil der CS-80-Ära (Brass, Strings, Pads, Leads, Glocken, Atmosphären). Modrad = Vibrato, Aftertouch = heller. **SAVE** speichert den Sound unter einem Namen als eigenes Preset
  (Datei in `Dokumente/ARIA/Presets`, eine `.aria` je Sound); eigene Presets kommen nach den Werkssounds.
- **TEMPO** (im ARPEGGIATOR): das Tempo, wenn kein Host eins vorgibt (Standalone-App).

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
cc -O2 -o demo demo.c ../dsp/aria.c ../dsp/params.c ../dsp/params_data.c -lm && ./demo
```

It checks the chord memory and the voice allocation, renders five demo sounds to `tests/out/*.wav` and measures the
CPU.

Unofficial and independent: ARIA is its own instrument, not made, endorsed or supported by any synthesizer maker.

License: MPL-2.0.
