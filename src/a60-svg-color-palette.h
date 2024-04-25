// svg custom palette-*- mode: C++ -*-

// alpha60
// bittorrent x scrape x data + analytics

// Copyright (c) 2024, Benjamin De Kosnik <b.dekosnik@gmail.com>

// This file is part of the alpha60 library.  This library is free
// software; you can redistribute it and/or modify it under the terms
// of the GNU General Public License as published by the Free Software
// Foundation; either version 3, or (at your option) any later
// version.

// This library is distributed in the hope that it will be useful, but
// WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// General Public License for more details.

#ifndef MiL_SVG_COLOR_PALETTE_H
#define MiL_SVG_COLOR_PALETTE_H 1

#include "a60-svg.h"


namespace svg {

  /**
     CIECAM02-UCS color space, perceptually different colors, equally spaced out

     Hand-picked values for a 67-68 BTIHA, aka cyberwar.
     Start with color constraints, generate 264 options, hand-pick to n=68.
     http://jnnnnn.github.io/category-colors-constrained.html
   */
  palette_qi<72> pqi_72 = {

    // SE, red to orange/yellow (18)
    color_qi(251,225,217), color_qi(254,231,192), color_qi(253,196,189),
    color_qi(212,130,149), color_qi(253,225,137), color_qi(252,205,149),
    color_qi(217,167,66), color_qi(254,205,15), color_qi(246,166,2),
    color_qi(205,129,38), color_qi(233,112,10), color_qi(208,73,7),
    color_qi(253,65,9), color_qi(243,28,78), color_qi(201,8,10),
    color_qi(202,105,73), color_qi(242,92,88), color_qi(175,77,68),
    /*
    color_qi(253,176,130), color_qi(253,162,71), color_qi(246,131,146),
    color_qi(254,103,148), color_qi(250,45,142), color_qi(213,9,126),
    */

    // SW, yellow, chartreuse, green, aquamarine (18)
    color_qi(204,246,233), color_qi(155,254,227), color_qi(253,233,69),
    color_qi(214,222,15), color_qi(211,254,20), color_qi(158,247,24),
    color_qi(10,216,17), color_qi(10,235,101), color_qi(27,232,149),
    color_qi(46,246,197), color_qi(224,248,106), color_qi(183,255,128),
    color_qi(97,189,41), color_qi(163,238,148), color_qi(207,229,137),
    color_qi(129,255,173), color_qi(158,227,189), color_qi(206,248,196),

    // NE lavender to red violet (18)
    color_qi(91,90,208), color_qi(125,91,240), color_qi(167,26,253),
    color_qi(181,157,251), color_qi(214,202,254), color_qi(178,86,249),
    color_qi(214,148,253), color_qi(188,121,216), color_qi(159,32,195),
    color_qi(245,17,253), color_qi(251,80,225), color_qi(206,98,203),
    color_qi(252,18,201), color_qi(235,125,171), color_qi(252,184,211),
    color_qi(255,226,241), color_qi(254,199,248), color_qi(221,177,238),

    // NW sky blue to indigo (18)
    color_qi(11,240,233), color_qi(16,224,248), color_qi(152,252,255),
    color_qi(199,236,249), color_qi(231,234,253), color_qi(187,217,253),
    color_qi(126,216,254), color_qi(95,153,253), color_qi(57,87,255),
    color_qi(22,130,251), color_qi(16,176,255), color_qi(48,199,253),
    color_qi(5,169,209), color_qi(204,214,228), color_qi(100,214,203),
    color_qi(146,166,254), color_qi(123,159,212), color_qi(105,154,179),
  };

} // namespace svg

#endif
