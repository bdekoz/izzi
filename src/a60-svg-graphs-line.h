// izzi line graphs -*- mode: C++ -*-

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

#ifndef izzi_SVG_GRAPHS_LINE_H
#define izzi_SVG_GRAPHS_LINE_H 1


namespace svg {

/// Some Example Line Charts.
/// https://www.highcharts.com/demo/highcharts/accessible-line

// Type sizes.
const auto lsz = 16; // large bold
const auto asz = 12; // sub headings
const auto ssz = 10; // sub sub headings

// https://developer.mozilla.org/en-US/docs/Web/SVG/Reference/

// stroke_dash_array
// Attribute/stroke-dasharray

// Marker styles.
// polyline marker-mid
// Attribute/marker-mid


/// Base function for generating SVG markers in a defs section.
marker_element
make_marker_element(const std::string id,
		    const svg::area<> ma, const svg::point_2t mcp,
		    const std::string raw)
{
  svg::marker_element mkr;
  mkr.start_element(id, ma, mcp);
  mkr.add_raw(raw.c_str());
  mkr.finish_element();
  return mkr;
}

marker_element
make_marker_circle(const std::string id,
		   const svg::area<> ma, const svg::point_2t mcp,
		   const uint radius, const svg::style styl)
{
  svg::circle_element c = make_circle(mcp, styl, radius);
  return make_marker_element(id, ma, mcp, c.str());
}

marker_element
make_marker_triangle(const std::string id,
		   const svg::area<> ma, const svg::point_2t mcp,
		   const uint radius, const svg::style styl)
{
  svg::path_element p = make_path_triangle(mcp, styl, radius);
  return make_marker_element(id, ma, mcp, p.str());
}

marker_element
make_marker_x(const std::string id,
	      const svg::area<> ma, const svg::point_2t mcp,
	      const uint radius, const svg::style styl)
{
  svg::path_element cm = svg::make_path_center_mark(mcp, styl, radius, radius / 2);
  return make_marker_element(id, ma, mcp, cm.str());
}

marker_element
make_marker_rect(const std::string id,
		 const svg::area<> ma, const svg::point_2t mcp,
		 const svg::style styl)
{
  svg::rect_element p = make_rect_centered(mcp, styl, ma);
  return make_marker_element(id, ma, mcp, p.str());
}


/// Make black/white/wcag markers.
void
make_markers(svg::svg_element& obj)
{
  using svg::color::red;
  using svg::color::wcag_lgray;
  using svg::color::wcag_gray;
  using svg::color::wcag_dgray;
  const svg::style style_r = { red, 1.0, red, 0.0, 0.5 };
  const svg::style style_wcaglg = { wcag_lgray, 1.0, wcag_lgray, 0.0, 0.5 };
  const svg::style style_wcagg = { wcag_gray, 1.0, wcag_gray, 0.0, 0.5 };
  const svg::style style_wcagdg = { wcag_dgray, 1.0, wcag_dgray, 0.0, 0.5 };

  obj.add_element(make_marker_circle("c4red", {4, 4}, {2, 2}, 2, style_r));

  obj.add_element(make_marker_circle("c4wcaglg", {4, 4}, {2, 2}, 2, style_wcaglg));
  obj.add_element(make_marker_circle("c4wcagdg", {4, 4}, {2, 2}, 2, style_wcagdg));
  obj.add_element(make_marker_circle("c4black", {4, 4}, {2, 2}, 2, svg::k::b_style));

  obj.add_element(make_marker_triangle("t4black", {4, 4}, {2, 2}, 2, svg::k::b_style));
  obj.add_element(make_marker_triangle("t4wcagg", {4, 4}, {2, 2}, 2, style_wcagg));

  obj.add_element(make_marker_x("x4wcagg", {4, 4}, {2, 2}, 2, style_wcaglg));

  obj.add_element(make_marker_rect("r4wcaglg", {4, 4}, {2, 2}, style_wcaglg));
  obj.add_element(make_marker_rect("r4wcagdg", {4, 4}, {2, 2}, style_wcagdg));
};



} // namepace svg

#endif
