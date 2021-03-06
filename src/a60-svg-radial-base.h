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


// RADIAL 1
// Radiate clockwise from 0 to 35x degrees about origin, placing each
// id at a point on the circumference. Duplicate points overlap.

/// Circumference adjustment such that lable is hight-centered in
/// kusama circle.
constexpr size_type
adjust_label_angle_for_text_height(const typography& typo)
{ return typo._M_size / 4; }


/// Sort vectors of strings to largest length string first. (Or use set<>).
void
sort_strings_by_size(strings& ids)
{
  auto lsizeless = [](const string& s1, const string& s2)
		   { return s1.size() < s2.size(); };
  sort(ids.begin(), ids.end(), lsizeless);
}


// RADIAL 2
// Radiate clockwise from 0 to 35x degrees about origin, placing each
// id at a point on the circumference. Duplicate ids splay, stack,
// or append/concatencate at, after, or around that point.

/// Spread ids on either side of an origin point, along circumference
/// path.
void
splay_ids_around(svg_element& obj, const typography& typo,
		 const strings& ids, const double angled,
		 const point_2t origin, double r, double rspace,
		 const bool satellitep = false)
{
  // If rspace is set for labels, and isn't adjusted for this radius,
  // make sure it is set low.
  if (rspace > r * 2)
    rspace = r * 2;

  // XXX rspace should be disambiguated.

  // Angle between sequential id's.
  //
  // If given r, point p on circumfrence r distance from origin, angle d
  // rspace is (one or more of XXX)
  //   1. the distance between p and a label on the arc from origin
  //   2. the distance between p and pprime on circumference with angle dprime
  //
  // then
  // angle dprime = 2theta, where theta from sin(theta) = (rspace / 2) / r.
  double angleprime = std::asin((rspace / 2) / r);
  double angleprimed = angleprime * (180 / k::pi);

  const double maxdeg = angleprimed * (ids.size() - 1);
  double angled2 = angled - (maxdeg / 2);

  // NB: value label is at r + rspace, so rprime has to be longer.
  double rlabel = r + rspace;
  double rprime = rlabel + (rspace * 2);
  for (const string& s: ids)
    {
      if (satellitep)
	{
	  // Make a ring on point on the kusama circle for the id.
	  auto [ x, y ] = get_circumference_point_d(angled2, r, origin);
	  auto rring = rspace / 2;
	  const id_render_state ridst = get_id_render_state(s);
	  style rstyl = ridst.styl;
	  point_2d_to_circle(obj, x, y, rstyl, rring);
	}

      auto p2 = get_circumference_point_d(angled2, rprime, origin);
      auto [ x2, y2 ] = p2;
      radial_text_r(obj, typo, s, x2, y2, angled2);
      angled2 += angleprimed;
    }
}


/// Spread ids past the origin point, along circumference path.
void
splay_ids_after(svg_element& obj, const typography& typo,
		const strings& ids, const double angled,
		const point_2t origin, double r, double rspace)
{
  double angledelta = typo._M_size;
  double anglet = angled;
  for (const string& s: ids)
    {
      auto p = get_circumference_point_d(angled, r + rspace, origin);
      auto [ x, y ] =  p;
      radial_text_r(obj, typo, s, x, y, angled);
      anglet -= angledelta;
    }
}


/// Spread ids after in stepping pattern outward.
void
splay_ids_stagger(svg_element& obj, const typography& typo,
		  const strings& ids, const double angled,
		  const point_2t origin, double r, double rspace)
{
  if (ids.size() > 1)
    {
      auto imiddle = ids.begin() + (ids.size() / 2);
      strings ids1(ids.begin(), imiddle);
      strings ids2(imiddle, ids.end());
      splay_ids_after(obj, typo, ids1, angled, origin, r, rspace);
      splay_ids_after(obj, typo, ids2, angled, origin, r + 125, rspace);
    }
  else
    splay_ids_after(obj, typo, ids, angled, origin, r, rspace);
}


/// Rotate and stack ids at origin point, extending radius for each
/// from point of origin.
void
stack_ids_at(svg_element& obj, const typography& typoo,
	     const strings& ids, const double angled,
	     const point_2t origin, double r, const double rdelta = 10)
{
  // Rotate 90 CW around origin, and spread out .
  typography typo(typoo);
  //typo._M_a = svg::typography::anchor::middle;
  //typo._M_align = svg::typography::align::center;

  for (const string& s: ids)
    {
      auto [ x2, y2 ] = get_circumference_point_d(angled, r, origin);
      radial_text_r(obj, typo, s, x2, y2, angled + 90);
      r += rdelta;
    }
}


/// Concatenate ids onto one line.
void
append_ids_at(svg_element& obj, const typography& typo,
	      const strings& ids, const double angled,
	      const point_2t origin, double r)
{
  // Get point, angle up for text.
  auto [ x, y ] = get_circumference_point_d(angled, r, origin);

  // Concatenate ids to one line.
  string scat;
  for (const string& s: ids)
    {
      if (!scat.empty())
	scat += ", ";
      scat += s;
    }
  radial_text_r(obj, typo, scat, x, y, angled);
}

} // namespace svg

#endif
