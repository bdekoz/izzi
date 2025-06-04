// svg grid and matrix placement -*- mode: C++ -*-

// Copyright (c) 2021,2024 Benjamin De Kosnik <b.dekosnik@gmail.com>

// This file is part of the izzi library.  This library is free
// software; you can redistribute it and/or modify it under the terms
// of the GNU General Public License as published by the Free Software
// Foundation; either version 3, or (at your option) any later
// version.

// This library is distributed in the hope that it will be useful, but
// WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// General Public License for more details.

#ifndef MiL_SVG_GRID_MATRIX_SYSTEMS_H
#define MiL_SVG_GRID_MATRIX_SYSTEMS_H 1

namespace svg {

/// Maximum number of individual items to be placed in grid cells.
/// A 1x4 grid can contain up to 4 items.
/// A 2x4 grid can contain up to 8 items.
/// A 3x3 grid can contain up to 9 items.
constexpr uint matrix_max_items = 8;


/// Grid style mode.
enum class matrix_variant { centerweight, centerweightsvg, veronica, slim };


/// Cell size 3x3 for n items with margin spacing on either size.
ulong
cell_size_3x3_matrix(const area<> a, const uint n, const double margin)
{
  const auto [width, height] = a;
  const auto minsz = std::min(width, height);
  const double minszadj = minsz - (2 * margin);
  const ulong delta = minszadj / ((n / 3) + 1);
  return delta;
}

/**
   Move around a three by three grid of glyphs

   1  2  3
   4  5  6
   7  8  9

   given

   1. a center point located at iteration 5,
   2. cell width,
   3. and iteration or cell number [0,8]

   The centerweight matrix_variant has cells arranged as above.
   The other variant styles, distort the matrix as specified below.
*/
point_2t
to_point_in_3x3_matrix(const area<> a, const uint i, const double margin,
		       const matrix_variant vnt = matrix_variant::slim)
{
  using std::make_tuple;

  // v0 Centerweight grid layout.
  std::array<point_2t, 9> kernel;
  if (vnt == matrix_variant::centerweight)
    {
      constexpr std::array<point_2t, 9> kernelcw
	{
	  make_tuple(-1, 1), make_tuple(0, 1), make_tuple(1, 1),
	  make_tuple(-1, 0), make_tuple(0, 0), make_tuple(1, 0),
	  make_tuple(-1, -1), make_tuple(0, -1), make_tuple(1, -1)
	};
      kernel = kernelcw;
    }

  // v3 Centerweight grid layout, svg coordinate system.
  if (vnt == matrix_variant::centerweightsvg)
    {
      constexpr std::array<point_2t, 9> kernelcwsvg
	{
	  make_tuple(-1, -1), make_tuple(0, -1), make_tuple(1, -1),
	  make_tuple(-1, 0), make_tuple(0, 0), make_tuple(1, 0),
	  make_tuple(-1, 1), make_tuple(0, 1), make_tuple(1, 1)
	};
      kernel = kernelcwsvg;
    }

  // v1 Veronica layout 1.
  if (vnt == matrix_variant::veronica)
    {
      constexpr std::array<point_2t, 9> kernelver
	{
	  make_tuple(-1, 0), make_tuple(0, 0), make_tuple(1, 0),
	  make_tuple(-1, 1), make_tuple(0, -1), make_tuple(0, 0),
	  make_tuple(-1, -1), make_tuple(0, 1), make_tuple(1, 0)
	};
      kernel = kernelver;
    }

  // v2 Very Slim layout 2.
  if (vnt == matrix_variant::slim)
    {
      constexpr std::array<point_2t, 9> kernelslim
	{
	  make_tuple(-1, -1), make_tuple(0, -1), make_tuple(1, -1),
	  make_tuple(0, 0), make_tuple(1, 0), make_tuple(-1, 0),
	  make_tuple(-1, 1), make_tuple(0, 1), make_tuple(1, 1)
	};
      kernel = kernelslim;
    };


  // Sanity check iteration count.
  if (i > 8)
    {
      string m("to_point_in_3x3_matrix: ");
      m += "error iteration count too big for matrix, i is: ";
      m += std::to_string(i);
      m += k::newline;
      throw std::runtime_error(m);
    }

  const auto delta = cell_size_3x3_matrix(a, matrix_max_items, margin);
  const point_2t origin = a.center_point();
  auto [ x, y ] = origin;
  auto [ xm, ym ] = kernel[i];
  auto xnew = x + (xm * delta);
  auto ynew = y + (ym * delta);
  return make_tuple(xnew, ynew);
}


/**
   Move around a 2 by 4 grid of glyphs

   1  2  3  4
   5  6  7  8

   given

   1. a center point located equidistant to 2, 3, 6, 7,
   2. cell width,
   3. and iteration or cell number [0,7]
*/
point_2t
to_point_in_2x4_matrix(const area<> a, const uint i, const double margin)
{
  const point_2t origin = a.center_point();

  const auto [width, height] = a;
  const double widthadj = width - (2 * margin);
  const ulong xdelta = widthadj / ((matrix_max_items / 2) + 1);

  const double heightadj = height - (2 * margin);
  const ulong ydelta = heightadj / 2;

  using p2d = point_2t;
  static std::array<point_2t, 8> kernel =
    {
      // Modified centerweight grid layout.
      p2d(-1.5, -0.5), p2d(-0.5, -0.5), p2d(0.5, -0.5), p2d(1.5, -0.5),
      p2d(-1.5, 0.5), p2d(-0.5, 0.5), p2d(0.5, 0.5), p2d(1.5, 0.5)
    };

  // Sanity check iteration count.
  if (i > 7)
    {
      string m("to_point_in_2x4_matrix: ");
      m += "error iteration count too big for matrix, i is: ";
      m += std::to_string(i);
      m += k::newline;
      throw std::runtime_error(m);
    }

  auto [ x, y ] = origin;
  auto [ xm, ym ] = kernel[i];
  auto xnew = x + (xm * xdelta);
  auto ynew = y + (ym * ydelta);
  return std::make_tuple(xnew, ynew);
}


/// For positioning a linear list of glyphs along a horizontal line.
/// Assuming i is from find_id_index with tag, so is an offset starting at zero.
point_2t
to_point_in_1x8_matrix(const area<> a, const uint i, const double margin)
{
  using std::make_tuple;

  const auto [width, height] = a;
  const auto widthadj = width - (2 * margin);
  const ulong xdelta = widthadj / (matrix_max_items + 1);

  const uint xoff = margin + xdelta + (i * xdelta);
  const uint yoff = height / 2;

  return make_tuple(xoff, yoff);
}


/// For positioning a linear list of glyphs along a horizontal line.
/// @param n is number of matrix cells, greater than one.
/// @param i is from find_id_index with tag, so is an offset starting at zero.
/// @param centeredp adjusts to the center of the cell, default is left aligned.
/// @param y is what y axis to use.
point_2t
to_point_in_1xn_matrix(const area<> a, const uint n, const uint i,
		       const double margin, const double y,
		       const bool centeredp = false)
{
  using std::make_tuple;

  const auto [width, height] = a;
  const auto widthadj = width - (2 * margin);
  const double xdelta = widthadj / n;

  double xoff = margin + (i * xdelta);

  if (centeredp)
    xoff += xdelta / 2;

  return make_tuple(xoff, y);
}


/// Default matrix position.
point_2t
to_point_in_matrix(const area<> a, const uint i, const double margin)
{ return to_point_in_3x3_matrix(a, i, margin); }


} // namespace svg


#endif
