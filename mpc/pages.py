#!/usr/bin/env python3
# SPDX-License-Identifier: MPL-2.0
# Copyright (C) 2026 swtdr34mz (ARIA)
"""ARIA on the MPC's screen: the pages (function keys) and their controls, as the SDK's ui.json.

    python3 mpc/pages.py <params.json> <ui.json>

params.json comes from mpc/gen/gen_params.c (every parameter's MPC name, kind and choices). Each page is four rows of
four: a row is one Q-Link group (the MPC frames it and its four knobs turn it), one section each. Names here are the
MPC names (dsp/params_data.c: ARIA_TSHORT, ARIA_GSHORT; "B." in front for timbre B). Standard library only."""
import json
import sys

K, B, F, S, M = "knob", "bipolar", "fader", "switch", "menu"   # (M: a knob whose double tap lists the choices)
E = "empty"
PAGES = [   # (six: the MPC's function keys; the rest is in the parameter list)
    ("FILTER", [[("Cutoff 1", K), ("Reso 1", K), ("Env Amt 1", B), ("KB Trk 1", S)],
                [("Cutoff 2", K), ("Reso 2", K), ("Env Amt 2", B), ("KB Trk 2", S)],
                [("HP 1", S), ("Flt Link", S), ("Flt Order", S), ("Overload", K)],
                [("Level", F), ("Pan Spread", K), ("Detune", K), ("Glide", K)]]),
    ("OSC", [[("O1 Octave", S), ("O1 TriSaw", K), ("O1 Width", K), ("O1 WaveMix", K)],
             [("O2 Octave", S), ("O2 TriSaw", K), ("O2 Width", K), ("O2 WaveMix", K)],
             [("O1 Freq", B), ("O2 Freq", B), ("FM Amount", K), ("O2 Sync", S)],
             [("Mix Osc1", F), ("Mix Osc2", F), ("Mix Ring", F), ("Mix Noise", F)]]),
    ("ENV", [[("F Attack", F), ("F Decay", F), ("F Sustain", F), ("F Release", F)],
             [("A Attack", F), ("A Decay", F), ("A Sustain", F), ("A Release", F)],
             [("F Curve", K), ("F Velo", S), ("A Curve", K), ("A Velo", S)],
             [("Unison", S), ("Mono", S), ("Voice Mode", S), ("Split Pt", M)]]),
    ("MOD", [[("LFO1 Rate", K), ("LFO1 Amt", K), ("LFO1 Wave", M), ("LFO1 Dest", M)],
             [("LFO2 Rate", K), ("LFO2 Amt", K), ("LFO2 Wave", M), ("LFO2 Dest", M)],
             [("MO Freq", K), ("MO Wave", M), ("MO>Pitch", K), ("MO>Filter", K)],
             [("Vib Rate", K), ("Vib Amt", K), ("MO Audio", S), ("Mix ModOsc", F)]]),
    ("ARP/FX", [[("Arp", S), ("Arp Mode", M), ("Arp Rate", M), ("Arp Oct", S)],
                [("Arp Gate", K), ("Arp Swing", K), ("Arp Latch", S), ("Arp Length", M)],
                [("Chord On", S), ("Chord Key", S), ("Key Map", M), ("Chord Learn", S)],
                [("Dly Time L", K), ("Dly Fdbk", K), ("Dly Diff", K), ("Dly Mix", F)]]),
    ("STEPS", [[("Step %d" % (r * 4 + c + 1), S) for c in range(4)] for r in range(4)]),
]
EXTRA = {"Init": "switch", "Chord Learn": "switch"}     # (the MPC plugin's own: not in params.json)


def main(argv):
    if len(argv) != 3:
        print(__doc__)
        return 2
    params = {p["name"]: p for p in json.load(open(argv[1]))}
    pages = []
    for name, rows in PAGES:
        assert len(name) <= 8, name
        controls = []
        for row in rows:
            assert len(row) == 4, (name, row)
            for c in row:
                if c == E:
                    controls.append("empty")
                    continue
                pname, w = c
                kind = params[pname]["kind"] if pname in params else EXTRA.get(pname)
                if kind is None:
                    sys.exit("%s: no parameter named %r" % (name, pname))
                if w == S and kind == "choice" and len(params[pname]["choices"]) > 4:
                    sys.exit("%s: %s has %d choices: a menu, not a switch" % (name, pname, len(params[pname]["choices"])))
                if w == M:
                    controls.append({"param": pname, "widget": "knob", "overlay": "menu"})
                elif w == B:
                    controls.append({"param": pname, "widget": "knob", "polarity": "bipolar"})
                elif w == F:
                    controls.append({"param": pname, "widget": "fader"})
                else:
                    controls.append({"param": pname, "widget": w})
        pages.append({"name": name, "controls": controls})
    ui = {"kit": {"theme": "mono", "accent": "ffff5b2e"}, "qlinkGroups": "rows", "pages": pages,
          "style": {"background": "ff151517", "accent": "ffff5b2e"}}
    with open(argv[2], "w") as f:
        json.dump(ui, f, indent=2)
        f.write("\n")
    print("ui.json: %d pages" % len(pages))
    return 0


if __name__ == "__main__":
    sys.exit(main(sys.argv))
