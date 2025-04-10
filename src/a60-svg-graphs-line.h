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

#include "a60-svg-grid-matrix-systems.h"
#include "a60-json-basics.h"

namespace {

/// Glyph type sizes.
constexpr auto lsz = 16; // large bold
constexpr auto asz = 12; // sub headings
constexpr auto ssz = 10; // sub sub headings

/// Glyph size and margins
constexpr svg::area<> achart = { 900, 600 };
constexpr auto cpx = std::get<0>(achart.center_point());
constexpr uint marginx = 20;
constexpr uint marginy = 20;

} // end anonymous namespace


namespace svg {

/**
   Line Graphs / Line Charts.

   Some Example:
   https://www.highcharts.com/demo/highcharts/accessible-line

   Outline:

   input has 2 columns: x, y
     - how many x, what is range, what is delta
     - how many y, what is range, what is delta

   plot as grid/matrix system given above.

   line: points, linestyle

   x axis: title, tick mark spacing, tick mark style
   y axis: title, tick mark spacing, tick mark style
*/



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
  using svg::k::b_style;
  const svg::style style_r = { red, 1.0, red, 0.0, 0.5 };
  const svg::style style_wcaglg = { wcag_lgray, 1.0, wcag_lgray, 0.0, 0.5 };
  const svg::style style_wcagg = { wcag_gray, 1.0, wcag_gray, 0.0, 0.5 };
  const svg::style style_wcagdg = { wcag_dgray, 1.0, wcag_dgray, 0.0, 0.5 };

  auto m1 = make_marker_circle("c4red", {4, 4}, {2, 2}, 2, style_r);
  auto m2 = make_marker_circle("c4wcaglg", {4, 4}, {2, 2}, 2, style_wcaglg);
  auto m3 = make_marker_circle("c4wcagdg", {4, 4}, {2, 2}, 2, style_wcagdg);
  auto m4 = make_marker_circle("c4black", {4, 4}, {2, 2}, 2, b_style);
  auto m5 = make_marker_triangle("t4black", {4, 4}, {2, 2}, 2, b_style);
  auto m6 = make_marker_triangle("t4wcagg", {4, 4}, {2, 2}, 2, style_wcagg);
  auto m7 = make_marker_x("x4wcagg", {4, 4}, {2, 2}, 2, style_wcaglg);
  auto m8 = make_marker_rect("r4wcaglg", {4, 4}, {2, 2}, style_wcaglg);
  auto m9 = make_marker_rect("r4wcagdg", {4, 4}, {2, 2}, style_wcagdg);

  svg::defs_element def;
  def.start_element();
  def.add_raw(m1.str());
  def.add_raw(m2.str());
  def.add_raw(m3.str());
  def.add_raw(m4.str());
  def.add_raw(m5.str());
  def.add_raw(m6.str());
  def.add_raw(m7.str());
  def.add_raw(m8.str());
  def.add_raw(m9.str());
  def.finish_element();
  obj.add_element(def);
};


/// Returns a svg_element with the chart and labels
/// Assume:
/// vgrange x axis is monotonically increasing
svg_element
make_line_graph(const svg::area<> aplate, const vrange& points,
		const string title, const string xlabel, const string ylabel)
{
  using namespace std;

  // Scale vrange input to graph.
  svg_element lgraph(title, "line graph", aplate, false);

  // Split values and compute ranges for x/y axis.
#if 0
  vector<double> pointsx(points.size());
  auto llo = [](const point_2t& pt) { return std::get<0>(pt); };
  std::transform(points.begin(), points.end(), pointsx.begin(), llo);
  auto mmx = minmax_element(pointsx.begin(), pointsx.end());
  auto minx = *mmx.first;
  auto maxx = *mmx.second;
#endif

  vector<double> pointsy(points.size());
  std::transform(points.begin(), points.end(), pointsy.begin(),
		 [](const auto& pt) { return std::get<1>(pt); });
  auto mmy = minmax_element(pointsy.begin(), pointsy.end());
  auto miny = *mmy.first;
  auto maxy = *mmy.second;

  // Locate graph area on plate area.
  // aplate is plate area with margins, but find out graphable area without margins.
  // pwidth = marginx + gwidth + marginx
  // pheight = marginy + gheight + marginy
  auto [ pwidth, pheight ] = aplate;
  //double gwidth = pwidth - (2 * marginx);
  double gheight = pheight - (2 * marginy);

  // Transform data points to scaled cartasian points in graph area.
  vrange cpoints;
  const double chartyo = pheight - marginy;

  uint i = 0;
  for (const point_2t& pt : points)
    {
      auto [ vx, vy ] = pt;

      // At bottom of graph.
      point_2t xmatrix = to_point_in_1xn_matrix(aplate, points.size(),
						i++, marginx, chartyo);
      double x = get<0>(xmatrix);

      // Y axis grows up from chartyo.
      const double ylen = scale_value_on_range(vy, miny, maxy, 0, gheight);
      double y = chartyo - ylen;

      cpoints.push_back(make_tuple(x, y));
    }

  // Plot transformed points.
  const style styl1 = { color::wcag_lgray, 0.0, color::wcag_lgray, 1.0, 4 };
  polyline_element pl1 = make_polyline(cpoints, styl1, "4", "c4wcagg");
  lgraph.add_element(pl1);

  // Add labels.
  point_2t xlabelp = make_tuple(pwidth / 2, chartyo + (marginy / 2));
  styled_text(lgraph, xlabel, xlabelp, k::apercu_typo);

  point_2t ylabelp = make_tuple(marginx / 2, pheight / 2);
  styled_text_r(lgraph, ylabel, ylabelp, k::apercu_typo, 90);

  return lgraph;
}

} // namepace svg

#endif
