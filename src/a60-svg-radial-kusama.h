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
    Draw line and value on ray from origin as part of kusama.
    skip (origin to rstart) + rspace + line of length lineline + rspace + value

    Returns the length along the arc of the generated glyph minus text part.
*/
int
radiate_line_and_value(svg_element& obj, const point_2t origin,
		       const double angled, const size_type v,
		       const int rspace, const int rstart, const int linelen,
		       const typography& typo,
		       const style styl = { color::black, 1, color::black, 1, 2 })
{
  const double angleda = adjust_angle_rotation(angled, k::rrotation::cw);
  const int rbase(rstart + rspace);
  point_2t pl1 = get_circumference_point_d(angleda, rbase, origin);
  point_2t pl2 = get_circumference_point_d(angleda, rbase + linelen, origin);
  points_to_line(obj, styl, pl1, pl2);

  // Length used of glyphs along radiated ray from origin, if any.
  int glyphr = rbase + linelen + rspace;
  radial_text_r(obj, typo, std::to_string(v), glyphr, origin, angled);
  return glyphr - rstart;
}


/**
    Draw glyph on ray from origin as part of kusama.
*/
int
radiate_glyph(svg_element& obj, const point_2t origin, const double angled,
	      const id_render_state idst,
	      const int kr, const int rspace, const int rstart)
{
  // Kusama circle radius.
  // Assumed to scale per value/value_max ratio.
  const double angleda = adjust_angle_rotation(angled, k::rrotation::cw);

  const string glyphtext = idst.name;
  const double glyphscale = idst.multiple;
  const double glyphrotate = idst.rotate;

  int glyphr(0);
  if (idst.is_visible(svg::k::select::svg))
    {
      // SVG to be inserted is
      // - square canvas width/height of 100 pixels
      // - glyph centered in this canvas
      // - glyph circle diameter = glyphscale value in pixels
      // Implies:
      // kr * 2 is desired width of circle in glyph.
      const double scale(kr * 2 / glyphscale);
      const double scaledsize = scale * 100;
      const double scaledglyph = scale * glyphscale;
      const double svgr = rstart + rspace + (scaledglyph / 2);
      point_2t p = get_circumference_point_d(angleda, svgr, origin);
      insert_svg_at(obj, glyphtext, p, 100, scaledsize,
		    angleda + glyphrotate);
      glyphr += rspace + scaledsize;
    }

  if (idst.is_visible(svg::k::select::vector))
    {
      const int vr = rstart + rspace + kr;
      point_2t p = get_circumference_point_d(angleda, vr, origin);
      auto [ x, y ] = p;
      point_2d_to_circle(obj, x, y, idst.styl, kr);
      glyphr += rspace + (2 * kr);
    }

  return glyphr;
}


/**
    Draw glyph and id on ray from origin as part of kusama.
    skip (origin to rstart) + rspace + glyph of radius + rspace + id

    @origin is center of the primary/base circle for kusama renderings.
    @radius is of the kusama satellite circle on the ray.
    @rstart is the length from origin along the ray to begin rendering
    @rspace is the distance between text/vector/glyph elements

    Returns the length along the arc of the generated glyph.
*/
int
radiate_glyph_and_id(svg_element& obj, const point_2t origin,
		     const size_type v, const size_type value_max,
		     const int radius, const int rspace, const int rstart,
		     const string id, const typography& typo)
{
  // Length used of glyphs along radiated ray from origin, if any.
  int glyphr(0);
  const double angled = get_angle(v, value_max);
  const id_render_state idst = get_id_render_state(id);

  // Switch based on id_render_state settings.
  if (idst.is_visible(svg::k::select::glyph))
    {
      const double kr((double(v) / value_max) * radius);
      glyphr += radiate_glyph(obj, origin, angled, idst, kr, rspace, rstart);
    }

  // Id name.
  if (idst.is_visible(svg::k::select::text) && !id.empty())
    {
      const int idr = rstart + glyphr + rspace;
      radial_text_r(obj, typo, id, idr, origin, angled);

      // NB: This is only an estimate of the text block size.
      // Should be getComputedTextLength
      glyphr += id.size() * char_width_to_px(typo._M_size);
    }

  return glyphr;
}


/**
   Draw these ids around a kusama low orbit circle on arcs from the origin circle.

   Simplest version, make satellite circle on circumfrence and splay
   or append id's around it.
*/
int
kusama_ids_orbit_high(svg_element& obj, const strings& ids, const point_2t origin,
		      const size_type v, const size_type value_max,
		      const int radius, const int rspace, const int rstart,
		      const typography& typo,
		      const bool wbyvaluep, const bool satellitep)
{
  // Center of glyph, a point on origin circle circumference.
  const double angled = get_angle(v, value_max);

  int glyphr(0);
  auto linelen = rspace;
  glyphr = radiate_line_and_value(obj, origin, angled, v,
				  rspace, rstart, linelen, typo);

  // Add number of characters of value as string * size of each character.
  glyphr += get_label_spaces(value_max) * char_width_to_px(typo._M_size);

  if (satellitep)
    {
      // If satellitep, draw circle in the default style in low orbit to
      // hang the rest of the glyphs off of, in high orbit...
      glyphr += radiate_glyph_and_id(obj, origin, v, value_max, radius, rspace,
				     rstart + glyphr, "", typo);
    }

  // Variation based on splay_ids_around center point,
  // where center is point on arc from origin...
  double kr(0);
  if (wbyvaluep)
    kr = ((double(v) / value_max) * radius);
  else
    kr = 4; // XXX wbyvaluep to weigh int, if 0 by value?

  // glyphr += rspace;

  // Distance betwen id spheres on high-orbit kusama.
  // NB for low values, make sure distance is at least text height away.
  const double distance = std::max(kr * 3, char_height_to_px(typo._M_size));

  const double ar = rstart + glyphr;
  const double anglea = adjust_angle_at_orbit_for_distance(ar, distance);
  const double maxdeg = anglea * (ids.size() - 1);
  double angled2 = angled - (maxdeg / 2);

  int maxglyphr2(0);
  for (const string& id : ids)
    {
      int glyphr2(0);
      const id_render_state idst = get_id_render_state(id);
      if (idst.is_visible(svg::k::select::glyph))
	{
	  glyphr2 = radiate_glyph(obj, origin, angled2, idst, kr, rspace,
				  rstart + glyphr);
	}

      if (idst.is_visible(svg::k::select::text) && !id.empty())
	{
	  const int idr = rstart + glyphr + glyphr2 + rspace;
	  radial_text_r(obj, typo, id, idr, origin, angled2);

	  // NB: This is only an estimate of the text block size.
	  // Should be getComputedTextLength
	  glyphr2 += id.size() * char_width_to_px(typo._M_size);
	}

      // Advance and bookkeeping for the next round.
      angled2 += anglea;
      maxglyphr2 = std::max(maxglyphr2, glyphr2);
    }
  glyphr += maxglyphr2;

  return glyphr;
}


/**
   Draw these ids as a glyph on the circumference of origin circle.

   Simplest version.
*/
// need p, and v/vmax
void
kusama_ids_orbit_low(svg_element& obj, const strings& ids, const point_2t origin,
		     const size_type v, const size_type value_max,
		     const int radius, const int rspace, const int rstart,
		     const typography& typo, const bool wbyvaluep)
{
  // Center of glyph, a point on origin circle circumference.
  const double angled = get_angle(v, value_max);

  int glyphr = 0;
  if (ids.size() == 1)
    {
      // Low orbit.
      auto linelen = rspace;
      glyphr = radiate_line_and_value(obj, origin, angled, v,
				      rspace, rstart, linelen, typo);

      // Add number of characters of value as string * size of each character.
      glyphr += get_label_spaces(value_max) * char_width_to_px(typo._M_size);

      glyphr += radiate_glyph_and_id(obj, origin, v, value_max, radius, rspace,
				     rstart + glyphr, ids.front(), typo);
    }
  else
    {
      // High orbit.
      // Do what's left (non-specialized ids) as per usual.
      kusama_ids_orbit_high(obj, ids, origin, v, value_max,
			    radius, rspace, rstart, typo, wbyvaluep, true);
    }
}


/// Layer one value's ids.
void
kusama_ids_at_uvalue(svg_element& obj, const point_2t origin, strings& ids,
		     const size_type v, const size_type value_max,
		     const int radius, const int rspace, const int rstart,
		     const typography& typo, const bool weighbyvaluep)
{
  // Draw resulting points, ids, values.
  constexpr bool overlayrayp = false;

  // Draw this id's kusama circle on the circumference of origin
  // circle.
  kusama_ids_orbit_low(obj, ids, origin, v, value_max, radius, rspace, rstart,
		       typo, weighbyvaluep);

  // Iff overlay rays to check text and glyph alignment.
  if (overlayrayp)
    {
      const style rstyl = { color::red, 1.0, color::red, 1.0, 2 };
      const double angled = get_angle(v, value_max);
      radiate_line_and_value(obj, origin, angled, v, rspace, rstart,
			     radius * 4, typo, rstyl);
    }
}


/**
   Split values into

   Go through values, if there are two values that are adjacent,
     - move the first value out to a larger radius, rprime.
     - render the first value at rprime, remove it from the list.
     - save the second value and continue

   A point cluster is a circle whos radius is proportionate to the
   number of duplicate ids at that point.  Duplicate ids splay, stack,
   or append/concatencate at, after, or around that point cluster.
*/
void
kusama_collision_transforms(svg_element& obj, const point_2t origin,
			    std::vector<size_type> vuvalues, vvstrings vids,
			    const int radius, const int rspace, const int rstart,
			    const typography& typo, const bool weighbyvaluep)
{
#if 0
  std::vector<point_2tn> vpointns;
  // Find initial point on the circumference of the circle closest
  // to current value, aka initial circumference point (ICP).
  const double angledo = get_angle(v, value_max);
  const double angled = adjust_angle_rotation(angledo, k::rrotation::cw);
  point_2t p = get_circumference_point_d(angled, radius, origin);
  vpointns.push_back(std::make_tuple(p, ids.size()));
#endif

  // Threshold is the range such that a value is considered adjacent
  // for collisions. If v > previous value + threshold, then the
  // points are not considered neighbors.
  const size_type threshold(1);
  const size_type value_max = vuvalues.back();

  // Stash of values/ids for near pass, but later.
  vvstrings vidsnear;
  std::vector<size_type> vuvaluesnear;

  // Process far values/ids.
  // Draw furthest points, ids, values if distance(v, vnext) >= threshold.
  bool skip = false;
  for (uint i = 0; i < vuvalues.size(); ++i)
    {
      strings& ids = vids[i];
      size_type v = vuvalues[i];
      size_type vnext = i + 1 < vuvalues.size() ? vuvalues[i + 1] : vuvalues[i];

      if (v + threshold >= vnext && !skip)
	{
	  kusama_ids_at_uvalue(obj, origin, ids, v, value_max,
			       radius, rspace, rstart, typo, weighbyvaluep);
	  skip = true;
	}
      else
	{
	  vidsnear.push_back(ids);
	  vuvaluesnear.push_back(v);
	  skip = false;
	}
    }

  // Return remaining in arguments.
  vuvalues = vuvaluesnear;
  vids = vidsnear;

#if 0
  // Massage data to fit without overlaps, given:
  // 1 unique set of values
  // 2 for each unique value, the ids that match
  // 3 for each unique value, the proposed point and size of the circle
  for (uint i = 0; i < vuvalues.size(); ++i)
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

	  while (detect_collision(p, rcur, prevp, rprev))
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
  for (const auto& v : vuvalues)
    {
      // Extract all the ids for a given value.
      auto irange = uvaluemm.equal_range(v);
      auto ibegin = irange.first;
      auto iend = irange.second;
      strings ids;
      for (auto i = ibegin; i != iend; ++i)
	ids.push_back(i->second);
      sort_strings_by_size(ids);
      vids.push_back(ids);
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
    }

  // First pass, collision-avoidance.
  if (collisionp)
    {
      const int rstart = radius * 3;
      kusama_collision_transforms(obj, origin, vuvalues, vids,
				  radius, rspace, rstart, typo, weighbyvaluep);
    }

  // Draw remaining points, ids, values.
  for (uint i = 0; i < vuvalues.size(); ++i)
    {
      // NB: vpointns valued from smallest to largest, so reverse so that
      // smaller is drawn last and is more visible.
      int j = vuvalues.size() - 1 - i;
      auto& ids = vids[j];
      auto v = vuvalues[j];

      kusama_ids_at_uvalue(obj, origin, ids, v, value_max,
			   radius, rspace, radius, typo, weighbyvaluep);
    }

  return obj;
}

} // namespace svg

#endif
