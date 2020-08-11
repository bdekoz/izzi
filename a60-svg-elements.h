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

  void
  add_filter(const size_type n)
  { _M_sstream << filter::str(n); }

  void
  add_filter(const string s)
  { _M_sstream << filter::str(s); }

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
{ _M_sstream  << "</text>" << std::endl; }

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
{ _M_sstream  << " />" << std::endl; }


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
{ _M_sstream  << " />" << std::endl; }


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
{ _M_sstream  << " />" << std::endl; }


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
{ _M_sstream  << " />" << std::endl; }



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
    size_type		_M_length;
  };

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
  start_element(string name)
  { _M_sstream << "<path id=" << k::quote << name << k::quote << k::space; }

  void
  finish_element();
};

void
path_element::finish_element()
{ _M_sstream  << " />" << std::endl; }


/**
   Text on a Path SVG element.

   Specification reference:
   https://developer.mozilla.org/en-US/docs/Web/SVG/Element/textPath

   Attributes:
   href, path, method, side, spacing, startOffset, textLength, lengthAdjust
*/
struct text_path_element : virtual public text_element
{
  string path_name;

  text_path_element(string name): path_name(name) { }

  virtual void
  add_text(string txt)
  {
    // Start text_path_element...
    _M_sstream << "<textPath xlink:href="
	       << k::quote << '#' << path_name << k::quote << '>';

    _M_sstream << txt;

    // End text_path_element...
    _M_sstream << "</textPath>" << k::space;
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
  /// Although one can nest SVG elements inside another SVG by using an
  /// 'image_element', the display quality is lacking in inkscape. To
  /// work around this, insert the contents of the nested SVG into a
  /// group element instead.
  void
  add_raw(string s)
  { _M_sstream << k::space << s << std::endl; }

  void
  start_element()
  { _M_sstream << "<g>" << std::endl; }

  /// For groups of elements that have the same name.
  void
  start_element(string name)
  {
    _M_sstream << "<g id=" << k::quote << name << k::quote
	       << ">" << std::endl;
  }

  // For groups of elements that have the same style.
  void
  start_element(string name, const style& sty)
  {
    _M_sstream << "<g id=" << k::quote << name << k::quote << k::space;
    add_style(sty);
    _M_sstream << '>' << std::endl;
  }

  void
  start_element(string name, const transform, const string ts)
  {
    _M_sstream << "<g id=" << k::quote << name << k::quote;
    add_transform(ts);
    _M_sstream << '>' << std::endl;
  }

  void
  finish_element();
};

void
group_element::finish_element()
{ _M_sstream  << "</g>" << std::endl; }


/**
   A SVG object element.
*/
struct svg_element : virtual public element_base
{
  using area = svg::area<>;

  const string	_M_name;
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

  const auto
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
