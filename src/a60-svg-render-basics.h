// svg render basics -*- mode: C++ -*-

// Copyright (C) 2014-2020 Benjamin De Kosnik <b.dekosnik@gmail.com>

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

int
scale_proportional_to_area(int radius, int weight)
{
  // Scale proportional to area of generated circle.
  // π = 3.14159265358979323846
  const double pi(22/7);
  double a1 = radius * radius * pi;
  double ap = a1 * weight;
  int rpa = std::sqrt(ap/pi);
  return rpa;
}


int
scale_proportional_to_weight(int radius, int weight)
{
  int rpr = radius * weight;
  return rpr;
}


/// Scale value from min to max on range (nfloor, nceil).
double
normalize_value_on_range(const size_type value, const size_type min,
			 const size_type max,
			 const size_type nfloor, const size_type nceil)
{
  double rmultp(nceil - nfloor);
  double valnum(value - min);
  double valdenom(max - min);
  double weightn = (rmultp * (valnum / valdenom)) + nfloor;
  return weightn;
}



/// Take input size and make a one channel (single-image) SVG form.
svg_element
make_svg_1_channel(const int deltax, const int deltay, const string& outbase)
{
  using namespace svg;
  area<> a = { deltax, deltay };
  return svg_element(outbase, a);
}


void
make_1_channel_insert(svg_element& obj, string insert1)
{
  if (!insert1.empty())
    {
      rect_element r1;
      r1.str(insert1);
      obj.add_element(r1);
    }
}


/// Take input size and make a two channel (single-image) SVG form.
svg_element
make_svg_2_channel(const int deltax, const int deltay, const string& outbase)
{
  using namespace svg;
  area<> a = { 2 * deltax, deltay };
  return svg_element(outbase, a);
}


void
make_2_channel_insert(svg_element& obj, string insert1, string insert2)
{
  if (!insert1.empty())
    {
      rect_element r1;
      r1.str(insert1);
      obj.add_element(r1);
    }

  if (!insert2.empty())
    {
      rect_element r2;
      r2.str(insert2);
      obj.add_element(r2);
    }
}


/// Text at @origin, with a transformation=rotation of @deg about @origin add  .
void
styled_text(svg_element& obj, const string text, const point_2t origin,
	    svg::typography typo)
{
  auto [ x, y ] = origin;
  text_element::data dt = { size_type(x), size_type(y), text, typo };
  text_element t;
  t.start_element();
  t.add_data(dt);
  t.finish_element();
  obj.add_element(t);
}


/// Text at @origin, with a transformation=rotation of @deg about @origin add  .
void
styled_text_r(svg_element& obj, const string text, const point_2t origin,
	      svg::typography typo, const double deg, const point_2t rorigin)
{
  auto [ x, y ] = origin;
  auto [ rx, ry ] = rorigin;
  text_element::data dt = { size_type(x), size_type(y), text, typo };
  text_element t;
  t.start_element();
  t.add_data(dt, svg::transform::rotate(deg, rx, ry));
  t.finish_element();
  obj.add_element(t);
}


/// Text at size.
void
sized_text(svg_element& obj, svg::typography typo, const int sz,
	   const string text, const int tx, const int ty)
{
  typo._M_size = sz;
  text_element::data dt = { tx, ty, text, typo };
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
  typo._M_size = sz;
  text_element::data dt = { tx, ty, text, typo };
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


/// Line between two points.
void
points_to_line(svg_element& obj, const svg::style s,
	       const point_2t origin, const point_2t end,
	       const string dasharray = "")
{
  auto [ xo, yo ] = origin;
  auto [ xe, ye ] = end;
  line_element l;
  line_element::data dr = { int(xo), int(xe), int(yo), int(ye) };
  l.start_element();
  l.add_data(dr, dasharray);
  l.add_style(s);
  l.finish_element();
  obj.add_element(l);
}


/// Point to rectangle.
void
point_2d_to_rect(svg_element& obj, double x, double y, svg::style s,
		 int width = 4, int height = 4)
{
  rect_element r;
  using size_type = svg::size_type;
  size_type xi = static_cast<size_type>(std::round(x));
  size_type yi = static_cast<size_type>(std::round(y));
  rect_element::data dr = { xi, yi, width, height };

  r.start_element();
  r.add_data(dr);
  r.add_style(s);
  r.finish_element();
  obj.add_element(r);
}


/// Center a rectangle at this point.
void
point_to_rect_centered(svg_element& obj, const point_2t origin, svg::style s,
		       int width = 4, int height = 4)
{
  auto [ x, y ] = origin;
  x -= (width / 2);
  y -= (height / 2);
  point_2d_to_rect(obj, x, y, s, width, height);
}


/// Point to rectangle blur
void
point_2d_to_rect_blur(svg_element& obj, double x, double y, svg::style s,
		      string filterstr, int width = 4, int height = 4)
{
  rect_element r;
  using size_type = svg::size_type;
  size_type xi = static_cast<size_type>(std::round(x));
  size_type yi = static_cast<size_type>(std::round(y));
  rect_element::data dr = { xi, yi, width, height };

  r.start_element();
  r.add_data(dr);
  r.add_style(s);
  r.add_filter(filterstr);
  r.finish_element();
  obj.add_element(r);
}


/// Draws a circle around a point (x,y), of style (s), of radius (r).
void
point_2d_to_circle(svg_element& obj, double x, double y, svg::style s,
		   const int r = 4, const string transform = "")
{
  circle_element c;
  using size_type = svg::size_type;
  size_type xi = static_cast<size_type>(std::round(x));
  size_type yi = static_cast<size_type>(std::round(y));
  circle_element::data dc = { xi, yi, r };

  c.start_element();
  c.add_data(dc, transform);
  c.add_style(s);
  c.finish_element();
  obj.add_element(c);
}


/// Draws a ring centered at origin of radius r, with outer and inner
/// radial gradient of blurspace in each direction.
/// klr == fade from
/// fadeklr == fade to. Background is transparent if none.
void
point_2d_to_ring_halo(svg_element& obj, const point_2t origin,
		      const size_type radius, const double blurspace,
		      const svg::color klr,
		      const svg::color fadeklr = color::none,
		      const double opacity = 1)
{
  auto [ xd, yd ] = origin;
  const size_type x(xd);
  const size_type y(yd);

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
  circle_element::data dco = { x, y, static_cast<size_type>(oring) };
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
		int r = 4, const uint nrays = 10)
{
  using size_type = svg::size_type;

  size_type xi = static_cast<size_type>(std::round(x));
  size_type yi = static_cast<size_type>(std::round(y));

  // End points on the ray.
  // Pick a random ray, use an angle in the range [0, 2pi].
  static std::mt19937_64 rg(std::random_device{}());
  auto distr = std::uniform_real_distribution<>(0.0, 2 * 22/7);
  auto disti = std::uniform_int_distribution<>(-3, 3);

  for (uint i = 0; i < nrays; ++i)
    {
      double theta = distr(rg);
      double rvary = disti(rg);

      size_type xe = xi + (r + rvary) * std::cos(theta);
      size_type ye = yi + (r + rvary) * std::sin(theta);

      line_element::data dr = { xi, xe, yi, ye };
      line_element ray;
      ray.start_element();
      ray.add_data(dr);
      ray.add_style(s);
      ray.finish_element();
      obj.add_element(ray);
    }
}


/// Angle in radians.
point_2t
get_circumference_point(const double angler, const double r,
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
  return get_circumference_point(angler, r, origin);
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


void
place_ray_at_angle(svg_element& obj, const point_2t& origin,
		   const point_2t& circump, const style& s,
		   const string id = "")
{
  auto [xo, yo] = origin;
  auto [xc, yc] = circump;

  line_element::data dr = { int(xo), int(xc), int(yo), int(yc) };
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


/// Make path segment between two points on a circumference of radius r.
/// Points like: get_circumference_point_d(zero_angle_north_cw(0), r, origin)
string
make_path_arc_circumference(const point_2t& start, const point_2t& end,
			    const int r, const int arcflag = 0,
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
		     const point_2t& end, const int r,
		     const int arcflag = 0, const int sweepflag = 0)
{
  // Define path as starting at origin, line to circumference point start,
  // arc to circumfrence point end, line back to origin.
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
		     const double endd, const int r,
		     const int arcflag = 0, const int sweepflag = 0)
{
  const point_2t start = get_circumference_point_d(startd, r, origin);
  const point_2t end = get_circumference_point_d(endd, r, origin);
  return make_path_arc_closed(origin, start, end, r, arcflag, sweepflag);
}


/// Plus or x tilt mark as closed path.
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


/// Point to center mark as crossed lines.
void
point_to_plus_lines(svg_element& obj, const style& styl,
		    const point_2t origin, const int radius)
{
  auto d0 = zero_angle_north_cw(0);
  auto d6 = zero_angle_north_cw(180);
  auto d3 = zero_angle_north_cw(90);
  auto d9 = zero_angle_north_cw(270);
  point_2t p0 = get_circumference_point_d(d0, radius, origin);
  point_2t p6 = get_circumference_point_d(d6, radius, origin);
  point_2t p3 = get_circumference_point_d(d3, radius, origin);
  point_2t p9 = get_circumference_point_d(d9, radius, origin);
  points_to_line(obj, styl, p0, p6);
  points_to_line(obj, styl, p9, p3);
}


/// Import svg file, convert it to svg_element for insertion.
/// ifile is a plain SVG file with a 1:1 aspect ratio.
string
file_to_svg_insert(const string ifile)
{
  string isvg;

  // Read SVG to insert.
  std::ifstream ifs(ifile);
  if (ifs.good())
    {
      // Strip out any XML version line in the SVG file.
      // Search for and discard lines with "xml version", iff exists
      string xmlheader;
      getline(ifs, xmlheader);
      if (xmlheader.find("xml version") == string::npos)
	ifs.seekg(0, ifs.beg);

      std::ostringstream oss;
      oss << ifs.rdbuf();
      isvg = oss.str();
    }
  else
    {
      string m("svg_file_to_svg_insert:: insert nested SVG failed ");
      m += ifile;
      m += k::newline;
      throw std::runtime_error(m);
    }
  return isvg;
}


/// Import svg file, convert it to svg_element for insertion.
string
element_to_svg_insert(const string isvgpre)
{
  string isvg;

  // Read SVG to insert, remove anything previous to <svg"
  auto svgepos = isvgpre.find("<svg version");
  if (svgepos != string::npos)
    {
      // Strip out any XML version line in the SVG file.
      // Search for and discard lines with "xml version", iff exists
      isvg = isvgpre.substr(svgepos);
    }
  else
    {
      string m("svg_element_to_svg_insert:: insert nested SVG failed of size: ");
      m += std::to_string(isvgpre.size());
      m += k::newline;
      m += "of body:";
      m += k::newline;
      m += isvgpre;
      throw std::runtime_error(m);
    }
  return isvg;
}


/// Embed svg in group element.
/// @origin is where glyph placement is inside containing svg element.
/// @origsize is original file width/height constant
/// @isize is final  width/height
/// @isvg is the string from one of the two functions above (*_to_svg_insert).
/// @syl is overide style information: defaults to no_style.
///
/// NB This only works is the file has no styles set in svg, group, or
/// individual element definitions (like circle, path, rectangle,
/// etc.).
///
/// See: https://developer.mozilla.org/en-US/docs/Web/SVG/Element/svg
svg_element
insert_svg_at(svg_element& obj, const string isvg,
	      const point_2t origin, const double origsize, const double isize,
	      const double angled = 0, const style& styl = k::no_style)
{
  // offset
  auto [ objx, objy ] = origin;
  const int x = objx - (isize / 2);
  const int y = objy - (isize / 2);
  string xformtranslate(transform::translate(x, y));

  // scaled
  string xformscale;
  if (isize != origsize)
    {
      const double scalex(isize / origsize);
      xformscale = transform::scale(scalex);
    }

  // rotated
  string xformrotate;
  if (angled != 0)
    {
      xformrotate = transform::rotate(90 - angled, objx, objy);
    }

  // Order of transformations matters...
  string ts(xformrotate + k::space + xformtranslate + k::space + xformscale);

  group_element gsvg;
  gsvg.start_element("inset svg", transform(), ts, styl);
  gsvg.add_raw(isvg);
  gsvg.finish_element();
  obj.add_element(gsvg);

  return obj;
}

} // namespace svg

#endif
