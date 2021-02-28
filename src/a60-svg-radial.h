// svg radial, sunburst / RAIL forms -*- mode: C++ -*-

// alpha60
// bittorrent x scrape x data + analytics

// Copyright (c) 2018-2021, Benjamin De Kosnik <b.dekosnik@gmail.com>

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


#include "a60-svg-radial-base.h"
#include "a60-svg-radial-direction-arc.h"

namespace svg {

/// Hash map of unique id to (not necessarily) unique value.
/// Use this for sorting by id.
using value_type = long long;
using value_set = std::set<value_type>;


/// Hash multimap of unique value to (perhaps multiple) unique ids.
/// Use this form for sorting by value.
using id_value_umap = std::unordered_map<string, value_type>;
using value_id_ummap = std::unordered_multimap<value_type, string>;


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


/**
   Draw text on the circumference of a circle of radius r centered at (cx, cy)
   corresponding to the angle above.
*/
void
radiate_id_by_value(svg_element& obj, const point_2t origin,
		    const typography& typo, string pname,
		    size_type pvalue, size_type pmax, double r,
		    bool rotatep)
{
  auto adj = adjust_label_angle_for_text_height();
  const double angled = get_angle(pvalue, pmax) - adj;

  auto [ x, y ] = get_circumference_point_d(angled, r, origin);

  string label = make_label_for_value(pname, pvalue, get_label_spaces());
  if (rotatep)
    radial_text_r(obj, typo, label, x, y, angled);
  else
    radial_text_r(obj, typo, label, x, y, 0);
}


/*
  Create radial viz of names from input file arranged clockwise around
  the edge of a circle circumference. The text of the names can be
  rotated, or not.

 Arguments are:

 rdenom == scaling factor for radius of circle used for display, where
  larger values (used as a denominator) make smaller (tighter) circles.

 rotatep == rotate name text to be on an arc from the origin of the
 circle.

*/
svg_element
radiate_ids_per_value_on_arc(svg_element& obj, const point_2t origin,
			     const typography& typo,
			     const id_value_umap& ivm,
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


/// Map ids with one value to a point cluster radiating out from a center.
void
radiate_ids_by_uvalue(svg_element& obj, const point_2t origin,
		      const typography& typo, const strings& ids,
		      size_type pvalue, size_type pmax, double r,
		      double rspace)
{
  // Find point on the circumference of the circle closest to value
  // (pvalue).
  const double angled = get_angle(pvalue, pmax);
  double anglet = angled - adjust_label_angle_for_text_height();
  auto [ x, y ] = get_circumference_point_d(anglet, r, origin);

  // Consolidate label text to be "VALUE -> " with labelspaces spaces.
  string label = make_label_for_value("", pvalue, get_label_spaces());
  radial_text_r(obj, typo, label, x, y, anglet);

  // Next, print out the various id's on an arc with a bigger radius.
  //splay_ids_around(obj, typo, ids, angled, origin, r + rspace, rspace);
  //splay_ids_after(obj, typo, ids, angled, origin, r + rspace, rspace);
  //splay_ids_stagger(obj, typo, ids, angled, origin, r + rspace, rspace);
  //stack_ids_at(obj, typo, ids, angled, origin, r + 65, 10);

  append_ids_at(obj, typo, ids, angled, origin, r + rspace);
}


/// Radiate as above, but group similar values such that they are
/// splayed, and not written on top of each other on the same
/// arc/angle.
svg_element
radiate_ids_per_uvalue_on_arc(svg_element& obj, const point_2t origin,
			      const typography& typo, const id_value_umap& ivm,
			      const size_type value_max,
			      const int radius, const int rspace)
{
  // Make circle perimeter with an arrow to orientate display of data.
  svg::style styl(typo._M_style);
  styl._M_fill_color = svg::color::black;
  styl._M_fill_opacity = 0;
  styl._M_stroke_opacity = 1;
  styl._M_stroke_size = 3;
  insert_direction_arc_at(obj, origin, radius, styl);

  // Convert from string id-keys to int value-keys, plus an ordered
  // set of all the unique values.
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
/// Radiate clockwise from 0 to 35x degrees about origin, placing each
/// id at a point cluster on the circumference. A point cluster is a
/// circle whos radius is proportionate to the number of duplicate ids
/// at that point.  Duplicate ids splay, stack, or
/// append/concatencate at, after, or around that point cluster.
void
kusama_collision_transforms(const point_2t origin,
			    std::vector<size_type> vuvalues,
			    std::vector<point_2tn>& vpointns,
			    const size_type value_max, const int radius,
			    const int rspace, const bool outwardp = true)
{
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
      double rcur = rspace * n;

      // Fixed angle, just push point along ray from origin until no
      // conflicts.
      const double angled = get_angle(v, value_max);
      double angler = (k::pi / 180.0) * angled;

      // Points near p that are under threshold (and lower-indexed in
      // vpointns).
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
      std::clog << i << k::tab << "neighbors: " << neighbors
		<< std::endl;

      // Search backward and fixup.... in practice results in overlap
      // with lowest-index or highest-index neighbor.
      //
      // So... search forward and fixup. Not ideal; either this or
      // move to collision detection with multiple neighbor points.
      //
      // If collisions, extend radius outward from origin and try
      // again until the collision is removed and the previous
      // neighbor circles don't overlap.
      double rext = radius + rcur; // ??? XXX
      for (uint k = neighbors; k > 0; --k)
	{
	  // Get neighbor point, starting with lowest-index neighbor.
	  auto& prevpn = vpointns[i - k];
	  auto& [ prevp, prevn ] = prevpn;
	  double rprev = radius * prevn; // ??? XXX

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
}


/// 1
/// Draw these ids as a kusama circle on the circumference of origin
/// circle.
///
/// Simplest version, make satellite circle on circumfrence and splay
/// or append id's around it
void
kusama_id_by_uvalue_1(svg_element& obj, const strings& ids, const point_2t p,
		      const size_type n, const size_type n_total,
		      const size_type v, const size_type value_max,
		      const int radius, const int rspace,
		      const typography& typo, const style styl,
		      const bool byvaluep, const bool satellitep)
{
  // NB: Don't want the computed rprime radius be larger than the
  // original radius. So, take the minimum here.
  auto [ x, y ] = p;

  double rprime;
  const double rmin = rspace;
  if (byvaluep)
    {
      // Weigh by value/value_max and n/n_total.
      double rfactor = std::min(value_max, v * n);
      rprime = (rfactor / value_max) * radius;
    }
  else
    {
      // Weigh by n/n_total.
      double nradius = (n / n_total) * radius;
      if (nradius > rmin)
	rprime = nradius;
      else
	rprime = rmin;
    }
  point_2d_to_circle(obj, x, y, styl, rprime);

  // Find point aligned with this value's origin point (same arc),
  // but on the circumference of the kusama circle, not original circle.
  const double angled = get_angle(v, value_max);
  const double anglet = angled - adjust_label_angle_for_text_height();

  // Draw value and pointer to center of clustered ids.
  auto rprimex = rprime + rspace;
  const auto& plabel = get_circumference_point_d(anglet, rprimex, p);
  auto [xlabel, ylabel] = plabel;
  string label = make_label_for_value("", v, get_label_spaces());
  radial_text_r(obj, typo, label, xlabel, ylabel, anglet);

  // Draw ids.
#if 0
  append_ids_at(obj, typo, ids, anglet, p, rprimex);
#else
  splay_ids_around(obj, typo, ids, anglet, p, rprime, rspace, satellitep);
#endif
}


/// 2
/// Draw these ids as a glyph on the circumference of origin circle.
///
/// Simplest version, for gender male/female glyphs using unicode
void
kusama_id_by_uvalue_2(svg_element& obj, const strings& ids,
		      const point_2t origin, const point_2t p,
		      const size_type n, const size_type n_total,
		      const size_type v, const size_type value_max,
		      const int radius, const int rspace,
		      const typography& typo, const style styl,
		      const bool byvaluep)
{
  // Get cache, list of specialized id matches.
  const id_render_state_umap& cache = get_id_render_state_cache();

  // Center of glyph, a point on origin circle circumference.
  auto [ x, y] = p;
  const double angled = get_angle(v, value_max);
  const double anglet = angled - adjust_label_angle_for_text_height();

  // Loop through specialized list, and do these first.
  style dstyl = styl;
  strings idsremaining;
  for (const string& id : ids)
    {
      if (cache.count(id) == 1)
	{
	  // String is special, customized based on render_state.
	  const id_render_state idst = get_id_render_state(id);
	  if (idst.is_visible(svg::k::select::glyph))
	    {
	      // Kusama circle radius.
	      // Find point aligned with this value's origin point
	      // (same arc), but on the circumference of the kusama
	      // circle, not original circle.
	      double rfactor = std::min(value_max, v * n);
	      double rr = (rfactor / value_max) * radius;

	      // Switch based on id_render_state settings.
	      const string glyphtext = idst.name;
	      const double glyphscale = idst.multiple;
	      const double glyphrotate = idst.rotate;

	      if (idst.is_visible(svg::k::select::svg))
		{
		  // SVG to be inserted is
		  // - square canvas width/height of 100 pixels
		  // - glyph centered in this canvas
		  // - glyph circle diameter = glyphscale value in pixels
		  // Implies:
		  // rr * 2 is desired width of circle in glyph.
		  const double scale(rr * 2 / glyphscale);
		  const int scaledsize = 100 * scale;
		  insert_svg_at(obj, glyphtext, p, 100, scaledsize,
				angled + glyphrotate);
		}

	      if (idst.is_visible(svg::k::select::text))
		{
		  typography typog(typo);
		  typog._M_size = rr * 2 * glyphscale;
		  radial_text_r(obj, typog, glyphtext, x, y,
				      angled + glyphrotate);
		}

	      if (idst.is_visible(svg::k::select::vector))
		{
		  // Iff the only id, then use id-specialized color to
		  // draw kusama circle, and skip ring/satellite.
		  if (ids.size() == 1)
		    dstyl = idst.styl;
		  idsremaining.push_back(id);
		}
	    }
	}
      else
	{
	  idsremaining.push_back(id);
	}
    }

  // Deal with label.
  // Deal with remaining ids.
  if (idsremaining.empty())
    {
      // Just print out label.
      point_2t pt = get_circumference_point_d(anglet, radius, origin);
      auto [ xt, yt] = pt;
      radial_text_r(obj, typo, std::to_string(v), xt, yt, anglet);
    }
  else
    {
      // Do what's left (non-specialized ids) as per usual.
      const bool satellitep = ids.size() != 1;
      kusama_id_by_uvalue_1(obj, idsremaining, p, n, n_total, v, value_max,
			    radius, rspace, typo, dstyl, byvaluep, satellitep);
    }
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
			     const int rspace,
			     const bool weighbyvaluep = true,
			     const bool collisionp = false,
			     const bool arrowp = false)
{
  svg::style styl(typo._M_style);

  // Make circle perimeter with an arrow to orientate display of data.
  if (arrowp)
    {
      // Direction glyph.
      svg::style stylinset(styl);
      stylinset._M_fill_opacity = 0;
      stylinset._M_stroke_opacity = 1;
      stylinset._M_stroke_size = 3;
      insert_direction_arc_at(obj, origin, radius, stylinset);

      // Title on arc.

      // Make arc text path
      const auto [ mindeg, maxdeg ] = get_radial_range();
      auto mina = align_angle_to_north(mindeg);
      auto maxa = align_angle_to_north(maxdeg);

      point_2t pmin = get_circumference_point_d(mina, radius, origin);
      point_2t pmax = get_circumference_point_d(maxa, radius, origin);

      string titlearc = make_path_arc_circumference(pmax, pmin, radius);

      string arc_name("arc-text");
      path_element parc(false);
      path_element::data da = { titlearc, 0 };
      parc.start_element(arc_name);
      parc.add_data(da);
      parc.add_style(k::b_style);
      parc.finish_element();
      obj.add_element(parc);

      // Make label text and style it.
      string imetrictype("web vitals 2020");
      typography typo = k::apercu_typo;
      typo._M_size = 10;
      typo._M_a = typography::anchor::start;
      typo._M_align = typography::align::left;

      // Put it together.
      text_element::data dt = { 0, 0, imetrictype, typo };
      text_path_element tp(arc_name, "30%");
      tp.start_element();
      tp.add_data(dt);
      tp.finish_element();
      obj.add_element(tp);
    }

  // Convert from string id-keys to int value-keys, plus an ordered
  // set of all the unique values.
  value_set uvalues;
  value_id_ummap uvaluemm = to_value_id_mmap(ivm, uvalues);

  // Map out preliminary data points. For each unique value in vuvalues
  //
  // - VIDS
  // - construct a vector of strings that have that value, sorted
  // - short to long
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

      // Find initial point on the circumference of the circle closest
      // to current value, aka initial circumference point (ICP).
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
  if (collisionp)
    kusama_collision_transforms(origin, vuvalues, vpointns, value_max,
				radius, rspace);


  // Draw resulting points, ids, values.
  // NB: vpointns valued from smallest to largest, so reverse so that
  // smaller is more visible.
  const id_render_state& dst = get_id_render_state("");
  for (uint i = 0; i < vpointns.size(); ++i)
    {
      int j = vpointns.size() - 1 - i;
      auto& ids = vids[j];
      auto v = vuvalues[j];
      auto& pn = vpointns[j];
      auto& [ p, n ] = pn;

      // Draw this id's kusama circle on the circumference of origin
      // circle.
      kusama_id_by_uvalue_2(obj, ids, origin, p, n, vpointns.size(),
			    v, value_max, radius, rspace,
			    typo, styl, weighbyvaluep);
    }

  return obj;
}

} // namespace svg

#endif
