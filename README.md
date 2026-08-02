# izzi

These are C++ sources designed to produce images from a subset of the
SVG standard, using a header-only library. It is useful for
graphing, cartography, generative design, and data-intensive
computational expression.


**PREREQUISITES**

Development and use is hosted on top-of-tree Linux. At the moment,
this means Fedora 41 or Alma 9 operating systems on multi-core
x86_64 hardware.

Some familiarity with Linux, GNU coding styles, C++17-2x, and the SVG standard are assumed.


**USING**

Source Documentation [here](https://bdekoz.github.io/izzi/).


See the files in the [examples](https://github.com/bdekoz/izzi/tree/main/examples) subdirectory.


**HAMONSHU WAVE CURVES**

[`src/a60-svg-curves-hamonshu.h`](src/a60-svg-curves-hamonshu.h) provides a
C++20, header-only catalogue and deterministic procedural SVG paths for 153
wave studies indexed to Mori Yuzan's 1903 *Hamonshu*, volume 2. The API emits
normalized motif linework inside a caller-supplied `svg::hamonshu::pattern_box`;
map projection, clipping, styling, and SVG layer assembly remain with the
caller.

[`examples/curves-hamonshu.cc`](examples/curves-hamonshu.cc) validates the
catalogue and renders every motif. Build and run it from the repository root:

```sh
g++ -Isrc -std=c++20 -Wall -Wextra -Wpedantic -Werror \
  examples/curves-hamonshu.cc -o /tmp/curves-hamonshu
(cd /tmp && ./curves-hamonshu)
```

The generated `curves-hamonshu.svg` contains one titled group per source
specimen. The Smithsonian Libraries scan is CC0; the descriptive English
labels are not translations of historical captions.
