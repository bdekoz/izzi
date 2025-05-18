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

/// Polyline/line creation options.
/// 1: use one line with css dasharray and markers mid, end points
/// 2: use two lines one with css dasharray, one with path tooltips
constexpr svg::ushort line_1_polyline(100);
constexpr svg::ushort line_2_polyline_tooltips(200);

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

/// Per-graph constants, metadata, text.
struct graph_rstate : public render_state_base
{
  // Margins/Spaces
  static constexpr uint marginx		= 100;
  static constexpr uint marginy		= 100;

  /// Glyph type sizes.
  static constexpr uint ttitlesz	= 16; // title large bold
  static constexpr uint th1sz		= 12; // h1
  static constexpr uint tpsz		= 10; // text, paragraph,
  static constexpr uint tticsz		= 7; // tic text

  // Labels.
  string		title;		// graph/chart title
  string		xlabel;		// x axis label
  string		ylabel;
  string		xticku;		// x axis tick mark units postfix
  string		yticku;
  static constexpr uint xtickdigits	= 1;

  style			lstyle;		// line style
  stroke_style		sstyle;		// stroke style, if any.
};


/// Return set of paths corresponding to marker shapes with tooltips.
string
make_line_graph_markers_tips(const vrange& points, const vrange& cpoints,
			     const graph_rstate& gstate, const double radius)
{
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

      const string& linecap = gstate.sstyle.linecap;
      const bool roundp = linecap == "round" || linecap == "circle";
      const bool squarep = linecap == "square";
      const bool trianglep = linecap == "triangle";

      // Markers default to closed paths that are filled with no stroke.
      // Setting visible to vector | echo induces outline behavior.
      style styl = gstate.lstyle;
      styl._M_fill_opacity = 1;
      if (gstate.is_visible(select::echo))
	styl._M_fill_color = color::white;


      // Circle Centered.
      // svg::circle_element c = make_circle(cpoints[i], gstate.lstyle, r);
      if (roundp)
	{
	  circle_element c;
	  circle_element::data dc = { cx, cy, radius };
	  c.start_element();
	  c.add_data(dc);
	  c.add_style(styl);
	  c.add_raw(element_base::finish_hard);
	  c.add_title(tipstr);
	  c.add_raw(string { circle_element::pair_finish_tag } + k::newline);
	  ret += c.str();
	}

      // Square Centered.
      // svg::rect_element r = (cpoints[i], gstate.lstyle, {2 * r, 2 * r});
      if (squarep)
	{
	  rect_element r;
	  rect_element::data dr = { cx - radius, cy - radius,
				    2 * radius, 2 * radius };
	  r.start_element();
	  r.add_data(dr);
	  r.add_style(styl);
	  r.add_raw(element_base::finish_hard);
	  r.add_title(tipstr);
	  r.add_raw(string { rect_element::pair_finish_tag } + k::newline);
	  ret += r.str();
	}

      // Triangle Centered.
      // svg::path_element t = (cpoints[i], gstate.lstyle, {2 * r, 2 * r});
      if (trianglep)
	{
	  path_element p = make_path_triangle(cpoints[i], styl, radius, 120,
					      false);
	  p.add_title(tipstr);
	  p.add_raw(string { path_element::pair_finish_tag } + k::newline);
	  ret += p.str();
	}

      // Throw if marker style not supported.
      if (!roundp && !squarep && !trianglep)
	{
	  string m("make_line_graph_markers_tips:: ");
	  m += "linecap value invalid or missing, currently set to: ";
	  m += linecap;
	  m += k::newline;
	  throw std::runtime_error(m);
	}
    }
  return ret;
}


/// Axis Labels
/// Axis X/Y Ticmarks
svg_element
make_line_graph_annotations(const area<> aplate,
			    const vrange& points,
			    const graph_rstate& gstate,
			    const double xscale = 1, const double yscale = 1,
			    const typography typo = k::apercu_typo)
{
  using namespace std;
  svg_element lanno(gstate.title, "line graph annotation", aplate, false);

  // Locate graph area on plate area.
  auto [ pwidth, pheight ] = aplate;
  double gwidth = pwidth - (2 * gstate.marginx);
  double gheight = pheight - (2 * gstate.marginy);
  const double chartyo = pheight - gstate.marginy;
  const double chartxe = pwidth - gstate.marginx;

  // Base typo for axis.
  typography anntypo = typo;
  anntypo._M_style = k::wcaglg_style;
  anntypo._M_size = graph_rstate::th1sz;

  // Axes and Labels
  if (gstate.is_visible(select::axis))
    {
      lanno.add_raw(group_element::start_group("axes-" + gstate.title));

      // Add axis labels.
      point_2t xlabelp = make_tuple(pwidth / 2, chartyo + (gstate.marginy / 2));
      styled_text(lanno, gstate.xlabel, xlabelp, anntypo);

      point_2t ylabelp = make_tuple(gstate.marginx / 2, pheight / 2);
      styled_text(lanno, gstate.ylabel, ylabelp, anntypo);

      // Add axis lines.
      line_element lx = make_line({gstate.marginx, chartyo}, {chartxe, chartyo},
				  gstate.lstyle);
      line_element ly = make_line({gstate.marginx, chartyo},
				  {gstate.marginx, gstate.marginy},
				  gstate.lstyle);
      lanno.add_element(lx);
      lanno.add_element(ly);

      lanno.add_raw(group_element::finish_group());
    }

  // Base typo for tic labels.
  // NB: Assume pointsx/pointsy are monotonically increasing.
  anntypo._M_size = graph_rstate::tticsz;
  if (gstate.is_visible(select::ticks))
    {
      lanno.add_raw(group_element::start_group("ticks-" + gstate.title));

      // Separate tic label values for x/y axis.
      vspace pointsx;
      vspace pointsy;
      split_vrange(points, pointsx, pointsy, xscale, yscale);
      auto [ maxx, maxy ] = minmax_vrange(points, xscale, yscale);
      pointsx.push_back(maxx);
      pointsy.push_back(maxy);

      // X tic labels
      auto minx = 0;
      const double xrange(maxx - minx);
      const double xscale(gwidth / xrange);

      const double ygo = gstate.marginy + gheight + graph_rstate::th1sz;

      // Filter tic labels to unique smallest subset of significant labels.
      vspace xpointsn = narrow_vspace(pointsx, gstate.xtickdigits);

      // Add x tic labels.
      std::set<double> xpoints(xpointsn.begin(), xpointsn.end());
      for (const double& x: xpoints)
	{
	  //const uint xui = x * xticsteps;
	  const double xto = gstate.marginx + (x * xscale);

	  ostringstream oss;
	  oss << fixed << setprecision(gstate.xtickdigits) << x;
	  const string sxui = oss.str() + gstate.xticku;

	  styled_text(lanno, sxui, {xto, ygo}, anntypo);
	}

      // Y tic labels
      auto miny = 0;
      const double yrange(maxy - miny);
      const double yscale(gheight / yrange);

      // Positions for left and right y-axis tic labels.
      const double xgol = gstate.marginx - graph_rstate::th1sz;		// left
      const double xgor = gstate.marginx + gwidth + graph_rstate::th1sz;// right

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
	  styled_text(lanno, syui, {xgol, yto}, anntypo);
	  styled_text(lanno, syui, {xgor, yto}, anntypo);
	}

      lanno.add_raw(group_element::finish_group());
    }

  return lanno;
}


/// Returns a svg_element with the chart and labels
/// Assume:
/// vgrange x axis is monotonically increasing
svg_element
make_line_graph(const svg::area<> aplate, const vrange& points,
		const graph_rstate& gstate,
		const point_2t xrange, const point_2t yrange)
{
  using namespace std;
  svg_element lgraph(gstate.title, "line graph", aplate, false);

  auto [ minx, maxx ] = xrange;
  auto [ miny, maxy ] = yrange;

  // Locate graph area on plate area.
  // aplate is total plate area with margins, aka
  // pwidth = marginx + gwidth + marginx
  // pheight = marginy + gheight + marginy
  auto [ pwidth, pheight ] = aplate;
  double gwidth = pwidth - (2 * gstate.marginx);
  double gheight = pheight - (2 * gstate.marginy);
  const double chartyo = pheight - gstate.marginy;

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
