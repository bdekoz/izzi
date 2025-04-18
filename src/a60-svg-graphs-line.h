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

#include <set>

#include "a60-json-basics.h"
#include "a60-svg-grid-matrix-systems.h"
#include "a60-svg-render-state.h"


namespace {

// Color and style constants.
using svg::color::red;
using svg::color::wcag_lgray;
using svg::color::wcag_gray;
using svg::color::wcag_dgray;
using svg::k::b_style;

const svg::style style_r = { red, 1.0, red, 0.0, 0.5 };
const svg::style style_wcaglg = { wcag_lgray, 1.0, wcag_lgray, 0.0, 0.5 };
const svg::style style_wcagg = { wcag_gray, 1.0, wcag_gray, 0.0, 0.5 };
const svg::style style_wcagdg = { wcag_dgray, 1.0, wcag_dgray, 0.0, 0.5 };

/// Glyph type sizes.
constexpr auto lsz = 16; // title large bold
constexpr auto asz = 12; // h1
constexpr auto ssz = 10; // text, sub sub headings
constexpr auto ticsz = 7; // tic text

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
  static constexpr uint marginx = 100;
  static constexpr uint marginy = 100;
};


/// Return set of paths corresponding to marker shapes with tooltips.
string
make_line_graph_markers_tips(const vrange& points, const vrange& cpoints,
			     const graph_rstate& gstate, const double r)
{
  const string finish_hard { element_base::finish_tag };

  string ret;
  for (uint i = 0; i < points.size(); i++)
    {
      auto [ vx, vy ] = points[i];
      auto [ cx, cy ] = cpoints[i];

      //svg::circle_element c = make_circle(cpoints[i], gstate.lstyle, r);
      circle_element c;
      circle_element::data dc = { cx, cy, r };
      c.start_element();
      c.add_data(dc);
      c.add_style(gstate.lstyle);
      c.add_raw(finish_hard);

      title_element tooltip;
      string tipstr;
      tipstr += std::to_string(vy);
      tipstr += '%';
      tipstr += k::comma + k::space;
      tipstr += std::to_string(vx);
      tipstr += "ms";
      tooltip.start_element(tipstr);
      tooltip.finish_element();
      c.add_raw(tooltip.str());

      c.add_raw(circle_element::tag_closing);

      ret += c.str();
    }
  return ret;
}


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

  // x
  vector<double> pointsx(points.size());
  std::transform(points.begin(), points.end(), pointsx.begin(),
		 [](const point_2t& pt) { return std::get<0>(pt); });
  auto mmx = minmax_element(pointsx.begin(), pointsx.end());
  auto minx = *mmx.first;
  auto maxx = *mmx.second;

  // y
  vector<double> pointsy(points.size());
  std::transform(points.begin(), points.end(), pointsy.begin(),
		 [](const point_2t& pt) { return std::get<1>(pt); });
  auto mmy = minmax_element(pointsy.begin(), pointsy.end());
  auto miny = *mmy.first;
  auto maxy = *mmy.second;

  // Locate graph area on plate area.
  // aplate is total plate area with margins, aka
  // pwidth = marginx + gwidth + marginx
  // pheight = marginy + gheight + marginy
  auto [ pwidth, pheight ] = aplate;
  double gwidth = pwidth - (2 * gstate.marginx);
  double gheight = pheight - (2 * gstate.marginy);

  const double chartyo = pheight - gstate.marginy;
  const double chartxe = pwidth - gstate.marginx;

  // Transform data points to scaled cartasian points in graph area.
  vrange cpoints;
  for (uint i = 0; i < points.size(); i++)
    {
      const point_2t& pt = points[i];
      auto [ vx, vy ] = pt;

      // At bottom of graph.
      const double xlen = scale_value_on_range(vx, minx, maxx, 0, gwidth);
      double x = gstate.marginx + xlen;

      // Y axis grows up from chartyo.
      const double ylen = scale_value_on_range(vy, miny, maxy, 0, gheight);
      double y = chartyo - ylen;

      cpoints.push_back(make_tuple(x, y));
    }

  // Plot transformed points.
  lgraph.add_raw(group_element::start_group("polyline-" + gstate.title));

#if 0
  // Use polylines and markerspoints
  polyline_element pl1 = make_polyline(cpoints, gstate.lstyle,
				       gstate.dasharray, gstate.markerspoints);
  lgraph.add_element(pl1);
#else
  // Use polyline base and set of marker paths with orignal values as tooltips on top.
  polyline_element pl1 = make_polyline(cpoints, gstate.lstyle, gstate.dasharray);
  lgraph.add_element(pl1);

  lgraph.add_raw(group_element::start_group("points-values" + gstate.title));
  string markers = make_line_graph_markers_tips(points, cpoints, gstate, 2);
  lgraph.add_raw(markers);
  lgraph.add_raw(group_element::finish_group());
#endif


  lgraph.add_raw(group_element::finish_group());

  // Add annotations, labels, metadata
  if (annotationsp)
    {
      // Start annotation group.
      lgraph.add_raw(group_element::start_group("annotation-" + gstate.title));

      // Base typo for axis and tic labels.
      auto anntypo = k::apercu_typo;
      anntypo._M_style = style_wcaglg;
      anntypo._M_size = asz;

      // Add axis labels.
      point_2t xlabelp = make_tuple(pwidth / 2, chartyo + (gstate.marginy / 2));
      styled_text(lgraph, gstate.xlabel, xlabelp, anntypo);

      point_2t ylabelp = make_tuple(gstate.marginx / 2, pheight / 2);
      styled_text_r(lgraph, gstate.ylabel, ylabelp, anntypo, 90);

      // Add axis lines.
      line_element lx = make_line({gstate.marginx, chartyo}, {chartxe, chartyo},
				  gstate.lstyle);
      line_element ly = make_line({gstate.marginx, chartyo},
				  {gstate.marginx, gstate.marginy},
				  gstate.lstyle);
      lgraph.add_element(lx);
      lgraph.add_element(ly);

      anntypo._M_size = ticsz;

      // X tic marks
      // X tic labels
      // x axis is monotonically increasing
      for (uint i = 0; i < cpoints.size(); i++)
	{
	  const point_2t& cpt = cpoints[i];
	  const double x = std::get<0>(cpt);
	  const double yto = chartyo + asz;

	  const point_2t& pt = points[i];
	  const string xval = std::to_string(static_cast<uint>(std::get<0>(pt)));
	  styled_text(lgraph, xval, {x, yto}, anntypo);
	}

      // Y tic marks
      // Y tic labels
      const double yrange(maxy - miny);
      const double yscale(gheight / yrange);

      const double xto = gstate.marginx - asz;

      // Filter tic labels to unique smallest subset of significant
      // lables of yticsteps.
      const double yticsteps = 10; // number of y tic labels
      const double ydelta = yrange / yticsteps;
      vector<uint> ypointssig(pointsy.size());
      std::transform(pointsy.begin(), pointsy.end(), ypointssig.begin(),
		     [ydelta](const double& d) { return static_cast<uint>(d / ydelta); });
      std::set<double> ypoints(ypointssig.begin(), ypointssig.end());

      // Add y tic labels.
      for (const double& y:  ypoints)
	{
	  const uint yui = y * ydelta;
	  const double yto = chartyo - (yui * yscale);
	  styled_text(lgraph, std::to_string(yui), {xto, yto}, anntypo);
	}

      // End group
      lgraph.add_raw(group_element::finish_group());
    }

  return lgraph;
}

} // namepace svg

#endif
