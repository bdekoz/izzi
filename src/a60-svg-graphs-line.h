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

#ifndef izzi_SVG_LINE_GRAPHS_H
#define izzi_SVG_LINE_GRAPHS_H 1

#include <set>

#include "izzi-json-basics.h"
#include "a60-svg-grid-matrix-systems.h"
#include "a60-svg-render-state.h"
#include "a60-svg-markers.h"


namespace svg {

/// Polyline/line options.
///
/// 1: use one line with css dasharray and markers mid, end points
/// 2: use two lines: one with css dasharray and no marker_form, two
///    with explicit marker paths and added text tooltips
/// 3: use two lines and add js + image tooltips: like 2 above
///    but add image tooltips, with js controlling image visibility.
enum graph_mode : ushort
  {
    chart_line_style_1	= 100,
    chart_line_style_2	= 200,
    chart_line_style_3	= 300
  };

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

/// Per-graph constants, metadata, configuration, text.
struct graph_rstate : public render_state_base
{
  using area_type = area<space_type>;

#if 0
  /// 900 x 600
  /// Margins/Spaces
  static constexpr uint xmargin		= 100;
  static constexpr uint ymargin		= 100;

  /// Type sizes
  static constexpr uint ttitlesz	= 16; // title large bold
  static constexpr uint th1sz		= 12; // h1
  static constexpr uint tpsz		= 10; // text, paragraph,
  static constexpr uint tticsz		= 7; // tic text
#else
  /// 1920 x 1080
  /// Margins/Spaces
  static constexpr uint xmargin		= 200;
  static constexpr uint ymargin		= 200;

  /// Type sizes
  static constexpr uint ttitlesz	= 26; // title large bold
  static constexpr uint th1sz		= 18; // h1
  static constexpr uint tpsz		= 10; // text, paragraph, marker
  static constexpr uint tticsz		= 14; // tic text
#endif

  /// Key data: title, area, mode
  string		title;		/// graph title
  area_type		graph_area;	/// graph area
  graph_mode		mode;		/// chart_line_style_n to use

  /// Labels, tic units.
  static constexpr uint xticdigits	= 1; // fp sig digits xaxis
  static constexpr uint yticdigits	= 10; // number y tic labelsf
  string		xlabel;		// x axis label
  string		ylabel;
  string		xticu;		// x axis tick mark units postfix
  string		yticu;

  /// Line/Outline/Markers/Tooltip styles
  style			lstyle;		/// line style
  stroke_style		sstyle;		/// marker stroke style, if any.

  /// Image Tooltip
  area_type		tooltip_area;	/// chart_line_style_3 tooltip size
  string		tooltip_id;	/// chart_line_style_3 toolip id prefix
  string		tooltip_images;	/// chart_line_style 3 set of image elements
};


/// Simplify sorted vrange by removing interior duplicates.
vrange
find_change_points(const vrange& vr)
{
  // Transform range to the simplest expression, where multiple points
  // without significant vertical change are coalesed to starting
  // control point and ending control point.
  vrange simplevr;
  point_2t last = { -1.0, -1.0 };
  double duprangep(false);
  for (const point_2t& pt : vr)
    {
      auto [ x, y] = pt;
      if (y != get<1>(last))
	{
	  if (duprangep == true)
	    {
	      simplevr.push_back(last);
	      duprangep = false;
	    }
	  simplevr.push_back(pt);
	}
      else
	duprangep = true;
      last = pt;
    }
  return simplevr;
}


/// Tramsform change points to points where the y-axis (% visual complete) changes
/// @param points already simplified change points
vrange
find_visual_change_points(const vrange& points)
{
  vrange simplest;
  point_2t last = { -1.0, -1.0 };
  for (const point_2t& pt : points)
    {
      auto [ x, y] = pt;
      if (y != get<1>(last))
	simplest.push_back(pt);
      last = pt;
    }
  return simplest;
}


/// Tramsform change points to points where the x-axis (time) matches
/// a value in onlypoints.
///
/// @param points already simplified change points
vrange
find_tooltip_points(const vrange& points, const vspace& onlypoints)
{
  vrange edited;
  for (const space_type& matchx : onlypoints)
    {
      for (const auto&  pt : points)
	{
	  auto [ x, y ] = pt;
	  if (x == matchx)
	    {
	      edited.push_back(pt);
	      break;
	    }
	}
    }
  return edited;
}


/// Map data points to cartestian points on graph area.
/// @param data points
vrange
transform_to_graph_points(const vrange& points,
			  const graph_rstate& gstate,
			  const point_2t xrange, const point_2t yrange)
{
  auto [ minx, maxx ] = xrange;
  auto [ miny, maxy ] = yrange;

  // Locate graph area on plate area.
  // aplate is total plate area with margins, aka
  // pwidth = xmargin + gwidth + xmargin
  // pheight = ymargin + gheight + ymargin
  auto [ pwidth, pheight ] = gstate.graph_area;
  double gwidth = pwidth - (2 * gstate.xmargin);
  double gheight = pheight - (2 * gstate.ymargin);
  const double chartyo = pheight - gstate.ymargin;

  // Transform data points to scaled cartasian points in graph area.
  vrange cpoints;
  for (uint i = 0; i < points.size(); i++)
    {
      const point_2t& pt = points[i];
      auto [ vx, vy ] = pt;

      // At bottom of graph.
      const double xlen = scale_value_on_range(vx, minx, maxx, 0, gwidth);
      double x = gstate.xmargin + xlen;

      // Y axis grows up from chartyo.
      const double ylen = scale_value_on_range(vy, miny, maxy, 0, gheight);
      double y = chartyo - ylen;

      cpoints.push_back(std::make_tuple(x, y));
    }
  return cpoints;
}


/// Return set of images for image tooltips, one for each point.
/// @param aimg is size of image embedded inside svg element.
/// @pathprefix is the path to the directory with the store of images
/// @idimgbase is the root name for what will be document level unique names of
/// sequentially numbered images (say fximage-, for fximage-101 et al)
/// Expected, zero filled imageid.
/// 2025-06-26-android-15-ptablet-talkback-4usted-firefox_13188.webp
group_element
make_line_graph_images(const vrange& points, const graph_rstate& gstate,
		       const string imgprefix,
		       const string imgpath = "../filmstrip/",
		       const string imgext = ".webp")
{
  group_element g;
  g.start_element(gstate.title + "-tooltip-images");
  for (const point_2t p : points)
    {
      std::ostringstream oss;
      oss << std::setfill('0') << std::setw(5);
      oss << static_cast<uint>(std::get<0>(p));
      const string xms = oss.str();

      const string isrc = imgpath + imgprefix + xms + imgext;
      const string imgid = gstate.tooltip_id + xms;
      auto [ width, height ] = gstate.tooltip_area;

      image_element i;
      image_element::data di = { isrc, 0, 0, width, height };
      i.start_element(imgid);
      i.add_data(di, "anonymous", "hidden", "");
      //i.add_data(di, "anonymous", "", "none");
      i.finish_element();
      g.add_element(i);
    }
  g.finish_element();
  return g;
}


/// Make marker or composite markers for one marker location.
string
make_marker_instance(const marker_shape form, const point_2t& cpoint,
		     const style styl, const double radius,
		     const string tipstr = "", const string imgid = "")
{
  string pointstr;
  marker_element mrkr;
  element_base& mkr = mrkr;
  switch (form)
    {
    case marker_shape::circle:
      mkr = make_circle_marker(cpoint, styl, radius, tipstr, "", imgid);
      break;
    case marker_shape::triangle:
      mkr = make_polygon_marker(cpoint, styl, radius, 3, tipstr, imgid);
      break;
    case marker_shape::square:
      mkr = make_rect_marker(cpoint, styl, radius, tipstr, "", imgid);
      break;
    case marker_shape::hexagon:
      mkr = make_polygon_marker(cpoint, styl, radius, 6, tipstr, imgid);
      break;
    case marker_shape::octahedron:
      mkr = make_octahedron(cpoint, styl, radius);
      break;
    case marker_shape::icosahedron:
      mkr = make_icosahedron(cpoint, styl, radius);
      break;
    case marker_shape::sunburst:
      mkr = make_rect_rays(cpoint, styl, radius, 7);
      break;
    case marker_shape::x:
      {
	auto [ cx, cy ] = cpoint;
	string xform = transform::rotate(45, cx, cy);
	string xttr = mkr.make_transform_attribute(xform);
	mkr = make_path_center_mark(cpoint, styl, radius, radius / 3, xttr);
	break;
      }
    case marker_shape::blob:
      mkr = make_path_blob(cpoint, styl, radius, 5);
      break;
    case marker_shape::lauburu:
      mkr = make_lauburu(cpoint, styl, radius);
      break;
    case marker_shape::wave:
      mkr = make_path_ripple(cpoint, styl, radius * 1.5, 2, 2);
      break;
    default:
      string m("make_marker_instance:: ");
      m += "marker_shape (";
      m += to_string(form);
      m += ")";
      m += "is invalid or missing, skipping...";
      m += k::newline;
      std::clog << m << std::endl;
      break;
    }
  pointstr = mkr.str();
  return pointstr;
}


/// Return set of paths of marker shapes with text tooltips.
/// NB: For graph_mode >= chart_line_style_2
string
make_line_graph_markers(const vrange& points, const vrange& cpoints,
			const graph_rstate& gstate, const double radius,
			const string imgidbase = "")
{
  string ret;
  for (uint i = 0; i < points.size(); i++)
    {
      auto [ vx, vy ] = points[i];
      auto cpoint = cpoints[i];

      // If there are tooltip images, link in here.
      string imgid(imgidbase);
      if (!imgid.empty())
	{
	  std::ostringstream oss;
	  oss << std::setfill('0') << std::setw(5);
	  oss << static_cast<uint>(vx);
	  const string xms = oss.str();
	  string imgidn(imgid + xms);
	  imgid = script_element::tooltip_attribute(imgidn);
	}

      // Generate displayed tooltip text....
      string tipstr(gstate.title);
      tipstr += k::newline;
      tipstr += "week: ";
      tipstr += std::to_string(static_cast<uint>(vy));
      //tipstr += gstate.yticu;
      tipstr += k::newline;

      std::ostringstream oss;
      oss.imbue(std::locale(""));
      oss << vx;
      tipstr += oss.str();
      //tipstr += std::to_string(static_cast<uint>(vx));
      //tipstr += gstate.xticu;

      // Markers default to closed paths that are filled with no stroke.
      // Setting visible to vector | echo induces outline behavior.
      style styl = gstate.lstyle;
      styl._M_fill_opacity = 1;
      if (gstate.is_visible(select::echo))
	{
	  styl._M_fill_color = color::white;
	  styl._M_stroke_size = 0.5;
	}
      else
	styl._M_stroke_opacity = 0;

      const auto& form = gstate.sstyle.marker_form;
      ret += make_marker_instance(form, cpoint, styl, radius, tipstr, imgid);

      // Add additional marker or markers.
      // dr		== distance from p1 for echo/rep marker
      // shrinkf	== how much to reduce the echo/rep from the original
      const double dr = radius * 2;
      const double shrinkf(0.75);
      const ushort rep = gstate.sstyle.marker_reps;
      if (rep > 0 && i + 1 < points.size())
	{
	  // Find the next point, and then inch along along the line connecting.
	  auto [ cx1, cy1 ] = cpoint;
	  auto cpointnext = cpoints[i + 1];
	  auto [ cx2, cy2 ] = cpointnext;

	  // Calculate the vector from p1 to p2
	  double dx = cx2 - cx1;
	  double dy = cy2 - cy1;

	  // Calculate the distance between p1 and p2
	  double d = distance_cartesian(cpoint, cpointnext);

	  // Calculate the unit vector from p1 to p2
	  double unit_x = dx / d;
	  double unit_y = dy / d;

	  // Calculate by moving distance dr from p1 in the direction of the unit vector
	  double x3 = cx1 + dr * unit_x;
	  double y3 = cy1 + dr * unit_y;
	  point_2t rpoint(x3, y3);

	  ret += make_marker_instance(form, rpoint, styl, radius * shrinkf,
				      "", "");
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
make_line_graph_annotations(const vrange& points, const graph_rstate& gstate,
			    const point_2t xrange, const point_2t yrange,
			    const double xscale = 1, const double yscale = 1,
			    const typography typo = k::apercu_typo)
{
  using namespace std;

  // Locate graph area on plate area.
  auto [ pwidth, pheight ] = gstate.graph_area;
  double gwidth = pwidth - (2 * gstate.xmargin);
  double gheight = pheight - (2 * gstate.ymargin);
  const double chartyo = pheight - gstate.ymargin;
  const double chartxo = gstate.xmargin;
  const double chartxe = pwidth - gstate.xmargin;

  auto [ xmin, xmax ] = xrange;
  auto [ ymin, ymax ] = yrange;

  // Separate tic label values for each (x, y) axis, find ranges for each.
  const vrange cpoints = transform_to_graph_points(points, gstate,
						   xrange, yrange);
  auto [ maxx, maxy ] = max_vrange(points, gstate.xticdigits, xscale, yscale);

  const double xrangec(maxx - xmin);
  const double gxscale(gwidth / xrangec);
  const double yrangec(maxy - ymin);
  const double gyscale(gheight / yrangec);

  // Y axis is simpler, 0, 10, 20, ..., 80, 90, 100 in percent.
  const double ydelta = yrangec / gstate.yticdigits;

  // Derive the number of tick marks.

  // Use a multiple of 5 to make natural counting easier.
  // Start with an assumption of 20 tic marks for the x axis.
#if MOZ
  double xtickn(xrangec * 2); // .5 sec
  if (xtickn < 10)
    xtickn = 10;
  if (xtickn > 26)
    xtickn = xrangec;
#else
  double xtickn = 53;
#endif

  // X axis is seconds, xtickn minimum delta is 0.1 sec.
#if MOZ
  double xticmindelta = 0.1;
  double xdelta = std::max(xrangec / xtickn, xticmindelta);
  // Round up to significant digits, so if xdelta is 0.18 round to 0.2.
  xdelta = std::round(xdelta * gstate.xticdigits * 10) / (gstate.xticdigits * 10);
#else
  double xticmindelta = 1;
  double xdelta = std::max(xrangec / xtickn, xticmindelta);
#endif

  // Base typo for annotations.
  typography anntypo = typo;
  anntypo._M_style = k::wcagg_style;

  svg_element lanno(gstate.title, "line graph annotation",
		    gstate.graph_area, false);

  // Chart title.
  if (gstate.is_visible(select::title))
    {
      anntypo._M_size = graph_rstate::ttitlesz;
      anntypo._M_w = typography::weight::bold;

      const double lyo = graph_rstate::ymargin / 2;
      const double loffset = double(anntypo._M_size);
      point_2t xlabelp = make_tuple(pwidth / 2, lyo + loffset);
      styled_text(lanno, gstate.title, xlabelp, anntypo);
    }

  // Axes Lines and Tic Labels
  if (gstate.is_visible(select::axis))
    {
      lanno.add_raw(group_element::start_group("axes-" + gstate.title));

      anntypo._M_size = graph_rstate::th1sz;
      anntypo._M_w = typography::weight::medium;

      // Add axis labels.
      const double loffset = double(anntypo._M_size) * 1.5;

      const double lyo = pheight - graph_rstate::ymargin / 2;
      point_2t xlabelp = make_tuple(pwidth / 2, lyo + loffset);
      string xlabel = gstate.xlabel;
      std::transform(xlabel.begin(), xlabel.end(), xlabel.begin(),
		     [](unsigned char c){ return std::toupper(c); });
      styled_text(lanno, xlabel, xlabelp, anntypo);

      const double lxo = graph_rstate::xmargin / 2;
      point_2t ylabelp = make_tuple(lxo - loffset, pheight / 2);
      string ylabel = gstate.ylabel;
      std::transform(ylabel.begin(), ylabel.end(), ylabel.begin(),
		     [](unsigned char c){ return std::toupper(c); });
      styled_text_r(lanno, ylabel, ylabelp, anntypo, -90, ylabelp);

      // Add axis lines.
      if (gstate.is_visible(select::vector))
	{
	  line_element lx = make_line({chartxo, chartyo}, {chartxe, chartyo},
				      gstate.lstyle);
	  line_element ly = make_line({chartxo, chartyo}, {chartxo, gstate.ymargin},
				      gstate.lstyle);
	  lanno.add_element(lx);
	  lanno.add_element(ly);
	}

      anntypo._M_w = typography::weight::normal;
      lanno.add_raw(group_element::finish_group());
    }

  // Horizontal lines linking left and right y-axis tic label value to each other,
  // perhaps with magnification-ready micro text.
  if (gstate.is_visible(select::linex))
    {
      lanno.add_raw(group_element::start_group("tic-y-lines-" + gstate.title));

      style hlstyl = gstate.lstyle;
      hlstyl._M_stroke_color = color::gray05;

      anntypo._M_size = 3;
      anntypo._M_w = typography::weight::normal;

      for (double y = ymin + ydelta; y < maxy + ydelta; y += ydelta)
	{
	  // Base line layer.
	  const double yto = chartyo - (y * gyscale);
	  const double ytol = yto - (hlstyl._M_stroke_size / 2);
	  auto lxe = make_line({chartxo + graph_rstate::th1sz, ytol},
			       {chartxe - graph_rstate::th1sz, ytol}, hlstyl);
	  lanno.add_element(lxe);

	  // Add y-axis tic numbers along line for use when magnified.
	  if (gstate.is_visible(select::alt))
	    {
	      // Skip first and last as covered by either Y-axes tic marks.
	      for (double x = xmin + xdelta; x < maxx - xdelta; x += xdelta)
		{
		  const double xto = chartxo + (x * gxscale);
		  const string syui = std::to_string(static_cast<uint>(y)) + gstate.yticu;
		  styled_text(lanno, syui, {xto, yto}, anntypo);
		}
	    }
	}

      lanno.add_raw(group_element::finish_group());
    }

  // Generate tic marks
  const double ygo = gstate.ymargin + gheight + graph_rstate::th1sz;
  if (gstate.is_visible(select::ticks))
    {
      // Base typo for tic labels.
      // NB: Assume pointsx/pointsy are monotonically increasing.
      anntypo._M_size = graph_rstate::tticsz;
      anntypo._M_baseline = typography::baseline::central;

      // X tic labels
      lanno.add_raw(group_element::start_group("tic-x-" + gstate.title));
      for (uint i = 0; i < points.size(); i++)
	{
	  const double xto = std::get<0>(cpoints[i]);
	  const double x = std::get<0>(points[i]);

	  // Make sure data point range within domain min and max.
	  if (x <= xmax)
	    {
	      ostringstream oss;
#if MOZ
	      oss << fixed << setprecision(gstate.xticdigits) << x;
#else
	      oss << std::trunc(x);
#endif
	      const string sxui = oss.str() + gstate.xticu;
	      styled_text(lanno, sxui, {xto, ygo}, anntypo);
	    }
	}
      lanno.add_raw(group_element::finish_group());

      // Y tic labels
      // Positions for left and right y-axis tic labels.
      lanno.add_raw(group_element::start_group("tic-y-" + gstate.title));
      const double yticspacer = graph_rstate::th1sz * 2;
      const double xgol = gstate.xmargin - yticspacer;			// left
      const double xgor = gstate.xmargin + gwidth + yticspacer;         // right
      const double starty = ymin != 0 ? ymin : ymin + ydelta; // skip zero label
      for (double y = starty; y < maxy + ydelta; y += ydelta)
	{
	  const double yto = chartyo - (y * gyscale);
	  const string syui = std::to_string(static_cast<uint>(y)) + gstate.yticu;
	  styled_text(lanno, syui, {xgol, yto}, anntypo);
	  styled_text(lanno, syui, {xgor, yto}, anntypo);
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
/// @param metadata = image filename prefix for tooltips if present
svg_element
make_line_graph(const vrange& points, const graph_rstate& gstate,
		const point_2t xrange, const point_2t yrange,
		const double marker_radius = 3.0)
{
  using namespace std;
  const vrange cpoints = transform_to_graph_points(points, gstate,
						   xrange, yrange);

  // Plot path of points on cartesian plane.
  const string gname = gstate.title + "_line_graph";
  svg_element lgraph(gname, "line graph", gstate.graph_area, false);
  if (gstate.is_visible(select::vector))
    {
      if (gstate.mode == chart_line_style_1)
	{
	  // Use polyline and CSS-based marker_form all in one line on layer 1.
	  lgraph.add_raw(group_element::start_group("polyline-" + gstate.title));
	  polyline_element pl1 = make_polyline(cpoints, gstate.lstyle, gstate.sstyle);
	  lgraph.add_element(pl1);
	  lgraph.add_raw(group_element::finish_group());
	}
      if (gstate.mode == chart_line_style_2)
	{
	  // Use polyline base line on layer 1.
	  // Use set of marker points paths with value as text tooltips on layer 2.
	  lgraph.add_raw(group_element::start_group("polyline-" + gstate.title));
	  stroke_style no_markerstyle = gstate.sstyle;
	  no_markerstyle.marker_defs = "";
	  polyline_element pl1 = make_polyline(cpoints, gstate.lstyle, no_markerstyle);
	  lgraph.add_element(pl1);
	  lgraph.add_raw(group_element::finish_group());

	  // Markers + text tooltips.
	  lgraph.add_raw(group_element::start_group("markers-" + gstate.title));
	  string markers = make_line_graph_markers(points, cpoints, gstate, marker_radius);
	  lgraph.add_raw(markers);
	  lgraph.add_raw(group_element::finish_group());
	}
      if (gstate.mode == chart_line_style_3)
	{
	  string m("make_line_graph:: ");
	  m += "chart_line_style_3 requires use of different overloaded function";
	  m += k::newline;
	  throw std::runtime_error(m);
	}
    }

  return lgraph;
}


/// Line graph 3 needs more parameters.
svg_element
make_line_graph(const vrange& points, const vrange& tpoints, graph_rstate& gstate,
		const point_2t xrange, const point_2t yrange,
		const string metadata, script_element::scope scontext)
{
  using namespace std;
  const vrange cpoints = transform_to_graph_points(points, gstate,
						   xrange, yrange);

  // Plot path of points on cartesian plane.
  const string gname = gstate.title + "_line_graph";
  svg_element lgraph(gname, "line graph", gstate.graph_area, false);
  if (gstate.is_visible(select::vector))
    {
      if (gstate.mode == chart_line_style_3)
	{
	  // Use polyline base line on layer 1 of control points (subset points).
	  // Use set of control points marker paths with value as text tooltips on layer 2.
	  // Use set of image points (subset control points) image elements on layer 3.
	  lgraph.add_raw(group_element::start_group("polyline-" + gstate.title));
	  stroke_style no_markerstyle = gstate.sstyle;
	  no_markerstyle.marker_defs = "";
	  polyline_element pl1 = make_polyline(cpoints, gstate.lstyle, no_markerstyle);
	  lgraph.add_element(pl1);
	  lgraph.add_raw(group_element::finish_group());

	  // Markers + text tooltips, add image id + js to make image visible.
	  // Use simplified points, aka only the visual change points.
	  const vrange& ctpoints = transform_to_graph_points(tpoints, gstate,
							     xrange, yrange);
	  lgraph.add_raw(group_element::start_group("markers-" + gstate.title));
	  string markers = make_line_graph_markers(tpoints, ctpoints, gstate, 3,
						   gstate.tooltip_id);
	  lgraph.add_raw(markers);
	  lgraph.add_raw(group_element::finish_group());

	  // Add tool images to graph_rstate.
	  // Add this plus script at the same layer of the DOM, which varies.
	  const string imgprefix = metadata + k::hyphen + gstate.title + "_";
	  group_element ttips = make_line_graph_images(tpoints, gstate, imgprefix);
	  if (scontext == script_element::scope::element)
	    {
	      lgraph.add_element(ttips);
	      lgraph.add_raw(script_element::tooltip_script(scontext).str());
	    }
	  else
	    gstate.tooltip_images = ttips.str();
	}
    }

  return lgraph;
}

} // namepace svg

#endif
