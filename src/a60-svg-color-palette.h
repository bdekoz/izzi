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

#include "a60-svg-color.h"


namespace svg {

  /**
     Hand-picked values for a 67-68 BTIHA, aka cyberwar.

     Start with color constraints, generate 264 options, hand-pick to n=68.
     http://jnnnnn.github.io/category-colors-constrained.html
   */
  palette_qi pqi_68 = {
    
    {251, 225, 217}, {254, 231, 192}, {253, 196, 189}, {212, 130, 149}
  }

} // namespace svg

#endif