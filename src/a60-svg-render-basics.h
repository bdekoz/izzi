// svg render basics -*- mode: C++ -*-

// Copyright (C) 2014-2026 Benjamin De Kosnik <b.dekosnik@gmail.com>

// This file is part of the alpha60-MiL SVG library.  This library is
// free software; you can redistribute it and/or modify it under the
// terms of the GNU General Public License as published by the Free
// Software Foundation; either version 3, or (at your option) any
// later version.

// This library is distributed in the hope that it will be useful, but
// WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// General Public License for more details.

#ifndef MiL_SVG_RENDER_BASICS_H
#define MiL_SVG_RENDER_BASICS_H 1

//#include "a60-svg-codecvt.h"
#include "a60-svg-radial-fill-hexagon.h"

namespace svg {

double
scale_proportional_to_area(double radius, double weight)
{
  // Scale proportional to area of generated circle.
  // π = 3.14159265358979323846
  const double pi(22/7);
  double a1 = radius * radius * pi;
  double ap = a1 * weight;
  double rpa = std::sqrt(ap/pi);
  return rpa;
}


double
scale_proportional_to_weight(double radius, double weight)
{
  double rpr = radius * weight;
  return rpr;
}


/// Text element at @param origin, with style and optional transform.
text_element
style_text(const string text, const point_2t origin, const typography typo,
	   const string xtransf = "")
{
  auto [ x, y ] = origin;
  text_element::data dt = { x, y, text, typo };
  text_element t;
  t.start_element();
  t.add_data(dt, xtransf);
  t.finish_element();
  return t;
}


/// Text element at @param origin, rotated with @param deg centered at
/// @param rorigin in a @param rr direction. With typograph and style
/// via @param typo.
text_element
style_text_r(const string text, const point_2t origin, const typography& typo,
	     const double deg, const point_2t rorigin,
	     const k::rrotation rr = k::rrotation::none)
{
  auto [ rx, ry ] = rorigin;
  typography typor(typo);
  typor._M_baseline = svg::typography::baseline::central;
  if (rr == k::rrotation::cw)
    {
      typor._M_anchor = svg::typography::anchor::start;
      typor._M_align = svg::typography::align::left;
    }
  if (rr == k::rrotation::ccw)
    {
      typor._M_anchor = svg::typography::anchor::end;
      typor._M_align = svg::typography::align::right;
    }
  const string tx = svg::transform::rotate(deg, rx, ry);
  text_element t = style_text(text, origin, typor, tx);
  return t;
}


/// Text at @param origin, with style and transform
void
styled_text(element_base& obj, const string text, const point_2t origin,
	    const typography typo, const string xform = "")
{
  text_element t = style_text(text, origin, typo, xform);
  obj.add_element(t);
}


/// Text at @param origin, with style and ...
/// a transformation=rotation of @param deg about origin.
void
styled_text_r(element_base& obj, const string text, const point_2t origin,
	      const typography typo, const double deg)
{
  text_element t = style_text_r(text, origin, typo, deg, origin);
  obj.add_element(t);
}


/// Text at @param origin, with style and ...
///  a transformation=rotation of @param deg about @param rorigin.
void
styled_text_r(element_base& obj, const string text, const point_2t origin,
	      const typography typo, const double deg, const point_2t rorigin)
{
  text_element t = style_text_r(text, origin, typo, deg, rorigin);
  obj.add_element(t);
}


/// Text at @param origin, with style and link.
void
styled_text_link(element_base& obj, const string text, const point_2t origin,
		 const typography typo, const string uri)
{
  // Convert uri to utf8 so that it can be inserted into svg_element
  // Fucking '&', mostly.
  // string uriconv = convert_to_utf8(uri);
  string uriconv(uri);
  size_t pos = 0;
  while ((pos = uriconv.find('&', pos)) != std::string::npos)
    {
      uriconv.replace(pos, 1, "&amp;");
      pos += 5;
    }

  string astart = "<a href=";
  astart += k::quote;
  astart += uriconv;
  astart += k::quote;
  astart +=">";
  astart += k::newline;
  obj.add_raw(astart);

  text_element t = style_text(text, origin, typo);
  obj.add_element(t);

  string afinish = "</a>";
  afinish += k::newline;
  obj.add_raw(afinish);
}


/// Text at size.
void
sized_text(element_base& obj, svg::typography typo, const int sz,
	   const string text, const int tx, const int ty)
{
  typo._M_size = sz;
  styled_text(obj, text, {tx, ty}, typo);
}


/// Text at size, with a transformation=rotation.
void
sized_text_r(element_base& obj, svg::typography typo, const int sz,
	     const string text, const int tx, const int ty, const double deg)
{
  typo._M_size = sz;
  styled_text_r(obj, text, {tx, ty}, typo, deg);
}


/// Text of maxlen length, overflow goes on line below.
uint
text_line_n(svg_element& obj, const point_2t origin, const string text,
	    const svg::typography typo, const int sz, const uint maxlen)
{
  auto [ x, y ] = origin;
  string textcut(text);
  while (textcut.size() > maxlen)
    {
      // Find last space character in the specified maxium range, aka mark.
      auto sppos = textcut.find_last_of(k::space, maxlen);
      if (sppos == string::npos)
	sppos = maxlen;
      else
	{
	  // Cut after space (mark).
	  sppos += 1;
	}

      string namesubs = textcut.substr(0, sppos);
      sized_text(obj, typo, sz, namesubs, x, y);
      textcut = textcut.substr(sppos);
      y += sz;
    }
  sized_text(obj, typo, sz, textcut, x, y);
  return y;
}


/// Text of maxlen length rotated, overflow goes on line below.
uint
text_line_n_r(svg_element& obj, const point_2t origin, const string text,
	      const svg::typography typo, const uint sz, const uint maxlen,
	      const uint lettingsz = 0)
{
  const auto [ x, y ] = origin;
  const double line_max = std::ceil(double(text.size()) / maxlen);
  const uint lines(line_max);

  uint xmin(x);
  string textcut(text);

  uint linen(0);
  while (linen < lines)
    {
      // Find x offset, with first line being the max length above the
      // origin, working down. The last line is at the origin.
      auto xp = x - ((sz + lettingsz) * (lines - linen - 1));

      size_t epos(0);
      if (textcut.size() < maxlen)
	epos = textcut.size();
      else
	{
	  // Find last space character in the specified maxium range, aka mark.
	  auto sppos = textcut.find_last_of(k::space, maxlen);
	  if (sppos == string::npos)
	    sppos = maxlen;
	  else
	    {
	      // Cut after space (mark).
	      sppos += 1;
	    }
	  epos = sppos;
	}

      string namesubs = textcut.substr(0, epos);
      sized_text_r(obj, typo, sz, namesubs, xp, y, -90);
      textcut = textcut.substr(epos);
      ++linen;
      xmin = std::min(xmin, uint(xp));
    }
  return xmin;
}


/// Create rect_element at origin
rect_element
make_rect(const point_2t origin, const style s, const area<> a,
	  const string filterstr = "", const string xform = "")
{
  auto [ width, height ] = a;
  auto [ x, y ] = origin;

  rect_element rect;
  rect_element::data dr = { x, y, width, height };
  rect.start_element();
  rect.add_data(dr);

  if (!xform.empty())
    rect.add_transform(xform);

  rect.add_style(s);

  // Add blur with filter here.
  if (!filterstr.empty())
    rect.add_filter(filterstr);

  rect.finish_element();
  return rect;
}


/// Create rect_element centered at origin
rect_element
make_rect_centered(const point_2t origin, const style s, const area<> a,
		   const string filterstr = "", const string xform = "")
{
  auto [ width, height ] = a;
  auto [ x, y ] = origin;
  x -= (width / 2);
  y -= (height / 2);
  point_2t oprime { x, y };

  rect_element rect = make_rect(oprime, s, a, filterstr, xform);
  return rect;
}


/// Create rectangle element with title and tooltip information.
rect_element
make_rect_marker(const point_2t origin, const style s,
		 const space_type r, const string title,
		 const string filterstr = "", const string imgid = "")
{
  auto [ cx, cy ] = origin;

  rect_element rect;
  rect_element::data dr = { cx - r, cy - r, 2 * r, 2 * r };
  rect.start_element();
  rect.add_data(dr);
  rect.add_style(s);
  if (!filterstr.empty())
    rect.add_filter(filterstr);
  if (!imgid.empty())
    rect.add_raw(imgid);
  rect.add_raw(element_base::finish_tag_hard);
  rect.add_title(title);
  rect.add_raw(string { rect_element::pair_finish_tag } + k::newline);

  return rect;
}


/// Make circle element.
/// @param origin is the point (x,y) that is the center of the circle
/// @param s is the visual style
/// @param r is the circle radius
/// @param xform is the transform, if any.
circle_element
make_circle(const point_2t origin, const style s,
	    const space_type r, const string xform = "")
{
  circle_element c;
  auto [ x, y ] = origin;
  circle_element::data dc = { x, y, r };
  c.start_element();
  c.add_data(dc, xform);
  c.add_style(s);
  c.finish_element();
  return c;
}


/// Make circle element with title and tooltip information.
/// @param origin is the point (x,y) that is the center of the circle
/// @param s is the visual style
/// @param r is the circle radius
/// @param title is the text to be displayed as a title tooltip
/// @param img is a link to be displayed as a hover tooltip
/// @param xform is the transform, if any.
circle_element
make_circle_marker(const point_2t origin, const style s,
		   const space_type r, const string title,
		   const string xform = "", const string imgid = "")
{
  circle_element c;
  auto [ x, y ] = origin;
  circle_element::data dc = { x, y, r };
  c.start_element();
  c.add_data(dc, xform);
  c.add_style(s);
  if (!imgid.empty())
    c.add_raw(imgid);
  c.add_raw(element_base::finish_tag_hard);
  c.add_title(title);
  c.add_raw(string { circle_element::pair_finish_tag } + k::newline);
  return c;
}


/// Draws a ring centered at origin of radius r, with outer and inner
/// radial gradient of blurspace in each direction.
/// klr == fade from
/// fadeklr == fade to. Background is transparent if none.
void
point_to_ring_halo(svg_element& obj, const point_2t origin,
		   const space_type radius, const double blurspace,
		   const svg::color klr,
		   const svg::color fadeklr = color::none,
		   const double opacity = 1)
{
  using atype = decltype(obj._M_area)::atype;

  auto [ xd, yd ] = origin;
  const atype x(xd);
  const atype y(yd);

  // outer ring == upper bound, radius + variance.
  const double oring = radius + blurspace;

  // inner ring == lower bound, radius - variance.
  const double iring = radius - blurspace;

  // mangled args for name.
  std::ostringstream oss;
  oss << "x" << std::to_string(x) << k::hyphen
      << "y" << std::to_string(y) << k::hyphen
      << "r" << std::to_string(radius) << k::hyphen
      << "blurspace" << std::to_string(blurspace);
  const string mangle(oss.str());

  // outer
  // strategy: make a bigger circle cprime, then do a radial gradient to it
  // starting gradient from color at radius to 100% white/tranparent at cprime.
  const string rgrado_name(string("radialout") + k::hyphen + mangle);
  radial_gradient rgrado;
  rgrado.start_element(rgrado_name);
  rgrado.stop(rgrado.offset_percentage(radius, oring), fadeklr, 0);
  rgrado.stop(rgrado.offset_percentage(radius, oring), klr, opacity);
  rgrado.stop("100%", color::white, 0);
  rgrado.finish_element();
  obj.add_element(rgrado);

  circle_element co;
  circle_element::data dco = { x, y, atype(oring) };
  co.start_element();
  co.add_data(dco);
  co.add_fill(rgrado_name);
  co.finish_element();
  obj.add_element(co);

  // inner
  // strategy: make a smaller circle cprime, then do a radial gradient from it
  // starting gradient from white/transparent at cprime to color at r.
  const string rgradi_name(string("radialin") + k::hyphen + mangle);
  radial_gradient rgradi;
  rgradi.start_element(rgradi_name);
  rgradi.stop(rgradi.offset_percentage(iring, radius), fadeklr, 0);
  rgradi.stop("100%", klr, opacity);
  rgradi.finish_element();
  obj.add_element(rgradi);

  // Float/Int conversion and rounding, add one to radius to close gap.
  circle_element ci;
  circle_element::data dci = { x, y, radius + 1 };
  ci.start_element();
  ci.add_data(dci);
  ci.add_fill(rgradi_name);
  ci.finish_element();
  obj.add_element(ci);
}


/// Make polygon element.
/// @param origin is the point (x,y) that is the center of the circle
/// @param s is the visual style
/// @param r is the circle radius
/// @param xform is the transform, if any.
polygon_element
make_polygon(const vrange& points, const style s)
{
  polygon_element c;
  c.start_element();
  c.add_data(points);
  c.add_style(s);
  c.finish_element();
  return c;
}


/// Line primitive.
line_element
make_line(const point_2t origin, const point_2t end, style s,
	  const string dasharray = "")
{
  auto [ xo, yo ] = origin;
  auto [ xe, ye ] = end;
  line_element l;
  line_element::data dr = { xo, xe, yo, ye };
  l.start_element();
  l.add_data(dr, dasharray);
  l.add_style(s);
  l.finish_element();
  return l;
}


/// Lines radiating from center point (x,y).
group_element
make_line_rays(const point_2t origin, const style s,
	       const space_type r = 4, const uint nrays = 10)
{
  // End points on the ray.
  // Pick a random ray, use an angle in the range [0, 2pi].
  static std::mt19937_64 rg(std::random_device{}());
  auto distr = std::uniform_real_distribution<>(0.0, 2 * 22/7);
  auto disti = std::uniform_int_distribution<>(-3, 3);
  auto [ x, y ] = origin;

  group_element g;
  g.start_element("rays-" + std::to_string(nrays) + "-" + std::to_string(r));
  for (uint i = 0; i < nrays; ++i)
    {
      double theta = distr(rg);
      double rvary = disti(rg);

      double xe = x + (r + rvary) * std::cos(theta);
      double ye = y + (r + rvary) * std::sin(theta);

      line_element::data dr = { x, xe, y, ye };
      line_element ray;
      ray.start_element();
      ray.add_data(dr);
      ray.add_style(s);
      ray.finish_element();

      g.add_element(ray);
    }
  g.finish_element();
  return g;
}


/// Rectangles of varios sizes rotated from center point (x,y).
group_element
make_rect_rays(const point_2t origin, const style s,
	       const space_type r = 4, const uint nrays = 10)
{
  // End points on the ray.
  // Pick a random ray, use an angle in the range [0, 2pi].
  static std::mt19937_64 rg(std::random_device{}());
  auto distr = std::uniform_real_distribution<>(0.4, 1.7);
  //auto disti = std::uniform_int_distribution<>(-3, 3);
  auto [ x, y ] = origin;

  const space_type rwidth = r / 5; // center mark uses r/3
  group_element g;
  g.start_element("rrays-" + std::to_string(nrays) + "-" + std::to_string(r));
  for (uint i = 0; i < nrays; ++i)
    {
      //double theta = distr(rg);
      //double rvary = r + disti(rg);
      double rvary = r * distr(rg);
      area<> a = { rwidth, rvary };
      string rotate = svg::transform::rotate(36 * i, x, y);
      rect_element ray = make_rect_centered(origin, s, a, "", rotate) ;
      g.add_element(ray);
    }
  g.finish_element();
  return g;
}


/// Polyline primitive.
/// @param points the points in the polyline
/// @param s style for the polyline
/// @param s stroke_style for the polyline
polyline_element
make_polyline(const vrange& points, const style s,
	      const stroke_style sstyle = { })
{
  polyline_element pl(points);
  pl.start_element();
  pl.add_data(sstyle);
  pl.add_style(s);
  pl.finish_element();
  return pl;
}


/// Angle in radians.
point_2t
get_circumference_point_r(const double angler, const double r,
			  const point_2t origin)
{
  auto [ cx, cy ] = origin;
  double x(cx + (r * std::cos(angler)));
  double y(cy - (r * std::sin(angler)));
  return std::make_tuple(x, y);
}


/// Angle in degrees.
point_2t
get_circumference_point_d(const double ad, const double r,
			  const point_2t origin)
{
  double angler = (k::pi / 180.0) * ad;
  return get_circumference_point_r(angler, r, origin);
}


/// Zero degrees is top, going clockwise (cw).
double
zero_angle_north_cw(double angled)
{
 // Change rotation to CW instead of CCW (or anti-clockwise).
  angled = 360 - angled;

  // Rotate 90 CCW, so that the first element will be at the top
  // vertical axis, instead of the right middle axis.
  angled += 90;

  return angled;
}


/// Zero degrees is top, going clockwise (cw).
double
zero_angle_north_ccw(double angled)
{
  // Rotate 90 CCW, so that the first element will be at the top
  // vertical axis, instead of the right middle axis.
  angled += 90;

  return angled;
}


/// Make single path segment.
string
make_path_data_from_points(const vrange& lpoints)
{
  std::ostringstream ossa;
  for (uint i = 0; i < lpoints.size(); ++i)
    {
      auto [x, y ] = lpoints[i];
      // SVG path_element.
      // start at "M x y" and
      // each subsequent line segment is of form "L x y"
      if (i == 0)
	ossa << "M" << k::space;
      else
	ossa << "L" << k::space;
      ossa << x << svg::k::space << y << k::space;
    }
  return ossa.str();
}


/// Draw path given serialized path data.
/// Can be used to make pinstripes, ie top and bottom line layers.
/// top style defaults: fill opac(0), stroke opac(1), stroke sz 1
/// bottom style defaults: fill opac(0), stroke opac(1), stroke sz 1.25
path_element
make_path(const string& pathda, const style& styl, const string id = "",
	  const bool selfclosingtagp = true, const string xattr = "")
{
  // Draw path with this endpoint.
  path_element pe;
  if (to_string(styl._M_stroke_color) != to_string(svg::color::none))
    {
      path_element::data da = { pathda, 0 };
      if (id.empty())
	pe.start_element();
      else
	pe.start_element(id);
      pe.add_data(da);
      pe.add_style(styl);
      if (!xattr.empty())
	pe.add_raw(xattr);
      if (selfclosingtagp)
	pe.finish_element();
      else
	pe.add_raw(element_base::finish_tag_hard);
    }
  return pe;
}


/// Center an polygon at this point.
/// radius 4 is pixels to draw out from center point.
/// pointsn is number of points to draw (8 for octogon)
path_element
make_path_polygon(const point_2t origin, const style s,
		  const double r, const uint pointsn,
		  const bool selfclosingtagp = true, const string xattr = "")
{
  // Find points: orig, orig + (120 x 1), orig + (120 x 2).
  const double angle(360.0/pointsn);
  double zo = zero_angle_north_cw(angle);

  // Find n points on a circle, connnected.
  vrange pointz;
  for (uint i = 0; i < pointsn; ++i)
    {
      point_2t p = get_circumference_point_d(zo + (angle * i), r, origin);
      pointz.push_back(p);
    }

  // Final point to close path.
  pointz.push_back(pointz.front());
  string pathda = make_path_data_from_points(pointz);

  const string id = "polygon-n" + std::to_string(pointsn) + "-r" + std::to_string(r);
  path_element polyg = make_path(pathda, s, id, selfclosingtagp, xattr);
  return polyg;
}


/// Make a polygon marker for line graphs.
path_element
make_polygon_marker(const point_2t origin, const style s,
		    const double r, const uint pointsn, const string title,
		    const string xattr = "")
{
  // XXX imagid
  path_element polyg = make_path_polygon(origin, s, r, pointsn, false, xattr);
  polyg.add_title(title);
  polyg.add_raw(string { circle_element::pair_finish_tag } + k::newline);
  return polyg;
}


/// Make path segment between two points on a circumference of radius r.
/// Points like: get_circumference_point_d(zero_angle_north_cw(0), r, origin)
string
make_path_arc_circumference(const point_2t& start, const point_2t& end,
			    const space_type r, const int arcflag = 0,
			    const int sweepflag = 1)
{
  // Define arc.
  // A rx ry x-axis-rotation large-arc-flag sweep-flag x y
  std::ostringstream oss;
  oss << "M" << k::space << to_string(start) << k::space;
  oss << "A" << k::space;
  oss << std::to_string(r) << k::space << std::to_string(r) << k::space;
  oss << 0 << k::space << arcflag << k::space << sweepflag << k::space;
  oss << to_string(end) << k::space;
  return oss.str();
}


/// Make closed path between two points and the center of a circle of radius r.
/// Points like: get_circumference_point_d(zero_angle_north_cw(0), r, origin)
string
make_path_arc_closed(const point_2t& origin, const point_2t& start,
		     const point_2t& end, const space_type r,
		     const int arcflag = 0, const int sweepflag = 0)
{
  // Define path as starting at origin, line to circumference point start,
  // arc to circumfernce point end, line back to origin.
  // A rx ry x-axis-rotation large-arc-flag sweep-flag x y
  // where (large) arc flag is true if arc angle delta is > 180
  // where sweep flag is
  // true if outer (movement CW)
  // false if inner (CCW).
  std::ostringstream oss;
  oss << "M" << k::space << to_string(origin) << k::space;
  oss << "L" << k::space << to_string(start) << k::space;
  oss << "A" << k::space;
  oss << std::to_string(r) << k::space << std::to_string(r) << k::space;
  oss << 0 << k::space << arcflag << k::space << sweepflag << k::space;
  oss << to_string(end) << k::space;
  oss << "L" << k::space << to_string(origin) << k::space;
  return oss.str();
}


/// Same but with degree range arguments instead of points.
/// NB: Assumes appropriate zero_angle_north_cw/ccw adjustments on startd/endd.
string
make_path_arc_closed(const point_2t& origin, const double startd,
		     const double endd, const space_type r,
		     const int arcflag = 0, const int sweepflag = 0)
{
  const point_2t start = get_circumference_point_d(startd, r, origin);
  const point_2t end = get_circumference_point_d(endd, r, origin);
  return make_path_arc_closed(origin, start, end, r, arcflag, sweepflag);
}


/// Make blob shape
/// @param origin center of the shape.
/// @param s style for the polyline
/// @param size radius of mark
path_element
make_path_blob(const point_2t origin, const style s, const double size)
{
  auto [ ox, oy ] = origin;
  std::srand(std::time(0));

  // Generate main points
  const int numCurves = 5 + std::rand() % 4; // 5-8 curves
  vrange points;
  vrange controlPoints;
  for (int i = 0; i < numCurves; i++)
    {
      double angle = (2 * k::pi * i) / numCurves;
      double variation = 0.5 + (std::rand() % 100) / 100.0;
      double radius = size * variation;

      double x = ox + radius * cos(angle);
      double y = oy + radius * sin(angle);
      points.push_back({x, y});

      // Generate control point for this segment
      double controlAngle = angle + k::pi / numCurves;
      double controlRadius = size * (0.3 + 0.4 * (std::rand() % 100) / 100.0);
      double cx = ox + controlRadius * cos(controlAngle);
      double cy = oy + controlRadius * sin(controlAngle);
      controlPoints.push_back({cx, cy});
    }

  // Start the path data
  std::stringstream data;
  auto [ p0x, p0y ] = points[0];
  data << "M" << p0x << "," << p0y << k::space;

  // Create smooth quadratic curves
  for (int i = 0; i < numCurves; i++)
    {
      int nextIdx = (i + 1) % numCurves;

      // Use control point from next segment for smooth transition
      auto [ cx, cy ] = controlPoints[nextIdx];
      auto [ pix, piy ] = points[i];
      auto [ pnx, pny ] = points[nextIdx];

      // Adjust control point to ensure smooth connection
      double smoothX = (pix + pnx) / 2;
      double smoothY = (piy + pny) / 2;

      cx = cx * 0.6 + smoothX * 0.4;
      cy = cy * 0.6 + smoothY * 0.4;

      data << " Q" << cx << "," << cy << " " << pnx << "," << pny;
    }

  data << " Z";
  string pdata = data.str();
  path_element pth = make_path(pdata, s);
  return pth;
}


/// Plus or x tilt mark as closed path that can be filled.
path_element
make_path_center_mark(const point_2t& origin, const style styl,
		      const int len, const int width,
		      const string xform = "")
{
  // Define path as starting at origin, move half width up and to left then
  // move around as if making a plus sign.
  const auto [ xo, yo ] = origin;

  // Move to top left of origin, start here.
  const double whalf(width / 2);
  const int lenw = len - whalf;
  const auto x = xo - whalf;
  const auto y = yo - whalf;

  std::ostringstream oss;
  oss << "M" << k::space << x << k::comma << y << k::space;

  // left
  oss << "H" << k::space << x - lenw << k::space;
  oss << "V" << k::space << y + width << k::space;
  oss << "H" << k::space << x << k::space;

  // bottom
  oss << "V" << k::space << y + lenw + width << k::space;
  oss << "H" << k::space << x + width << k::space;
  oss << "V" << k::space << y + width << k::space; // bottom part

  // right
  oss << "H" << k::space << x + lenw + width << k::space;
  oss << "V" << k::space << y << k::space;
  oss << "H" << k::space << x + width << k::space;

  // top
  oss << "V" << k::space << y - lenw << k::space;
  oss << "H" << k::space << x << k::space;
  oss << "V" << k::space << y << k::space;

  const string pathdata = oss.str();

  string id("center-mark-");
  string attr(std::to_string(width) + "-" + std::to_string(len));
  id += attr;

  path_element cm = make_path(pathdata, styl, id, true, xform);
  return cm;
}


/// Make waves.
/// @param points the points in the polyline
/// @param s style for the polyline
/// @param s stroke_style for the polyline
polyline_element
make_path_ripple(const point_2t origin, const style s,
		 const double length, const double amp, const double periods)
{
  auto [ ox, oy ] = origin;
  vrange points;
  points.push_back(origin);
  for (int i = 0; i <= 100; i++)
    {
      double x = ox + (i * length) / 100.0;
      double y = oy + amp * sin((i * periods * 2 * k::pi) / 100.0);
      points.push_back({x, y});
    }

  polyline_element pl = make_polyline(points, s);
  return pl;
}


// Hexagon and tessalations.

/// Center rings of hexagons at this point.
/// @param origin is the center point
/// @param r is the radius/side length of hexagon.
/// @param hexn is the number of hexagons total
/// @param cfillp is the center of the hexagon filled or open
/// @param styl apply as style to this element
/// @param xform any optional transform
group_element
make_hexagon_honeycomb(const point_2t origin, const double r,
		       const uint hexn, const bool cfillp,
		       const style styl, const string xform = "")
{
  using std::to_string;

  group_element g;
  string gbase = "hexagon-honeycomb-";
  string gname = gbase + to_string(uint(r)) + k::hyphen + to_string(hexn);
  g.start_element(gname, xform);

  auto hexpoints = radiate_hexagon_honeycomb(origin, r, hexn, cfillp);
  for (const auto& phex : hexpoints)
    {
      // Make hexagon spiral.
      //auto [ p, d ] = phex;
      path_element pth = make_path_polygon(phex, styl, r, 6);
      g.add_element(pth);
    }

  g.finish_element();
  return g;
}


/// Center rings of text in a hexagon pattern at this point.
/// @param origin is the center point
/// @param r is the radius/side length of hexagon.
/// @param hexn is the number of hexagons total
/// @param cfillp is the center of the hexagon filled or open
/// @param s is the text
/// @param typo is the typography for the text
/// @param xform any optional transform
group_element
make_text_honeycomb(const point_2t origin, const double r,
		    const uint hexn, const bool cfillp, const string s,
		    const typography typo, const string xform = "")
{
  using std::to_string;

  group_element g;
  string gbase = "text-honeycomb-";
  string gname = gbase + to_string(uint(r)) + k::hyphen + to_string(hexn);
  g.start_element(gname, xform);

  // Group all text objects here, and specify that rotation for the
  // group is to be centered at the origin.
  auto [ x, y ] = origin;
  const string txrotatepoint = svg::transform::rotate(0, x, y);
  group_element ginner;
  ginner.start_element(gbase + "inner", txrotatepoint);

  auto hexpoints = radiate_hexagon_honeycomb(origin, r, hexn, cfillp);
  auto hexangles = get_honeycomb_angles(origin, hexpoints, true);
  for (uint i = 0; !s.empty() && i < hexpoints.size(); i++)
    {
      const auto& p = hexpoints[i];
      const double d = hexangles[i];
      text_element t = style_text_r(s, p, typo, d, p, k::rrotation::cw);
      //text_element t = style_text_r(s, p, typo, d, origin, k::rrotation::cw);
      ginner.add_element(t);
    }

  ginner.finish_element();

  g.add_element(ginner);
  g.finish_element();

  return g;
}


/// Make octahedron shape (8) in 2D simulated 3D
/// @ret group element of polygon_elements
//polyline_element
group_element
make_octahedron(const point_2t origin, const style& s, const double radius)
{
  // 1. Define the 6 vertices of a regular octahedron in 3D
  std::array<point_3t, 6> vertices =
    {{
      { radius, 0, 0}, {-radius, 0, 0},  // Right, Left
      { 0, radius, 0}, { 0, -radius, 0},  // Top, Bottom
      { 0, 0, radius}, { 0, 0, -radius}  // Front, Back
    }};

  // 2. Define the 8 triangular faces using vertex indices
  // Top pyramid faces
  std::vector<std::array<int, 3>> faces =
    {
      {2, 0, 4}, {2, 4, 1}, {2, 1, 5}, {2, 5, 0}, // Top 4
      {3, 0, 4}, {3, 4, 1}, {3, 1, 5}, {3, 5, 0}  // Bottom 4
    };

  auto lisoproject = [](const point_3t p3, const point_2t p2)
  {
    // Standard isometric projection coefficients
    auto [ x2, y2 ] = p2;
    auto [ x3, y3, z3 ] = p3;
    double x2d = (x3 - z3) * 0.866;
    double y2d = y3 + (x3 + z3) * 0.5;
    point_2t pp = { x2 + x2d, y2 - y2d };
    return pp;
  };

  group_element go;
  go.start_element("polygon-oct-r-" + std::to_string(radius));
  for (const auto& face : faces)
    {
      point_2t p1 = lisoproject(vertices[face[0]], origin);
      point_2t p2 = lisoproject(vertices[face[1]], origin);
      point_2t p3 = lisoproject(vertices[face[2]], origin);
      vrange points = { p1, p2, p3 };
      polygon_element plyg = make_polygon(points, s);
      go.add_element(plyg);
    }
  go.finish_element();
  return go;



#if 0
  auto [ x, y ] = origin;
  const double PI = 3.14159265358979323846;
  const uint sides = 8;

  // Calculate all vertices
  vrange vertices;
  for (uint i = 0; i < sides; ++i)
    {
      double angle = i * (2 * PI / sides);
      point_2t p = { x + radius * cos(angle), y + radius * sin(angle) };
      vertices.push_back(p);
    }

  // Draw faces as polygons, group as meta polygon.
  // Outer group.
  group_element go;
  go.start_element("polygon-oct-r-" + std::to_string(radius));

  for (uint i = 0; i < sides; ++i)
    {
      int next = (i + 1) % sides;
      vrange points;
      points.push_back({ x, y });
      point_2t& pvi = vertices[i];
      points.push_back(pvi);
      point_2t& pnext = vertices[next];
      points.push_back(pnext);

      polygon_element plyg = make_polygon(points, s);
      go.add_element(plyg);
    }

  go.finish_element();
  return go;
#endif

#if 0
  /// XXX this isn't ending correctly and cannot be filled
  auto [ centerX, centerY ] = origin;

  // Octahedron vertices (6 vertices)
  double vertices[6][3] =
    {
      {1, 0, 0}, {-1, 0, 0},  // Right/Left
      {0, 1, 0}, {0, -1, 0},  // Top/Bottom
      {0, 0, 1}, {0, 0, -1}   // Front/Back
    };

  // Project vertices
  int projX[6];
  int projY[6];
  for (int i = 0; i < 6; i++)
    {
      projX[i] = centerX + radius * (vertices[i][0] * 0.707 - vertices[i][2] * 0.707);
      projY[i] = centerY + radius * (vertices[i][0] * 0.408 + vertices[i][1] * 0.816 + vertices[i][2] * 0.408);
  }

  // Octahedron edges (12 edges)
  int edges[12][2] =
    {
      {0,2}, {0,3}, {0,4}, {0,5},  // From right vertex
      {1,2}, {1,3}, {1,4}, {1,5},  // From left vertex
      {2,4}, {2,5}, {3,4}, {3,5}   // Top/bottom connections
    };

  vrange points;
  for (int i = 0; i < 12; i++)
    {
      int v1 = edges[i][0];
      int v2 = edges[i][1];
      point_2t p1 = { projX[v1], projY[v1] };
      point_2t p2 = { projX[v2], projY[v2] };
      points.push_back(p1);
      points.push_back(p2);
    }
  polyline_element p = make_polyline(points, s);
  return p;
#endif

#if 0
  double vertices[6][3] =
    {
      {1, 0, 0}, {-1, 0, 0}, {0, 1, 0}, {0, -1, 0}, {0, 0, 1}, {0, 0, -1}
    };

  auto [ centerX, centerY ] = origin;
  int projX[6], projY[6];
  for (int i = 0; i < 6; i++)
    {
      projX[i] = centerX + radius * (vertices[i][0] * 0.707 - vertices[i][2] * 0.707);
      projY[i] = centerY + radius * (vertices[i][0] * 0.408 + vertices[i][1] * 0.816 + vertices[i][2] * 0.408);
    }

  // Define faces with colors
  struct OctaFace
  {
    int v[3];
    std::string color;
  };

  OctaFace faces[8] =
    {
      {{0, 2, 4}, "#FF6B6B"}, {{1, 2, 4}, "#4ECDC4"},
      {{0, 2, 5}, "#45B7D1"}, {{1, 2, 5}, "#96CEB4"},
      {{0, 3, 4}, "#FECA57"}, {{1, 3, 4}, "#FF9F43"},
      {{0, 3, 5}, "#EE5A24"}, {{1, 3, 5}, "#C4E538"}
    };

  // Draw faces as polygons, group as meta polygon.
  // Outer group.
  group_element go;
  go.start_element("polygon-oct-r-" + std::to_string(radius));
  for (int i = 0; i < 8; i++)
    {
      vrange points;
      for (int j = 0; j < 3; j++)
	{
	  point_2t pt = { projX[faces[i].v[j]], projY[faces[i].v[j]] };
	  points.push_back(pt);
	}

      polygon_element plyg = make_polygon(points, s);
      go.add_element(plyg);
    }
  go.finish_element();
  return go;
#endif
}


/// Make icosahedron shape (20) in 2D simulated 3D
//make_icosahedron_3d(int centerX, int centerY, int size)
group_element
make_icosahedron(const point_2t origin, const style& s, const double radius)
{
  auto [ centerX, centerY ] = origin;

  // Icosahedron vertices (12 vertices)
  double phi = (1 + sqrt(5)) / 2; // Golden ratio
  double vertices[12][3] =
    {
      {0, 1, phi}, {0, 1, -phi}, {0, -1, phi}, {0, -1, -phi},
      {1, phi, 0}, {1, -phi, 0}, {-1, phi, 0}, {-1, -phi, 0},
      {phi, 0, 1}, {phi, 0, -1}, {-phi, 0, 1}, {-phi, 0, -1}
    };

  // Normalize and scale vertices
  double normalized[12][3];
  for (int i = 0; i < 12; i++)
    {
      double length = sqrt(vertices[i][0]*vertices[i][0] +
			   vertices[i][1]*vertices[i][1] +
			   vertices[i][2]*vertices[i][2]);
      normalized[i][0] = vertices[i][0] / length;
      normalized[i][1] = vertices[i][1] / length;
      normalized[i][2] = vertices[i][2] / length;
    }

  // Project vertices
  int projX[12], projY[12];
  for (int i = 0; i < 12; i++)
    {
      projX[i] = centerX + radius * (normalized[i][0] * 0.707 - normalized[i][2] * 0.707);
      projY[i] = centerY + radius * (normalized[i][0] * 0.408 + normalized[i][1] * 0.816 + normalized[i][2] * 0.408);
    }

  // Icosahedron edges (30 edges)
  int edges[30][2] =
    {
      {0,2}, {0,4}, {0,6}, {0,8}, {0,10},    // From vertex 0
      {1,3}, {1,4}, {1,6}, {1,9}, {1,11},    // From vertex 1
      {2,5}, {2,7}, {2,8}, {2,10},           // From vertex 2
      {3,5}, {3,7}, {3,9}, {3,11},           // From vertex 3
      {4,6}, {4,8}, {4,9},                   // From vertex 4
      {5,7}, {5,8}, {5,9},                   // From vertex 5
      {6,10}, {6,11},                        // From vertex 6
      {7,10}, {7,11},                        // From vertex 7
      {8,9}, {10,11}                         // Remaining edges
    };

  group_element g;
  g.start_element("icosahedron-" + std::to_string(radius));
  for (int i = 0; i < 30; i++)
    {
      int v1 = edges[i][0];
      int v2 = edges[i][1];
      point_2t p1 = { projX[v1], projY[v1] };
      point_2t p2 = { projX[v2], projY[v2] };
      line_element l = make_line(p1, p2, s);
      g.add_element(l);
    }
  g.finish_element();
  return g;
}


/// Make grid palette for display.
/// NB @param klrs can be color_qis or array/palette.
svg_element
display_color_qis(const auto& klrs,
		  const area<> a, const typography& typobase)
{
  const auto [ awidth, aheight ] = a;

  typography typo = typobase;
  typo._M_style = k::w_style;
  typo._M_size = 9;
  typo._M_align = typography::align::left;
  typo._M_baseline = typography::baseline::central;
  typo._M_anchor = typography::anchor::start;

  // Draw out colors.
  double rwidth = 20;
  double rheight = 80;
  auto rspace = 4;
  auto typsz = 7;

  // Turn off RAII.
  svg_element obj("color_qis_" + std::to_string(klrs.size()) + "_palette",
		  a, false);
  obj.start();

  auto x = rwidth, y = rheight;
  auto xoffset = 0;
  for (const auto& klr : klrs)
    {
      // Color block
      const style s = { klr, 1.0, klr, 0.0, 2 };
      point_2t p = { x + xoffset, y };
      rect_element r = make_rect_centered(p, s, { rwidth, rheight});
      obj.add_element(r);

      // Label.
      sized_text_r(obj, typo, typsz, to_string(klr),
		   x + xoffset, y - rheight / 2 + rspace, 90);

      if (xoffset + rwidth + rspace < awidth - rwidth - rspace)
	xoffset += rwidth + rspace;
      else
	{
	  xoffset = 0;
	  y += (rheight + rspace + rspace);
	}
    }

  obj.finish_element();
  return obj;
}

} // namespace svg

#endif
