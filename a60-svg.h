// svg API -*- mode: C++ -*-

// Copyright (C) 2019 Benjamin De Kosnik <b.dekosnik@gmail.com>

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

#include <string>
#include <vector>
#include <tuple>
#include <sstream>

/**
 *  Scalable Vector Graphics (SVG) namespace
 */
namespace svg {

  //  Using size_type = std::size_t;
  using size_type = int;

  using std::string;
  using strings = std::vector<string>;

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

} // namespace svg


#include "a60-svg-color.h"			// colore, colorq, colorband
#include "a60-svg-base.h"	  // area, style, filter, transform, typography
#include "a60-svg-constants.h"
#include "a60-svg-elements.h"
#include "a60-svg-elements-components.h"
#include "a60-svg-render-basics.h"

#endif
