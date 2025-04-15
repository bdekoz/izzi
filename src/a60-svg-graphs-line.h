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

#include "a60-json-basics.h"
#include "a60-svg-grid-matrix-systems.h"
#include "a60-svg-render-state.h"


namespace {

/// Glyph type sizes.
constexpr auto lsz = 16; // large bold
constexpr auto asz = 12; // sub headings
constexpr auto ssz = 10; // sub sub headings

/// Glyph size and margins
constexpr svg::area<> achart = { 900, 600 };
constexpr auto cpx = std::get<0>(achart.center_point());

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


/// Create a set of markers bounded by a rectangle of size n.
string
make_marker_set_n(const double i)
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

  // 4 / 2, etc.
  const double h(i/2);
  const string si = std::to_string(static_cast<uint>(i));
  auto m1 = make_marker_circle("c" + si + "red", {i, i}, {h, h}, h, style_r);
  auto m2 = make_marker_circle("c" + si + "wcaglg", {i, i}, {h, h}, h, style_wcaglg);
  auto m3 = make_marker_circle("c" + si + "wcagdg", {i, i}, {h, h}, h, style_wcagdg);
  auto m4 = make_marker_circle("c" + si + "black", {i, i}, {h, h}, h, b_style);
  auto m5 = make_marker_triangle("t" + si + "black", {i, i}, {h, h}, h, b_style);
  auto m6 = make_marker_triangle("t" + si + "wcagg", {i, i}, {h, h}, h, style_wcagg);
  auto m7 = make_marker_x("x" + si + "wcagg", {i, i}, {h, h}, h, style_wcaglg);
  auto m8 = make_marker_rect("r" + si + "wcaglg", {i, i}, {h, h}, style_wcaglg);
  auto m9 = make_marker_rect("r" + si + "wcagdg", {i, i}, {h, h}, style_wcagdg);

  std::ostringstream oss;
  oss << m1.str() << m2.str() << m3.str() << m4.str()
      << m5.str() << m6.str() << m7.str() << m8.str() << m9.str();
  return oss.str();
}


/// Make black/white/wcag markers with sizes 4x4 and 2x2.
void
make_markers(svg::svg_element& obj)
{

  svg::defs_element def;
  def.start_element();

  string m2 = make_marker_set_n(2);
  def.add_raw(m2);

  string m4 = make_marker_set_n(4);
  def.add_raw(m4);

  def.finish_element();
  obj.add_element(def);
};


/// Per-graph constants.
struct graph_rstate : public render_state_base
{
  // Labels.
  string		title;
  string		xlabel;
  string		ylabel;

  // Lines, markers, points.
  svg::style		lstyle;
  string		dasharray;
  string		markerspoints;

  // Margins/Spaces
  static constexpr uint marginx = 50;
  static constexpr uint marginy = 50;
};


/// Returns a svg_element with the chart and labels
/// Assume:
/// vgrange x axis is monotonically increasing
svg_element
make_line_graph(const svg::area<> aplate, const vrange& points,
		const graph_rstate& gstate, const bool annotationsp = true)
{
  using namespace std;

  // Scale vrange input to graph.
  svg_element lgraph(gstate.title, "line graph", aplate, false);

  // Split values and compute ranges for x/y axis.
  vector<double> pointsy(points.size());
  std::transform(points.begin(), points.end(), pointsy.begin(),
		 [](const point_2t& pt) { return std::get<1>(pt); });
  auto mmy = minmax_element(pointsy.begin(), pointsy.end());
  auto miny = *mmy.first;
  auto maxy = *mmy.second;

  // Locate graph area on plate area.
  //
  // aplate is plate area with margins, but graphable area without margins...
  // pwidth = marginx + gwidth + marginx
  // pheight = marginy + gheight + marginy
  auto [ pwidth, pheight ] = aplate;
  //double gwidth = pwidth - (2 * marginx);
  double gheight = pheight - (2 * gstate.marginy);

  // Transform data points to scaled cartasian points in graph area.
  const double chartyo = pheight - gstate.marginy;
  vrange cpoints;
  uint i = 0;
  for (const point_2t& pt : points)
    {
      auto [ vx, vy ] = pt;

      // At bottom of graph.
      point_2t xmatrix = to_point_in_1xn_matrix(aplate, points.size(),
						i++, gstate.marginx, chartyo);
      double x = get<0>(xmatrix);

      // Y axis grows up from chartyo.
      const double ylen = scale_value_on_range(vy, miny, maxy, 0, gheight);
      double y = chartyo - ylen;

      cpoints.push_back(make_tuple(x, y));
    }

  // Plot transformed points.
  polyline_element pl1 = make_polyline(cpoints, gstate.lstyle,
				       gstate.dasharray, gstate.markerspoints);
  lgraph.add_element(pl1);

  if (annotationsp)
    {
      // Add axis labels.
      point_2t xlabelp = make_tuple(pwidth / 2, chartyo + (gstate.marginy / 2));
      styled_text(lgraph, gstate.xlabel, xlabelp, k::apercu_typo);

      point_2t ylabelp = make_tuple(gstate.marginx / 2, pheight / 2);
      styled_text_r(lgraph, gstate.ylabel, ylabelp, k::apercu_typo, 90);
    }

  return lgraph;
}

} // namepace svg

#endif
