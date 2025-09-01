// alpha60 bar graphs/charts -*- mode: C++ -*-

// alpha60
// bittorrent x scrape x data + analytics

// Copyright (c) 2020-2021, 2025 Benjamin De Kosnik <b.dekosnik@gmail.com>

// This file is part of the alpha60 library.  This library is free
// software; you can redistribute it and/or modify it under the terms
// of the GNU General Public License as published by the Free Software
// Foundation; either version 3, or (at your option) any later
// version.

// This library is distributed in the hope that it will be useful, but
// WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// General Public License for more details.

#ifndef a60_SVG_BAR_GRAPHS_H
#define a60_SVG_BAR_GRAPHS_H 1


namespace svg {

//// Distance between center and top/bottom block
const double vspace(60);

/// Distance between label and top/bottom of line or text
const double vlspace(10);

// Type sizes.
const auto lsz = 16; // large bold
const auto asz = 12; // sub headings
const auto ssz = 10; // sub sub headings

/// Compute xoffset.
uint
get_xoffset(const string& id, const uint xstart, const double hspace)
{ return xstart + (k::age_value_to_multiple(id, true) * hspace); };

using id_size = ulong;
using umids = std::unordered_map<string, id_size>;
using vumids = std::vector<umids>;

svg_element  
make_bar_graph(umids agesvo, const area<> a = svg::k::v1080p_h,
	       const double yscale = 0.7, const uint rwidth = 10)
{
    // Vector of total age_value counts per decile for birth ages.
  vul vsum(k::age_values.size(), 0);
  for (const auto& val : agesvo)
    {
      auto [ id, count ] = val;
      uint xoff = get_xoffset(id, xstart, hspace);

      // rect
      double rheight = scale_value_on_range(count, 1, maxn, 10, ydelta);
      double rystart = voffstart - (rheight / 2);
      point_to_rect_centered(obj, std::make_tuple(xoff, rystart),
			     get_metadata_style(), rwidth, rheight);

      // text
      sized_text(obj, typov, asz, to_string(count), xoff,
		 voffstart - rheight - vlspace);
}

 
} // namepace a60::metadata

#endif
