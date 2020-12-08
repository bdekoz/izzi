// svg elements -*- mode: C++ -*-

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

#ifndef MiL_SVG_ELEMENTS_H
#define MiL_SVG_ELEMENTS_H 1


namespace svg {

/**
   @defgroup elements Elements
  SVG Elements
  @{
*/

/// Abstract base class for all SVG Elements.
struct element_base
{
  using stream_type = std::ostringstream;

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
  str() { return _M_sstream.str(); }

  void
  str(const string& s) { return _M_sstream.str(s); }

  // Add raw string to group, filter, blend/gradient elements.
  void
  add_raw(const string& raw)
  { _M_sstream << k::space << raw << k::newline; }

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

  void
  add_style(const style& sty)
  { _M_sstream << style::str(sty); }

  /// Common transforms include rotate(180)
  void
  add_transform(const string s)
  {
    if (!s.empty())
      _M_sstream << k::space << "transform=" << k::quote << s << k::quote;
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
  void
  start_element()
  { _M_sstream << "<g>" << k::newline; }

  /// For groups of elements that have the same name.
  ///
  /// Also, although one can nest SVG elements inside another SVG by
  /// using an 'image_element', the display quality is lacking in
  /// inkscape. To work around this, insert the contents of the nested
  /// SVG into a named group element instead.
  void
  start_element(string name)
  {
    _M_sstream << "<g id=" << k::quote << name << k::quote
	       << ">" << k::newline;
  }

  // For groups of elements that have the same style.
  void
  start_element(string name, const style& sty)
  {
    _M_sstream << "<g id=" << k::quote << name << k::quote << k::space;
    add_style(sty);
    _M_sstream << '>' << k::newline;
  }

  void
  start_element(string name, const transform, const string ts)
  {
    _M_sstream << "<g id=" << k::quote << name << k::quote;
    add_transform(ts);
    _M_sstream << '>' << k::newline;
  }

  void
  finish_element();
};

void
group_element::finish_element()
{ _M_sstream  << "</g>" << k::newline; }


/**
   Definitions SVG element. Storage space for elements used later.

   Specification reference:
   https://developer.mozilla.org/en-US/docs/Web/SVG/Element/defs

   Attributes:
   id
 */
struct defs_element : virtual public element_base
{
  void
  start_element()
  { _M_sstream << "<defs>" << k::newline; }

  void
  finish_element();
};

void
defs_element::finish_element()
{ _M_sstream  << "</defs>" << k::newline; }


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
     feBlend,
     feImage,
     feOffset,	  // dx="0", dy="0"
     feGaussianBlur,
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
struct gradient_element
: virtual public element_base, virtual public defs_element
{
  enum class type
    {
      linearGradient,
      meshgradient,
      radialGradient,
      stop
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
  offset_percentage(const size_type numer, const size_type denom)
  {
    const double ratio = static_cast<double>(numer)/static_cast<double>(denom);
    const size_type perc(round(ratio * 100));
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
  start_element(const string id, const size_type radius = 0,
		const size_type cx = 0, const size_type cy = 0)
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
  start_element(const string id, const size_type radius,
		const size_type cx, const size_type cy, const size_type fr,
		const size_type fx, const size_type fy)
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
    size_type		_M_x_origin;
    size_type		_M_y_origin;
    string		_M_text;
    typography		_M_typo;
  };

  // So text_path_element can substitute the text_path part without
  // duplicating the text formatting and style parts....
  virtual void
  add_text(string txt)
  { _M_sstream << txt; }

  /// Either serialize immediately (as below), or create data structure
  /// that adds data to data_vec and then finish_element serializes.
  void
  add_data(const data& d, string trans = "")
  {
    const string x("__x");
    const string y("__y");
    const string anchor("__anchor");
    const string attr("__attr");
    const string style("__style");

    string strip = R"_delimiter_(x="__x" y="__y" text-anchor="__anchor"  __attr __style)_delimiter_";

    // Add attributes.
    string_replace(strip, x, std::to_string(d._M_x_origin));
    string_replace(strip, y, std::to_string(d._M_y_origin));
    string_replace(strip, anchor, d._M_typo.to_string(d._M_typo._M_a));
    string_replace(strip, attr, d._M_typo.add_attribute());
    string_replace(strip, style, svg::style::str(d._M_typo._M_style));
    _M_sstream << strip;
    add_transform(trans);
    _M_sstream << '>';

    // Add text data.
    add_text(d._M_text);
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
    size_type		_M_x_origin;
    size_type		_M_y_origin;
    size_type		_M_width;
    size_type		_M_height;
  };

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
{ _M_sstream  << " />" << k::newline; }


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
    size_type		_M_x_origin;
    size_type		_M_y_origin;
    size_type		_M_width;
    size_type		_M_height;
  };

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

    string strip = R"_delimiter_(xlink:href="__ref" x="__x" y="__y" width="__w" height="__h"
)_delimiter_";

    string_replace(strip, ref, d._M_xref);
    string_replace(strip, x, std::to_string(d._M_x_origin));
    string_replace(strip, y, std::to_string(d._M_y_origin));
    string_replace(strip, w, std::to_string(d._M_width));
    string_replace(strip, h, std::to_string(d._M_height));
    _M_sstream << strip;
  }

  void
  start_element()
  { _M_sstream << "<image "; }

  void
  finish_element();
};


void
image_element::finish_element()
{ _M_sstream  << " />" << k::newline; }


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
    size_type		_M_x_origin;
    size_type		_M_y_origin;
    size_type		_M_radius;
  };

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
{ _M_sstream  << " />" << k::newline; }


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
    size_type		_M_x_begin;
    size_type		_M_x_end;
    size_type		_M_y_begin;
    size_type		_M_y_end;
  };

  // Either serialize immediately (as below), or create data structure
  // that adds data to data_vec and then finish_element serializes.
  void
  add_data(const data& d)
  {
    const string x1("__x1");
    const string x2("__x2");
    const string y1("__y1");
    const string y2("__y2");

    string strip = R"_delimiter_(x1="__x1" y1="__y1" x2="__x2" y2="__y2"
)_delimiter_";

    string_replace(strip, x1, std::to_string(d._M_x_begin));
    string_replace(strip, x2, std::to_string(d._M_x_end));
    string_replace(strip, y1, std::to_string(d._M_y_begin));
    string_replace(strip, y2, std::to_string(d._M_y_end));
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
{ _M_sstream  << " />" << k::newline; }



/**
   Path SVG element.

   Specification reference:
   https://developer.mozilla.org/en-US/docs/Web/SVG/Element/path

   Attributes:
   d, pathLength
*/
struct path_element : virtual public defs_element
{
  struct data
  {
    string		_M_d;
    size_type		_M_length;
  };

  // Put in defs section, making it not drawn but still usefule for text_path.
  bool			_M_visible;

  path_element(const bool visible = true) : _M_visible(visible) { }

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
  {
    if (!_M_visible)
      defs_element::start_element();
    _M_sstream << "<path ";
  }

  void
  start_element(string name)
  {
    if (!_M_visible)
      defs_element::start_element();
    _M_sstream << "<path id=" << k::quote << name << k::quote << k::space;
  }

  void
  finish_element();
};

void
path_element::finish_element()
{
  _M_sstream  << " />";
  if (!_M_visible)
    defs_element::finish_element();
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
		    const string off = "", const string which = "")
  : path_name(name), offset(off), side(which) { }

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
   A SVG object element.
*/
struct svg_element : virtual public element_base
{
  using area = svg::area<>;

  const string		_M_name;
  const area		_M_area;
  const unit		_M_unit;
  const typography&	_M_typo;

  svg_element(const string __title, const area& __cv = k::a4_096_v,
	      const unit u = svg::unit::pixel,
	      const typography& __typo = k::smono_typo)
    : _M_name(__title), _M_area(__cv), _M_unit(u), _M_typo(__typo)
  { start(); }

  svg_element(const svg_element& other)
  : _M_name(other._M_name), _M_area(other._M_area),
    _M_unit(other._M_unit), _M_typo(other._M_typo)
  { }

  ~svg_element() { finish(); }

  const point_2t
  center_point()
  { return std::make_tuple(_M_area._M_width / 2, _M_area._M_height / 2); }

  void
  start_element();

  void
  finish_element();

  void
  add_title();

  void
  add_filters();

  void
  add_element(element_base& e)
  { _M_sstream << e.str(); }

  void
  write();

  void
  start()
  {
    this->start_element();
    this->add_title();
    this->add_filters();
  }

  void
  finish()
  {
    this->finish_element();
    this->write();
  }
};
/// @} group elements

} // namespace svg

#endif
