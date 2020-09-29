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


/// Text at size
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


/// Text at size, with a transformation=rotation added.
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


/// Text at size, arranged around an origin of a circle with radius r.
void
radial_text_r(svg_element& obj, const typography& typo,
	      string text, int tx, int ty, const double deg = 0.0)
{
  typography typot(typo);
  typot._M_a = svg::typography::anchor::start;
  typot._M_align = svg::typography::align::left;

  if (deg > 0)
    sized_text_r(obj, typot, typot._M_size, text, tx, ty, 360 - deg);
  else
    sized_text(obj, typot, typot._M_size, text, tx, ty);
}


/// Pont to rectangle.
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
/// radial gradient of fuzzpercentage * radius in each direction.
/// Background is transparent.
void
point_2d_to_ring_halo(svg_element& obj, const point_2t origin,
		      const size_type radius, const double fuzzpercentage,
		      const svg::color klr, const double opacity = 1)
{
  auto [ xd, yd ] = origin;
  const size_type x(xd);
  const size_type y(yd);

  const size_type variance = std::round(radius * fuzzpercentage);

  // outer ring == upper bound, radius + variance.
  const size_type oring = radius + variance;

  // inner ring == lower bound, radius - variance.
  const size_type iring = radius - variance;

  // mangled args for name.
  std::ostringstream oss;
  oss << "x" << std::to_string(x) << k::hyphen
      << "y" << std::to_string(y) << k::hyphen
      << "r" << std::to_string(radius) << k::hyphen
      << "fuzz" << std::to_string(variance);
  const string mangle(oss.str());

  // outer
  // strategy: make a bigger circle cprime, then do a radial gradient to it
  // starting gradient from color at radius to 100% white/tranparent at cprime.
  const string rgrado_name(string("radialout") + k::hyphen + mangle);
  radial_gradient rgrado;
  rgrado.start_element(rgrado_name);
  rgrado.stop(rgrado.offset_percentage(radius, oring), color::none, 0);
  rgrado.stop(rgrado.offset_percentage(radius, oring), klr, opacity);
  rgrado.stop("100%", color::white, 0);
  rgrado.finish_element();
  obj.add_element(rgrado);

  circle_element co;
  circle_element::data dco = { x, y, oring };
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
  rgradi.stop(rgradi.offset_percentage(iring, radius), color::none, 0);
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


double
align_angle_to_glyph(double angled)
{
 // Change rotation to CW instead of CCW (or anti-clockwise).
  angled = 360 - angled;

  // Rotate 90 CCW, so that the first element will be at the top
  // vertical axis, instead of the right middle axis.
  angled += 90;

  return angled;
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
/// Points like: get_circumference_point_d(align_angle_to_glyph(0), r, origin)
string
make_path_arc_circumference(const point_2t& start, const point_2t& end,
			    const int r)
{
  // Define arc.
  // A rx ry x-axis-rotation large-arc-flag sweep-flag x y
  std::ostringstream oss;
  oss << "M" << k::space << to_string(start) << k::space;
  oss << "A" << k::space;
  oss << std::to_string(r) << k::space << std::to_string(r) << k::space;
  oss << 0 << k::space << 0 << k::space << 1 << k::space;
  oss << to_string(end) << k::space;
  return oss.str();
}


/// Make closed path between two points and the center of a circle of radius r.
/// Points like: get_circumference_point_d(align_angle_to_glyph(0), r, origin)
string
make_path_arc(const point_2t& origin, const point_2t& start,
	      const point_2t& end, const int r)
{
  // Define path as starting at origin, line to circumference point start,
  // arc to circumfrence point end, line back to origin.
  // A rx ry x-axis-rotation large-arc-flag sweep-flag x y
  std::ostringstream oss;
  oss << "M" << k::space << to_string(origin) << k::space;
  oss << "L" << k::space << to_string(start) << k::space;
  oss << "A" << k::space;
  oss << std::to_string(r) << k::space << std::to_string(r) << k::space;
  oss << 0 << k::space << 0 << k::space << 0 << k::space;
  oss << to_string(end) << k::space;
  oss << "L" << k::space << to_string(origin) << k::space;
  return oss.str();
}


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


string
make_path_arc(const point_2t& origin, const double startd, const double endd,
	      const int r)
{
  auto alignstartd = align_angle_to_glyph(startd);
  const point_2t start = get_circumference_point_d(alignstartd, r, origin);

  auto alignendd = align_angle_to_glyph(endd);
  const point_2t end = get_circumference_point_d(alignendd, r, origin);
  return make_path_arc(origin, start, end, r);
}


/// Embed svg in group element.
/// origin is where glyph placement is inside containing svg element.
/// iflile is a plain SVG file with a 1:1 aspect ratio.
/// isize is image width/height
svg_element
insert_svg_at(svg_element& obj, const string ifile,
	      const point_2t origin, const double origsize, const double isize,
	      const double angled = 0)
{
  // Read SVG to insert.
  std::ifstream ifs(ifile);
  string isvg;
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
      string m("insert_svg_at_center:: insert nested SVG failed ");
      m += ifile;
      m += k::newline;
      throw std::runtime_error(m);
    }

  // Insert nested SVG element.

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
  gsvg.start_element("inset radial svg", transform(), ts);
  gsvg.add_raw(isvg);
  gsvg.finish_element();
  obj.add_element(gsvg);

  return obj;
}

} // namespace svg

#endif
