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


/// Tile at size
void
sized_text(svg_element& obj, svg::typography typo, int sz, string title,
	   int tx, int ty)
{
  typo._M_size = sz;
  text_element::data dt = { tx, ty, title, typo };
  text_element t;
  t.start_element();
  t.add_data(dt);
  t.finish_element();
  obj.add_element(t);
}


/// Same, with a transformation=rotation added.
void
sized_text_r(svg_element& obj, svg::typography typo, int sz, string title,
	     int tx, int ty, svg::transform, int deg)
{
  typo._M_size = sz;
  text_element::data dt = { tx, ty, title, typo };
  text_element t;
  t.start_element();
  t.add_data(dt, svg::transform::rotate(deg, tx, ty));
  t.finish_element();
  obj.add_element(t);
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


/// Make path segment between two points on a circumfrence of radius r.
/// Points like: get_circumference_point_d(align_angle_to_glyph(0), r, origin)
string
make_path_circular_arc(const point_2t& start, const point_2t& end, const int r)
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
