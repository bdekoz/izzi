// svg render basics -*- mode: C++ -*-

// Copyright (C) 2014-2025 Benjamin De Kosnik <b.dekosnik@gmail.com>

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


/// Text at @origin, with style.
void
styled_text(svg_element& obj, const string text, const point_2t origin,
	    svg::typography typo)
{
  using atype = decltype(obj._M_area)::atype;
  auto [ x, y ] = origin;
  text_element::data dt = { atype(x), atype(y), text, typo };
  text_element t;
  t.start_element();
  t.add_data(dt);
  t.finish_element();
  obj.add_element(t);
}


/// Text at @origin, with style and transform
void
styled_text(svg_element& obj, const string text, const point_2t origin,
	    svg::typography typo, const string xform)
{
  using atype = decltype(obj._M_area)::atype;
  auto [ x, y ] = origin;
  text_element::data dt = { atype(x), atype(y), text, typo };
  text_element t;
  t.start_element();
  t.add_data(dt, xform);
  t.finish_element();
  obj.add_element(t);
}


/// Text at @origin, with style and ...
///  a transformation=rotation of @deg about @rorigin.
void
styled_text_r(svg_element& obj, const string text, const point_2t origin,
	      svg::typography typo, const double deg)
{
  using atype = decltype(obj._M_area)::atype;
  auto [ x, y ] = origin;
  text_element::data dt = { atype(x), atype(y), text, typo };
  text_element t;
  t.start_element();
  t.add_data(dt, svg::transform::rotate(deg, x, y));
  t.finish_element();
  obj.add_element(t);
}


/// Text at @origin, with style and ...
///  a transformation=rotation of @deg about @rorigin.
void
styled_text_r(svg_element& obj, const string text, const point_2t origin,
	      svg::typography typo, const double deg, const point_2t rorigin)
{
  using atype = decltype(obj._M_area)::atype;
  auto [ x, y ] = origin;
  auto [ rx, ry ] = rorigin;
  text_element::data dt = { atype(x), atype(y), text, typo };
  text_element t;
  t.start_element();
  t.add_data(dt, svg::transform::rotate(deg, rx, ry));
  t.finish_element();
  obj.add_element(t);
}


/// Text at @origin, with style and link.
void
styled_text_link(svg_element& obj, const string text, const point_2t origin,
		 svg::typography typo, const string uri)
{
  using atype = decltype(obj._M_area)::atype;
  auto [ x, y ] = origin;
  text_element::data dt = { atype(x), atype(y), text, typo };
  text_element t;
  t.start_element();
  t.add_data(dt);
  t.finish_element();

  string astart = "<a href=";
  astart += k::quote;
  astart += uri;
  astart += k::quote;
  astart +=">";
  astart += k::newline;
  obj.add_raw(astart);

  obj.add_element(t);

  string afinish = "</a>";
  afinish += k::newline;
  obj.add_raw(afinish);
}



/// Text at size.
void
sized_text(svg_element& obj, svg::typography typo, const int sz,
	   const string text, const int tx, const int ty)
{
  using atype = decltype(obj._M_area)::atype;
  typo._M_size = sz;
  text_element::data dt = { atype(tx), atype(ty), text, typo };
  text_element t;
  t.start_element();
  t.add_data(dt);
  t.finish_element();
  obj.add_element(t);
}


/// Text at size, with a transformation=rotation.
void
sized_text_r(svg_element& obj, svg::typography typo, const int sz,
	     const string text, const int tx, const int ty, const double deg)
{
  using atype = decltype(obj._M_area)::atype;
  typo._M_size = sz;
  text_element::data dt = { atype(tx), atype(ty), text, typo };
  text_element t;
  t.start_element();
  t.add_data(dt, svg::transform::rotate(deg, tx, ty));
  t.finish_element();
  obj.add_element(t);
}


/// Text of maxlen length, overflow goes on line below.
uint
text_line_n(svg_element& obj, const point_2t origin, const string text,
	    svg::typography typo, const int sz, const uint maxlen)
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
	      svg::typography typo, const uint sz, const uint maxlen,
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


/// Line primitive.
line_element
make_line(const point_2t origin, const point_2t end, svg::style s,
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


/// Line between two points.
void
points_to_line(svg_element& obj, const svg::style s,
	       const point_2t origin, const point_2t end,
	       const string dasharray = "")
{
  line_element l = make_line(origin, end, s, dasharray);
  obj.add_element(l);
}


/// Point to rectangle.
void
point_2d_to_rect(svg_element& obj, double x, double y, svg::style s,
		 double width = 4, double height = 4)
{
  using atype = decltype(obj._M_area)::atype;
  rect_element r;
  rect_element::data dr = { atype(x), atype(y), width, height };
  r.start_element();
  r.add_data(dr);
  r.add_style(s);
  r.finish_element();
  obj.add_element(r);
}


/// Point to rectangle blur
void
point_2d_to_rect_blur(svg_element& obj, double x, double y, svg::style s,
		      string filterstr, double width = 4, double height = 4)
{
  using atype = decltype(obj._M_area)::atype;

  rect_element r;
  rect_element::data dr = { atype(x), atype(y), width, height };

  r.start_element();
  r.add_data(dr);
  r.add_style(s);
  r.add_filter(filterstr);
  r.finish_element();
  obj.add_element(r);
}


/// Center a rectangle at this point.
void
point_to_rect_centered(svg_element& obj, const point_2t origin, svg::style s,
		       double width = 4, double height = 4)
{
  auto [ x, y ] = origin;
  x -= (width / 2);
  y -= (height / 2);
  point_2d_to_rect(obj, x, y, s, width, height);
}


/// Draws a circle around a point (x,y), of style (s), of radius (r).
void
point_2d_to_circle(svg_element& obj, double x, double y, svg::style s,
		   const space_type r = 4, const string xform = "")
{
  using atype = decltype(obj._M_area)::atype;
  circle_element c;
  using atype = decltype(obj._M_area)::atype;
  circle_element::data dc = { atype(x), atype(y), r };
  c.start_element();
  c.add_data(dc, xform);
  c.add_style(s);
  c.finish_element();
  obj.add_element(c);
}


/// Draws a circle around a point (x,y), of style (s), of radius (r).
void
point_to_circle(svg_element& obj, const point_2t origin, svg::style s,
		   const space_type r = 4, const string xform = "")
{
  using atype = decltype(obj._M_area)::atype;
  circle_element c;
  auto [ x, y ] = origin;
  circle_element::data dc = { atype(x), atype(y), r };
  c.start_element();
  c.add_data(dc, xform);
  c.add_style(s);
  c.finish_element();
  obj.add_element(c);
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


/// Lines radiating from center point (x,y).
void
point_2d_to_ray(svg_element& obj, double x, double y, svg::style s,
		space_type r = 4, const uint nrays = 10)
{
  using atype = decltype(obj._M_area)::atype;

  // End points on the ray.
  // Pick a random ray, use an angle in the range [0, 2pi].
  static std::mt19937_64 rg(std::random_device{}());
  auto distr = std::uniform_real_distribution<>(0.0, 2 * 22/7);
  auto disti = std::uniform_int_distribution<>(-3, 3);

  for (uint i = 0; i < nrays; ++i)
    {
      double theta = distr(rg);
      double rvary = disti(rg);

      atype xe = x + (r + rvary) * std::cos(theta);
      atype ye = y + (r + rvary) * std::sin(theta);

      line_element::data dr = { atype(x), xe, atype(y), ye };
      line_element ray;
      ray.start_element();
      ray.add_data(dr);
      ray.add_style(s);
      ray.finish_element();
      obj.add_element(ray);
    }
}


void
place_ray_at_angle(svg_element& obj, const point_2t& origin,
		   const point_2t& circump, const style& s,
		   const string id = "")
{
  using atype = decltype(obj._M_area)::atype;

  auto [xo, yo] = origin;
  auto [xc, yc] = circump;

  line_element::data dr = { atype(xo), atype(xc), atype(yo), atype(yc) };
  line_element ray;

  if (id.empty())
    ray.start_element();
  else
    ray.start_element(id);
  ray.add_data(dr);
  ray.add_style(s);
  ray.finish_element();
  obj.add_element(ray);
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
/// Assumes pinstripe, ie top and bottom line layers.
/// top style defaults: fill opac(0), stroke opac(1), stroke sz 1
/// bottom style defaults: fill opac(0), stroke opac(1), stroke sz 1.25
string
draw_path_data(const string& pathda,
	       const style& topstyl, const style& bottomstyl)
{
  std::ostringstream oss;

  // Draw path with this endpoint.
  path_element::data da = { pathda, 0 };

  // Draw colored line and outline. Draw outline first, so
  // it will be on the lower layer in the SVG file.
  if (to_string(bottomstyl._M_stroke_color) != to_string(svg::color::none))
    {
      path_element pebase;
      pebase.start_element();
      pebase.add_data(da);
      pebase.add_style(bottomstyl);
      pebase.finish_element();
      oss << pebase.str() << k::space;
    }

  path_element pe;
  pe.start_element();
  pe.add_data(da);
  pe.add_style(topstyl);
  pe.finish_element();
  oss << pe.str() << k::space;

  return oss.str();
}


/// Center a triangle at this point.
path_element
make_path_triangle(const point_2t origin, svg::style s,
		   const double r = 4, const double angle = 120)
{
  // Find points: orig, orig + (120 x 1), orig + (120 x 2).
  double zo = zero_angle_north_cw(angle);
  point_2t p1 =  get_circumference_point_d(zo, r, origin);
  point_2t p2 =  get_circumference_point_d(zo + (angle * 1), r, origin);
  point_2t p3 =  get_circumference_point_d(zo + (angle * 2), r,  origin);
  vrange pointz = { p1, p2, p3, p1 };
  string pathda = make_path_data_from_points(pointz);

  // Make closed path.
  path_element tri(true);
  path_element::data pthdata = { pathda, 0 };
  string argname = std::to_string(r) + k::hyphen + std::to_string(angle);
  tri.start_element("triangle-" + argname);
  tri.add_data(pthdata);
  tri.add_style(s);
  tri.finish_element();
  return tri;
}


/// Center a triangle at this point.
void
point_to_triangle(svg_element& obj, const point_2t origin, svg::style s,
		  const double r = 4, const double angle = 120)
{
  path_element tri = make_path_triangle(origin, s, r, angle);
  obj.add_element(tri);
}


/// Center an octogon at this point.
/// radius 4 is pixels to draw out from center point.
/// pointsn is number of points to draw (8 for octogon)
path_element
make_path_octogon(const point_2t origin, svg::style s, const double r = 4,
		  const uint pointsn = 8)
{
  // Find points: orig, orig + (120 x 1), orig + (120 x 2).
  const double angle(360.0/8);
  double zo = zero_angle_north_cw(angle);

  // Eight points on a circle, connnected.
  vrange pointz;
  for (uint i = 0; i < pointsn; ++i)
    {
      point_2t p = get_circumference_point_d(zo + (angle * i), r, origin);
      pointz.push_back(p);
    }

  // Final point to close path.
  pointz.push_back(pointz.front());
  string pathda = make_path_data_from_points(pointz);

  // Make closed path.
  path_element oct(true);
  path_element::data pthdata = { pathda, 0 };
  oct.start_element("octogon-" + std::to_string(r));
  oct.add_data(pthdata);
  oct.add_style(s);
  oct.finish_element();
  return oct;
}


/// Center an octogon at this point.
void
point_to_octogon(svg_element& obj, const point_2t origin, svg::style s,
		  const double r = 4)
{
  path_element oct = make_path_octogon(origin, s, r);
  obj.add_element(oct);
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


/// Plus or x tilt mark as closed path that can be filled.
string
make_path_center_mark(const point_2t& origin, const int len, const int width)
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

  return oss.str();
}


/// Crossed lines, no fill. X marks the ....
string
make_crossed_lines(const point_2t origin, svg::style s, const double radius,
		   const double tiltd = 0.0)
{
  auto d0 = zero_angle_north_cw(0 + tiltd);
  auto d6 = zero_angle_north_cw(180 + tiltd);
  auto d3 = zero_angle_north_cw(90 + tiltd);
  auto d9 = zero_angle_north_cw(270 + tiltd);
  point_2t p0 = get_circumference_point_d(d0, radius, origin);
  point_2t p6 = get_circumference_point_d(d6, radius, origin);
  point_2t p3 = get_circumference_point_d(d3, radius, origin);
  point_2t p9 = get_circumference_point_d(d9, radius, origin);

  line_element l1 = make_line(p0, p6, s);
  line_element l2 = make_line(p3, p9, s);

  std::ostringstream oss;
  oss << l1.str();
  oss << l2.str();
  return oss.str();
}


/// Point to center mark as crossed lines.
/// Default is a plus sign at origin, but @tiltd can rotate.
void
point_to_crossed_lines(svg_element& obj, const point_2t origin,
		       const style& styl, const int radius,
		       const double tiltd = 0.0)
{
  string pl = make_crossed_lines(origin, styl, radius, tiltd);
  obj.add_raw(pl);
}

} // namespace svg

#endif
