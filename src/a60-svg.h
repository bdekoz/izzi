// svg API -*- mode: C++ -*-

// Copyright (C) 2019-2025 Benjamin De Kosnik <b.dekosnik@gmail.com>

// This file is part of the alpha60-MiL SVG library.  This library is
// free software; you can redistribute it and/or modify it under the
// terms of the GNU General Public License as published by the Free
// Software Foundation; either version 3, or (at your option) any
// later version.

// This library is distributed in the hope that it will be useful, but
// WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// General Public License for more details.

#ifndef MiL_SVG_H
#define MiL_SVG_H 1

#include <cstddef>
#include <cmath>
#include <algorithm>
#include <array>
#include <tuple>
#include <string>
#include <vector>
#include <set>
#include <unordered_map>
#include <sstream>

/**
 *  Scalable Vector Graphics (SVG) namespace
 */
namespace svg {

/// Base string types.
using std::string;
using std::string_view;
using strings = std::vector<string>;
using vstrings = strings;
using vvstrings = std::vector<strings>;

// Utility function, like regex_replace.
inline void
string_replace(std::string& target, const std::string& match,
	       const std::string& replace)
{
  size_t pos = 0;
  while((pos = target.find(match, pos)) != std::string::npos)
    {
      target.replace(pos, match.length(), replace);
      pos += replace.length();
    }
}


/// Base integer type: positive and negative, signed integral value.
using ushort = unsigned short;
using uint = unsigned int;
using ulong = unsigned long;
using ssize_type = int;

/// Base floating point type.
using space_type = double;


/// Scale value from min to max on range (nfloor, nceil).
double
scale_value_on_range(const ssize_type value, const ssize_type min,
		     const ssize_type max,
		     const ssize_type nfloor, const ssize_type nceil)
{
  double rmultp(nceil - nfloor);
  double valnum(value - min);
  double valdenom(max - min);
  double weightn = (rmultp * (valnum / valdenom)) + nfloor;
  return weightn;
}


/// Resolution of output display device.
double&
get_dpi()
{
  static double dpi(96.0);
  return dpi;
}


/// Conversion between point size to pixels given dpi density.
double
pt_to_px(const uint i = 1)
{
  // 1pt is equal to exactly 1/72th of an inch.
  // On a 72dpi output device (display), this is 1 (aka 72 * 1/72).
  // On a 90dpi output device (display), this is 1.25 (aka 90 * 1/72).
  // On a 96dpi output device (display), this is 1.33 (aka 96 * 1/72).
  // constexpr double dpimult = (get_dpi() / 72);

  // Or 18 pt -> 13.5 px -> 1.33
  // Or 30 pt -> 22.5 px -> 1.33
  constexpr double dpimult = 1.33;
  return std::lround(i * dpimult);
}


/// Approximate pixel height of type of point size @sz.
constexpr double
char_width_to_px(const uint sz)
{ return 0.58 * sz; }


/// Approximate pixel height of type of point size @sz.
constexpr double
char_height_to_px(const uint sz)
{ return 0.94 * sz; }


/**
 *  SVG Constants
 */
namespace constants {

/// Formatting character constants.
constexpr char space(' ');
constexpr char quote('"');
constexpr char hyphen('-');
constexpr char comma(',');
constexpr char tab('\t');
constexpr char newline('\n');


/**
    Numeric constants.
    pi = double(22)/double(7);
    pi = 3.14159265358979323846
*/
constexpr double pi(3.14159265358979323846);

} // namespace constants

/// Inject nested namepace constants into svg namespace with alias k.
namespace k = constants;

} // namespace svg


#include "a60-svg-color.h"			// color, color_qi, color_qf
#include "a60-svg-color-palette.h"
#include "a60-svg-color-band.h"

#include "izzi-points.h"	     // 2d-points, range, distance
#include "a60-svg-base-types.h"	     // area, style, filter, transform, typography
#include "a60-svg-constants.h"
#include "a60-svg-elements.h"
#include "a60-svg-elements-components.h"
#include "a60-svg-render-state.h"
#include "a60-svg-render-basics.h"
#include "a60-svg-composite-and-layer-basics.h"
#include "a60-svg-sequences.h"

#endif
