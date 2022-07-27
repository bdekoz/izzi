// svg API -*- mode: C++ -*-

// Copyright (C) 2019-2022 Benjamin De Kosnik <b.dekosnik@gmail.com>

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

#include <cmath>
#include <array>
#include <tuple>
#include <string>
#include <vector>
#include <unordered_map>
#include <sstream>

/**
 *  Scalable Vector Graphics (SVG) namespace
 */
namespace svg {

/// Base string type.
using std::string;
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
using size_type = int;

/// Base floating point type.
using space_type = double;

/// Point (x,y) in 2D space.
using point_2t = std::tuple<space_type, space_type>;

/// Point (x,y) in 2D space with weight n.
using point_2tn = std::tuple<point_2t, size_type>;


/// Convert point_2t to string.
string
to_string(point_2t p)
{
  auto [ x, y ] = p;
  std::ostringstream oss;
  oss << x << ',' << y;
  return oss.str();
}


/// Latitude and Longitude Ranges.
using vrange = std::vector<point_2t>;
using vvranges = std::vector<vrange>;


/// Find cartesian distance between two 2D points.
space_type
distance_cartesian(const point_2t& p1, const point_2t& p2)
{
  auto [ x1, y1 ] = p1;
  auto [ x2, y2 ] = p2;
  auto distancex = (x2 - x1) * (x2 - x1);
  auto distancey = (y2 - y1) * (y2 - y1);
  space_type distance = sqrt(distancex + distancey);
  return distance;
}


// Does point p1 of radius r1 intersect point p2 with radius r2?
// https://developer.mozilla.org x 2D_collision_detection
bool
detect_collision(const point_2t& p1, const int r1, const point_2t& p2,
		 const int r2)
{
  bool ret(false);
  if (distance_cartesian(p1, p2) < r1 + r2)
    ret = true;
  return ret;
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


#include "a60-svg-color.h"			// color, colorq, colorband
#include "a60-svg-base-types.h"	  // area, style, filter, transform, typography
#include "a60-svg-constants.h"
#include "a60-svg-elements.h"
#include "a60-svg-elements-components.h"
#include "a60-svg-render-state.h"
#include "a60-svg-render-basics.h"
#include "a60-svg-composite-and-layer-basics.h"
#include "a60-svg-sequences.h"

#endif
