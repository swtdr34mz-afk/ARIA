#!/usr/bin/env python3
# SPDX-License-Identifier: MPL-2.0
# Copyright (C) 2026 swtdr34mz (ARIA)
"""ARIA's MPC skin (Plugin Skins/: TUI.json, Q-Links.json and the pictures) from the renderer's output.

    python3 mpc/skin/make_skin.py <render folder> mpc/plugin/skins

The renderer (render_skin.cpp) draws each MPC page with the VST3's own art and cuts every control out at every value;
this places each picture where it was cut from, bound to the MPC plugin's parameter (the renderer gives its number):

  knobs, faders  film strips (a frame a value), dragged; a double tap opens the MPC's editor (its list for a choice)
  switches       a picture a value, a tap the other (the arp's step keys too)
  choices        up to four: a button each, the one chosen lit; more: one button showing the value, a tap the next

The pages sit on the function keys as the renderer says (key, and sub: pressing the key again): OSC, MIXER,
FILTER / VOICES, ENV, LFO / FX, DIV / ARP. On each the 16 Q-Links turn up to four groups, the top row of Q-Links the
first group and so on down, a group framed on the screen when its controls are side by side. Standard library only
(Pillow, if there, makes the pictures 256 colours: a fraction of the size).
"""
import json
import os
import shutil
import sys

class O(list):
    """A JSON object kept in order, with repeated keys (the overlays' two "version"s)."""
    def __init__(self, *pairs):
        super().__init__(pairs)


def dump(v, ind=0):
    sp = "  " * ind
    if isinstance(v, O):
        if not v:
            return "{}"
        return "{\n" + ",\n".join("%s  %s: %s" % (sp, json.dumps(k), dump(x, ind + 1)) for k, x in v) + "\n" + sp + "}"
    if isinstance(v, list):
        if not v:
            return "[]"
        if all(not isinstance(x, (O, list)) for x in v):
            return "[" + ", ".join(json.dumps(x) for x in v) + "]"
        return "[\n" + ",\n".join(sp + "  " + dump(x, ind + 1) for x in v) + "\n" + sp + "]"
    return json.dumps(v)


def box(r):
    return "%d %d %d %d" % tuple(r)


def image_data(img):
    return O(("version", 2), ("imageType", "Regular"), ("colour", "0"), ("image", img))


def bounds(r, focus="No", hide_handles=None, show="Show"):
    if hide_handles:
        return O(("version", 2), ("acceptsHWFocus", focus), ("showWhenDataModelInvalid", show), ("whenVisible", "Always"),
                 ("boundsType", "Absolute"), ("bounds", box(r)), ("additionalInvalidatingHandles", hide_handles))
    return O(("version", 1), ("acceptsHWFocus", focus), ("showWhenDataModelInvalid", show), ("whenVisible", "Always"),
             ("boundsType", "Absolute"), ("bounds", box(r)))


def entry(name, typ, data, b, remap=None):
    return O(("version", 2),
             ("componentData", O(("version", 1), ("name", name), ("type", typ), ("data", data))),
             ("handle remapping", O(("version", 1), ("map", [O(("key", k), ("value", v)) for k, v in (remap or [])]))),
             ("bounds", b))


def action(on, handler, handle, extra):
    return O(("version", 1), ("onAction", on), ("handler", handler), ("handleName", handle), ("additionalData", extra))


def component(actions, children, ignore_mouse=False):
    clear = O(("version", 1), ("colour", "0"), ("image", ""))
    return O(("version", 2), ("actions", actions),
             ("backgroundData", O(("version", 1), ("focussed", clear), ("unfocussed", clear))),
             ("ignoreMousePresses", ignore_mouse), ("disableCoarseDataWheel", False), ("componentsData", children))


FOCUS_LOST = O(("version", 1), ("onAction", "Focus Lost"), ("handler", "Focus Lost"), ("handleName", "Focus Lost"),
               ("additionalData", ""))


def overlay(typ, data):
    return component([], [entry("overlay", typ, data, bounds((0, 0, 1280, 800), "Yes"))], ignore_mouse=True)


def handle(n):
    return "Parameter %d" % n


def main(argv):
    if len(argv) != 3:
        print(__doc__)
        return 2
    src, out = argv[1], argv[2]
    m = json.load(open(os.path.join(src, "manifest.json")))
    if os.path.isdir(out):
        shutil.rmtree(out)
    os.makedirs(out)
    used = set()

    def use(f):
        used.add(f)
        return f

    def knob_overlay(bipolar):
        return overlay("KnobOverlay", O(("version", 2), ("version", 1), ("actions", [FOCUS_LOST]), ("overlayType", "Knob With Header"),
                                        ("scaleType", "Linear"), ("polarityType", "Bipolar" if bipolar else "Unipolar"),
                                        ("showMinusSign", True), ("handleName", "Data")))
    defs = [("knob overlay", knob_overlay(False)), ("knob overlay bipolar", knob_overlay(True)),
            ("menu overlay", overlay("MenuOverlay", O(("version", 1), ("version", 1), ("actions", [FOCUS_LOST]),
                                                      ("withNumbers", False), ("handleName", "Data"))))]
    focus = O(("version", 1), ("backgroundColour", "19ffffff"), ("outlineColour", "ffff5b2e"), ("backgroundInset", 1.0),
              ("outlineThickness", 1.0))
    names = {}                           # the discrete parameters' names (replacementStrings)

    def tab_children(page):
        kids = [entry("panel", "Image", image_data(use(page["background"])), bounds((0, 0, 1280, 628)))]
        boxes = []                       # (parameter, rect): for the Q-Link frames
        for i, c in enumerate(page["controls"]):
            n = c["mpc"]
            if c["names"]:
                names[n] = c["names"]
            h = handle(n)
            if c["kind"] == "knob":
                x, y, w, hh = c["rect"]
                ov = "menu overlay" if len(c["names"]) > 2 else "knob overlay bipolar" if c["bipolar"] else "knob overlay"
                knob = O(("version", 3), ("knobType", "FilmStrip"), ("filmStrip", use(c["strip"])), ("numFrames", c["frames"]),
                         ("invert", False), ("dragOrientation", "Vertical"), ("handleName", "Data"))
                name = "%s knob %d p%d" % (page["name"].lower(), i, n)
                defs.append((name, component(
                    [action("Mouse Down", "Q-Link", "Data", ""), action("Double Click", "Show Overlay", "Data", ov),
                     action("Enter Pressed", "Show Overlay", "Data", ov)],
                    [entry("knob", "Knob", knob, bounds((0, 0, w, hh))),
                     entry("focus", "Focus", focus, O(("version", 1), ("acceptsHWFocus", "No"), ("showWhenDataModelInvalid", "Show"),
                                                      ("whenVisible", "WhenFocussed"), ("boundsType", "Absolute"),
                                                      ("bounds", box((0, 0, w, hh)))))])))
                kids.append(entry("p%d" % n, name, O(("version", 1), ("handleName", "Data")), bounds(c["rect"], "Yes"), [("Data", h)]))
                boxes.append((n, c["rect"]))
            elif c["kind"] == "button":
                for r in c["ranges"]:
                    hide = ["IndexedEnabling/%d/%d/%s" % (r["a"], c["steps"], h)]
                    data = O(("version", 2), ("onImage", use(r["image"])), ("offImage", r["image"]), ("buttonId", r["next"]),
                             ("numButtonsInGroup", c["steps"]), ("handleName", h), ("gestureBehaviour", "Instant"))
                    kids.append(entry("p%d" % n, "Button", data, bounds(c["rect"], "No", hide, "Hide")))
                boxes.append((n, c["rect"]))
            else:                        # a choice's buttons: radio buttons, one a choice
                if len(c["rows"]) != c["steps"]:
                    sys.exit("p%d: %d buttons for %d values" % (n, len(c["rows"]), c["steps"]))
                for k, row in enumerate(c["rows"]):
                    data = O(("version", 2), ("onImage", use(row["on"])), ("offImage", use(row["off"])), ("buttonId", k),
                             ("numButtonsInGroup", c["steps"]), ("handleName", h), ("gestureBehaviour", "Instant"))
                    kids.append(entry("p%d" % n, "Button", data, bounds(row["rect"])))
                    boxes.append((n, row["rect"]))
        return kids, boxes

    def frame(group, boxes):
        """The Q-Link group's frame: around its controls, with an even gap that touches no other control; none when
        it would have to take in another control (the group's controls are not side by side)."""
        mine = [r for p, r in boxes if p in group]
        if not mine:
            return None
        x0, y0 = min(r[0] for r in mine), min(r[1] for r in mine)
        x1, y1 = max(r[0] + r[2] for r in mine), max(r[1] + r[3] for r in mine)

        def cuts(a, b, c, d):
            for p, r in boxes:
                if p in group:
                    continue
                if min(c, r[0] + r[2]) - max(a, r[0]) > 0 and min(d, r[1] + r[3]) - max(b, r[1]) > 0:
                    return True
            return False
        if cuts(x0, y0, x1, y1):
            return None
        for pad in (6, 5, 4, 3, 2, 1, 0):
            a, b, c, d = max(0, x0 - pad), max(0, y0 - pad), min(1280, x1 + pad), min(628, y1 + pad)
            if not cuts(a, b, c, d):
                return (a, b, c - a, d - b)
        return (x0, y0, x1 - x0, y1 - y0)

    tabs, qmaps = [], []
    for page in m["pages"]:
        kids, boxes = tab_children(page)
        rects, q = [], {}
        for row, g in enumerate(page["qlinks"]):
            r = frame([p for p in g if p >= 0], boxes)
            if r:
                rects.append(r)
            for col, n in enumerate(g):
                q[13 + col - 4 * row] = n
        tabs.append(O(("version", 1), ("tabName", page["name"]), ("fnKeyIndex", page["key"]), ("fnKeySubIndex", page["sub"]),
                      ("componentDefinition", component([], kids)),
                      ("qlinkBoundsData", [box(r) for r in rects])))
        qmaps.append(O(("Tab", page["key"] + 1), ("SubTab", page["sub"] + 1), ("Bank Direction", "RowTopDown"),
                       ("Q-Links", O(*[("Q-Link %d" % n, q.get(n, -1)) for n in range(1, 17)]))))

    program = m["program"]             # (Program mode: the renderer's list, Q-Link 1 bottom left)
    repl = [O(("key", handle(n)), ("value", v)) for n, v in sorted(names.items())]
    tui = O(("pageData", O(("version", 1),
                           ("componentDefinitions", O(("version", 2), ("importFiles", []),
                                                      ("localComponentDefinitions", [O(("key", k), ("value", v)) for k, v in defs]))),
                           ("replacementStrings", repl),
                           ("info", O(("version", 1), ("type", "CompleteDescription"))),
                           ("tabs", tabs))))
    ql = O(("version", 4), ("info", O(("version", 1), ("type", "CompleteDescription"))),
           ("Screen Mode Q-Links", O(("version", 4), ("map", qmaps))),
           ("Program Mode Q-Links", O(*[("Q-Link %d" % (n + 1), p) for n, p in enumerate(program)])))
    with open(os.path.join(out, "TUI.json"), "w") as f:
        f.write(dump(tui) + "\n")
    with open(os.path.join(out, "Q-Links.json"), "w") as f:
        f.write(dump(ql) + "\n")
    try:
        from PIL import Image
    except ImportError:
        Image = None
        print("(no Pillow: the pictures copied as they are, many times larger)")
    for f in sorted(used):
        if Image:
            im = Image.open(os.path.join(src, f)).convert("RGB")
            im.quantize(256, method=Image.MEDIANCUT, dither=Image.NONE).save(os.path.join(out, f), optimize=True)
        else:
            shutil.copy2(os.path.join(src, f), os.path.join(out, f))
    print("TUI.json: %d pages, %d components; Q-Links.json; %d pictures in %s" % (len(tabs), len(defs), len(used), out))
    return 0


if __name__ == "__main__":
    sys.exit(main(sys.argv))
