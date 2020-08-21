// svg base -*- mode: C++ -*-

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

#ifndef MiL_SVG_BASE_H
#define MiL_SVG_BASE_H 1


#include <map>
#include <ostream>
#include <fstream>
#include <cstdint>


// Utility function, like regex_replace.
inline void
string_replace(std::string& target, const std::string& match,
	       const std::string& replace)
{
  size_t pos = 0;
  while((pos = target.find(match, pos)) != std::string::npos)
    {
      target.replace(pos, match.length(), replace);
      pos += replace.length();
    }
}


namespace svg {

/// Measurement abstraction type, conversion function.
enum class unit
  {
   centimeter,		// cm
   millimeter,		// mm
   inch,		// in
   pixel		// px, 1 pixel x 96 PPI = .264583 mm
  };

const string
to_string(const unit e)
{
  using enum_map_type = std::map<unit, std::string>;

  static enum_map_type enum_map;
  if (enum_map.empty())
    {
      enum_map[unit::centimeter] = "cm";
      enum_map[unit::millimeter] = "mm";
      enum_map[unit::inch] = "in";
      enum_map[unit::pixel] = "px";
    }
  return enum_map[e];
}


double
pt_to_px(uint i)
{
  // 1pt is equal to exactly 1/72th of an inch.
  // On a 72dpi output device (display), this is 1 (aka 72 * 1/72).
  // On a 90dpi output device (display), this is 1.25 (aka 90 * 1/72).
  // On a 96dpi output device (display), this is 1.33 (aka 96 * 1/72).
  const uint dpi = k::get_dpi();
  return (i * (dpi / 72));
}


/**
   Area/Page/Canvas/Drawing area description.
   Size, origin location in 2D (x,y), heigh, width

   ANSI Letter mm == (unit::millimeter, 215.9, 279.4);
   ANSI Letter pixels == (unit::pixel, 765, 990);

   ISO A4 mm == (unit::millimeter, 210, 297);
   ISO A4 pixels == (unit::pixel, 744.09, 1052.36);

   720p pixels == (unit::pixel, 1280, 720);
   1080p pixels == (unit::pixel, 1920, 1080);

   SVG coordinate system is (0,0) is top leftmost.
*/
template<typename T1 = svg::size_type>
struct area
{
  using size_type = T1;
  using unit = svg::unit;

  size_type		_M_width;
  size_type		_M_height;
};


/// Datum consolidating style preferences.
struct style
{
  colorq		_M_fill_color;
  double		_M_fill_opacity;

  colorq		_M_stroke_color;
  double		_M_stroke_opacity;
  double		_M_stroke_size;

  static string
  str(const style& s)
  {
    const string space("; ");
    std::ostringstream stream;
    stream << k::space;
    stream << "style=" << k::quote;
    stream << "fill:" << colorq::to_string(s._M_fill_color) << space;
    stream << "fill-opacity:" << s._M_fill_opacity << space;
    stream << "stroke:" << colorq::to_string(s._M_stroke_color) << space;
    stream << "stroke-opacity:" << s._M_stroke_opacity << space;
    stream << "stroke-width:" << s._M_stroke_size << k::quote;
    return stream.str();
  }
};


/// Datum consolidating transform preferences.
struct transform
{
  static string
  rotate(int deg)
  {
    std::ostringstream stream;
    stream << "rotate(" << deg << ")";
    return stream.str();
  }

  static string
  rotate(int deg, int x, int y)
  {
    std::ostringstream stream;
    stream << "rotate(" << deg << k::comma << x << k::comma << y << ")";
    return stream.str();
  }

  static string
  scale(double factor)
  {
    std::ostringstream stream;
    stream << "scale(" << factor << ")";
    return stream.str();
  }

  static string
  translate(int x, int y)
  {
    std::ostringstream stream;
    stream << "translate(" << x << k::comma << y << ")";
    return stream.str();
  }
};


/**
   Character rendering, type, fonts, styles.

   Expect to keep changing the output, so use this abstraction to set
   styling defaults, so that one can just assign types instead of doing
   a bunch of search-and-replace operations when changing type
   characteristics.

   SVG Fonts
   https://developer.mozilla.org/en-US/docs/Web/SVG/Tutorial/SVG_fonts
*/
struct typography
{
  // Text alignment.
  enum class align { left, center, right, justify, justifyall,
		     start, end, inherit, matchparent, initial, unset };

  // How to align text to a given point.
  // middle == center the middle of the text block at point.
  // start  == start the text block at point.
  // end    == end the text block at point.
  enum class anchor { start, middle, end, inherit };

  // Light is 300, Normal is 400, and Medium is 500 in CSS.
  enum class weight { xlight, light, normal, medium, bold, xbold };

  // Face variant.
  enum class property { normal, italic };

  // Find installed fonts on linux with `fc-list`
  std::string		_M_face;	// System font name
  size_type		_M_size;	// Display size
  style			_M_style;

  // Style attributes for text
  // font-weight, transform,
  // https://developer.mozilla.org/en-US/docs/Web/SVG/Element/font-face
  anchor		_M_a;
  align			_M_align;
  weight		_M_w;
  property		_M_p;

  const std::string
  to_string(const align a) const
  {
    using enum_map_type = std::map<align, std::string>;

    static enum_map_type enum_map;
      if (enum_map.empty())
	{
	  enum_map[align::left] = "left";
	  enum_map[align::center] = "center";
	  enum_map[align::right] = "right";
	  enum_map[align::justify] = "justify";
	  enum_map[align::justifyall] = "justify-all";
	  enum_map[align::start] = "start";
	  enum_map[align::end] = "end";
	  enum_map[align::inherit] = "inherit";
	  enum_map[align::matchparent] = "match-parent";
	  enum_map[align::initial] = "initial";
	  enum_map[align::unset] = "unset";
	}
      return enum_map[a];
  }

  const std::string
  to_string(const anchor a) const
  {
    using enum_map_type = std::map<anchor, std::string>;

    static enum_map_type enum_map;
    if (enum_map.empty())
      {
	enum_map[anchor::start] = "start";
	enum_map[anchor::middle] = "middle";
	enum_map[anchor::end] = "end";
	enum_map[anchor::inherit] = "inherit";
      }
    return enum_map[a];
    }

  const std::string
  to_string(const weight w) const
  {
    using enum_map_type = std::map<weight, std::string>;

    static enum_map_type enum_map;
    if (enum_map.empty())
      {
	enum_map[weight::xlight] = "200";
	enum_map[weight::light] = "300";
	enum_map[weight::normal] = "400";
	enum_map[weight::medium] = "500";
	enum_map[weight::bold] = "600";
	  enum_map[weight::xbold] = "700";
      }
    return enum_map[w];
  }

  const std::string
  to_string(const property p) const
  {
    using enum_map_type = std::map<property, std::string>;

    static enum_map_type enum_map;
    if (enum_map.empty())
      {
	enum_map[property::normal] = "normal";
	enum_map[property::italic] = "italic";
      }
    return enum_map[p];
  }

  const std::string
  add_attribute() const
  {
    const std::string name("__name");
    const std::string size("__size");
    const std::string align("__align");

    std::string strip1 =						\
      R"(font-family="__name" font-size="__sizepx" text-align="__align" )";

    string_replace(strip1, name, _M_face);
    string_replace(strip1, size, std::to_string(_M_size));
    string_replace(strip1, align, to_string(_M_align));

    const std::string weight("__weight");
    const std::string property("__prop");

    std::string strip2 =					\
      R"(font-weight="__weight" font-style="__prop")";

    string_replace(strip2, weight, to_string(_M_w));
    string_replace(strip2, property, to_string(_M_p));

    return strip1 + strip2;
  }
};

} // namespace svg

#endif
