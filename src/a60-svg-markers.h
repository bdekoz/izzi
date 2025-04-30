// svg path/line markers points -*- mode: C++ -*-

// alpha60
// bittorrent x scrape x data + analytics

// Copyright (c) 2025, Benjamin De Kosnik <b.dekosnik@gmail.com>

// This file is part of the alpha60 library.  This library is free
// software; you can redistribute it and/or modify it under the terms
// of the GNU General Public License as published by the Free Software
// Foundation; either version 3, or (at your option) any later
// version.

// This library is distributed in the hope that it will be useful, but
// WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// General Public License for more details.

#ifndef MiL_SVG_MARKERS_H
#define MiL_SVG_MARKERS_H 1


namespace svg {

/**
   Marker styles, ways to make line start, mid points, and enpoints look distinct.

   https://developer.mozilla.org/en-US/docs/Web/SVG/Reference/

   stroke_dash_array
   Attribute/stroke-dasharray

   polyline marker-mid
   Attribute/marker-mid
*/

/// Base function for generating SVG markers in a defs section.
marker_element
make_marker_element(const std::string id,
		    const area<> ma, const point_2t mcp,
		    const std::string raw)
{
  marker_element mkr;
  mkr.start_element(id, ma, mcp);
  mkr.add_raw(raw.c_str());
  mkr.finish_element();
  return mkr;
}

marker_element
make_marker_circle(const std::string id,
		   const area<> ma, const point_2t mcp,
		   const uint radius, const style styl)
{
  circle_element c = make_circle(mcp, styl, radius);
  return make_marker_element(id, ma, mcp, c.str());
}

marker_element
make_marker_triangle(const std::string id,
		   const area<> ma, const point_2t mcp,
		   const uint radius, const style styl)
{
  path_element p = make_path_triangle(mcp, styl, radius);
  return make_marker_element(id, ma, mcp, p.str());
}

marker_element
make_marker_x(const std::string id,
	      const area<> ma, const point_2t mcp,
	      const uint radius, const style styl)
{
  path_element cm = make_path_center_mark(mcp, styl, radius, radius / 2);
  return make_marker_element(id, ma, mcp, cm.str());
}

marker_element
make_marker_rect(const std::string id,
		 const area<> ma, const point_2t mcp,
		 const style styl)
{
  rect_element p = make_rect_centered(mcp, styl, ma);
  return make_marker_element(id, ma, mcp, p.str());
}


/// Create a set of markers bounded by a rectangle of size n.
string
make_marker_set_n(const double i)
{
  // 4 / 2, etc.
  const double h(i/2);
  const string si = std::to_string(static_cast<uint>(i));
  auto m1 = make_marker_circle("c" + si + "red", {i, i}, {h, h}, h, k::r_style);
  auto m2 = make_marker_circle("c" + si + "wcaglg", {i, i}, {h, h}, h, k::wcaglg_style);
  auto m3 = make_marker_circle("c" + si + "wcagdg", {i, i}, {h, h}, h, k::wcagdg_style);
  auto m4 = make_marker_circle("c" + si + "black", {i, i}, {h, h}, h, k::b_style);
  auto m5 = make_marker_triangle("t" + si + "black", {i, i}, {h, h}, h, k::b_style);
  auto m6 = make_marker_triangle("t" + si + "wcagg", {i, i}, {h, h}, h, k::wcagg_style);
  auto m7 = make_marker_x("x" + si + "wcagg", {i, i}, {h, h}, h, k::wcaglg_style);
  auto m8 = make_marker_rect("r" + si + "wcaglg", {i, i}, {h, h}, k::wcaglg_style);
  auto m9 = make_marker_rect("r" + si + "wcagdg", {i, i}, {h, h}, k::wcagdg_style);

  std::ostringstream oss;
  oss << m1.str() << m2.str() << m3.str() << m4.str()
      << m5.str() << m6.str() << m7.str() << m8.str() << m9.str();
  return oss.str();
}


/// Make black/white/wcag markers with sizes 4x4 and 2x2.
void
make_markers(svg_element& obj)
{

  defs_element def;
  def.start_element();

  string m2 = make_marker_set_n(2);
  def.add_raw(m2);

  string m4 = make_marker_set_n(4);
  def.add_raw(m4);

  def.finish_element();
  obj.add_element(def);
};

} // namespace svg

#endif
