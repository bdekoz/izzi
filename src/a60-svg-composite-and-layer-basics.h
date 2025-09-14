// svg composite and layer basics -*- mode: C++ -*-

// Copyright (C) 2014-2020, 2025 Benjamin De Kosnik <b.dekosnik@gmail.com>

// This file is part of the alpha60-MiL SVG library.  This library is
// free software; you can redistribute it and/or modify it under the
// terms of the GNU General Public License as published by the Free
// Software Foundation; either version 3, or (at your option) any
// later version.

// This library is distributed in the hope that it will be useful, but
// WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// General Public License for more details.

#ifndef MiL_SVG_COMPOSITE_AND_LAYER_BASICS_H
#define MiL_SVG_COMPOSITE_AND_LAYER_BASICS_H 1


namespace svg {

/// Take input size and make a one channel (single-image) SVG form.
svg_element
make_svg_1_channel(const space_type deltax, const space_type deltay,
		   const string& outbase)
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
make_svg_2_channel(const space_type deltax, const space_type deltay,
		   const string& outbase)
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


/// Paint the edges of a physical page.
/// Assumes page is square.
////
/// @param rlen is height of painted rectangle from edge.
/// NB: 0.125 is a common bleed == 12 pixels.
/// First attempt was, common bleed plus 2 pixel "on page" -> 14. Not enough.
void
paint_edges_with_char_index(svg_element& obj, const area<> a, const char firstc,
			    const svg::color klr = color::duboisgreen2,
			    const double rlen = 24.0)
{
  // Paint edges.
  const std::locale loc("");
  const bool alphap = std::isalpha(firstc, loc);

  // Position the first character from (a to z + digit } to x values 0 to 26.
  // Find this index, with numbers and symbols at the end.
  const uint maxc = 27;
  uint cidx(maxc - 1);
  if (alphap)
    cidx = (static_cast<uint>(firstc) % 65) - 32;

  const double deltac = double(std::min(a._M_width, a._M_height)) / maxc;
  double deltax(deltac * cidx);
  double deltay(deltac * cidx);

  style estyl = svg::k::b_style;
  estyl.set_colors(klr);

  // Top edge, right to left starting outer to inner
  point_to_rect(obj, { a._M_width - deltax - rlen - deltac, 0 },
		estyl, deltac, rlen);

  // Bottom edge, left to right starting innner to outer.
  point_to_rect(obj, { deltax + rlen, a._M_height - rlen },
		   estyl, deltac, rlen);

  // Right side edge, up from bottom to top.
  point_to_rect(obj,
		{ a._M_width - rlen, a._M_height - deltay - rlen - deltac },
		estyl, rlen, deltac);
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
      string m("element_to_svg_insert:: insert nested SVG failed of size: ");
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
/// @param obj is containing svg
/// @param origin is where glyph placement is inside containing svg element.
/// @param origsize is original file width/height constant
/// @param isize is final  width/height
/// @param isvg is the raw svg string to insert, assumes _M_lifetime == false.
/// @param styl is overide style information: defaults to no_style.
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
  gsvg.start_element("inset svg", ts, styl);
  gsvg.add_raw(isvg);
  gsvg.finish_element();
  obj.add_element(gsvg);

  return obj;
}


/// Take @param obj as some kind of inner svg element, and embed it as
/// a nested svg at a location centered at @param pos on the outer
/// svg.
/// Returns a svg_element that can then be add_element from outer svg.
svg_element
nest_inner_svg_element_centered(const svg_element& obj, const point_2t& p)
{
  // Find centered position.
  const auto a = obj._M_area;
  const auto [ width, height ] = a;
  const auto [ xo, yo ] = p;

  bool outofbounds(false);

  double x(0);
  if (xo > (width / 2))
    x = xo - (width / 2);
  else
    outofbounds = true;

  double y(0);
  if (yo > (height / 2))
    y = yo - (height / 2);
  else
    outofbounds = true;

  if (outofbounds)
    throw std::runtime_error("nest_inner_svg_element_centered::out of bounds");

  svg_element nested_obj("inner-" + obj._M_name, a, false);
  nested_obj.start_element({x, y}, a);
  nested_obj.add_element(obj);
  nested_obj.finish_element();
  return nested_obj;
}


/// Take @param obj as some kind of inner element_base, and embed it as
/// a nested svg at a location centered at @param pos on the outer
/// svg.
/// Returns a svg_element that can then be add_element from outer svg.
svg_element
nest_inner_element(const element_base& eb, const point_2t& p,
		   const area<> a, const string name,
		   const bool centerp = true)
{
  // Find centered position.
  const auto [ width, height ] = a;
  const auto [ xo, yo ] = p;

  double x(xo);
  double y(yo);

  if (centerp)
    {
      bool outofbounds(false);

      if (xo > (width / 2))
	x = xo - (width / 2);
      else
	outofbounds = true;

      if (yo > (height / 2))
	y = yo - (height / 2);
      else
	outofbounds = true;

      if (outofbounds)
	throw std::runtime_error("nest_inner_element::out of bounds");
    }

  svg_element nested_obj("inner-" + name, a, false);
  nested_obj.start_element({x, y}, a);
  nested_obj.add_element(eb);
  nested_obj.finish_element();
  return nested_obj;
}


/// Composite frame on bleed.
/// For printed objects with a center gutter, some intra-page
/// adjustments are necessary.
/// @param slxt is odd/even (left/right)
/// @param bleedin is bleed size for one edge in inches. (1/8)
/// @param bleedxoffset is distance from spine pushed outward.
void
composite_bleed_areas(svg_element& obj,
		      const svg::select slxt, const double bleedin,
		      const double bleedxoffset = 0)
{
  const double bleedpx = get_dpi() * bleedin;
  const double bleedpxo = get_dpi() * (bleedin + (bleedxoffset / 2));

  if (slxt == svg::select::odd)
    {
      // LHS
      // Individual glyph shapes grid.
      // Left, center vertical and move to right edge horizontal.
      const point_2t p = { bleedpx, bleedpxo };
      obj.start_element(p, obj._M_area);
    }
  if (slxt == svg::select::even)
    {
      // RHS
      // Radial metadata dimensions grid.
      // Right, center vertical and move to left edge horizontal
      const point_2t p = { bleedxoffset * get_dpi(), bleedpxo };
      obj.start_element(p, obj._M_area);
    }
}

} // namespace svg

#endif
