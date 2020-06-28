// svg radial, sunburst / RAIL forms -*- mode: C++ -*-

// alpha60
// bittorrent x scrape x data + analytics

// Copyright (c) 2018-2020, Benjamin De Kosnik <b.dekosnik@gmail.com>

// This file is part of the alpha60 library.  This library is free
// software; you can redistribute it and/or modify it under the terms
// of the GNU General Public License as published by the Free Software
// Foundation; either version 3, or (at your option) any later
// version.

// This library is distributed in the hope that it will be useful, but
// WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// General Public License for more details.

#ifndef MiL_SVG_RENDER_RADIAL_H
#define MiL_SVG_RENDER_RADIAL_H 1

#include <cmath>
#include <chrono>
#include <iostream>
#include <iomanip>
#include <algorithm>

#include "a60-svg.h"


namespace svg {

// Or use set with lt.
void
sort_strings_by_size(strings& ids)
{
  auto lsizeless = [](const string& s1, const string& s2)
		   { return s1.size() < s2.size(); };
  sort(ids.begin(), ids.end(), lsizeless);
}


/// Hash map of unique id to (not necessarily) unique value.
/// Use this for sorting by id.
using value_type = long long;
using id_value_umap = std::unordered_map<string, value_type>;

/// Hash multimap of unique value to (perhaps multiple) unique ids.
/// Use this form for sorting by value.
using value_id_ummap = std::unordered_multimap<value_type, string>;
using value_set = std::set<value_type>;

/// Remove all from map that match the input (matches) strings.
/// Return found match entries.
id_value_umap
remove_matches_id_value_map(id_value_umap& ivm, const strings& matches)
{
  id_value_umap foundmap;
  for (const auto& key: matches)
    {
      id_value_umap::iterator iter = ivm.find(key);
      if (iter != ivm.end())
	{
	  // Insert found element into return map....
	  foundmap.insert(*iter);

	  // Remove found elment from originating map (ivm)
	  ivm.erase(iter);
	}
    }
  return foundmap;
}


/// Convert id_value_umap to value_id_mmap + set of unique values.
value_id_ummap
to_value_id_mmap(const id_value_umap& ivm, value_set& uniquev)
{
  value_id_ummap vimm;
  for (auto& e: ivm)
    {
      string s = e.first;
      size_type i = e.second;
      vimm.insert(make_pair(i, s));
      uniquev.insert(i);
    }
  return vimm;
}


double
normalize_value_on_range(const size_type value, const size_type min,
			 const size_type max,
			 const size_type nfloor, const size_type nceil)
{
  double rmultp(nceil - nfloor);
  double valnum(value - min);
  double valdenom(max - min);
  double weightn = (rmultp * (valnum / valdenom)) + nfloor;
  return weightn;
}


double
align_angle_to_glyph(double angled)
{
 // Change rotation to CW instead of CCW (or anti-clockwise).
  angled = 360 - angled;

  // Rotate 90 CCW, so that the first element will be at the top
  // vertical axis, instead of the right middle axis.
  angled += 90;

  return angled;
}


// Given rdenom scaling factor and SVG canvas, compute effective radius value.
inline double
get_radius(const svg_element& obj, const uint rdenom)
{ return std::min(obj._M_area._M_height, obj._M_area._M_width) / rdenom; }


// Max number of non-overlapping degrees in circle, such that the
// beginning and the end have a discernable gap. Total degrees in a
// circle are 360, but then the beginning and the end of the radial
// display are in the same place. So, shave a bit off both for the optics.
constexpr double mindeg = 10;
constexpr double maxdeg = 350;


inline double
get_angle(size_type pvalue, size_type pmax)
{
  // Normalize [0, pmax] to range [mindeg, maxdeg] and put pvalue in it.
  double angled = normalize_value_on_range(pvalue, 0, pmax, mindeg, maxdeg);
  return align_angle_to_glyph(angled);
}


/// Angle in radians.
point_2t
get_circumference_point(const double angler, const double r,
			const point_2t origin)
{
  auto [ cx, cy ] = origin;
  double x(cx + (r * std::cos(angler)));
  double y(cy - (r * std::sin(angler)));
  return std::make_tuple(x, y);
}


/// Angle in degrees.
point_2t
get_circumference_point_d(const double ad, const double r,
			  const point_2t origin)
{
  double angler = (k::pi / 180.0) * ad;
  return get_circumference_point(angler, r, origin);
}


void
place_text_at_angle(svg_element& obj, const typography& typo, string label,
		    int tx, int ty, const double deg = 0.0)
{
  text_element::data dt = { tx, ty, label, typo };
  text_element t;
  t.start_element();

  // IFF degrees, then rotate text.
  if (deg > 0)
    t.add_data(dt, svg::transform::rotate(360 - deg, tx, ty));
  else
    t.add_data(dt);

  t.finish_element();
  obj.add_element(t);
}


/// Insert arc + arrow glyph that traces path of start to finish trajectory.
svg_element
insert_direction_arc_at(svg_element& obj, const point_2t origin,
			const double rr, svg::style s, const uint spacer = 10)
{
  const double r = rr - spacer;
  point_2t p0 = get_circumference_point_d(align_angle_to_glyph(mindeg),
					  r, origin);
  point_2t p4 = get_circumference_point_d(align_angle_to_glyph(maxdeg),
					  r, origin);

  // Define arc.
  std::ostringstream ossa;
  ossa << "M" << k::space << to_string(p0) << k::space;
  ossa << "A" << k::space;
  ossa << std::to_string(r) << k::comma << std::to_string(r) << k::space;
  ossa << align_angle_to_glyph(1) << k::space;
  ossa << "1, 1" << k::space;
  ossa << to_string(p4) << k::space;

  // Adjust style so the stroke color matches the fill color.
  s._M_stroke_color = s._M_fill_color;

  auto rspacer = spacer - 2;
  auto anglemax = align_angle_to_glyph(maxdeg);
  point_2t p5 = get_circumference_point_d(anglemax, r + rspacer, origin);
  point_2t p7 = get_circumference_point_d(anglemax, r - rspacer, origin);

  // Circumference arc length desired is radius times the angle of the arc.
  auto theta = 2 * rspacer / r;
  auto thetad = theta * 180 / k::pi;
  point_2t p6 = get_circumference_point_d(align_angle_to_glyph(maxdeg + thetad),
					  r, origin);

  // Define marker.
  std::ostringstream ossm;
  ossm << "M" << k::space << to_string(p4) << k::space;
  ossm << "L" << k::space;
  ossm << to_string(p5) << k::space;
  ossm << to_string(p6) << k::space;
  ossm << to_string(p7) << k::space;
  ossm << to_string(p4) << k::space;

  // Adjust style so that fill will be shown, and stroke hidden.
  s._M_fill_opacity = 1;
  s._M_stroke_opacity = 0;

  // End marker path.
  path_element pmarker;
  path_element::data dm = { ossm.str(), 0 };
  pmarker.start_element();
  pmarker.add_data(dm);
  pmarker.add_style(s);
  pmarker.finish_element();
  obj.add_element(pmarker);

  // Reset style.
  s._M_fill_opacity = 0;
  s._M_stroke_opacity = 1;

  // Arc path.
  path_element parc;
  path_element::data da = { ossa.str(), 0 };
  parc.start_element();
  parc.add_data(da);
  parc.add_style(s);
  parc.finish_element();
  obj.add_element(parc);

  return obj;
}


// RADIAL 1
// Radiate clockwise from 0 to 35x degrees about origin, placing each
// id at a point on the circumference. Duplicate points overlap.


string
make_label_for_value(string pname, size_type pvalue, const uint valuewidth = 9)
{
  // Consolidate label text to be "VALUE -> NAME"
  std::ostringstream oss;
  oss.imbue(std::locale(""));
  oss << std::setfill(' ') << std::setw(valuewidth) << std::left << pvalue;
  string label = oss.str() + " -> " + pname;
  return label;
}


/*
  Draw text on the circumference of a circle of radius r centered at (cx, cy)
  corresponding to the angle above.
*/
void
radiate_id_by_value(svg_element& obj, const point_2t origin,
		    const typography& typo, string pname,
		    size_type pvalue, size_type pmax, double r, bool rotatep)
{
  const double angled = get_angle(pvalue, pmax);
  double angler = (k::pi / 180.0) * angled;
  auto [ x, y ] = get_circumference_point(angler, r, origin);

  string label = make_label_for_value(pname, pvalue, 2);
  if (rotatep)
    place_text_at_angle(obj, typo, label, x, y, angled);
  else
    place_text_at_angle(obj, typo, label, x, y, 0);
}


/*
  Create radial viz of names from input file arranged clockwise around
  the edge of a circle circumference. The text of the names can be
  rotated, or not.

 Arguments are:

 rdenom == scaling factor for radius of circle used for display, where
  larger values (used as a denominator) make smaller (tighter) circles.

 rotatep == rotate name text to be on an arc from the origin of the circle.

*/
svg_element
radiate_ids_per_value_on_arc(svg_element& obj, const point_2t origin,
			     const typography& typo, const id_value_umap& ivm,
			     const size_type value_max,
			     const int rdenom, bool rotatep)
{
  // Probe/Marker display.
  // Loop through map key/values and put on canvas.
  const double r = get_radius(obj, rdenom);
  for (const auto& v : ivm)
    {
      string pname(v.first);
      size_type pvalue(v.second);
      if (pvalue)
	radiate_id_by_value(obj, origin, typo, pname, pvalue, value_max,
			    r, rotatep);
    }

  return obj;
}


// RADIAL 2
// Radiate clockwise from 0 to 35x degrees about origin, placing each
// id at a point on the circumference. Duplicate ids splay, stack,
// or append/concatencate at, after, or around that point.

// Spread ids on either side of an origin point, along circumference path.
void
splay_ids_around(svg_element& obj, const typography& typo, const strings& ids,
		 const double angled, const point_2t origin, double r,
		 double rspace)
{
  const double maxdeg = rspace * (ids.size() - 1);
  double angled2 = angled - maxdeg / 2;
  for (const string& s: ids)
    {
      double angler2 = (k::pi / 180.0) * angled2;
      auto [ x2, y2 ] = get_circumference_point(angler2, r, origin);
      place_text_at_angle(obj, typo, s, x2, y2, angled2);
      angled2 += rspace;
    }
}


// Spread ids past the origin point, along circumference path.
void
splay_ids_after(svg_element& obj, const typography& typo, const strings& ids,
		const double angled, const point_2t origin, double r,
		double rspace)
{
  double angledt(angled);
  for (const string& s: ids)
    {
      double angler = (k::pi / 180.0) * angledt;
      auto [ x, y ] = get_circumference_point(angler, r, origin);
      place_text_at_angle(obj, typo, s, x, y, angledt);
      angledt -= rspace;
    }
}


void
splay_ids_stagger(svg_element& obj, const typography& typo, const strings& ids,
		  const double angled, const point_2t origin, double r,
		  double rspace)
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


// Rotate and stack ids at origin point, extending radius for each
// from point of origin.
void
stack_ids_at(svg_element& obj, const typography& typoo, const strings& ids,
	     const double angled, const point_2t origin, double r,
	     const double rincrement = 10)
{
  // Rotate 90 CW around origin, and spread out .
  typography typo(typoo);
  typo._M_a = svg::typography::anchor::middle;
  typo._M_align = svg::typography::align::center;

  const double angler = (k::pi / 180.0) * angled;
  for (const string& s: ids)
    {
      auto [ x2, y2 ] = get_circumference_point(angler, r, origin);
      place_text_at_angle(obj, typo, s, x2, y2, angled + 90);
      r += rincrement;
    }
}


void
append_ids_at(svg_element& obj, const typography& typo, const strings& ids,
	      const double angled, const point_2t origin, double r)
{
  // Get point, angle up for text.
  double angler = (k::pi / 180.0) * angled;
  auto [ x, y ] = get_circumference_point(angler, r, origin);

  // Concatenate ids to one line.
  string scat;
  for (const string& s: ids)
    {
      if (!scat.empty())
	scat += ", ";
      scat += s;
    }
  place_text_at_angle(obj, typo, scat, x, y, angled);
}


// Map ids with one value to a point cluster radiating out from a center.
void
radiate_ids_by_uvalue(svg_element& obj, const point_2t origin,
		      const typography& typo, const strings& ids,
		      size_type pvalue, size_type pmax, double r,
		      double rspace [[maybe_unused]])
{
  // Find point on the circumference of the circle closest to value (pvalue).
  const double angled = get_angle(pvalue, pmax);
  double angler = (k::pi / 180.0) * angled;
  auto [ x, y ] = get_circumference_point(angler, r, origin);

  // Consolidate label text to be "VALUE -> " with valuewidth spaces.
  // NB: this should be the number of significant digits in pmax.
  // So, 100 == 2, 10k == 5
  string label = make_label_for_value("", pvalue, 2);
  place_text_at_angle(obj, typo, label, x, y, angled);

  // Next, print out the various id's on an arc with a bigger radius.
  //splay_ids_around(obj, typo, ids, angled, origin, r + rspace, rspace);
  //splay_ids_after(obj, typo, ids, angled, origin, r + rspace, rspace);
  //splay_ids_stagger(obj, typo, ids, angled, origin, r + rspace, rspace);
  //stack_ids_at(obj, typo, ids, angled, origin, r + 65, 10);

  append_ids_at(obj, typo, ids, angled, origin, r + rspace);
}


// Radiate as above, but group similar values such that they are
// splayed, and not written on top of each other on the same
// arc/angle.
svg_element
radiate_ids_per_uvalue_on_arc(svg_element& obj, const point_2t origin,
			      const typography& typo, const id_value_umap& ivm,
			      const size_type value_max,
			      const int radius, const int rspace)
{
  // Make circle perimeter with an arrow to orientate display of data.
  svg::style styl(typo._M_style);
  styl._M_fill_color = svg::colore::black;
  styl._M_fill_opacity = 0;
  styl._M_stroke_opacity = 1;
  styl._M_stroke_size = 3;
  insert_direction_arc_at(obj, origin, radius, styl);

  // Convert from string id-keys to int value-keys, plus an ordered set of all
  // the unique values.
  value_set uvalues;
  value_id_ummap uvaluemm = to_value_id_mmap(ivm, uvalues);
  for (const auto& v : uvalues)
    {
      // Extract all the ids for a given value.
      auto irange = uvaluemm.equal_range(v);
      auto ibegin = irange.first;
      auto iend = irange.second;
      strings ids;
      for (auto i = ibegin; i != iend; ++i)
	ids.push_back(i->second);
      sort_strings_by_size(ids);

      if (v)
	radiate_ids_by_uvalue(obj, origin, typo, ids, v, value_max,
			      radius, rspace);
    }
  return obj;
}


// RADIAL 3
// Radiate clockwise from 0 to 35x degrees about origin, placing each
// id at a point cluster on the circumference. A point cluster is a
// circle whos radius is proportionate to the number of duplicate ids
// at that point.  Duplicate ids splay, stack, or
// append/concatencate at, after, or around that point cluster.

double
distance_cartesian(const point_2t& p1, const point_2t& p2)
{
  auto [ x1, y1 ] = p1;
  auto [ x2, y2 ] = p2;
  auto distancex = (x2 - x1) * (x2 - x1);
  auto distancey = (y2 - y1) * (y2 - y1);
  double distance = sqrt(distancex + distancey);
  return distance;
}


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


// Rotate and stack ids at origin point, extending radius for each
// from point of origin.
void
kusama_ids_at_point(svg_element& obj, const style& styl, const strings& ids,
		    const point_2t p, double r)
{
  string scat;
  for (const string& s: ids)
    {
      if (!scat.empty())
	scat += ", ";
      scat += s;
    }

  auto [ x, y] = p;
  point_2d_to_circle(obj, x, y, styl, r);
}


/**
   Radiate as above *_per_uvalue_on_arc function, but group similar
   values such that they are globbed into a sattelite circle, ids
   splayed around the satellite, and not written on top of each other
   on the same arc/angle.

   Framing circle radius is the result of dividing page dimensions by (rdenom).

   Satellite circle radius is the product of the number of ids with
   the same value times a base multipler (rbase).

   When overlap is detected, move outward on radius if true, otherwise
   move in.
*/
svg_element
kusama_ids_per_uvalue_on_arc(svg_element& obj, const point_2t origin,
			     const typography& typo, const id_value_umap& ivm,
			     const size_type value_max, const int radius,
			     const int rspace)
{
  // Make circle perimeter with an arrow to orientate display of data.
  svg::style styl(typo._M_style);
  styl._M_fill_color = svg::colore::black;
  styl._M_fill_opacity = 0;
  styl._M_stroke_opacity = 1;
  styl._M_stroke_size = 3;
  // insert_direction_arc_at(obj, origin, radius, styl);

  // Convert from string id-keys to int value-keys, plus an ordered set of all
  // the unique values.
  value_set uvalues;
  value_id_ummap uvaluemm = to_value_id_mmap(ivm, uvalues);

  // Map out preliminary data points.
  // For each unique value in vuvalues
  // - VIDS
  // - construct a vector of strings that have that value, sorted short to long
  //
  // - VPOINTNS
  // - constuct a vector of points on the circumference with weight
  std::vector<size_type> vuvalues(uvalues.begin(), uvalues.end());
  std::vector<strings> vids;
  std::vector<point_2tn> vpointns;
  for (const auto& v : vuvalues)
    {
      auto count = uvaluemm.count(v);
      std::clog << "value, count: " << v << ',' << count << std::endl;

      // Extract all the ids for a given value.
      auto irange = uvaluemm.equal_range(v);
      auto ibegin = irange.first;
      auto iend = irange.second;
      strings ids;
      for (auto i = ibegin; i != iend; ++i)
	ids.push_back(i->second);
      sort_strings_by_size(ids);
      vids.push_back(ids);

      // Find initial point on the circumference of the circle closest to
      // current value, aka initial circumference point (ICP).
      const double angled = get_angle(v, value_max);
      point_2t p = get_circumference_point_d(angled, radius, origin);
      vpointns.push_back(std::make_tuple(p, ids.size()));
    }

  // Remove zero value ids.
  if (vuvalues.front() == 0)
    {
      auto& ids = vids[0];
      std::clog << "eliding ids with value zero: " << std::endl;
      for (const auto& s: ids)
	std::clog << s << std::endl;

      vuvalues.erase(vuvalues.begin());
      vids.erase(vids.begin());
      vpointns.erase(vpointns.begin());
    }


  // Further normalization and collision-avoidance.
#if 0
  // Threshold is the range such that a value is considered adjacent
  // for collisions. If v > previous value + threshold, then the
  // points are not considered neighbors.
  const int threshold(200);

  // Massage data to fit without overlaps, given:
  // 1 unique set of values
  // 2 for each unique value, the ids that match
  // 3 for each unique value, the proposed point and size of the circle
  for (uint i = 0; i < vpointns.size(); ++i)
    {
      auto v = vuvalues[i];
      auto& pn = vpointns[i];
      auto& [ p, n ] = pn;
      double rcur = rad* n;

      // Fixed angle, just push point along ray from origin until no conflicts.
      const double angled = get_angle(v, value_max);
      double angler = (k::pi / 180.0) * angled;

      // Points near p that are under threshold (and lower-indexed in vpointns).
      uint neighbors(0);

      // Find neighbors.
      uint j(i);
      while (i > 0 && j > 0)
	{
	  j -= 1;

	  // Only if within threshold, know values are ordered.
	  if (vuvalues[i] - vuvalues[j] < threshold)
	    ++neighbors;
	  else
	    break;
	}
      std::clog << i << k::tab << "neighbors: " << neighbors << std::endl;

      // Search backward and fixup.... in practice results in overlap with
      // lowest-index or highest-index neighbor.
      //
      // So... search forward and fixup. Not ideal; either this or
      // move to collision detection with multiple neighbor points.
      //
      // If collisions, extend radius outward from origin and try
      // again until the collision is removed and the previous
      // neighbor circles don't overlap.
      double rext = r + rcur;
      for (uint k = neighbors; k > 0; --k)
	{
	  // Get neighbor point, starting with lowest-index neighbor.
	  auto& prevpn = vpointns[i - k];
	  auto& [ prevp, prevn ] = prevpn;
	  double rprev = rbase * prevn;

	  while (is_collision_detected(p, rcur, prevp, rprev))
	    {
	      // Find new point further out from origin.
	      if (outwardp)
		rext += (2.5 * rcur);
	      else
		rext -= (2.5 * rcur);
	      p = get_circumference_point(angler, rext, origin);
	    }
	}
    }
#endif

  // Draw resulting points, ids, values.
  for (uint i = 0; i < vpointns.size(); ++i)
    {
      auto v = vuvalues[i];
      auto& pn = vpointns[i];
      auto& [ p, n ] = pn;

      // Draw this id's kusama circle on the circumference of origin circle.

      // NB: Don't want this radius be larger than the original
      // radius, but some dimensions (sexuality) have multiple id's
      // with the max (male, cis). So, take the minimum here.
      double rfactor = std::min(value_max, v * n);
      double rr = (rfactor / value_max) * radius;
      kusama_ids_at_point(obj, styl, vids[i], p, rr);

      // Find point aligned with this value's origin point (same arc),
      // but on the circumference of the kusama circle, not original circle.
      const double angled = get_angle(v, value_max);

      // Draw value and pointer to center of clustered ids.
      auto rspacex = rr + rspace;
      const auto& plabel = get_circumference_point_d(angled, rspacex, p);
      auto [xlabel, ylabel] = plabel;
      string label = make_label_for_value("", v, 2);
      place_text_at_angle(obj, typo, label, xlabel, ylabel, angled);

      // Draw ids.
      rspacex += (3 * rspace);
      // append_ids_at(obj, typo, vids[i], angled, p, rspacex);
      splay_ids_around(obj, typo, vids[i], angled, p, rspacex, rspace);
    }

  return obj;
}

} // namespace svg

#endif
