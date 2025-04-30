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

#include "izzi-json-basics.h"
#include "a60-svg-grid-matrix-systems.h"
#include "a60-svg-render-state.h"
#include "a60-svg-markers.h"


namespace {

/// Glyph type sizes.
constexpr auto lsz = 16; // title large bold
constexpr auto asz = 12; // h1
constexpr auto ssz = 10; // text, sub sub headings
constexpr auto ticsz = 7; // tic text

/// Glyph size and margins.
constexpr svg::area<> achart = { 900, 600 };
constexpr auto cpx = std::get<0>(achart.center_point());

/// Polyline/line creation options.
constexpr svg::ushort line_1_polyline(100);
constexpr svg::ushort line_2_polyline_tooltips(200);

} // end anonymous namespace


namespace svg {

/// Split range, so one dimension of (x,y) cartesian plane.
using vspace = std::vector<double>;


/// Decompose/split 2D ranges to 1D spaces.
void
split_vrange(const vrange& cpoints, vspace& xpoints, vspace& ypoints)
{
 for (const auto& [x, y] : cpoints)
   {
     xpoints.push_back(x);
     ypoints.push_back(y);
   }
}


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


/// Per-graph constants, metadata, text.
struct graph_rstate : public render_state_base
{
  // Margins/Spaces
  static constexpr uint marginx = 100;
  static constexpr uint marginy = 100;

  // Labels.
  string		title;		// graph/chart title
  string		xlabel;		// x axis label
  string		ylabel;
  string		xticku;		// x axis tick mark units postfix
  string		yticku;

  style			lstyle;		// line style
  stroke_style		sstyle;		// stroke style, if any.
};


/// Return set of paths corresponding to marker shapes with tooltips.
string
make_line_graph_markers_tips(const vrange& points, const vrange& cpoints,
			     const graph_rstate& gstate, const double radius)
{
  const string finish_hard(string { element_base::finish_tag } + k::newline);

  string ret;
  for (uint i = 0; i < points.size(); i++)
    {
      auto [ vx, vy ] = points[i];
      auto [ cx, cy ] = cpoints[i];

      // Generate displayed tooltip text....
      string tipstr(gstate.title);
      tipstr += k::newline;
      tipstr += std::to_string(static_cast<uint>(vy));
      tipstr += '%';
      tipstr += k::comma;
      tipstr += k::space;
      tipstr += std::to_string(static_cast<uint>(vx));
      tipstr += "ms";

      const bool roundp = gstate.sstyle.linecap == "round";
      const bool squarep = gstate.sstyle.linecap == "square";

      // Markers are closed paths that are filled with no stroke.
      style fillstyl = gstate.lstyle;
      fillstyl._M_stroke_opacity = 0;
      fillstyl._M_fill_opacity = 1;

      // Centered.
      // svg::circle_element c = make_circle(cpoints[i], gstate.lstyle, r);
      if (roundp)
	{
	  circle_element c;
	  circle_element::data dc = { cx, cy, radius };
	  c.start_element();
	  c.add_data(dc);
	  c.add_style(fillstyl);
	  c.add_raw(finish_hard);
	  c.add_title(tipstr);
	  c.add_raw(string { circle_element::tag_closing } + k::newline);
	  ret += c.str();
	}

      // Centered.
      // svg::rect_element r = (cpoints[i], gstate.lstyle, {2 * r, 2 * r});
      if (squarep)
	{
	  rect_element r;
	  rect_element::data dr = { cx - radius, cy - radius,
				    2 * radius, 2 * radius };
	  r.start_element();
	  r.add_data(dr);
	  r.add_style(fillstyl);
	  r.add_raw(finish_hard);
	  r.add_title(tipstr);
	  r.add_raw(string { rect_element::tag_closing } + k::newline);
	  ret += r.str();
	}

      if (!roundp && !squarep)
	throw std::runtime_error("make_line_graph_markers_tips linecap missing");
    }
  return ret;
}
  

/// Axis Labels
/// Axis X/Y Ticmarks
void
make_line_graph_annotations(const area<> aplate,
			    const vspace& pointsx, const vspace& pointsy,
			    const graph_rstate& gstate,
			    svg_element& lgraph,
			    const typography typo = k::apercu_typo)
{
  using namespace std;
  const string finish_hard(string { element_base::finish_tag } + k::newline);

  // Locate graph area on plate area.
  auto [ pwidth, pheight ] = aplate;
  double gwidth = pwidth - (2 * gstate.marginx);
  double gheight = pheight - (2 * gstate.marginy);
  const double chartyo = pheight - gstate.marginy;
  const double chartxe = pwidth - gstate.marginx;

  // Base typo for axis.
  typography anntypo = typo;
  anntypo._M_style = k::wcaglg_style;
  anntypo._M_size = asz;
  if (gstate.is_visible(select::axis))
    {
      lgraph.add_raw(group_element::start_group("axes-" + gstate.title));

      // Add axis labels.
      point_2t xlabelp = make_tuple(pwidth / 2, chartyo + (gstate.marginy / 2));
      styled_text(lgraph, gstate.xlabel, xlabelp, anntypo);

      point_2t ylabelp = make_tuple(gstate.marginx / 2, pheight / 2);
      styled_text(lgraph, gstate.ylabel, ylabelp, anntypo);

      // Add axis lines.
      line_element lx = make_line({gstate.marginx, chartyo}, {chartxe, chartyo},
				  gstate.lstyle);
      line_element ly = make_line({gstate.marginx, chartyo},
				  {gstate.marginx, gstate.marginy},
				  gstate.lstyle);
      lgraph.add_element(lx);
      lgraph.add_element(ly);

      lgraph.add_raw(group_element::finish_group());
    }

  // Base typo for tic labels.
  // NB: Assume pointsx/pointsy are monotonically increasing.
  anntypo._M_size = ticsz;
  if (gstate.is_visible(select::ticks))
    {
      lgraph.add_raw(group_element::start_group("ticks-" + gstate.title));

      // X tic labels
      auto mmx = minmax_element(pointsx.begin(), pointsx.end());
      auto minx = *mmx.first;
      auto maxx = *mmx.second;

      const double xrange(maxx - minx);
      const double xscale(gwidth / xrange);

      const double ygo = gstate.marginy + gheight + asz;

      // Filter tic labels to unique smallest subset of significant labels.
      const double xticsteps = 100; // number of x tic labels
      auto xnarrowl = [xticsteps](const double& d)
		      { return static_cast<uint>(d / xticsteps); };
      vector<uint> xpointsi(pointsx.size());
      std::transform(pointsx.begin(), pointsx.end(), xpointsi.begin(),
		     xnarrowl);
      std::set<double> xpoints(xpointsi.begin(), xpointsi.end());

      // Add x tic labels.
      for (const double& x: xpoints)
	{
	  const uint xui = x * xticsteps;
	  const double xto = gstate.marginx + (xui * xscale);
	  const string sxui = std::to_string(xui) + gstate.xticku;
	  styled_text(lgraph, sxui, {xto, ygo}, anntypo);
	}

      // Y tic labels
      auto mmy = minmax_element(pointsy.begin(), pointsy.end());
      auto miny = *mmy.first;
      auto maxy = *mmy.second;

      const double yrange(maxy - miny);
      const double yscale(gheight / yrange);

      const double xgo = gstate.marginx - asz;

      // Filter tic labels to unique smallest subset of significant
      // lables of yticsteps.
      const double yticsteps = 10; // number of y tic labels
      const double ydelta = yrange / yticsteps;

      auto ynarrowl = [ydelta](const double& d)
		      { return static_cast<uint>(d / ydelta); };
      vector<uint> ypointsi(pointsy.size());
      std::transform(pointsy.begin(), pointsy.end(), ypointsi.begin(),
		     ynarrowl);
      std::set<double> ypoints(ypointsi.begin(), ypointsi.end());

      // Add y tic labels.
      for (const double& y: ypoints)
	{
	  const uint yui = y * ydelta;
	  const double yto = chartyo - (yui * yscale);
	  const string syui = std::to_string(yui) + gstate.yticku;
	  styled_text(lgraph, syui, {xgo, yto}, anntypo);
	}

      lgraph.add_raw(group_element::finish_group());
    }
}


/// Returns a svg_element with the chart and labels
/// Assume:
/// vgrange x axis is monotonically increasing
svg_element
make_line_graph(const svg::area<> aplate, const vrange& points,
		const graph_rstate& gstate,
		const point_2t xrange = { },
		const point_2t yrange = { })
{
  using namespace std;

  // Scale vrange input to graph.
  svg_element lgraph(gstate.title, "line graph", aplate, false);

  // Split values and compute ranges for x/y axis.

  // x
  vector<double> pointsx(points.size());
  std::transform(points.begin(), points.end(), pointsx.begin(),
		 [](const point_2t& pt) { return std::get<0>(pt); });
  double minx = get<0>(xrange);
  double maxx = get<1>(xrange);
  if (minx == 0 && maxx == 0)
    {
      auto mmx = minmax_element(pointsx.begin(), pointsx.end());
      minx = *mmx.first;
      maxx = *mmx.second;
    }

  // y
  vector<double> pointsy(points.size());
  std::transform(points.begin(), points.end(), pointsy.begin(),
		 [](const point_2t& pt) { return std::get<1>(pt); });
  double miny = get<0>(yrange);
  double maxy = get<1>(yrange);
  if (miny == 0 && maxy == 0)
    {
      auto mmy = minmax_element(pointsy.begin(), pointsy.end());
      miny = *mmy.first;
      maxy = *mmy.second;
    }

  // Locate graph area on plate area.
  // aplate is total plate area with margins, aka
  // pwidth = marginx + gwidth + marginx
  // pheight = marginy + gheight + marginy
  auto [ pwidth, pheight ] = aplate;
  double gwidth = pwidth - (2 * gstate.marginx);
  double gheight = pheight - (2 * gstate.marginy);
  const double chartyo = pheight - gstate.marginy;
  //const double chartxe = pwidth - gstate.marginx;

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

  // Add annotations first: any labels, metadata, ticmarks, legends
  make_line_graph_annotations(aplate, pointsx, pointsy, gstate, lgraph);

  // Plot path of points on cartesian plane.
  // Grouped tooltips have to be the last, aka top layer of SVG to work (?).
  //constexpr ushort line_strategy = line_1_polyline;
  constexpr ushort line_strategy = line_2_polyline_tooltips;
  if (gstate.is_visible(select::vector))
    {
      if constexpr(line_strategy == line_1_polyline)
	{
	  // Use polylines and markerspoints
	  polyline_element pl1 = make_polyline(cpoints, gstate.lstyle,
					       gstate.sstyle);
	  lgraph.add_element(pl1);
	}
      if constexpr(line_strategy == line_2_polyline_tooltips)
	{
	  // Use polyline base and set of marker paths with orignal values
	  // as tooltips on top.
	  lgraph.add_raw(group_element::start_group("polyline-" + gstate.title));
	  polyline_element pl1 = make_polyline(cpoints, gstate.lstyle,
					       gstate.sstyle);
	  lgraph.add_element(pl1);
	  lgraph.add_raw(group_element::finish_group());

	  lgraph.add_raw(group_element::start_group("values-" + gstate.title));
	  string markers = make_line_graph_markers_tips(points, cpoints,
							gstate, 3);
	  lgraph.add_raw(markers);
	  lgraph.add_raw(group_element::finish_group());
	}
    }

  return lgraph;
}

} // namepace svg

#endif
