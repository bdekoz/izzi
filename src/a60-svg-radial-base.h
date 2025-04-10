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


namespace svg::constants {

/// Radial rotation direction.
enum class rrotation
  {
    cw,			///< Clockwise
    ccw			///< Counter-clockwise
  };

} // namespace svg::constants

namespace svg {

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

/// Convenience for setting radial range.
point_2t
set_radial_range(const space_type rmin, const space_type rmax)
{
  point_2t& rrange = get_radial_range();
  point_2t rold = rrange;
  auto& min = std::get<0>(rrange);
  auto& max = std::get<1>(rrange);
  min = rmin;
  max = rmax;
  return rold;
}


/// Transform a value on a range to an angle on the radial range.
inline double
get_angle(ssize_type pvalue, ssize_type pmax)
{
  // Normalize [0, pmax] to range [mindeg, maxdeg] and put pvalue in it.
  const auto [ mindeg, maxdeg ] = get_radial_range();
  double d = normalize_value_on_range(pvalue, 0, pmax, mindeg, maxdeg);
  return d;
}


/// Adjust angle above to CW/CCW orientation.
inline double
adjust_angle_rotation(const double dorig, const k::rrotation rot)
{
  double d(0.0);
  if (rot == k::rrotation::cw)
    d = zero_angle_north_cw(dorig);
  if (rot == k::rrotation::ccw)
    d = zero_angle_north_ccw(dorig);
  return d;
}


/**
   Angle adjustment such that two points on the circumference path of
   radius r from origin are a minimum of @distance apart.

   NB: If it cannot be computed directly, returns a minimum adjustment
   angle that defaults to 0.25 degrees.
*/
inline double
adjust_angle_at_orbit_for_distance(double r, double dist,
				   const double minadjust = 0.25)
{
  // Angle between sequential id's.
  //
  // Given r,
  // point p on circumference r distance from origin with angle d
  // point pprime on circumference r from origin with angle dprime
  // distance is the cartesian distance between p and pprime
  //
  // then
  // distance = 2 * theta, where theta from sin(theta) = (rspace / 2) / r.
  double angle(minadjust);
  const double arg((dist / 2) / r);
  if (arg < 1.0)
    {
      double angler = std::asin(arg);
      angle = angler * (180.0 / k::pi);
    }

  // Verify and iterate, if it could not be computed above.
  const point_2t origin(0, 0);
  const point_2t p1 = get_circumference_point_d(0, r, origin);
  point_2t p2 = get_circumference_point_d(0 + angle, r, origin);
  while (distance_cartesian(p1, p2) < dist)
    {
      angle += minadjust;
      p2 = get_circumference_point_d(0 + angle, r, origin);
    }

  return angle;
}


/// The number of significant digits in @maxval.
uint
significant_digits_in(ssize_type maxval)
{
  uint sigdigits(1);
  while (maxval > 1)
    {
      maxval /= 10;
      ++sigdigits;
    }
  return sigdigits;
}


/// Get the label space.
/// Value -> Name, as a string where value has labelspaces of fill
/// NB: Should be the number of significant digits in pmax plus separators.
/// So, 10 == 2, 100 == 3, 10k == 5 + 1
ssize_type&
get_label_spaces()
{
  static ssize_type lspaces;
  return lspaces;
}


/// Set the number of label spaces.
void
set_label_spaces(ssize_type spaces)
{ get_label_spaces() = spaces; }


/// Make radial labels.
string
make_label_for_value(string pname, ssize_type pvalue,
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


/// Sort vectors of strings to largest length string first. (Or use set<>).
void
sort_strings_by_size(strings& ids)
{
  auto lsizeless = [](const string& s1, const string& s2)
		   { return s1.size() < s2.size(); };
  sort(ids.begin(), ids.end(), lsizeless);
}


/// Text with typography, arranged cw around points (x,y) on a circle.
void
radial_text_cw(svg_element& obj, string text, const typography& typo,
		const point_2t origin, const double deg)
{
  typography typot(typo);
  typot._M_anchor = svg::typography::anchor::start;
  typot._M_align = svg::typography::align::left;
  typot._M_baseline = svg::typography::baseline::central;
  styled_text_r(obj, text, origin, typot, 360 - deg);
}


/// Text with typography, arranged ccw around points (x,y) on a circle.
void
radial_text_ccw(svg_element& obj, string text, const typography& typo,
		const point_2t origin, const double deg)
{
  typography typot(typo);
  typot._M_anchor = svg::typography::anchor::end;
  typot._M_align = svg::typography::align::right;
  typot._M_baseline = svg::typography::baseline::central;
  styled_text_r(obj, text, origin, typot, 360 - deg + 180);
}


/// Text with typography, arranged cw around points (x,y) on a circle.
void
radial_text_cw(svg_element& obj, string text, const typography& typo,
		const point_2t origin, const double deg, const point_2t rorigin)
{
  typography typot(typo);
  typot._M_anchor = svg::typography::anchor::start;
  typot._M_align = svg::typography::align::left;
  typot._M_baseline = svg::typography::baseline::central;
  styled_text_r(obj, text, origin, typot, 360 - deg, rorigin);
}


/// Text with typography, arranged ccw around points (x,y) on a circle.
void
radial_text_ccw(svg_element& obj, string text, const typography& typo,
		const point_2t origin, const double deg, const point_2t rorigin)
{
  typography typot(typo);
  typot._M_anchor = svg::typography::anchor::end;
  typot._M_align = svg::typography::align::right;
  typot._M_baseline = svg::typography::baseline::central;
  styled_text_r(obj, text, origin, typot, 360 - deg + 180, rorigin);
}


/**
   Easier to read radial text that switches orientation at 180
   degrees, instead of going upside-down as it follows the circle
   around the circumference, aka mirrored or symmetric radial text.

   Text with typography, arranged around an origin of a circle with radius r.
   Text is align left CW (1,180)
   Text is aligh right CCW (181, 359)

   NB: Assume value is un-adjusted, aka from get_angle

   roriginp is a switch to use a render origin, not the text position,
   as the rotation origin.
*/
void
radial_text_r(svg_element& obj, string text, const typography& typo,
	      const int r, const point_2t origin, const double deg,
	      const bool roriginp = false)
{
  if (deg <= 180)
    {
      auto dcw = zero_angle_north_cw(deg);
      const point_2t p = get_circumference_point_d(dcw, r, origin);
      if (!roriginp)
	radial_text_cw(obj, text, typo, p, dcw);
      else
	radial_text_cw(obj, text, typo, p, dcw, origin);
    }
  else
    {
      // Mapping some values...

      // d 210 cw -> d 150 ccw
      // d 240 cw -> d 120 ccw
      // d 270 cw -> d 90 ccw
      // d 330 cw -> d 30 ccw

      // 210 - 180 == 30 -> 180 - 30 == 150
      // 240 - 180 == 60 -> 180 - 60 == 120
      // 330 - 180 == 150 -> 180 - 150 == 30

      auto dp = deg - 180;
      auto dccw = zero_angle_north_ccw(180 - dp);
      const point_2t p = get_circumference_point_d(dccw, r, origin);
      if (!roriginp)
	radial_text_ccw(obj, text, typo, p, dccw);
      else
	radial_text_ccw(obj, text, typo, p, dccw, origin);
    }
}


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

  const double angleprimed = adjust_angle_at_orbit_for_distance(r, rspace);
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
	  point_to_circle(obj, { x, y }, rstyl, rring);
	}

      radial_text_r(obj, s, typo, rprime, origin, angled2);
      angled2 += angleprimed;
    }
}


/// Spread ids past the origin point, along circumference path.
void
splay_ids_after(svg_element& obj, const typography& typo,
		const strings& ids, const double angledo,
		const point_2t origin, double r, double rspace)
{
  double angledelta = typo._M_size; // XXX
  double angled(angledo);
  for (const string& s: ids)
    {
      radial_text_r(obj, s, typo, r + rspace, origin, angled);
      angled -= angledelta;
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
  //typo._M_anchor = svg::typography::anchor::middle;
  //typo._M_align = svg::typography::align::center;

  for (const string& s: ids)
    {
      // XXX used to be angled + 90
      radial_text_r(obj, s, typo, r, origin, angled);
      r += rdelta;
    }
}


/// Concatenate ids onto one line.
void
append_ids_at(svg_element& obj, const typography& typo,
	      const strings& ids, const double angled,
	      const point_2t origin, double r)
{
  // Concatenate ids to one line.
  string scat;
  for (const string& s: ids)
    {
      if (!scat.empty())
	scat += ", ";
      scat += s;
    }
  radial_text_r(obj, scat, typo, r, origin, angled);
}

} // namespace svg

#endif
