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

#ifndef MiL_SVG_RADIAL_KUSAMA_H
#define MiL_SVG_RADIAL_KUSAMA_H 1


namespace svg {

/**
   Radiate clockwise from 0 to 35x degrees about origin, placing each
   id at a point cluster on the circumference. A point cluster is a
   circle whos radius is proportionate to the number of duplicate ids
   at that point.  Duplicate ids splay, stack, or
   append/concatencate at, after, or around that point cluster.
*/
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
      const double angledo = get_angle(v, value_max);
      const double angled = adjust_angle_rotation(angledo, k::rrotation::cw);
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


/**
   Draw these ids as a kusama circle on the circumference of origin
   circle.

   Simplest version, make satellite circle on circumfrence and splay
   or append id's around it
*/
void
kusama_ids_orbit_high(svg_element& obj, const strings& ids, const point_2t p,
		      const size_type n, const size_type n_total,
		      const size_type v, const size_type value_max,
		      const int radius, const int rspace,
		      const typography& typo,
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

  // Iff the only id, then use id-specialized color to draw kusama
  // circle, and skip ring/satellite.
  style styl = get_id_render_state("").styl;
  if (ids.size() == 1)
    styl = get_id_render_state(ids.front()).styl;

  point_2d_to_circle(obj, x, y, styl, rprime);

  // Find point aligned with this value's origin point (same arc),
  // but on the circumference of the kusama circle, not original circle.
  const double angled = get_angle(v, value_max);

  // Draw value and pointer to center of clustered ids.
  auto rprimex = rprime + rspace;
  string label = make_label_for_value("", v, get_label_spaces());
  radial_text_r(obj, typo, label, rprimex, p, angled);

  // Draw ids.
#if 0
  append_ids_at(obj, typo, ids, angled, p, rprimex);
#else
  splay_ids_around(obj, typo, ids, angled, p, rprime, rspace, satellitep);
#endif
}


/**
   Draw these ids as a glyph on the circumference of origin circle.

   Simplest version.
*/
void
kusama_ids_orbit_low(svg_element& obj, const strings& ids,
		     const point_2t origin, const point_2t p,
		     const size_type n, const size_type n_total,
		     const size_type v, const size_type value_max,
		     const int radius, const int rspace,
		     const typography& typo, const bool byvaluep)
{
  // Get cache, list of specialized id matches.
  const id_render_state_umap& cache = get_id_render_state_cache();

  // Center of glyph, a point on origin circle circumference.
  const double angledo = get_angle(v, value_max);
  const double angled = adjust_angle_rotation(angledo, k::rrotation::cw);

  // There should be no ids.empty.
  // ids.size == 1, "low orbit", kusama sphere is on/near the circumference.
  // ids.size > 1, ids are clustered in a "high orbit", some distance further
  const bool satellitep = ids.size() != 1;

  // Loop through specialized list, and do these first.
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
		  radial_text_r(obj, typog, glyphtext, radius, p,
				angledo + glyphrotate);
		}

	      if (idst.is_visible(svg::k::select::vector))
		idsremaining.push_back(id);
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
      radial_text_r(obj, typo, std::to_string(v), radius, origin, angledo);
    }
  else
    {
      // Do what's left (non-specialized ids) as per usual.
      kusama_ids_orbit_high(obj, idsremaining, p, n, n_total, v, value_max,
			    radius, rspace, typo, byvaluep, satellitep);
    }
}


/**
   Radiate as *_per_uvalue_on_arc function, but group similar
   values such that they are globbed into a satellite circle, ids
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
			     const int rspace, const bool weighbyvaluep = true,
			     const bool collisionp = false)
{
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
      const double angledo = get_angle(v, value_max);
      const double angled = adjust_angle_rotation(angledo, k::rrotation::cw);
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
  for (uint i = 0; i < vpointns.size(); ++i)
    {
      int j = vpointns.size() - 1 - i;
      auto& ids = vids[j];
      auto v = vuvalues[j];
      auto& pn = vpointns[j];
      auto& [ p, n ] = pn;

      // Draw this id's kusama circle on the circumference of origin
      // circle.
      kusama_ids_orbit_low(obj, ids, origin, p, n, vpointns.size(),
			   v, value_max, radius, rspace, typo, weighbyvaluep);

      // Iff overlay rays to check alignment.
      if (true)
	{
	  const style rstyl = { color::red, 1.0, color::red, 1.0, 2 };
	  const double angled = get_angle(v, value_max);
	  point_2t p2 = get_circumference_point_d(angled, radius * 3, origin);
	  points_to_line(obj, rstyl, origin, p2);
	  radial_text_r(obj, typo, std::to_string(v), radius * 3, origin,
			angled);
	}
    }

  return obj;
}

} // namespace svg

#endif
