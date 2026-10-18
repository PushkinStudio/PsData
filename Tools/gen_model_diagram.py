#!/usr/bin/env python3
"""Generates docs/assets/model-light.svg and model-dark.svg, the README's picture of a PsData tree.

Colours follow docs/assets/brand/README.md: ink, light, the tile and the vermilion accent.

One layout, two palettes, so the variants cannot drift. Run from anywhere:
    python3 Tools/gen_model_diagram.py
Drawn on a 1000-unit canvas because GitHub shows README images at roughly 830 px.
No XML comments in the output: a double hyphen inside one makes the whole SVG invalid.
"""

import os

PALETTES = {
    "light": {
        "bg": "none",
        "panel": "#ffffff",
        "panel_alt": "#f4f6f9",
        "stroke": "#c9d1dc",
        "text": "#161b22",
        "muted": "#5d6b80",
        "accent": "#ff5c39",
        "link": "#2f7dd1",
        "event": "#2f9e6e",
    },
    "dark": {
        "bg": "none",
        "panel": "#1e2630",
        "panel_alt": "#171d25",
        "stroke": "#3a4556",
        "text": "#e8edf4",
        "muted": "#97a3b6",
        "accent": "#ff5c39",
        "link": "#5aa6f0",
        "event": "#4cc493",
    },
}

SANS = "-apple-system, BlinkMacSystemFont, 'Segoe UI', Helvetica, Arial, sans-serif"
MONO = "ui-monospace, SFMono-Regular, Menlo, Consolas, 'Liberation Mono', monospace"


def box(x, y, w, h, fill, stroke, rx=10):
    return f'<rect x="{x}" y="{y}" width="{w}" height="{h}" rx="{rx}" fill="{fill}" stroke="{stroke}" stroke-width="1.5"/>'


def text(x, y, s, fill, size=15, weight=400, family=SANS, anchor="start"):
    s = s.replace("&", "&amp;").replace("<", "&lt;").replace(">", "&gt;")
    return (f'<text x="{x}" y="{y}" fill="{fill}" font-size="{size}" font-weight="{weight}" '
            f'font-family="{family}" text-anchor="{anchor}">{s}</text>')


def tag(x, y, label, color):
    w = 9 + len(label) * 7.4
    return (f'<rect x="{x}" y="{y - 13}" width="{w:.0f}" height="18" rx="9" fill="{color}" fill-opacity="0.14" stroke="{color}" stroke-width="1"/>'
            + text(x + w / 2, y, label, color, size=11.5, weight=600, anchor="middle"))


def render(p):
    out = []
    add = out.append
    add('<svg xmlns="http://www.w3.org/2000/svg" viewBox="0 0 1000 490" width="1000" height="490" '
        'role="img" aria-label="A PsData tree: links and bubbling events">')
    add('<defs>')
    for name, color in (("link", p["link"]), ("event", p["event"]), ("edge", p["stroke"])):
        add(f'<marker id="arrow-{name}" viewBox="0 0 10 10" refX="9" refY="5" markerWidth="7" markerHeight="7" orient="auto-start-reverse">'
            f'<path d="M0,0 L10,5 L0,10 z" fill="{color}"/></marker>')
    add('</defs>')

    # Root
    add(box(390, 18, 220, 52, p["panel"], p["accent"], rx=12))
    add(text(500, 41, "UFooRootData", p["text"], size=16, weight=700, family=MONO, anchor="middle"))
    add(text(500, 60, "UPsDataRoot", p["muted"], size=12, anchor="middle"))

    # Tree edges
    add(f'<path d="M450,70 C450,92 170,88 170,112" fill="none" stroke="{p["stroke"]}" stroke-width="2" marker-end="url(#arrow-edge)"/>')
    add(f'<path d="M550,70 C550,92 690,88 690,112" fill="none" stroke="{p["stroke"]}" stroke-width="2" marker-end="url(#arrow-edge)"/>')

    # World branch: static data
    add(box(40, 114, 260, 350, p["panel_alt"], p["stroke"], rx=14))
    add(text(60, 144, "World", p["text"], size=17, weight=700, family=MONO))
    add(text(60, 165, "static data from DataTables", p["muted"], size=12.5))
    add(text(60, 200, "DMAP Weapons", p["muted"], size=12.5, family=MONO))
    rows = [("rifle", "Damage 30 · FireRate 10.5"), ("pistol", "Damage 12 · FireRate 4")]
    for i, (key, detail) in enumerate(rows):
        y = 212 + i * 62
        highlight = p["link"] if key == "rifle" else p["stroke"]
        add(box(60, y, 220, 50, p["panel"], highlight, rx=8))
        add(text(76, y + 21, key, p["text"], size=15, weight=600, family=MONO))
        add(text(76, y + 39, detail, p["muted"], size=12, family=MONO))
    add(text(60, 372, "live state keeps keys;", p["muted"], size=12.5))
    add(text(60, 390, "links resolve them here", p["muted"], size=12.5))
    add(text(60, 408, "(DLINK, Validation())", p["muted"], size=12.5, family=MONO))

    # Battle branch: live state
    add(box(420, 114, 540, 350, p["panel_alt"], p["stroke"], rx=14))
    add(text(440, 144, "Battle", p["text"], size=17, weight=700, family=MONO))
    add(text(440, 165, "live state", p["muted"], size=12.5))
    add(text(440, 200, "DMAP Characters", p["muted"], size=12.5, family=MONO))

    # alice, expanded
    add(box(440, 212, 400, 196, p["panel"], p["stroke"], rx=10))
    add(text(456, 236, "alice", p["text"], size=15, weight=700, family=MONO))
    add(text(824, 236, "UFooCharacterData", p["muted"], size=12, family=MONO, anchor="end"))
    fields = [
        ("Name", "\"Alice\"", None),
        ("Health", "70", ("Event", p["event"])),
        ("Location.X", "5", ("Event · Bubbles", p["event"])),
        ("Weapon", "\"rifle\"", ("DLINK", p["link"])),
    ]
    for i, (name, value, badge) in enumerate(fields):
        y = 270 + i * 36
        add(f'<line x1="456" y1="{y - 22}" x2="824" y2="{y - 22}" stroke="{p["stroke"]}" stroke-width="1"/>')
        add(text(456, y, name, p["text"], size=14, family=MONO))
        add(text(586, y, value, p["muted"], size=14, family=MONO))
        if badge:
            add(tag(672, y - 1, badge[0], badge[1]))

    # bob, collapsed
    add(box(440, 418, 400, 30, p["panel"], p["stroke"], rx=8))
    add(text(456, 438, "bob", p["text"], size=14, weight=700, family=MONO))
    add(text(824, 438, "UFooCharacterData", p["muted"], size=12, family=MONO, anchor="end"))

    # Link: alice.Weapon resolves into World.Weapons.rifle
    add(f'<path d="M450,372 C380,372 360,237 284,237" fill="none" stroke="{p["link"]}" stroke-width="2.5" '
        f'stroke-dasharray="7 5" marker-end="url(#arrow-link)"/>')
    add(text(360, 392, "LinkByWeapon", p["link"], size=12.5, weight=600, family=MONO, anchor="middle"))
    add(text(360, 409, "resolves the key", p["link"], size=12, anchor="middle"))

    # Bubbling: Location.X change heard on Battle
    add(f'<path d="M846,334 C905,334 905,180 905,140" fill="none" stroke="{p["event"]}" stroke-width="2.5" marker-end="url(#arrow-event)"/>')
    add(text(940, 236, "XChanged", p["event"], size=12.5, weight=600, family=MONO, anchor="end"))
    add(text(940, 254, "bubbles up", p["event"], size=12.5, anchor="end"))
    add(f'<circle cx="905" cy="132" r="5" fill="{p["event"]}"/>')
    add(text(892, 136, "Battle->Bind(...)", p["event"], size=12, family=MONO, anchor="end"))

    add('</svg>')
    return "\n".join(out) + "\n"


def main():
    root = os.path.normpath(os.path.join(os.path.dirname(os.path.abspath(__file__)), ".."))
    out_dir = os.path.join(root, "docs", "assets")
    os.makedirs(out_dir, exist_ok=True)
    for name, palette in PALETTES.items():
        path = os.path.join(out_dir, f"model-{name}.svg")
        with open(path, "w", encoding="utf-8") as f:
            f.write(render(palette))
        print(path)


if __name__ == "__main__":
    main()
