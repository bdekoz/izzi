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

/// Point (x,y) in 2D space.
using point_2t = std::tuple<space_type, space_type>;

/// Point (x,y) in 2D space with weight n.
using point_2tn = std::tuple<point_2t, ssize_type>;


/// Convert point_2t to string.
string
to_string(point_2t p)
{
  auto [ x, y ] = p;
  std::ostringstream oss;
  oss << x << ',' << y;
  return oss.str();
}

/// Split range, so one dimension of (x,y) cartesian plane.
using vspace = std::vector<double>;

/// Latitude and Longitude Ranges.
using srange = std::set<point_2t>;
using vrange = std::vector<point_2t>;
using vvranges = std::vector<vrange>;


/// Decompose/split 2D ranges to 1D spaces, perhaps with scaling.
void
split_vrange(const vrange& cpoints, vspace& xpoints, vspace& ypoints,
	     const double xscale = 1, const double yscale = 1)
{
 for (const auto& [x, y] : cpoints)
   {
     xpoints.push_back(x / xscale);
     ypoints.push_back(y / yscale);
   }
}


/// Union two ranges.
vrange
union_vrange(const vrange& r1, const vrange& r2)
{
  vrange vr;
  vr.insert(vr.end(), r1.begin(), r1.end());
  vr.insert(vr.end(), r2.begin(), r2.end());
  return vr;
}


/// Simplify sorted vrange by removing interior duplicates.
vrange
find_vrange_change_points(const vrange& vr)
{
  // Transform range to the simplest expression, where multiple points
  // without significant vertical change are coalesed to starting
  // control point and ending control point.
  vrange simplevr;
  point_2t last = { -1.0, -1.0 };
  double duprangep(false);
  for (const point_2t& pt : vr)
    {
      auto [ x, y] = pt;
      if (y != get<1>(last))
	{
	  if (duprangep == true)
	    {
	      simplevr.push_back(last);
	      duprangep = false;
	    }
	  simplevr.push_back(pt);
	}
      else
	duprangep = true;
      last = pt;
    }
  return simplevr;
}


/// For each dimension of vrnage, find min/max and return (xmax, ymax)
/// NB: Assumes zero is min.
point_2t
max_vrange(vspace& xpoints, vspace& ypoints, const uint pown)
{
  sort(xpoints.begin(), xpoints.end());
  sort(ypoints.begin(), ypoints.end());

  // For x axis, need to insert padding iff axes are scaled down
  // and/or have values with truncated significant digits.
  const bool padp(true);
  if (padp)
    {
      const double sigd = pow(10, pown);

      const double dx = xpoints.back();
      double ix = std::round(dx * sigd) / sigd;
      if (ix > dx)
	xpoints.push_back(ix);

      const double dy = ypoints.back();
      uint iy = std::round(dy * sigd) / sigd;
      if (iy > dy)
	ypoints.push_back(iy);
    }

  // Find combined ranges, assume zero start.
  point_2t rangemaxx = std::make_tuple(xpoints.back(), ypoints.back());
  return rangemaxx;
}


/// Just the range info, none of the temporary objects.
point_2t
max_vrange(const vrange& points, const uint pown,
	   const double xscale = 1, const double yscale = 1)
{
  vspace pointsx;
  vspace pointsy;
  split_vrange(points, pointsx, pointsy, xscale, yscale);
  point_2t ret = max_vrange(pointsx, pointsy, pown);
  return ret;
}


/// Truncate double to double with pown signifigant digits.
vspace
narrow_vspace(const vspace& points, uint pown)
{
  const double sigd = pow(10, pown);
  vspace npoints;
  for (const double& d : points)
    {
      double dn(d);
      if (dn > 0)
	{
	  uint itrunc(dn * sigd);
	  npoints.push_back(itrunc / sigd);
	}
      else
	npoints.push_back(dn);
    }
  return npoints;
}


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
constexpr char greaterthan('>');
constexpr char lessthan('<');


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
#include "a60-svg-base-types.h"	     // area, style, filter, transform, typography
#include "a60-svg-constants.h"
#include "a60-svg-elements.h"
#include "a60-svg-elements-components.h"
#include "a60-svg-render-state.h"
#include "a60-svg-render-basics.h"
#include "a60-svg-composite-and-layer-basics.h"
#include "a60-svg-sequences.h"

#endif
