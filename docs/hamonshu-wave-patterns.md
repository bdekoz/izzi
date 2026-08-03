# Hamonshū volume 2 wave-pattern catalogue and rendering notes

[Izzi index](../index.md) · [Izzi README](../README.md) ·
[Cartofreako](https://github.com/bdekoz/cartofreako)

The generated `generated/svg/ocean-*.svg` projection suite combines the Natural
Earth 1:10m ocean polygon with vector interpretations of the wave studies in
Mori Yūzan's 1903 *Hamonshū*, volume 2. The cartofreako generator is
[`tests/generate-ocean.cc`](https://github.com/bdekoz/cartofreako/blob/main/tests/generate-ocean.cc).
Reusable motif construction and the complete, machine-readable 153-entry
catalogue now live in Izzi's
[`a60-svg-curves-hamonshu.h`](../src/a60-svg-curves-hamonshu.h)
and
[`a60-svg-curves-hamonshu-v2.inc`](../src/a60-svg-curves-hamonshu-v2.inc).
Cartofreako supplies the geographic tiling, projection, ocean clipping,
palette, and layer assembly around those paths.

The header also owns the deliberately curated set rendered by
[`examples/curves-hamonshu.cc`](../examples/curves-hamonshu.cc) and the
cartofreako ocean generator: 13 selected source motifs at seven curvature
ratios, for 91 variations. Keeping that selection beside the full catalogue
prevents the two consumers from drifting apart.

## Page and name conventions

The cartofreako provenance PDF has 28 scan pages, while the bound work
represented by those scans has 51 content pages.  Scan 1 is the front cover.
Scan 2 contains illustrated page 1 and a blank facing page.  Scans 3 through
27 contain two book pages apiece, and scan 28 is the back cover.  Consequently
the generator maps illustrated page 1 to PDF scan 2 and any later illustrated
page `p` to PDF scan `floor((p + 4) / 2)`.

The catalogue records each visually bounded pattern specimen separately.  It
contains 153 specimens from illustrated pages 1 through 49 and 51.  Page 50
is the colophon and contributes no motif.  One composition spans pages 24-25,
three bands span pages 46-47, and the page-51 collage contributes five
separately bounded specimens.

The National Diet Library catalogue says that the source has no annotations
such as pattern names (`文様名などの注記なし`).  There are therefore no
Japanese motif captions to translate.  Labels such as `nested-current-scrolls`
are concise descriptive English catalogue names, not historical titles.

Every catalogue motif has this stable source ID:

```text
hamonshu-page-PPP[-PPP]-motif-MM-descriptive-english-name
```

Curated ocean layers append the zero-based curvature-column index:

```text
hamonshu-page-PPP[-PPP]-motif-MM-descriptive-english-name-curvature-C
```

Each ocean layer contains a water-region path and an Izzi-generated line
path. Its SVG `<title>` records the illustrated page or page span, the
corresponding PDF scan, the motif ordinal, descriptive name, and curvature
ratio. Clip paths use the same variation ID with a `clip-` prefix. This makes
source and form lookup possible from either an SVG editor's layer panel or
the XML itself.

## Vector interpretation

The catalogue distinguishes individual bands, medallions, sprays, crests,
scrolls, fields, and multi-page compositions. The Izzi C++20 curve header
translates their visible geometric vocabulary into sixteen procedural line
families:
waterlines, crests, spirals, spray, arcs, lattices, bubbles, scrolls, fans,
breakers, braids, cascades, ripples, fountains, clouds, and cells.  Page and
motif metadata seed the spacing, phase, direction, and density, so each
catalogued specimen produces a distinct path treatment.

The public `curated_motif_selections` array identifies page/motif rows
`001/01`, `002/01`, `003/01`, `003/02`, `006/02`, `009/01`,
`017/03`, `020/04`, `023/02`, `039/02`, `040/01`, `046-047/02`,
and `051/05`. The public `curated_curvature_ratios` array contains
`0.25`, `0.45`, `0.70`, `1.0`, `1.30`, `1.65`, and `2.10`.
Across a row, only wave height, curl radius, and transverse displacement
change; the `1.0` column preserves the canonical form. Density, phase,
orientation, reflection, sampling, and SVG color remain fixed.

These paths are clean vector interpretations intended for map texture, not
pixel-by-pixel facsimile tracings of the paper scans.  Curves, ellipses, and
spirals are sampled into `svg::vrange` values and serialized with Izzi's
`svg::make_path_data_from_points`; Izzi also creates every final SVG path.

## Ocean geometry and projection

Only `ne_10m_ocean.shp` is read. GDAL simplifies its topology by 0.04 degrees,
then intersects it with 10-degree geographic tiles for Cahill-Keyes and
Star-X or 5-degree tiles for AuthaGraph, Myriahedral, and Voronoi. Polygon
pieces are cut into antimeridian-safe registered longitude bands before
projection. Native-cell bisection separates periodic cuts; Myriahedral and
Voronoi filled pieces are instead mapped face-locally and intersected with
each exact planar triangle. Segmentation before the forward transform keeps
curved projected edges smooth.

The surviving water-only pieces form a non-overlapping mosaic.  They are
assigned across the 91 curated motif/curvature layers, while a pale
complete-ocean path covers small coastal and polar fragments omitted from the
mosaic. Each variation's linework is clipped to precisely its assigned Natural
Earth water regions; no land geometry is loaded or drawn.

Each output preserves its projection's exact aspect ratio with a largest
dimension of 44 units. `main()` verifies the projection-specific view box,
base ocean groups, all 91 variation groups, all 91 clip paths, two paths per
variation, curated IDs and source/curvature titles, and finite coordinates.

## Rebuild

GDAL development files, `curl`, `unzip`, and the neighboring Izzi source tree
are required. Cartofreako's Makefile tracks the Izzi Hamonshū header and
catalogue as direct build dependencies. Its Natural Earth fetch target
downloads and verifies the pinned physical-vector bundle when needed. Run
from a cartofreako checkout:

```sh
make generate-ocean-ck
make generate-ocean-projections
```

The commands build `tests/generate-ocean`. The first writes
`generated/svg/ocean-ck-44-22.svg`; the second writes the AuthaGraph,
Myriahedral, Star-X, and Voronoi ocean SVGs under `generated/svg/`. `make
clean` removes the binary and the generated SVG, PDF, and PNG directories.

## Sources and provenance

- [Cartofreako volume 2 source PDF](https://github.com/bdekoz/cartofreako/blob/main/assets/adhoc/hamonshū.wave-studies.1903.jp.02.pdf)
- Volume 2 PDF SHA-256:
  `bd45650723dc7282efd08871b735ee12a1a9b307c4909c8f2a4e61917fe22b23`
- [Smithsonian Libraries digital edition of *Hamonshū*, volume 2](https://library.si.edu/digital-library/book/hamonshuy00mori)
- [National Diet Library catalogue record](https://ndlsearch.ndl.go.jp/books/R100000094-I214457)
- [Natural Earth 1:10m physical vectors](https://www.naturalearthdata.com/downloads/10m-physical-vectors/)
- [Cartofreako Natural Earth data note](https://github.com/bdekoz/cartofreako/blob/main/docs/natural-earth-10m-physical-vectors.md)

The Smithsonian marks the digitized volume CC0.  Natural Earth data is public
domain; version, archive checksum, and extracted files are recorded in the
repository data note.
