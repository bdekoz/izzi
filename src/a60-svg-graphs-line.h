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


namespace svg {

/// Polyline/line options.
/// 1: use one line with css dasharray and markers mid, end points
/// 2: use two lines: one with css dasharray and no markerspoints, two
///    with explicit marker paths and added text tooltips
/// 3: use two lines and add js + image tooltips: like 2 above
///    but add image tooltips, with js controlling image visibility.
constexpr svg::ushort line_chart_style_1(100);
constexpr svg::ushort line_chart_style_2(200);
constexpr svg::ushort line_chart_style_3(300);

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
  static constexpr uint xticdigits	= 1; // sig digits xaxis
  static constexpr uint yticdigits	= 10; // number y tic labels

  /// Glyph type sizes.
  static constexpr uint ttitlesz	= 16; // title large bold
  static constexpr uint th1sz		= 12; // h1
  static constexpr uint tpsz		= 10; // text, paragraph,
  static constexpr uint tticsz		= 7; // tic text

  // Labels.
  string		title;		// graph/chart title
  string		xlabel;		// x axis label
  string		ylabel;
  string		xticu;		// x axis tick mark units postfix
  string		yticu;

  style			lstyle;		// line style
  stroke_style		sstyle;		// stroke style, if any.
};


/// Return set of images for image tooltips, one for each point.
/// @param aimg is size of image embedded inside svg element.
/// @pathprefix is the path to the directory with the store of images
/// @idimgbase is the root name for what will be document level unique names of
/// sequentially numbered images (say fximage-, for fximage-101 et al)
string
make_line_graph_image_set(const area<> aimg, const vrange& points,
			  const string imgidbase, const string pathprefix,
			  const string imgprefix, const string imgext)
{
  string ret;
  for (const point_2t p : points)
    {
      const string xms = std::to_string(static_cast<uint>(std::get<0>(p)));
      const string isrc = pathprefix + imgprefix + xms + imgext;
      const string imgid = imgidbase + xms;
      auto [ width, height ] = aimg;

      image_element i;
      image_element::data di = { isrc, 0, 0, width, height };
      i.start_element(imgid);
      i.add_data(di, "hidden", "anonymous");
      i.finish_element();
      ret += i.str();
    }
  return ret;
}

/// Return set of paths of marker shapes with text tooltips.
string
make_line_graph_markers(const vrange& points, const vrange& cpoints,
			const graph_rstate& gstate, const double radius,
			const string imgidbase = "")
{
  string ret;
  for (uint i = 0; i < points.size(); i++)
    {
      auto [ vx, vy ] = points[i];
      auto [ cx, cy ] = cpoints[i];

      const string xms = std::to_string(static_cast<uint>(vx));
      const string imgid = imgidbase + xms;

      // Generate displayed tooltip text....
      string tipstr(gstate.title);
      tipstr += k::newline;
      tipstr += std::to_string(static_cast<uint>(vy));
      tipstr += '%';
      tipstr += k::comma;
      tipstr += k::space;
      tipstr += xms;
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
	  if (!imgidbase.empty())
	    c.add_raw(script_element::tooltip_attribute(imgid));
	  c.add_raw(element_base::finish_tag_hard);
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
	  if (!imgidbase.empty())
	    r.add_raw(script_element::tooltip_attribute(imgid));
	  r.add_raw(element_base::finish_tag_hard);
	  r.add_title(tipstr);
	  r.add_raw(string { rect_element::pair_finish_tag } + k::newline);
	  ret += r.str();
	}

      // Triangle Centered.
      // svg::path_element t = (cpoints[i], gstate.lstyle, {2 * r, 2 * r});
      if (trianglep)
	{
	  string xattr;
	  if (!imgidbase.empty())
	    xattr = script_element::tooltip_attribute(imgid);

	  path_element p = make_path_triangle(cpoints[i], styl, radius, 120,
					      false, xattr);
	  p.add_title(tipstr);
	  p.add_raw(string { path_element::pair_finish_tag } + k::newline);
	  ret += p.str();
	}

      // Throw if marker style not supported.
      if (!roundp && !squarep && !trianglep)
	{
	  string m("make_line_graph_markers:: ");
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
/// X line increments
///
/// @param aplate = total size of graph area
/// @param points = vector of {x,y} points to graph
/// @param gstate = graph render state
/// @param xscale = scale x axis by this ammount (1000 if converting ms to s)
/// @param yscale = scale y axis by this ammount
/// @param typo = typography to use for labels
svg_element
make_line_graph_annotations(const area<> aplate,
			    const vrange& points,
			    const graph_rstate& gstate,
			    const double xscalein = 1, const double yscalein = 1,
			    const typography typo = k::apercu_typo)
{
  using namespace std;
  svg_element lanno(gstate.title, "line graph annotation", aplate, false);

  // Locate graph area on plate area.
  auto [ pwidth, pheight ] = aplate;
  double gwidth = pwidth - (2 * gstate.marginx);
  double gheight = pheight - (2 * gstate.marginy);
  const double chartyo = pheight - gstate.marginy;
  const double chartxo = gstate.marginx;
  const double chartxe = pwidth - gstate.marginx;

  // Base typo for annotations.
  typography anntypo = typo;
  anntypo._M_style = k::wcagg_style;
  anntypo._M_size = graph_rstate::th1sz;

  // Axes and Labels
  if (gstate.is_visible(select::axis))
    {
      lanno.add_raw(group_element::start_group("axes-" + gstate.title));

      // Add axis labels.
      point_2t xlabelp = make_tuple(pwidth / 2, chartyo + (gstate.marginy / 2));
      styled_text(lanno, gstate.xlabel, xlabelp, anntypo);

      point_2t ylabelp = make_tuple(chartxo / 2, pheight / 2);
      styled_text(lanno, gstate.ylabel, ylabelp, anntypo);

      // Add axis lines.
      line_element lx = make_line({chartxo, chartyo}, {chartxe, chartyo},
				  gstate.lstyle);
      line_element ly = make_line({chartxo, chartyo}, {chartxo, gstate.marginy},
				  gstate.lstyle);
      lanno.add_element(lx);
      lanno.add_element(ly);

      lanno.add_raw(group_element::finish_group());
    }

  // Base typo for tic labels.
  // NB: Assume pointsx/pointsy are monotonically increasing.
  anntypo._M_size = graph_rstate::tticsz;
  anntypo._M_baseline = typography::baseline::central;

  // Separate tic label values for each (x, y) axis, find ranges for each.
  auto [ maxx, maxy ] = max_vrange(points, gstate.xticdigits, xscalein, yscalein);
  auto minx = 0;
  auto miny = 0;

  const double xrange(maxx - minx);
  const double xscale(gwidth / xrange);
  const double yrange(maxy - miny);
  const double yscale(gheight / yrange);

  // Derive the number of tick marks.

  // Use a multiple of 5 to make natural counting easier.
  // Start with an assumption of 20 tic marks for the x axis.
  double xtickn(xrange * 2); // .5 sec
  if (xtickn < 10)
    xtickn = 10;
  if (xtickn > 26)
    xtickn = xrange;

  // X axis is seconds, xtickn minimum delta is 0.1 sec.
  double xdelta = std::max(xrange / xtickn, 0.1);

  // Round up to significant digits, so if xdelta is 0.18 round to 0.2.
  xdelta = std::round(xdelta * gstate.xticdigits * 10) / (gstate.xticdigits * 10);

  // Y axis is simpler, 0, 10, 20, ..., 80, 90, 100 in percent.
  const double ydelta = yrange / gstate.yticdigits;

  // Generate tic marks
  const double ygo = gstate.marginy + gheight + graph_rstate::th1sz;
  if (gstate.is_visible(select::ticks))
    {
      // X tic labels
      lanno.add_raw(group_element::start_group("tic-x-" + gstate.title));
      for (double x = minx; x < maxx; x += xdelta)
	{
	  const double xto = chartxo + (x * xscale);
	  ostringstream oss;
	  oss << fixed << setprecision(gstate.xticdigits) << x;
	  const string sxui = oss.str() + gstate.xticu;
	  styled_text(lanno, sxui, {xto, ygo}, anntypo);
	}
      lanno.add_raw(group_element::finish_group());

      // Y tic labels
      // Positions for left and right y-axis tic labels.
      lanno.add_raw(group_element::start_group("tic-y-" + gstate.title));
      const double yticspacer = graph_rstate::th1sz * 2;
      const double xgol = gstate.marginx - yticspacer;			// left
      const double xgor = gstate.marginx + gwidth + yticspacer;         // right
      const double starty = miny != 0 ? miny : miny + ydelta; // skip zero label
      for (double y = starty; y < maxy + ydelta; y += ydelta)
	{
	  const double yto = chartyo - (y * yscale);
	  const string syui = std::to_string(static_cast<uint>(y)) + gstate.yticu;
	  styled_text(lanno, syui, {xgol, yto}, anntypo);
	  styled_text(lanno, syui, {xgor, yto}, anntypo);
	}
      lanno.add_raw(group_element::finish_group());
    }

  // Horizontal lines linking left and right y-axis tic label value to each other,
  // perhaps with magnification-ready micro text.
  if (gstate.is_visible(select::linex))
    {
      lanno.add_raw(group_element::start_group("tic-y-lines-" + gstate.title));

      style hlstyl = gstate.lstyle;
      hlstyl._M_stroke_color = color::gray10;

      anntypo._M_size = 3;
      anntypo._M_style.set_colors(color::gray20);
      for (double y = miny + ydelta; y < maxy + ydelta; y += ydelta)
	{
	  // Base line layer.
	  const double yto = chartyo - (y * yscale);
	  line_element lxe = make_line({chartxo + graph_rstate::th1sz, yto},
				       {chartxe - graph_rstate::th1sz, yto}, hlstyl);
	  lanno.add_element(lxe);

	  // Add y-axis tic numbers along line for use when magnified.
	  if (gstate.is_visible(select::alt))
	    {
	      // Skip first and last as covered by either Y-axes tic marks.
	      for (double x = minx + xdelta; x < maxx - xdelta; x += xdelta)
		{
		  const double xto = chartxo + (x * xscale);
		  const string syui = std::to_string(static_cast<uint>(y)) + gstate.yticu;
		  styled_text(lanno, syui, {xto, yto}, anntypo);
		}
	    }
	}

      lanno.add_raw(group_element::finish_group());
    }

  return lanno;
}


/// Returns a svg_element with the rendered line graph (char).
/// Assumptions:
/// vgrange x axis is monotonically increasing
///
/// NB1: Axes and labels drawn in a separate pass (make_line_graph_annotations).
/// NB2: Output file of x-axis point values for image tooltips if strategy = 3.
///
/// @param aplate = total size of graph area
/// @param points = vector of {x,y} points to graph
/// @param gstate = graph render state
/// @param xrange = unified x-axis range for all graphs if multiplot
/// @param yrange = unified y-axis range for all graphs if multiplot
/// @param line_strategy = type of graph to render, from simplest to tooltips
/// @param imgidbase = unique text prefix over multimap for image tooltips
svg_element
make_line_graph(const svg::area<> aplate, const vrange& points,
		const graph_rstate& gstate,
		const point_2t xrange, const point_2t yrange,
		const ushort line_strategy = line_chart_style_2,
		const string imgidbase = "")
{
  using namespace std;

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
  svg_element lgraph(gstate.title, "line graph", aplate, false);
  if (gstate.is_visible(select::vector))
    {
      if (line_strategy == line_chart_style_1)
	{
	  // Use polylines and markerspoints
	  lgraph.add_raw(group_element::start_group("polyline-" + gstate.title));
	  polyline_element pl1 = make_polyline(cpoints, gstate.lstyle,
					       gstate.sstyle);
	  lgraph.add_element(pl1);
	  lgraph.add_raw(group_element::finish_group());
	}
      if (line_strategy == line_chart_style_2)
	{
	  // Use polyline base and set of marker paths with orignal values
	  // as tooltips on top.
	  lgraph.add_raw(group_element::start_group("polyline-" + gstate.title));
	  stroke_style no_markerstyle = gstate.sstyle;
	  no_markerstyle.markerspoints = "";
	  polyline_element pl1 = make_polyline(cpoints, gstate.lstyle,
					       no_markerstyle);
	  lgraph.add_element(pl1);
	  lgraph.add_raw(group_element::finish_group());

	  // Markers + text tooltips.
	  lgraph.add_raw(group_element::start_group("markers-" + gstate.title));
	  string markers = make_line_graph_markers(points, cpoints, gstate, 3);
	  lgraph.add_raw(markers);
	  lgraph.add_raw(group_element::finish_group());
	}
      if (line_strategy == line_chart_style_3)
	{
	  // Use polyline base and set of marker paths with orignal values
	  // as tooltips on top.
	  lgraph.add_raw(group_element::start_group("polyline-" + gstate.title));
	  stroke_style no_markerstyle = gstate.sstyle;
	  no_markerstyle.markerspoints = "";
	  polyline_element pl1 = make_polyline(cpoints, gstate.lstyle,
					       no_markerstyle);
	  lgraph.add_element(pl1);
	  lgraph.add_raw(group_element::finish_group());

	  // Markers + text tooltips.
	  /// add attribute with image id
	  lgraph.add_raw(group_element::start_group("markers-" + gstate.title));
	  string markers = make_line_graph_markers(points, cpoints, gstate, 3,
						   imgidbase);
	  lgraph.add_raw(markers);
	  lgraph.add_raw(group_element::finish_group());

	  // Add images with image id, default to hidden
	  // (make_line_graph_image_set)

	  // Add javascript to control images
	  // (script_element::tooltip_script)

	  // Output points used to that screenshots can be matched to
	  // the appropriate marker point/tooltip.
	  const string ofname(gstate.title + "-x-axis-control-points.csv");
	  ofstream ofs(ofname, ios_base::out);
	  if (ofs.good())
	    {
	      for (const point_2t& p : points)
		{
		  const uint ui = get<0>(p);
		  ofs << ui << k::comma << k::space;
		}
	    }
	}
    }

  return lgraph;
}

} // namepace svg

#endif
