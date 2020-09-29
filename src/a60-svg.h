// svg API -*- mode: C++ -*-

// Copyright (C) 2019-2020 Benjamin De Kosnik <b.dekosnik@gmail.com>

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
#include <string>
#include <vector>
#include <tuple>
#include <sstream>

/**
 *  Scalable Vector Graphics (SVG) namespace
 */
namespace svg {

/// Base string type.
using std::string;
using strings = std::vector<string>;

/// Base integer type: positive and negative, signed integral value.
using size_type = int;

/// Base floating point type.
using space_type = double;
using point_2t = std::tuple<space_type, space_type>;
using point_2tn = std::tuple<point_2t, size_type>;


string
to_string(point_2t p)
{
  auto [ x, y ] = p;
  std::ostringstream oss;
  oss << x << ',' << y;
  return oss.str();
}


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


// Does point p1 of radius r1 instersect point p2 with radius r2?
// https://developer.mozilla.org x 2D_collision_detection
bool
is_collision_detected(const point_2t& p1, const int r1,
		      const point_2t& p2, const int r2)
{
  bool ret(false);
  if (distance_cartesian(p1, p2) < r1 + r2)
    ret = true;
  return ret;
}



/**
   SVG Constants
*/
namespace constants {

  // Formatting constants.
  constexpr char space(' ');
  constexpr char quote('"');
  constexpr char hyphen('-');
  constexpr char tab('\t');
  constexpr char newline('\n');
  constexpr char comma(',');

  // Numeric constants.
  // π = double(22)/double(7);
  // π = 3.14159265358979323846
  constexpr double pi(3.14159265358979323846);

  uint&
  get_dpi()
  {
    static uint dpi(96);
    return dpi;
  }
}

/// Inject constants with alias k.
namespace k = constants;

} // namespace svg


#include "a60-svg-color.h"			// color, colorq, colorband
#include "a60-svg-base.h"	  // area, style, filter, transform, typography
#include "a60-svg-constants.h"
#include "a60-svg-elements.h"
#include "a60-svg-elements-components.h"
#include "a60-svg-render-state.h"
#include "a60-svg-render-basics.h"

#endif
