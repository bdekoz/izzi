// svg circle and radial forms -*- mode: C++ -*-

// alpha60
// bittorrent x scrape x data + analytics

// Copyright (c) 2020-2021, Benjamin De Kosnik <b.dekosnik@gmail.com>

// This file is part of the alpha60 library.  This library is free
// software; you can redistribute it and/or modify it under the terms
// of the GNU General Public License as published by the Free Software
// Foundation; either version 3, or (at your option) any later
// version.

// This library is distributed in the hope that it will be useful, but
// WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// General Public License for more details.

#ifndef MiL_SVG_RADIAL_BASE_H
#define MiL_SVG_RADIAL_BASE_H 1

#include <set>
#include <chrono>
#include <iostream>
#include <iomanip>
#include <algorithm>

#include "a60-svg.h"



namespace svg {

/// Given rdenom scaling factor and SVG canvas, compute effective
/// radius value.
inline double
get_radius(const svg_element& obj, const uint rdenom)
{
  auto leastside = std::min(obj._M_area._M_height, obj._M_area._M_width);
  return leastside / rdenom;
}


/**
   Max number of non-overlapping degrees in radial form,
   as a tuple of (min, max).

   Total degrees in a circle are 360, but then the beginning and the
   end of the radial display are in the same place, which may
   confusing. So, shave a bit off both ends for the optics, so that
   there is a gap between the beginning and end of the generated
   radial viz. The default is such that the beginning and the end have
   a discernable gap.
*/
point_2t&
get_radial_range()
{
  static point_2t rrange = { 10, 350 };
  return rrange;
}


/// Transform a value on a range to an angle on the radial range.
inline double
get_angle(size_type pvalue, size_type pmax)
{
  // Normalize [0, pmax] to range [mindeg, maxdeg] and put pvalue in it.
  const auto [ mindeg, maxdeg ] = get_radial_range();
  double d = normalize_value_on_range(pvalue, 0, pmax, mindeg, maxdeg);
  return align_angle_to_north(d);
}


/// Get the label space.
/// Value -> Name, as a string where value has labelspaces of fill
/// NB: Should be the number of significant digits in pmax plus separators.
/// So, 10 == 2, 100 == 3, 10k == 5 + 1
size_type&
get_label_spaces()
{
  static size_type lspaces;
  return lspaces;
}

/// Set the number of label spaces.
void
set_label_spaces(size_type spaces)
{ get_label_spaces() = spaces; }


/// Or use set with lt.
void
sort_strings_by_size(strings& ids)
{
  auto lsizeless = [](const string& s1, const string& s2)
		   { return s1.size() < s2.size(); };
  sort(ids.begin(), ids.end(), lsizeless);
}


/// Make radial labels.
string
make_label_for_value(string pname, size_type pvalue,
		     const uint valuewidth = 9)
{
  // Consolidate label text to be "VALUE -> NAME"
  std::ostringstream oss;
  oss.imbue(std::locale(""));
  oss << std::setfill(' ') << std::setw(valuewidth)
      << std::left << pvalue;

  string label = oss.str() + " -> " + pname;
  return label;
}

} // namespace svg

#endif
