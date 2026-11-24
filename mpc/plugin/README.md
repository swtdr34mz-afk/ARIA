# ARIA for the MPC

An 8-voice, bi-timbral analogue-modelling polysynth by **swtdr34mz**, the same engine as the ARIA VST3.

## Installieren

1. Den Ordner `swtdr34mz - VST - ARIA` in den Ordner `VST2 Plugins` auf einem Laufwerk der MPC kopieren
   (USB-Stick, SD-Karte oder interne SSD).
2. Auf der MPC: *Preferences → Enhancements (Beta) → Enable Enhancements and External Plugin Support* einschalten
   (MPC OS 3.10.1 oder neuer), neu starten.
3. Eine Plugin-Spur anlegen, als Plugin **ARIA** (Hersteller swtdr34mz) wählen.

## Seiten (Funktionstasten)

| Seite | Inhalt |
|---|---|
| FILTER | Filter 1 und 2 (Cutoff, Resonanz, Hüllkurve, Keytracking), HP, LINK, Reihenfolge, Overload, Level, Spread, Detune, Glide |
| OSC | beide Oszillatoren (Oktave, Dreieck–Säge, Pulsbreite, Wave Mix), Stimmung, FM, Sync, Mixer |
| ENV | Filter- und Amp-Hüllkurve, Kurven, Velocity, Unison, Mono, Stimmen-Modus (Single/Split/Stack), Splitpunkt |
| MOD | LFO 1 und 2, Mod-Oszillator, Vibrato |
| ARP/FX | Arpeggiator, Chord Memory (ON, KEY, Key Map, LEARN), Diffusion Delay |
| STEPS | das Arp-Pattern: 16 Schritte an oder Pause |

Alles Weitere (Timbre B, die übrigen Delay-, LFO- und Mod-Osc-Regler, QUALITY, INIT) steht in der Parameterliste
des Plugins.

Die Q-Links drehen auf jeder Seite die vier Reihen (oben die erste). Der Arp läuft im Tempo der MPC und rastet im
Takt ein, wenn sie spielt. **QUALITY**: HIGH (2x Oversampling, Standard auf MPC XL / Live III) oder ECO (weniger
CPU, Standard auf den 32-Bit-MPCs); ULTRA nur, wenn die CPU-Anzeige viel Luft hat.

**LEARN** (Seite ARP): tippen, Akkord spielen, wieder tippen. Ohne KEY ist das der Akkord für jede Taste, mit KEY
danach die Taste spielen, auf der er liegen soll. Key Maps 1–4 sind ab Werk mit Akkorden in C-Dur und c-Moll belegt.
Akkorde und Key Maps werden mit dem Projekt gespeichert.

Unofficial and independent: ARIA is its own instrument, not made, endorsed or supported by any synthesizer maker.
