# ARIA for the MPC

An 8-voice, bi-timbral analogue-modelling polysynth by **swtdr34mz**, the same engine as the ARIA VST3.

## Installieren

1. Den Ordner `swtdr34mz - VST - ARIA` in den Ordner `VST2 Plugins` auf einem Laufwerk der MPC kopieren
   (USB-Stick, SD-Karte oder interne SSD).
2. Auf der MPC: *Preferences → Enhancements (Beta) → Enable Enhancements and External Plugin Support* einschalten
   (MPC OS 3.10.1 oder neuer), neu starten.
3. Eine Plugin-Spur anlegen, als Plugin **ARIA** (Hersteller swtdr34mz) wählen.

## Seiten (Funktionstasten)

Die Oberfläche ist das Panel des VST, Sektion für Sektion: dieselben Regler, Taster und Farben, nur größer. Ein
zweiter Druck auf dieselbe Funktionstaste öffnet deren zweite Seite.

| Taste | Seite | 2× gedrückt |
|---|---|---|
| 1 | **OSC**: Oszillator 1, Oszillator 2, Mod-Osc | **ENV**: Filter- und Amp-Hüllkurve (Fader), Kurve, Loop, Velocity |
| 2 | **MIXER**: Mixer (Fader), VCA, Output (Volume, Low Cut, QUALITY, INIT) | – |
| 3 | **FILTER**: beide Ladder-Filter (HP, LINK, Reihenfolge, Keytracking) | **VOICES**: Detune, Splitpunkt, Stimmen für A, Single/Split/Stack, Unison, Mono |
| 4 | **LFO**: LFO 1, LFO 2, Pitch-LFO | – |
| 5 | **FX**: Diffusion Delay | – |
| 6 | **DIV**: LOW SPLIT, Chord Memory (ON, KEY, Key Map, LEARN) | **ARP**: Arpeggiator mit den 16 Pattern-Schritten, Perform (Glide, Bend, HOLD) |

Regler: ziehen oder mit dem Q-Link drehen; Doppeltipp öffnet das große Werte-Fenster (bei Auswahlen die Liste).
Taster: antippen. Timbre B steht in der Parameterliste des Plugins.

Die Q-Links drehen auf jeder Seite bis zu vier Gruppen, die obere Q-Link-Reihe die erste; auf dem Bildschirm ist jede Gruppe eingerahmt. Der Arp läuft im Tempo der MPC und rastet im
Takt ein, wenn sie spielt. **QUALITY**: HIGH (2x Oversampling, Standard auf MPC XL / Live III) oder ECO (weniger
CPU, Standard auf den 32-Bit-MPCs); ULTRA nur, wenn die CPU-Anzeige viel Luft hat. MAX 16X ist für den Rechner
gedacht und auf der MPC meist zu schwer; RENDER 16X spielt auf der MPC wie ULTRA (sie rendert nicht offline).

**LEARN** (Seite DIV): tippen, Akkord spielen, wieder tippen. Ohne KEY ist das der Akkord für jede Taste, mit KEY
danach die Taste spielen, auf der er liegen soll. Key Maps 1–4 sind ab Werk mit Akkorden in C-Dur und c-Moll belegt.
Akkorde und Key Maps werden mit dem Projekt gespeichert.

Unofficial and independent: ARIA is its own instrument, not made, endorsed or supported by any synthesizer maker.
