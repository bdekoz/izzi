// svg elements -*- mode: C++ -*-

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

#ifndef MiL_SVG_ELEMENTS_H
#define MiL_SVG_ELEMENTS_H 1


namespace svg {

/**
   @defgroup elements SVG Elements
   @brief All svg components are derived from element_base as per
   [hierarchy](https://bdekoz.github.io/izzi/html/structsvg_1_1element__base__inherit__graph_org.svg "svg elements")

  @{
*/

/// Abstract base class for all SVG Elements.
struct element_base
{
  using stream_type = std::ostringstream;
  static constexpr const char*	finish_tag = " >";
  static constexpr string	finish_tag_hard = string(finish_tag) + k::newline;
  static constexpr const char*	self_finish_tag = " />";

  // Underlying units for 2D (x,y) mapping (same as area::atype).
  using atype = space_type; // ... floating point cartesian points
  //using atype = ssize_type; // ... integer cartesian points

  /// Virtual, only one buffer.
  stream_type		_M_sstream;

  virtual void
  start_element() = 0;

  virtual void
  finish_element() = 0;

  /// Empty when the output buffer is.
  bool
  empty() { return _M_sstream.str().empty(); }

  string
  str() const { return _M_sstream.str(); }

  void
  str(const string& s) { return _M_sstream.str(s); }

  // Add sub element e to base object in non-visible defs section
  void
  store_element(const element_base& e);

  // Add sub element e to base object
  void
  add_element(const element_base& e)
  { _M_sstream << e.str(); }

  void
  add_fill(const string id)
  {
    _M_sstream << k::space;
    _M_sstream << "fill=" << k::quote;
    _M_sstream << "url(#" << id << ")" << k::quote;
  }

  void
  add_filter(const string id)
  {
    _M_sstream << k::space;
    _M_sstream << "filter=" << k::quote;
    _M_sstream << "url(#" << id << ")" << k::quote;
  }

  // Add raw string to group; filter, blend/gradient elements.
  void
  add_raw(const string& raw)
  { _M_sstream << k::space << raw; }

  void
  add_style(const style& sty)
  { _M_sstream << to_string(sty); }

  void
  add_title(const string& t);

  /// Common transforms include rotate(180)
  string
  make_transform_attribute(const string s)
  {
    std::ostringstream oss;
    oss << k::space << "transform=" << k::quote << s << k::quote;
    return oss.str();
  }

  void
  add_transform(const string s)
  {
    if (!s.empty())
      _M_sstream << make_transform_attribute(s);
  }
};


/**
   Group SVG element.

   Specification reference:
   https://developer.mozilla.org/en-US/docs/Web/SVG/Element/g

   Attributes:
   x, y, width, height, xlink:xref, preserveAspectRatio
 */
struct group_element : virtual public element_base
{
  static string
  start_group(const string name)
  {
    string ret("<g");
    if (!name.empty())
      {
       ret += " id=";
       ret += k::quote;
       ret += name;
       ret += k::quote;
      }
    ret += ">";
    ret += k::newline;
    return ret;
  }

  static string
  finish_group()
  { return string("</g>") + k::newline; }



  /// For groups of elements that have the same name.
  ///
  /// Also, although one can nest SVG elements inside another SVG by
  /// using an 'image_element', the display quality is lacking in
  /// inkscape. To work around this, insert the contents of the nested
  /// SVG into a named group element instead.
  void
  start_element()
  { _M_sstream << start_group(""); }

  void
  start_element(string name)
  { _M_sstream << start_group(name); }

  // For groups of elements with style as specified.
  void
  start_element(string name, const style& sty)
  {
    _M_sstream << "<g id=" << k::quote << name << k::quote << k::space;
    add_style(sty);
    _M_sstream << '>' << k::newline;
  }

  // For groups of elements with a transform and style if specified.
  void
  start_element(string name, const string ts, const style& sty = k::no_style)
  {
    _M_sstream << "<g id=" << k::quote << name << k::quote;
    add_transform(ts);

    // Only add style if it is not the default argument.
    const color_qi nklr(color::none);
    const bool stylep = to_string(sty._M_fill_color) != to_string(nklr);
    if (stylep)
      add_style(sty);
    _M_sstream << '>' << k::newline;
  }

  void
  finish_element();
};

void
group_element::finish_element()
{ _M_sstream << finish_group(); }


/**
   Definitions SVG element. Storage space for elements used later.

   Specification reference:
   https://developer.mozilla.org/en-US/docs/Web/SVG/Element/defs

   Attributes:
   id
 */
struct defs_element : virtual public element_base
{
  static string
  start_defs()
  { return "<defs>"; }

  static string
  finish_defs()
  { return "</defs>"; }

  void
  start_element()
  { _M_sstream << start_defs() << k::newline; }

  void
  finish_element();
};

void
defs_element::finish_element()
{ _M_sstream << finish_defs() << k::newline; }

void
element_base::store_element(const element_base& e)
{
  _M_sstream << defs_element::start_defs();
  _M_sstream << e.str();
  _M_sstream << defs_element::finish_defs();
}


/**
   Link SVG element. a

   Specification reference:
   https://developer.mozilla.org/en-US/docs/Web/SVG/Element/a

   Attributes:
   id
 */
struct link_element : virtual public element_base
{
  void
  start_element()
  { _M_sstream << "<a "; }

  void
  finish_element();

  void
  add_data(const string& url)
  {
    _M_sstream << "href=" << k::quote << url << k::quote;
    _M_sstream << element_base::finish_tag;
  }

  /// Overload for rel=x form.
  /// https://developer.mozilla.org/en-US/docs/Web/HTML/Reference/Attributes/rel/preload
  /// @param relt attribute that describes the relationship of the
  /// target object to the link object
  /// @param ast type of resource.
  void
  add_data(const string& url, const string relt, const string ast,
	   const string cors = "anonymous")
  {
    _M_sstream << "rel=" << k::quote << relt << k::quote << k::space;
    _M_sstream << "href=" << k::quote << url << k::quote << k::space;
    _M_sstream << "as=" << k::quote << ast << k::quote << k::space;
    _M_sstream << "crossorigin=" << k::quote << cors << k::quote << k::space;
    _M_sstream << "referrerpolicy=" << k::quote << "no-referrer" << k::quote;
    _M_sstream << k::space;
    _M_sstream << element_base::self_finish_tag;
  }
};

void
link_element::finish_element()
{ _M_sstream << "</a>" << k::newline; }


/**
   Datum consolidating filter use and preferences.

   \verbatim
   <filter id="gblur10" x="0" y="0">
   <feGaussianBlur in="SourceGraphic" stdDeviation="10" />
   <feOffset dx="0" dy="0" />
   </filter>
   \endverbatim
*/
struct filter_element : virtual public element_base
{
  enum class type
    {
     feBlend, ///< Blend
     feImage, ///< Image
     feOffset, ///< Offset, dx="0", dy="0"
     feGaussianBlur, ///< Gaussian Blur
     feColorMatrix,
     feComponentTransfer
    };

  void
  start_element()
  { _M_sstream << "<filter>" << k::newline; }

  void
  start_element(const string id)
  {
    _M_sstream << "<filter id=" << k::quote << id << k::quote << ">"
	       << k::newline;
  }

  // Some filter effects get clipped when appied to an element's area,
  // so this allows filters to have an element + filter area instead.
  void
  start_element(const string id, const area<> blur_area, const point_2t p)
  {
    auto [ width, height ] = blur_area;
    auto [ x, y ] = p;
    _M_sstream << "<filter id=" << k::quote << id << k::quote << k::space
	       << "x=" << k::quote << x << k::quote << k::space
	       << "y=" << k::quote << y << k::quote << k::space
	       << "width=" << k::quote << width << k::quote << k::space
	       << "height=" << k::quote << height << k::quote
	       << ">"
	       << k::newline;
  }

  void
  finish_element();

  void
  add_data(const string fltr)
  { _M_sstream << fltr; }

  // https://drafts.fxtf.org/filter-effects/#elementdef-fegaussianblur
  // in == SourceGraphic, SourceAlpha, FillPaint, StrokePaint
  // dev == 1 or 1,1 (0 default if two then x, y direction)
  string
  gaussian_blur(string in, string dev)
  {
    // <feGaussianBlur in="SourceGraphic" stdDeviation="20" />
    stream_type stream;
    stream << "<feGaussianBlur in=";
    stream << k::quote << in << k::quote << k::space;
    stream <<  "stdDeviation=" << k::quote << dev << k::quote << k::space;
    stream <<  "/>";
    return stream.str();
  }

  string
  gaussian_blur(string dev)
  { return gaussian_blur("SourceGraphic", dev); }
};

void
filter_element::finish_element()
{ _M_sstream  << "</filter>" << k::newline; }


/**
   Gradient SVG elements.

   Specification reference:
   https://developer.mozilla.org/en-US/docs/Web/SVG/Element/radialGradient
   https://developer.mozilla.org/en-US/docs/Web/SVG/Element/linearGradient

   Note these are always inside a defs block.

   Attributes:
   id
 */
struct gradient_element : virtual public defs_element
{
  enum class type
    {
      linearGradient, ///< Linear
      meshgradient, ///< Mesh
      radialGradient, ///< Radial
      stop ///< stop, last
    };

  void
  start_element()
  { defs_element::start_element(); }

  void
  finish_element();

  // off == 10%
  void
  stop(const string off, const color& klr, const double opacity = 1.0)
  {
    _M_sstream << "<stop offset=" << k::quote << off << k::quote << k::space
	       << "stop-color=" << k::quote << to_string(klr) << k::quote;
    if (opacity < 1.0)
      {
	_M_sstream << k::space
		   << "stop-opacity=" << k::quote << opacity << k::quote;
      }
    _M_sstream << " />";
    _M_sstream << k::newline;
  }

  // Express two integers as a suitable offset string percentage.
  const string
  offset_percentage(const ssize_type numer, const ssize_type denom)
  {
    const double ratio = static_cast<double>(numer)/static_cast<double>(denom);
    const ssize_type perc(round(ratio * 100));
    return std::to_string(perc) + "%";
  }
};

void
gradient_element::finish_element()
{ defs_element::finish_element(); }


/// Circular gradients
/// https://developer.mozilla.org/en-US/docs/Web/SVG/Element/radialGradient
struct radial_gradient : virtual public gradient_element
{
  void
  start_element()
  {
    gradient_element::start_element();
    _M_sstream << "<radialGradient id=" << k::quote << "default" << k::quote;
    _M_sstream << ">" << k::newline;
  }

  // Radial gradient centered at (cx, cy) of radius.
  // Default for radius, cx, cy is "50%"
  void
  start_element(const string id, const ssize_type radius = 0,
		const ssize_type cx = 0, const ssize_type cy = 0)
  {
    gradient_element::start_element();
    _M_sstream << "<radialGradient id=" << k::quote << id << k::quote;
    if (radius > 0)
      {
      _M_sstream << k::space << "r=" << k::quote << radius << k::quote;
      _M_sstream << k::space << "cx=" << k::quote << cx << k::quote;
      _M_sstream << k::space << "cy=" << k::quote << cy << k::quote;
      }
    _M_sstream << ">" << k::newline;
  }

  // Radial gradient.
  // End circle (aka 100% stop) at (cx, cy) with radius.
  // Start circle (aka 0% stop) at (fx, fy) with radius fr.
  void
  start_element(const string id, const ssize_type radius,
		const ssize_type cx, const ssize_type cy, const ssize_type fr,
		const ssize_type fx, const ssize_type fy)
  {
    gradient_element::start_element();
    _M_sstream << "<radialGradient id=" << k::quote << id << k::quote
	       << k::space << "r=" << k::quote << radius << k::quote;
    _M_sstream << k::space << "cx=" << k::quote << cx << k::quote;
    _M_sstream << k::space << "cy=" << k::quote << cy << k::quote;
    if (fr > 0)
      {
	_M_sstream << k::space << "fx=" << k::quote << fx << k::quote;
	_M_sstream << k::space << "fy=" << k::quote << fy << k::quote;
	_M_sstream << k::space << "fr=" << k::quote << fr << k::quote;
      }
    _M_sstream << ">" << k::newline;
  }

  void
  finish_element();
};

void
radial_gradient::finish_element()
{
  _M_sstream << "</radialGradient>" << k::newline;
  gradient_element::finish_element();
}


/// Linear gradients
/// https://developer.mozilla.org/en-US/docs/Web/SVG/Element/linearGradient
struct linear_gradient : virtual public gradient_element
{
  void
  start_element()
  {
    gradient_element::start_element();
    _M_sstream << "<linearGradient id=" << k::quote << "default" << k::quote;
    _M_sstream << ">" << k::newline;
  }

  void
  finish_element();
};

void
linear_gradient::finish_element()
{
  _M_sstream << "</linearGradient>" << k::newline;
  gradient_element::finish_element();
}


/**
   Marker SVG elements defines a graphic used for drawing
   arrow[heads, tails, mid] on a poly line or path.

   Specification reference:
   https://developer.mozilla.org/en-US/docs/Web/SVG/Reference/Attribute/marker-mid

   Note these are always inside a defs block.

   Attributes:
   id
 */
struct marker_element : virtual public element_base
{
  void
  start_element() { }

  // markerWidth="8" markerHeight="8" refX="4" refY="4">
  void
  start_element(const string id, const area<> a, const point_2t p)
  {
    auto [ x, y ] = p;
    auto [ w, h ] = a;
    _M_sstream << "<marker id=" << k::quote << id << k::quote << k::space
	       << "markerWidth=" << k::quote << w << k::quote << k::space
	       << "markerHeight=" << k::quote << h << k::quote << k::space
	       << "refX=" << k::quote << x << k::quote << k::space
	       << "refY=" << k::quote << y << k::quote << " >"
	       << k::newline;
  }

  void
  finish_element();
};

void
marker_element::finish_element()
{
  _M_sstream << "</marker>" << k::newline;
}


/**
   Title SVG element. This is accessible/alt text.
   This element must be the first element in the svg objectc.

   A title element with no string indicates a decorative element to AT
   screenreaders.

   Specification reference:
   https://developer.mozilla.org/en-US/docs/Web/SVG/Element/title
*/
struct title_element : virtual public element_base
{
  void
  start_element()
  { _M_sstream << "<title>" << k::newline; }

  void
  start_element(const string t)
  {
    start_element();
    _M_sstream << t << k::newline;
  }

  void
  finish_element();
};

void
title_element::finish_element()
{ _M_sstream  << "</title>" << k::newline; }


/**
   Description SVG element.

   Specification reference:
   https://developer.mozilla.org/en-US/docs/Web/SVG/Element/desc
*/
struct desc_element : virtual public element_base
{
  void
  start_element() { }

  void
  start_element(const string dsc)
  { _M_sstream << "<desc>" << k::newline << dsc << k::newline; }

  void
  finish_element();
};

void
desc_element::finish_element()
{ _M_sstream  << "</desc>" << k::newline; }


/**
   Text SVG element.

   Specification reference:
   https://developer.mozilla.org/en-US/docs/Web/SVG/Element/text

   Attributes:
   x, y, dx, dy, text-anchor, rotate, textLength, lengthAdjust
*/
struct text_element : virtual public element_base
{
  struct data
  {
    atype		_M_x_origin;
    atype		_M_y_origin;
    string		_M_text;
    typography		_M_typo;
  };

  /// Either serialize immediately (as below), or create data structure
  /// that adds data to data_vec and then finish_element serializes.
  void
  add_data(const data& d,
	   const string trans = "", const unit utype = svg::unit::point)
  {
    const string x("__x");
    const string y("__y");
    const string attr("__attr");
    const string style("__style");

    string strip = R"_delimiter_(x="__x" y="__y" __attr __style)_delimiter_";

    // Add attributes.
    string_replace(strip, x, std::to_string(d._M_x_origin));
    string_replace(strip, y, std::to_string(d._M_y_origin));
    string_replace(strip, attr, d._M_typo.add_attribute(utype));
    string_replace(strip, style, to_string(d._M_typo._M_style));
    _M_sstream << strip;
    add_transform(trans);
    _M_sstream << '>';

    // Add text data.
    add_raw(d._M_text);
  }

  void
  start_element()
  { _M_sstream << "<text "; }

  void
  finish_element();

  /// For text list output, use tspan for line breaks.  This span
  /// creates a new horizontal line for every tspan block, starting at
  /// xpos with spacing dy (1.2em).
  static string
  start_tspan_y(uint xpos, string dy)
  {
    const string x("__x");
    const string dys("__dy");
    string strip = R"_delimiter_(<tspan x="__x" dy="__dy">)_delimiter_";
    string_replace(strip, x, std::to_string(xpos));
    string_replace(strip, dys, dy);
    return strip;
  }

  static string
  start_tspan_y(uint xpos, uint dy)
  { return start_tspan_y(xpos, std::to_string(dy)); }

  /// For text list output, use tspan for line breaks.  This span
  /// creates a new vertical line space for every tspan block, starting
  /// at xpos with horizontal spacing dx ("1.4em").
  static string
  start_tspan_x(uint xpos, string dx)
  {
    const string x("__x");
    const string dxs("__dx");
    string strip = R"_delimiter_(<tspan x="__x" dx="__dx">)_delimiter_";
    string_replace(strip, x, std::to_string(xpos));
    string_replace(strip, dxs, dx);
    return strip;
  }

  static string
  start_tspan_x(uint xpos, uint dx)
  { return start_tspan_x(xpos, std::to_string(dx)); }

  static string
  finish_tspan()
  { return "</tspan>"; }
};

void
text_element::finish_element()
{ _M_sstream  << "</text>" << k::newline; }


/// Make text span.
string
make_tspan_y_from_string_by_token(string s, uint xpos, const char token = ' ')
{
  string start(text_element::start_tspan_y(xpos, "0.5em"));
  string ret = start;
  for (uint i = 0; i < s.size(); ++i)
    {
      const char c = s[i];
      if (c != token)
	ret += c;
      else
	{
	  ret += text_element::finish_tspan();
	  if (i < s.size() - 1)
	    ret += start;
	}
    }
  ret += text_element::finish_tspan();
  return ret;
}


/**
   Rectangle SVG element.

   Specification reference:
   https://developer.mozilla.org/en-US/docs/Web/SVG/Element/rect

   Attributes:
   x, y, width, height, rx, ry
 */
struct rect_element : virtual public element_base
{
  struct data
  {
    atype		_M_x_origin;
    atype		_M_y_origin;
    atype		_M_width;
    atype		_M_height;
  };

  // Verbose opening/closing pair tags for circle_element.
  // Default assumes the more compact XML "self-closed tag" for circle element.
  static constexpr const char*  pair_open_tag = "<rect>";
  static constexpr const char*  pair_finish_tag = "</rect>";

  /// Either serialize immediately (as below), or create data structure
  /// that adds data to data_vec and then finish_element serializes.
  void
  add_data(const data& d)
  {
    const string x("__x");
    const string y("__y");
    const string w("__w");
    const string h("__h");

    string strip = R"_delimiter_(x="__x" y="__y" width="__w" height="__h"
)_delimiter_";

    string_replace(strip, x, std::to_string(d._M_x_origin));
    string_replace(strip, y, std::to_string(d._M_y_origin));
    string_replace(strip, w, std::to_string(d._M_width));
    string_replace(strip, h, std::to_string(d._M_height));
    _M_sstream << strip;
  }

  void
  start_element()
  { _M_sstream << "<rect "; }

  void
  finish_element();
};

void
rect_element::finish_element()
{ _M_sstream  << element_base::self_finish_tag << k::newline; }


/**
   Circle SVG element.

   Specification reference:
   https://developer.mozilla.org/en-US/docs/Web/SVG/Element/circle

   Attributes:
   x, y, width, height, xlink:xref, preserveAspectRatio
 */
struct circle_element : virtual public element_base
{
  struct data
  {
    atype		_M_x_origin;
    atype		_M_y_origin;
    atype		_M_radius;
  };

  // Verbose opening/closing pair tags for circle_element.
  // Default assumes the more compact XML "self-closed tag" for circle element.
  static constexpr const char*	pair_open_tag = "<circle>";
  static constexpr const char*	pair_finish_tag = "</circle>";

  // Either serialize immediately (as below), or create data structure
  // that adds data to data_vec and then finish_element serializes.
  void
  add_data(const data& d, string trans = "")
  {
    const string x("__x");
    const string y("__y");
    const string r("__r");

    string strip = R"_delimiter_(cx="__x" cy="__y" r="__r")_delimiter_";

    string_replace(strip, x, std::to_string(d._M_x_origin));
    string_replace(strip, y, std::to_string(d._M_y_origin));
    string_replace(strip, r, std::to_string(d._M_radius));
    _M_sstream << strip;
    add_transform(trans);
  }

  void
  start_element()
  { _M_sstream << "<circle "; }

  void
  finish_element();

};

void
circle_element::finish_element()
{ _M_sstream  << element_base::self_finish_tag << k::newline; }


/**
   Line SVG element.

   Specification reference:
   https://developer.mozilla.org/en-US/docs/Web/SVG/Element/line

   Attributes:
   x, y, width, height, xlink:xref, preserveAspectRatio
*/
struct line_element : virtual public element_base
{
  struct data
  {
    atype		_M_x_begin;
    atype		_M_x_end;
    atype		_M_y_begin;
    atype		_M_y_end;
  };

  // Either serialize immediately (as below), or create data structure
  // that adds data to data_vec and then finish_element serializes.
  void
  add_data(const data& d, const string dasharray = "")
  {
    const string x1("__x1");
    const string x2("__x2");
    const string y1("__y1");
    const string y2("__y2");
    const string dash("__darray");

    const bool dashp = !dasharray.empty();
    string stripf = \
    R"_delimiter_(x1="__x1" y1="__y1" x2="__x2" y2="__y2")_delimiter_";
    string stript = \
    R"_delimiter_(x1="__x1" y1="__y1" x2="__x2" y2="__y2" stroke-dasharray="__darray")_delimiter_";

    string strip = dashp ? stript : stripf;
    string_replace(strip, x1, std::to_string(d._M_x_begin));
    string_replace(strip, x2, std::to_string(d._M_x_end));
    string_replace(strip, y1, std::to_string(d._M_y_begin));
    string_replace(strip, y2, std::to_string(d._M_y_end));

    if (dashp)
      string_replace(strip, dash, dasharray);
    _M_sstream << strip;
  }

  void
  start_element()
  { _M_sstream << "<line "; }

  void
  start_element(string name)
  { _M_sstream << "<line id=" << k::quote << name << k::quote << k::space; }

  void
  finish_element();
};

void
line_element::finish_element()
{ _M_sstream  << element_base::self_finish_tag << k::newline; }


/**
   Polyline SVG element.

   Specification reference:
   https://developer.mozilla.org/en-US/docs/Web/SVG/Reference/Element/polyline

   Attributes:
   points, pathLength
*/
struct polyline_element : virtual public element_base
{
  // vector<point_2t> == vector<tuple<double,double>>
  vrange polypoints;

  polyline_element() { }

  polyline_element(const vrange& points) : polypoints(points) { }

  // Either serialize immediately (as below), or create data structure
  // that adds data to data_vec and then finish_element serializes.
  void
  add_data(const stroke_style& sstyl)
  {
    if (!polypoints.empty())
      {
	_M_sstream << "points=" << k::quote;
	for (const point_2t& pt : polypoints)
	  {
	    auto [ x, y ] = pt;
	    _M_sstream << x << k::comma << y << k::space;
	  }
	_M_sstream << k::quote << k::space;

	if (!sstyl.dasharray.empty())
	  {
	    _M_sstream << "stroke-dasharray=" << k::quote;
	    _M_sstream << sstyl.dasharray << k::quote << k::space;
	  }
	if (!sstyl.dashoffset.empty())
	  {
	    _M_sstream << "stroke-dashoffset=" << k::quote;
	    _M_sstream << sstyl.dashoffset << k::quote << k::space;
	  }
	if (!sstyl.linecap.empty())
	  {
	    _M_sstream << "stroke-linecap=" << k::quote;
	    _M_sstream << sstyl.linecap << k::quote << k::space;
	  }
	if (!sstyl.marker_form.empty())
	  {
	    // line_style 3
	    string mkr;
	    mkr += k::quote;
	    mkr += "url(#";
	    mkr += sstyl.marker_form;
	    mkr += ")";
	    mkr += k::quote;

	    _M_sstream << "marker-mid=" << mkr << k::space;
	    _M_sstream << "marker-end=" << mkr << k::space;
	  }
      }
  }

  void
  start_element()
  { _M_sstream << "<polyline "; }

  void
  start_element(string name)
  { _M_sstream << "<polyline id=" << k::quote << name << k::quote << k::space; }

  void
  finish_element();
};

void
polyline_element::finish_element()
{ _M_sstream  << element_base::self_finish_tag << k::newline; }



/**
   Path SVG element.

   Specification reference:
   https://developer.mozilla.org/en-US/docs/Web/SVG/Element/path

   Attributes:
   d, pathLength
*/
struct path_element : virtual public element_base
{
  struct data
  {
    string		_M_d;
    atype		_M_length;
  };

  static constexpr const char*	pair_finish_tag = "</path>";

  /// Either serialize immediately (as below), or create data structure
  /// that adds data to data_vec and then finish_element serializes.
  void
  add_data(const data& d)
  {
    const string pathd("__d");
    const string len("__l");

    string strip = R"_delimiter_(d="__d")_delimiter_";

    string_replace(strip, pathd, d._M_d);
    string_replace(strip, len, std::to_string(d._M_length));
    _M_sstream << strip;
  }

  void
  start_element()
  { _M_sstream << "<path "; }

  void
  start_element(const string name)
  { _M_sstream << "<path id=" << k::quote << name << k::quote << k::space; }

  void
  finish_element();
};

void
path_element::finish_element()
{
  _M_sstream  << element_base::self_finish_tag;
  _M_sstream << k::newline;
}


/**
   Text on a Path SVG element.

   Specification reference:
   https://developer.mozilla.org/en-US/docs/Web/SVG/Element/textPath

   Attributes:
   href, path, method, side, spacing, startOffset, textLength, lengthAdjust
*/
struct text_path_element : virtual public text_element
{
  string	path_name;
  string	offset; // "30%"
  string	side;   // "left" || "right" (use convex/concave side of path)

  text_path_element(const string name,
		    const string off = "", const string whichside = "")
  : path_name(name), offset(off), side(whichside) { }

  virtual void
  add_text(string txt)
  {
    // Start text_path_element...
    _M_sstream << "<textPath xlink:href="
	       << k::quote << '#' << path_name << k::quote;
    if (!offset.empty())
      _M_sstream << k::space << "startOffset="
		 << k::quote << offset << k::quote;
    if (!side.empty())
      _M_sstream << k::space << "side="
		 << k::quote << side << k::quote;
    _M_sstream << '>';

    _M_sstream << txt;

    // End text_path_element...
    _M_sstream << "</textPath>" << k::space;
  }
};



/**
   Image SVG element. This can be another SVG file, or can be a raster
   image format like PNG or JPEG.

   Specification reference:
   https://developer.mozilla.org/en-US/docs/Web/SVG/Element/image

   Attributes:
   x, y, width, height, xlink:xref, preserveAspectRatio
 */
struct image_element : virtual public element_base
{
  struct data
  {
    string		_M_xref;
    atype		_M_x_origin;
    atype		_M_y_origin;
    atype		_M_width;
    atype		_M_height;
  };

  void
  start_element(const string& id)
  {
    const string simg = "<image";
    _M_sstream << simg << k::space;
    if (!id.empty())
      _M_sstream << "id=" << k::quote << id << k::quote << k::space;
  }

  void
  start_element()
  { start_element(""); }

  void
  finish_element();

  /// Either serialize immediately (as below), or create data structure
  /// that adds data to data_vec and then finish_element serializes.
  void
  add_data(const data& d)
  {
    const string x("__x");
    const string y("__y");
    const string w("__w");
    const string h("__h");
    const string ref("__ref");

    string strip = R"_delimiter_(href="__ref" x="__x" y="__y" width="__w" height="__h")_delimiter_";

    string_replace(strip, ref, d._M_xref);
    string_replace(strip, x, std::to_string(d._M_x_origin));
    string_replace(strip, y, std::to_string(d._M_y_origin));
    string_replace(strip, w, std::to_string(d._M_width));
    string_replace(strip, h, std::to_string(d._M_height));
    _M_sstream << strip << k::space;
  }

  /// Visibility and other HTML/img attributes.
  /// @param vattr = visibility attribute, "visible" or "hidden"
  /// @param display = display attribute, "none" or "unset" or "initial"
  /// @param cors = CORS, "anonymous" or "use-credentials"
  /// @param lattr = loading attribute, "lazy" or "eager"
  void
  add_data(const data& d, const string cors, const string vattr,
	   const string display = "")
  {
    add_data(d);

    if (!cors.empty())
      _M_sstream << "crossorigin=" << k::quote << cors << k::quote << k::space;
    if (!vattr.empty())
      _M_sstream << "visibility=" << k::quote << vattr << k::quote << k::space;
    if (!display.empty())
      _M_sstream << "display=" << k::quote << display << k::quote << k::space;
  }
};


void
image_element::finish_element()
{ _M_sstream  << element_base::self_finish_tag << k::newline; }


/**
   So-called Foreign Objects.

   Using to get HTML video elements.

   https://developer.mozilla.org/en-US/docs/Web/SVG/Element/foreignObject
   https://stackoverflow.com/questions/40324916/video-tag-embedded-in-svg

Translate moves the origin from the top left to the specified
coordinates. If you embed an object at 0,0 it will be placed at the
new origin. In this case you must embed it at -translation
coordinates.

Even so, I had to increase the width and height. Why? I don't know. It
doesn't seem to be a scale by 2. If someone knows I am curious to
know.

<svg version="1.1" class="center-block" xmlns="http://www.w3.org/2000/svg" xmlns:xlink="http://www.w3.org/1999/xlink" width="800" height="600"  style="border: 1px solid black;">
    <g>
	<g transform="translate(151,104) scale(1,1)">
	    <rect x="0" y="0" width="300" height="200"></rect>
	    <foreignObject x="-151" y="-104" width="500" height="400">
		<video xmlns="http://www.w3.org/1999/xhtml" width="300" height="200" controls="" style="position: fixed; left: 151px; top: 104px;">
		    <source src="http://techslides.com/demos/sample-videos/small.mp4" type="video/mp4" />less
		</video>
	    </foreignObject>
	</g>
    </g>
</svg>
*/
struct foreign_element : virtual public element_base
{
  void
  start_element()
  { _M_sstream << "<g>" << k::newline; }

  // av == area of the foreign object and native video frame size
  // arect == area of displayed video as embedded inside svg element /aka page
  // scale_pair == x/y scaling factor
  void
  start_element(const point_2t origin, const area<> av, const area<> /*arect*/,
		const point_2t scale = std::make_tuple(1.0, 1.0))
  {
    const auto [ scalex, scaley ] = scale;
    const auto [ ox, oy ] = origin;

    //const auto [ width, height ] = arect;
    //auto xo = width/2;
    //auto yo = height/2;

    const auto [ vwidth, vheight ] = av;

    // Outer group.
    group_element go;
    go.start_element();
    _M_sstream << go.str() << k::newline;

    // Inner Group.
    group_element gi;
    //string tx = transform::translate(xo, yo);
    string tscl = transform::scale(scalex, scaley);
    gi.start_element(string("video-wrapper"), tscl);
    _M_sstream << gi.str() << k::newline;

    // Foreign Object
    string strip = R"(<foreignObject x="XXX" y="YYY" width="WWW" height="HHH">)";
    string_replace(strip, "WWW", std::to_string(vwidth));
    string_replace(strip, "HHH", std::to_string(vheight));
    string_replace(strip, "XXX", std::to_string(ox));
    string_replace(strip, "YYY", std::to_string(oy));
    _M_sstream  << strip << k::newline;
  }

  void
  finish_element();
};

void
foreign_element::finish_element()
{ _M_sstream  << " </foreignObject></g></g>" << k::newline; }


/// video HTML object embedded in SVG container.
/// NB: HTML elements video/audio/iframe/canvas can be used w/o foreignElement.
/// This approach uses HTML wrapped in foreign element.
/// https://www.w3.org/TR/SVG2/embedded.html#HTMLElements
/// https://developer.mozilla.org/en-US/docs/Web/HTML/Element/video
struct video_element : virtual public foreign_element
{
  void
  start_element(const string& id)
  {
    //const string svideo = "<html:video";
    const string svideo = R"(<video xmlns="http://www.w3.org/1999/xhtml" )";
    _M_sstream << svideo << k::space;
    if (!id.empty())
      _M_sstream << "id=" << k::quote << id << k::quote << k::space;
  }

  void
  start_element()
  { start_element(""); }

  /// Video.
  /// a is width and height of video as embedded in page
  /// r is the foreign object, with x/y offset and scaled size
  ///
  /// attr is attribues for video_element
  /// autoplay="true" or removed
  /// loop="true/false"
  /// muted="true/false"
  /// controls, controlslist,
  /// crossorigin, disablepictureinpicture, disableremoteplayback
  ///
  void
  add_data(const area<> a, const string src, const string mtype = "video/mp4",
	   const string attr = R"(autoplay="true" loop="true" muted="true")")
  {
    string strip = R"(width="WWW" height="HHH" )";
    string_replace(strip, "WWW", std::to_string(a._M_width));
    string_replace(strip, "HHH", std::to_string(a._M_height));
    _M_sstream << k::space;
    _M_sstream << strip << k::space;
    _M_sstream << attr << k::space;
    _M_sstream << element_base::finish_tag_hard;

    _M_sstream << "<source src=" << k::quote << src << k::quote << k::space;
    _M_sstream << "type=" << k::quote << mtype << k::quote;
    _M_sstream << element_base::self_finish_tag << k::newline;
  }

  void
  finish_element();
};

void
video_element::finish_element()
{ _M_sstream  << "</video>" << k::newline; }


/// iframe HTML object embedded in SVG container.
/// NB: HTML elements video/audio/iframe/canvas can be used w/o foreignElement.
/// This approach uses HTML wrapped in foreign element.
/// https://www.w3.org/TR/SVG2/embedded.html#HTMLElements
/// https://developer.mozilla.org/en-US/docs/Web/HTML/Element/iframe
struct iframe_element : virtual public foreign_element
{
  void
  start_element(const string& id)
  {
    //const string siframe = "<html:iframe";
    const string siframe = R"(<iframe xmlns="http://www.w3.org/1999/xhtml" )";
    _M_sstream << siframe << k::space;
    if (!id.empty())
      _M_sstream << "id=" << k::quote << id << k::quote << k::space;
  }

  void
  start_element()
  { start_element(""); }

  /// iframe.
  /// a is width and height of video as embedded in page
  /// r is the foreign object, with x/y offset and scaled size
  ///
  void
  add_data(const area<> a, const string src, const string mtype = "image/jpeg",
	   const string attr = R"(sandbox="allow-scripts allow-same-origin")")
  {
    string strip = R"(width="WWW" height="HHH" )";
    string_replace(strip, "WWW", std::to_string(a._M_width));
    string_replace(strip, "HHH", std::to_string(a._M_height));
    _M_sstream << k::space;
    _M_sstream << strip << k::space;
    //    _M_sstream << "src=" << k::quote << src << k::quote << k::space;
    _M_sstream << attr;
    _M_sstream << element_base::finish_tag_hard;

    // image/webp or image/jpeg
    _M_sstream << "<source src=" << k::quote << src << k::quote << k::space;
    _M_sstream << "type=" << k::quote << mtype << k::quote << k::space;
    _M_sstream << element_base::self_finish_tag << k::newline;
   }

  void
  finish_element();
};

void
iframe_element::finish_element()
{ _M_sstream  << "</iframe>" << k::newline; }


/// HTML object embedded in SVG container.
/// Unlike image_elements, object_elements are not locked down for scripting.
/// NB: HTML elements video/audio/object/canvas can be used w/o foreignElement.
/// This approach uses HTML wrapped in foreign element.
/// https://www.w3.org/TR/SVG2/embedded.html#HTMLElements
/// https://developer.mozilla.org/en-US/docs/Web/HTML/Element/object
struct object_element : virtual public foreign_element
{
  void
  start_element(const string& id)
  {
    //const string sobject = "<html:object";
    const string sobject = R"(<object xmlns="http://www.w3.org/1999/xhtml" )";
    _M_sstream << sobject << k::space;
    if (!id.empty())
      _M_sstream << "id=" << k::quote << id << k::quote << k::space;
  }

  void
  start_element()
  { start_element(""); }

  /// Add resource to object.
  /// @param a is width and height of video as embedded in page
  /// @param src is the resource URL
  /// @param mtype is the MIME type
  /// @param attr is any collection of ad-hoc HTML attributes.
  void
  add_data(const area<> a, const string src, const string mtype = "image/jpeg",
	   const string attr = R"(sandbox="allow-scripts allow-same-origin")")
  {
    string strip = R"(width="WWW" height="HHH" )";
    string_replace(strip, "WWW", std::to_string(a._M_width));
    string_replace(strip, "HHH", std::to_string(a._M_height));
    _M_sstream << k::space;
    _M_sstream << strip << k::space;
    _M_sstream << "data=" << k::quote << src << k::quote << k::space;
    _M_sstream << "type=" << k::quote << mtype << k::quote << k::space;
    _M_sstream << attr << k::space;
    _M_sstream << element_base::finish_tag_hard;
   }

  void
  finish_element();
};

void
object_element::finish_element()
{ _M_sstream  << "</object>" << k::newline; }


/**
   A SVG script element.

   https://developer.mozilla.org/en-US/docs/Web/SVG/Element/script
*/
struct script_element : virtual public element_base
{
  /// Where is the script element placed? On/within the element
  /// itself, or at the document (global)? Neither (none),
  /// alongside/same level (parent)?
  enum class scope
    {
      none, ///< Script scope removed
      document, ///< Scripts scoped to toplevel document
      parent, ///< Scripts scoped to parent
      element ///< Scripts scoped to element
    };

  void
  start_element(const string& id)
  {
    const string shead = R"(<script type="text/javascript" crossorigin="anonymous")";
    _M_sstream << shead << k::space;
    if (!id.empty())
      _M_sstream << "id=" << k::quote << id << k::quote << k::space;
    _M_sstream << element_base::finish_tag_hard;
  }

  void
  start_element()
  { start_element(""); }

  /// showTooltip(id)
  /// hideTooltip(id)
  /// event.x vs. event.pageX, event.y vs. event.pageY
  static const string&
  tooltip_javascript(const scope context)
  {
    static string js_show_element = R"(
    function showTooltip(event, tooltipId) {
      const tooltipimg = document.getElementById(tooltipId);
      if (tooltipimg) {
	const ge = tooltipimg.parentElement;
	const svge = ge.parentElement;
	const brect = ge.getBoundingClientRect();
	const bx = brect.left;
	const by = brect.top;

	tooltipimg.setAttribute('x', event.x + bx);
	tooltipimg.setAttribute('y', event.y + by);

	tooltipimg.setAttribute('visibility', 'visible');
      } else {
	console.error(`Element with ID "${tooltipId}" not found.`);
      }
    })";

    static string js_show_document = R"(
    function showTooltip(event, tooltipId) {
      const tooltipimg = document.getElementById(tooltipId);
      if (tooltipimg) {
	//tooltipimg.onload = function() {
	const ge = tooltipimg.parentElement;
	const svge = ge.parentElement;
	const brect = ge.getBoundingClientRect();
	const bx = brect.left;
	const by = brect.top;

	//const iheight = 150;
	const iheight = tooltipimg.offsetHeight; //!isNaN(iheight)
	tooltipimg.setAttribute('x', event.pageX - bx);
	tooltipimg.setAttribute('y', event.pageY - by - iheight);
	tooltipimg.setAttribute('visibility', 'visible');
	//tooltipimg.setAttribute('display', 'inline');
      } else {
	console.error(`Element with ID "${tooltipId}" not found.`);
      }
    })";

    static string js_hide = R"(
    function hideTooltip(tooltipId) {
      const tooltipimg = document.getElementById(tooltipId);
      tooltipimg.setAttribute('visibility', 'hidden');
      //tooltipimg.setAttribute('display', 'none');
    })";

    static string js;
    if (context == scope::element)
      js = js_show_element + k::newline + js_hide;
    if (context == scope::document || context == scope::parent)
      js = js_show_document + k::newline + js_hide;

    return js;
  }

  static const string
  tooltip_attribute(const string& id)
  {
    const string toolr("__toolt");
    string strip1 = R"_delimiter_(onmouseover="showTooltip(event, '__toolt')" )_delimiter_";
    string strip2 = R"_delimiter_(onmouseout="hideTooltip('__toolt')" )_delimiter_";
    string_replace(strip1, toolr, id);
    string_replace(strip2, toolr, id);
    return k::space + strip1 + k::space + strip2;
  }

  // Script element for js to control visibility of images.
  static const script_element
  tooltip_script(const scope context)
  {
    script_element scrpt;
    scrpt.start_element("tooltip-js");
    scrpt.add_data(script_element::tooltip_javascript(context));
    scrpt.finish_element();
    return scrpt;
  }


  /// Add string with script source.
  /// @param scriptstr script source
  void
  add_data(const string scriptstr)
  {
    _M_sstream << scriptstr;
    _M_sstream << k::newline;
   }

  void
  finish_element();
};

void
script_element::finish_element()
{ _M_sstream  << "</script>" << k::newline; }


/**
   A SVG object element.

   https://developer.mozilla.org/en-US/docs/Web/SVG/Element/svg
   https://developer.mozilla.org/en-US/docs/Web/SVG/SVG_as_an_Image
*/
struct svg_element : virtual public element_base
{
  using area = svg::area<atype>;

  const string		_M_name;
  const area		_M_area;
  const unit		_M_unit;
  const typography&	_M_typo;
  const bool		_M_lifetime;  // scope document scope element

  svg_element(const string __title, const area& __cv,
	      const bool lifetime = true,
	      const unit u = svg::unit::pixel,
	      const typography& __typo = k::smono_typo)
  : _M_name(__title), _M_area(__cv), _M_unit(u),
    _M_typo(__typo), _M_lifetime(lifetime)
  {
    if (_M_lifetime)
      start();
  }

  svg_element(const string __title, const string desc, const area& __cv,
	      const bool lifetime = true)
  : _M_name(__title), _M_area(__cv), _M_unit(svg::unit::pixel),
    _M_typo(svg::k::smono_typo), _M_lifetime(lifetime)
  {
    if (_M_lifetime)
      start(desc);
  }

  svg_element(const svg_element& other)
  : _M_name(other._M_name), _M_area(other._M_area),
    _M_unit(other._M_unit), _M_typo(other._M_typo),
    _M_lifetime(other._M_lifetime)
  { }

  ~svg_element()
  {
    if (_M_lifetime)
      finish();
  }

  const point_2t
  center_point()
  { return _M_area.center_point(); }

  void
  start_element();

  void
  start_element(const point_2t p, const area destarea,
		const style& sty = k::no_style);

  void
  finish_element();

  void
  add_title()
  { element_base::add_title(_M_name); }

  void
  add_desc(const string desc)
  {
    desc_element de;
    de.start_element(desc);
    de.finish_element();
    add_element(de);
  }

  void
  add_filters();

  void
  write();

  void
  start(const string& desc = "")
  {
    this->start_element();
    this->add_title();
    if (!desc.empty())
      this->add_desc(desc);
  }

  void
  finish(const bool writep = true)
  {
    this->finish_element();
    if (writep)
      this->write();
  }
};
/// @} group elements

} // namespace svg

#endif
