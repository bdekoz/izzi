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

/// The smallest (sattelite) radius size allowed in a kusama orbit.
int&
get_min_ring_size()
{
  static int rsz(1);
  return rsz;
}

/// By observation, type size 12pt = 5, 6pt = 2
int
set_min_ring_size(const int sz)
{
  int& rsz = get_min_ring_size();
  int rszold(rsz);
  rsz = sz;
  return rszold;
}


/// The minimum distance between satellites in high orbit.
double&
get_min_satellite_distance()
{
  static double k(4);
  return k;
}

/// By observation, 7pt = 5 minimum
/// NB: Make sure distance is at least text height away for lowest values.
double
set_min_satellite_distance(const double kuse)
{
  double& k = get_min_satellite_distance();
  double kold(k);
  k = kuse;
  return kold;
}


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
		       const style styl = {color::black, 1, color::black, .25, 1})
{
  const double angleda = adjust_angle_rotation(angled, k::rrotation::cw);
  const int rbase(rstart + rspace);
  point_2t pl1 = get_circumference_point_d(angleda, rbase, origin);
  point_2t pl2 = get_circumference_point_d(angleda, rbase + linelen, origin);
  points_to_line(obj, styl, pl1, pl2);

  // Length used of glyphs along radiated ray from origin, if any.
  int glyphr = rbase + linelen + rspace;
  typography typob(typo);
  typob._M_w = typography::weight::bold;
  radial_text_r(obj, std::to_string(v), typob, glyphr, origin, angled);
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
  // Kusama circle radius, enforce miniumum size.
  const int kra = std::max(kr, get_min_ring_size());

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
      const double scale(kra * 2 / glyphscale);
      const double scaledsize = scale * 100;
      const double scaledglyph = scale * glyphscale;
      const double svgr = rstart + rspace + (scaledglyph / 2);
      point_2t p = get_circumference_point_d(angleda, svgr, origin);

      string isvg = file_to_svg_insert(glyphtext);
      insert_svg_at(obj, isvg, p, 100, scaledsize, angleda + glyphrotate,
		    idst.styl);
      glyphr += rspace + scaledsize;
    }

  if (idst.is_visible(svg::k::select::vector))
    {
      const int vr = rstart + rspace + kra;
      point_2t p = get_circumference_point_d(angleda, vr, origin);
      point_to_circle(obj, p, idst.styl, kra);
      glyphr += rspace + (2 * kra);
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
      radial_text_r(obj, id, typo, idr, origin, angled);

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

   @linelen	Length of line on a ray from origin to value.

   @satdistance Multiple used to compute distance between two satellite values.
		Ueful defaults: 3.3 for 12pt, 2 for 7pt.
*/
int
kusama_ids_orbit_high(svg_element& obj, const point_2t origin, const strings& ids,
		      const size_type v, const size_type value_max,
		      const int radius, const int rspace, const int rstart,
		      const int linelen,
		      const typography& typo,
		      const bool wbyvaluep, const bool satellitep = false)
{
  // Center of glyph, a point on origin circle circumference.
  const double angled = get_angle(v, value_max);

  int glyphr = radiate_line_and_value(obj, origin, angled, v,
				      rspace, rstart, linelen, typo);

  // Add number of characters of value as string * size of each character.
  glyphr += significant_digits_in(value_max) * char_width_to_px(typo._M_size);

  // If satellitep, draw "leading" circle in the default style in low orbit to
  // hang the rest of the glyphs off of, in high orbit...
  if (satellitep)
    {
      glyphr += radiate_glyph_and_id(obj, origin, v, value_max, radius, rspace,
				     rstart + glyphr, "", typo);
      glyphr += rspace;
    }

  // Radius of satellite orbit.
  const double ar = rstart + glyphr;

  // Find the satellite radius(kr).
  // Variation based on splay_ids_around center point,
  // where center is point on arc from origin...
  double kr(0);
  if (wbyvaluep)
    kr = ((double(v) / value_max) * radius);
  else
    kr = get_min_ring_size();

  // Find distance betwen two satellites spheres on high-orbit kusama.
  // Want equivalent distances between satellites of different radius (values),
  // so this cannot be strictly a multiple of various radius from v0 to vmax.
  // NB: For lowest values, make sure distance is at least text height away.
  const double k = get_min_satellite_distance();
  const double distnce = k + (kr * 2);
  const double sr = ar + rspace + kr;
  const double anglea = adjust_angle_at_orbit_for_distance(sr, distnce);

  // Given distance and anglea, find start and end angles.
  const double maxdeg = anglea * (ids.size() - 1);
  double angled2 = angled - (maxdeg / 2);
  int maxglyphr2(0);
  for (const string& id : ids)
    {
      int glyphr2(0);
      const id_render_state idst = get_id_render_state(id);

      if (idst.is_visible(svg::k::select::glyph))
	glyphr2 = radiate_glyph(obj, origin, angled2, idst, kr, rspace, ar);

      if (idst.is_visible(svg::k::select::text) && !id.empty())
	{
	  const int idr = ar + glyphr2 + rspace;
	  radial_text_r(obj, id, typo, idr, origin, angled2);

	  // NB: This is only an estimate of the text block size.
	  // Should be getComputedTextLength
	  glyphr2 += id.size() * char_width_to_px(typo._M_size);
	}

      constexpr bool debugp = false;
      if (debugp)
	{
	  const int r4 = 200;
	  const style rstyl = { color::red, 1.0, color::red, 1.0, 0.5 };
	  const double delta = adjust_angle_rotation(angled2, k::rrotation::cw);
	  const point_2t pdbg = get_circumference_point_d(delta, r4, origin);
	  points_to_line(obj, rstyl, origin, pdbg);
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
void
kusama_ids_orbit_low(svg_element& obj, const point_2t origin, const strings& ids,
		     const size_type v, const size_type value_max,
		     const int radius, const int rspace, const int rstart,
		     const int linelen,
		     const typography& typo, const bool wbyvaluep)
{
  // Center of glyph, a point on origin circle circumference.
  const double angled = get_angle(v, value_max);

  int glyphr = 0;
  if (ids.size() == 1)
    {
      // Low orbit.
      glyphr = radiate_line_and_value(obj, origin, angled, v,
				      rspace, rstart, linelen, typo);

      // Add number of characters of value as string * size of each character.
      glyphr += significant_digits_in(value_max) * char_width_to_px(typo._M_size);

      glyphr += radiate_glyph_and_id(obj, origin, v, value_max, radius, rspace,
				     rstart + glyphr, ids.front(), typo);

      constexpr bool debugp = false;
      if (debugp)
	{
	  const int r4 = 200;
	  const style rstyl = { color::blue, 1.0, color::blue, 1.0, 0.5 };
	  const double delta = adjust_angle_rotation(angled, k::rrotation::cw);
	  const point_2t pdbg = get_circumference_point_d(delta, r4, origin);
	  points_to_line(obj, rstyl, origin, pdbg);
	}
    }
  else
    {
      // High orbit.
      // Do what's left (non-specialized ids) as per usual.
      kusama_ids_orbit_high(obj, origin, ids, v, value_max,
			    radius, rspace, rstart, linelen, typo, wbyvaluep,
			    false);
    }
}


/// Layer one value's glyphs and ids.
void
kusama_ids_at_uvalue(svg_element& obj, const point_2t origin, const strings& ids,
		     const size_type v, const size_type value_max,
		     const int radius, const int rspace, const int rstart,
		     const int linelen,
		     const typography& typo, const bool weighbyvaluep)
{
  // Draw this id's kusama circle on the circumference of origin
  // circle.
  kusama_ids_orbit_low(obj, origin, ids, v, value_max, radius, rspace, rstart,
		       linelen, typo, weighbyvaluep);

  // Iff overlay rays to check text and glyph alignment.
  constexpr bool overlayrayp = false;
  if (overlayrayp)
    {
      const style rstyl = { color::red, 1.0, color::red, 1.0, 2 };
      const double angled = get_angle(v, value_max);
      radiate_line_and_value(obj, origin, angled, v, rspace, rstart,
			     radius * 4, typo, rstyl);
    }

  // Put in max value.
  constexpr bool showmaxvp = true;
  if (showmaxvp)
    {
      const double anglemax = get_angle(value_max, value_max);
      radiate_line_and_value(obj, origin, anglemax, value_max, rspace, rstart,
			     linelen, typo);
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

   NB: invariant that @vuvalues > 1

  Threshold is the range such that a value is considered adjacent for
  collisions. If v > previous value + threshold, then the points are
  not considered neighbors.

  startlen is the point at which high-orbit is drawn after transform.
  @startlenm is the multiple of radius that is startlen.
*/
void
kusama_collision_transforms(svg_element& obj, const point_2t origin,
			    std::vector<size_type>& vuvalues, vvstrings& vids,
			    const size_type value_max,
			    const int radius, const int rspace, const int rstart,
			    const typography& typo, const bool weighbyvaluep,
			    const size_type threshold = 1,
			    const int startlenm = 5)
{
  // Stash of values/ids for near pass, but after transforms are done.
  vvstrings vidsnear;
  std::vector<size_type> vuvaluesnear;

  const int startlen = radius * startlenm;
  const int linelen = rspace * 2;

  // Process far values/ids in order.
  // Draw furthest points, ids, values if no high extension previously and:
  // 1. distance(v, vnext) >= threshold.
  // 2. distance(v, vprevious) >= threshold.
  bool skip = false;
  for (uint i = 0; i < vuvalues.size(); ++i)
    {
      const strings& ids = vids[i];
      const size_type v = vuvalues[i];
      const bool finalp = i + 1 == vuvalues.size();
      const bool firstp = i == 0;

      // Check for expired skip.
      // If last was high and now skip, re-evalue to make sure that
      // the last value was close enough to matter.
      // Continue as skipped only if < value_max / 4 ago, else reset.
      if (skip)
	{
	  const size_type vold = vuvalues[i - 1];
	  if (std::abs(v - vold) > (value_max / 4))
	    skip = false;
	}

      // Criteria for boosting into high orbit:
      // not previously boosted
      // not first, and last is within threshold
      // not last, and next is within threshold
      // first and last is close (within threshold with values wrapping)
      const bool lastyesp = !firstp && (v - threshold <= vuvalues[i - 1]);
      const bool nextyesp = !finalp && (v + threshold >= vuvalues[i + 1]);
      const bool firstyesp = firstp && \
		      (v + std::abs(value_max - vuvalues.back()) <= threshold);
      if (!skip && (nextyesp || lastyesp || firstyesp))
	{
	  kusama_ids_at_uvalue(obj, origin, ids, v, value_max, radius, rspace,
			       rstart + startlen - linelen, linelen, typo,
			       weighbyvaluep);
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
  vids = vidsnear;
  vuvalues = vuvaluesnear;
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
  // Make sure at least one dimension has values before proceeding.
  // For media objects, with value_max < 8, collision avoidance is minimal.
  // With aggregates with ivm.size() > 350, threshold 2-4 is best.
  if (collisionp && vuvalues.size() > 1)
    {
      // Approximate scaling given default typograhics.
      size_type threshold(1);
      if (value_max >= 20 && value_max < 190)
	threshold = 2;
      if (value_max >= 190 && value_max < 290)
	threshold = 3;
      if (value_max >= 290)
	threshold = 4;

      kusama_collision_transforms(obj, origin, vuvalues, vids, value_max,
				  radius, rspace, radius, typo, weighbyvaluep,
				  threshold);
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
			   radius, rspace, radius, rspace, typo, weighbyvaluep);
    }

  return obj;
}

} // namespace svg

#endif
