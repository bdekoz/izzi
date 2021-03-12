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

#ifndef MiL_SVG_RENDER_RADIAL_ARC_H
#define MiL_SVG_RENDER_RADIAL_ARC_H 1


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
radiate_id_at_value(svg_element& obj, const point_2t origin,
		    const typography& typo, string pname,
		    size_type pvalue, size_type pmax, double r,
		    bool rotatep)
{
  const double angled = get_angle(pvalue, pmax);
  string label = make_label_for_value(pname, pvalue, get_label_spaces());
  if (rotatep)
    radial_text_r(obj, typo, label, r, origin, angled);
  else
    radial_text_r(obj, typo, label, r, origin, 0);
}


/**
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
			     const int radius, bool rotatep)
{
  // Probe/Marker display.
  // Loop through map key/values and put on canvas.
  for (const auto& v : ivm)
    {
      string pname(v.first);
      size_type pvalue(v.second);
      if (pvalue)
	radiate_id_at_value(obj, origin, typo, pname, pvalue, value_max,
			    radius, rotatep);
    }

  return obj;
}


/// Map ids with one value to a point cluster radiating out from a center.
void
radiate_ids_at_uvalue(svg_element& obj, const point_2t origin,
		      const typography& typo, const strings& ids,
		      size_type pvalue, size_type pmax, double r,
		      double rspace)
{
  // Find point on the circumference of the circle closest to value
  // (pvalue).
  const double angled = get_angle(pvalue, pmax);

  // Consolidate label text to be "VALUE -> " with labelspaces spaces.
  string label = make_label_for_value("", pvalue, get_label_spaces());
  radial_text_r(obj, typo, label, r, origin, angled);

  // Next, print out the various id's on an arc with a bigger radius.
  //splay_ids_around(obj, typo, ids, angled, origin, r + rspace, rspace);
  //splay_ids_after(obj, typo, ids, angled, origin, r + rspace, rspace);
  //splay_ids_stagger(obj, typo, ids, angled, origin, r + rspace, rspace);
  //stack_ids_at(obj, typo, ids, angled, origin, r + 65, 10);

  append_ids_at(obj, typo, ids, angled, origin, r + rspace);
}


/**
   Radiate as above, but group similar values such that they are
   splayed, and not written on top of each other on the same
   arc/angle.
*/
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
  direction_arc_at(obj, origin, radius, styl);

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
	radiate_ids_at_uvalue(obj, origin, typo, ids, v, value_max,
			      radius, rspace);
    }
  return obj;
}

} // namespace svg

#endif
