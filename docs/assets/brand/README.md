# PsData brand

Made with the [logo-design skill](https://github.com/kaankiziltug/logo-design-skill), 2026-10-18.
The marks live one level up, in `docs/assets/`; this folder keeps the concept sheet, the social
preview and these notes. Three directions were explored ([`concepts.png`](concepts.png)); A was
chosen.

## The mark

**Nest:** a P whose bowl holds a D. Ps holds Data, and the D is a child nested inside its parent,
which is the PsData tree in one glyph. The D is the only thing that takes the accent colour, and the
lockup repeats it on the D of "Data".

## Palette

| Name | HEX | RGB | Use |
|---|---|---|---|
| Signal (vermilion) | `#FF5C39` | 255 92 57 | The D in the symbol and in the wordmark. The only accent. |
| Ink (graphite) | `#161B22` | 22 27 34 | P and wordmark on light backgrounds |
| Light | `#E8EDF4` | 232 237 244 | P and wordmark on dark backgrounds (the same cool white as Zerus) |
| Tile | `#1E2630` | 30 38 48 | App-icon tile, social preview background |

Print: Signal is about Pantone 171 C, and Ink about Pantone Black 6 C. Neither has been proofed, and
CMYK values were not specified. Check both against a physical guide before printing.

Contrast (WCAG ratio):

| Pair | Ratio |
|---|---|
| Signal on white | 3.07:1 |
| Signal on GitHub dark `#0D1117` | 6.17:1 |
| Signal on the tile | 4.98:1 |
| Signal on Unreal grey `#242424` | 5.06:1 |
| Ink on white | 17.3:1 |
| Light on GitHub dark | 16.1:1 |
| Light on the tile | 13.0:1 |

Every pair is at least 3:1, the minimum for graphics.

## Files

All in `docs/assets/` unless noted.

| File | What it is |
|---|---|
| `psdata-mark.svg` | Symbol, 256 square, transparent. For **light** backgrounds (graphite P, vermilion D). |
| `psdata-mark-dark.svg` | Symbol for **dark** backgrounds (light P with a thinned reversed cut, vermilion D). Added because one transparent two-tone file cannot work on both GitHub themes. |
| `psdata-mark-tile.svg` | Symbol on a graphite rounded tile. Self-contained, works on any background. |
| `psdata-wordmark-light.svg` / `-dark.svg` | Horizontal lockup, 943×256, all outlined paths. Both have the same canvas and glyph positions, so they can be swapped in a `<picture>`. |
| `Resources/Icon128.png` (plugin root) | 128×128 RGBA Unreal plugin icon, rendered from the tile. |
| `brand/social-preview.png` | 1280×640. Dark lockup at 720 px wide on the tile colour. Upload it in the repository's Settings → Social preview. |
| `brand/concepts.png` | Round-1 concept sheet: three concepts, one colour, with 64/32/16 px sizes. |

**README tip:** GitHub follows its own theme setting in `<picture>`:

```html
<picture>
  <source media="(prefers-color-scheme: dark)" srcset="psdata-wordmark-dark.svg">
  <img src="psdata-wordmark-light.svg" alt="PsData" width="380">
</picture>
```

For a small icon next to a title, use the same pattern with the two mark files, or use the tile on its
own.

## Construction (256 grid)

**The P**
- Stem is 36 wide, x 50 to 86, y 24 to 232.
- The bowl is a straight run from x 50 to 124, then a circular arc of radius 78 centred at (124, 102).
  The bowl is 156 tall, 75% of the mark, so the D has room.
- The horizontal bars are 32 against the 36 stem, an optical thinning of 11%.

**The counter and the D**
- The counter shares the arc's centre. Its inner arc is an ellipse, 42 × 46.
- The D is the counter inset by a uniform channel of 14 (5.5% of the canvas). Its straight run is 24,
  its arc 28 × 32.
- The D is 52 × 64, which keeps D proportions.
- Below about 20 px the channel closes. The colour version keeps the D visible by hue. The one-colour
  version then reads as a bold P.

**Reversed cut (dark backgrounds)**
- Stem 35 and bars 31. The outer contour and the D are unchanged, so the channel becomes 15.
- This compensates for light-on-dark irradiation.

**Tile**
- Continuous-curvature corners (squircle, corner start 56 / 256).
- Symbol ink height is 62% of the tile.
- Optically centred: 3 units left and 2 units up of the box centre, because the P's mass sits in the
  bowl.

**Wordmark** (constructed, no font)
- Cap height 100, x-height 74. Stems 14.5, horizontals 12.5, overshoot 1.5 on round forms.
- Every curve is an elliptical arc. P and D echo the symbol's construction: square corners, straight
  run plus arc.
- The s is two elliptical rings meeting tangentially at a horizontal spine. Its lower bowl is wider
  (54 against 50) for stability.
- The a is single-storey. The t is straight with no foot.
- The dark wordmark draws 0.5 unit thinner strokes on the same glyph origins.

**Lockup**
- Symbol ink height = 1.4 × cap height, centred on the cap band.
- Symbol-to-wordmark gap = 0.4 × cap height (about 1.65 symbol stems).
- Padding: 8 units, about 5% of the height.

**Clear space and minimum size** (suggested)
- Clear space: at least the symbol's stem width on every side of the symbol, and 0.5 × cap height
  around the lockup.
- Minimum sizes: symbol 16 px (the D shows from 24 px up), lockup 120 px wide.

**SVG hygiene**
- Filled paths only. No `<text>`, rasters, filters, masks, gradients, comments or transforms.
- Every file carries `<title>` and `role="img"`.

## Concepts (round 1, [`concepts.png`](concepts.png))

| Concept | Idea | Notes |
|---|---|---|
| **A, Nest** (chosen) | A P whose bowl holds a D: data nested in its parent, PsData in one glyph. | Letterform + negative space |
| **B, Outline** | The tree as an IDE outline: a change at the leaf climbs the connectors to the root. | Abstract |
| **C, Turnaround** | P, s and d as one point-symmetric figure: the same model at both ends of the wire. | Monogram |

## Why A

**A, Nest**
- It carries the name (P + D) and the tree/nesting idea in one glyph, without explaining.
- It has the simplest silhouette: a bold P. That makes it the best at 16 to 40 px and in the Unreal
  plugin list.
- The colour split works naturally (the data is the accent).
- It produced no unwanted readings when mirrored, rotated 180°, blurred or shrunk.

**B, Outline**
- It is honest and engineered, but it reads close to a generic "outline / list" UI icon.
- At a glance it reads as an "E".
- It turns into stripes at 16 px.

**C, Turnaround**
- It has the strongest structure of the three, but at large sizes it recalls the shekel sign (₪).
- It is dense at 16 px.

Directions dropped while sketching:
- A tree-P (reads as ₽, the ruble sign).
- A dot inside the P's counter (reads as a face).
- A d with a node (reads as a music note).
- An up-arrow tree (reads as a stick figure).
- Concentric D's (too close to Dropzone).
- Corner-nested squares (too close to Webtask).
- A P with a detached stem foot (too close to PagerDuty).
- Any "Ps" monogram tile, because Adobe Photoshop's icon is literally "Ps" in a rounded square.

## Test results

**svg_audit.py**
- All five final SVGs: 100/100 with no findings.
- The three concept symbols: 100/100.
- XML parses for every final, and none contains `--`.

**preview_sheet.py** (screenshotted with headless Chrome and reviewed)
- Size ladder 16 to 256 px: passes.
- Pixel test: at 16 px the channel goes grey and the mark reads as a P. The D is clear from 24 to 32 px.
- Greyscale: the D separates by value.
- One-colour black and white: the channel carries the D.
- Squint: distinct.
- Mirror and rotate 180°: only letter readings (q, d), nothing unwanted.
- Favicon, app-icon, avatar crop and header contexts: pass.
- Shelf test against framework-library marks: stands out.

**True-size checks** (16/24/32/40/64 px, on white, `#242424` and `#0D1117`)
- Wordmarks checked at 380 px on GitHub light and dark.
- Mark and tile checked at 64/32/16 px.
- Icon checked at 128 and 40 px on both Unreal greys.

## Not tested / open items

- **Trademark clearance was not done.** A professional search and a reverse image search are
  recommended before adoption.
- **Device to watch:** Percona and Parsehub use a P with a coloured disc in the counter. Ours uses a D
  shape cut by a channel, but the coloured-counter device is related.
- **Real apps not tested:** I did not run the Unreal Editor or GitHub. Their backgrounds were simulated
  with rsvg-convert and Chrome renders.
- **Tile edge on Unreal's grey:** the graphite tile has almost no luminance contrast against `#242424`
  (1.02:1). It separates only by its cooler hue, so there the P and D carry the icon.
  - If the icon needs to pop more in the plugin list, a vermilion tile (white P, graphite D) was tested
    and works.
- **Not built (outside the brief):**
  - A dedicated 16 px favicon cut.
  - A one-colour tile.
  - Stacked lockups.
  - Print CMYK masters.
- **The a in the wordmark** overlaps its ring and stem as subpaths in one path (nonzero union). This is
  fine on screen and in print. Cutters or embroidery need a merged outline from a vector editor;
  Inkscape was not available here.
