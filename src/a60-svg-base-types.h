// svg base -*- mode: C++ -*-

// Copyright (C) 2014-2024 Benjamin De Kosnik <b.dekosnik@gmail.com>

// This file is part of the alpha60-MiL SVG library.  This library is
// free software; you can redistribute it and/or modify it under the
// terms of the GNU General Public License as published by the Free
// Software Foundation; either version 3, or (at your option) any
// later version.

// This library is distributed in the hope that it will be useful, but
// WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// General Public License for more details.

#ifndef MiL_SVG_BASE_TYPES_H
#define MiL_SVG_BASE_TYPES_H 1


#include <map>
#include <ostream>
#include <fstream>
#include <cstdint>
//#include <compare>


namespace svg {

/// Measurement abstraction for absolute (not relative) measurements.
enum class unit
  {
    meter, m,		///< Meter
    centimeter, cm,	///< Centimeter
    millimeter, mm,	///< Milliimeter
    inch, in,		///< Inch
    pixel, px,		///< Pixel where 1 pixel x 96 PPI = .26 mm
    point, pt		///< Point where 1 pixel x 1/72 dpi x 96 PPI = .26 mm
  };

const string
to_string(const unit e)
{
  using enum_map_type = std::map<unit, std::string>;

  static enum_map_type enum_map;
  if (enum_map.empty())
    {
      enum_map[unit::meter] = "m";
      enum_map[unit::m] = "m";
      enum_map[unit::centimeter] = "cm";
      enum_map[unit::cm] = "cm";
      enum_map[unit::millimeter] = "mm";
      enum_map[unit::mm] = "mm";
      enum_map[unit::inch] = "in";
      enum_map[unit::in] = "in";
      enum_map[unit::pixel] = "px";
      enum_map[unit::px] = "px";
      enum_map[unit::point] = "pt";
      enum_map[unit::pt] = "pt";
    }
  return enum_map[e];
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
template<typename T1 = svg::space_type>
struct area
{
  using atype = T1;
  using unit = svg::unit;

  atype			_M_width;
  atype			_M_height;

  /// Given @param rdenom scaling factor and SVG canvas area by @obj,
  /// compute max effective segment size given number of segments @rdenom.
  /// NB: if rdenom is two then this is a max radius value centered
  /// on the page/frame.
  inline constexpr double
  max_segment_size_n(const uint rdenom = 2) const
  {
    double leastside = std::min(_M_height, _M_width);
    return leastside / rdenom;
  }

  ///  Convenience function for finding center.
  inline constexpr point_2t
  center_point() const
  {  return std::make_tuple(_M_width / 2, _M_height / 2); }

  /// Landscape, aka largest side is horizontal.
  area
  to_landscape() const
  {
    atype lg = 0, sm = 0;
    if (_M_width >= _M_height)
      {
	lg = _M_width;
	sm = _M_height;
      }
    else
      {
	lg = _M_height;
	sm = _M_width;
      }
    atype twidth = lg;
    atype theight = sm;
    return area(twidth, theight);
  }

  /// Portrait, aka largest side is vertical.
  area
  to_portrait() const
  {
    atype lg = 0, sm = 0;
    if (_M_width >= _M_height)
      {
	lg = _M_width;
	sm = _M_height;
      }
    else
      {
	lg = _M_height;
	sm = _M_width;
      }
    atype twidth = sm;
    atype theight = lg;
    return area(twidth, theight);
  }
};


/// Datum consolidating style preferences.
struct style
{
  color_qi		_M_fill_color;
  double		_M_fill_opacity;

  color_qi		_M_stroke_color;
  double		_M_stroke_opacity;
  double		_M_stroke_size;

  // auto operator<=>(const style&) const = default;

  /// Convenience function to set all colors at once.
  void
  set_colors(const color_qi& klr)
  {
    _M_fill_color = klr;
    _M_stroke_color = klr;
  }
};


/// Marker shape.
enum class marker_shape
  {
    round = 0,		///< Round
    triangle,		///< Triangle
    square,		///< Square
    hexagon,		///< Hexagon
    octahedron,		///< Octahedron (8) 3D
    icosahedron		///< Icosahedron (20) 3D
  };

const string
to_string(const marker_shape e)
{
  using enum_map_type = std::map<marker_shape, std::string>;

  static enum_map_type enum_map;
  if (enum_map.empty())
    {
      enum_map[marker_shape::round] = "round";
      enum_map[marker_shape::triangle] = "triangle";
      enum_map[marker_shape::square] = "square";
      enum_map[marker_shape::hexagon] = "hexagon";
      enum_map[marker_shape::octahedron] = "octahedron";
      enum_map[marker_shape::icosahedron] = "icosahedron";

    }
  return enum_map[e];
}


/// Additional path/line/polyline stroke styles.
/// NB: https://yuanchuan.dev/fun-with-stroke-dasharray
struct stroke_style
{
  /// Marker shapes.
  /// For graph_mode 1, this means the SVG equivalent of CSS elements:
  /// marker-start, marker-mid, marker-end
  /// For graph_mode 2, this means the marker_shape of the mark
  string		marker_form;

  /// Marker repetitions, if any. Default is zero.
  ushort		marker_reps;

  /// Line dash vs. space configuration.
  /// Options are single value, like 2, meaning 2 sized dash 2 sized space.
  /// Or, double values with size of dash followed by size of space.
  /// Or, more elaborate patterns of on, off
  /// https://developer.mozilla.org/en-US/docs/Web/CSS/stroke-dasharray
  string		dasharray;

  /// Starting offset for line dashes.
  /// https://developer.mozilla.org/en-US/docs/Web/CSS/stroke-dashoffset
  string		dashoffset;

  /// Shape of end of line segments in line dashes.
  /// SVG Values are: butt, round, square.
  /// https://developer.mozilla.org/en-US/docs/Web/CSS/stroke-linecap
  string		linecap;

  string		path_length;

  /// Create dash array variations with exponentially-increasing spaces.
  /// dasharray 1 = solid line
  /// dasharray 2 = 1 == 2 2
  /// dasharray 3 = 2 == 2 4
  /// dasharray 4 = 3 == 2 6
  /// @param lsize is style._M_stroke_size
  static string
  create_dasharray_n(const uint lsize, const uint n)
  {
    using std::to_string;
    string da;
    if (n > 1)
      {
	const uint da_sz = lsize + 1;
	da = to_string(da_sz) + k::space + to_string(da_sz * (n - 1));
      }
    return da;
  }
};


const string
to_string(const style& s)
{
  const string space("; ");
  std::ostringstream stream;
  stream << k::space;
  stream << "style=" << k::quote;
  stream << "fill:" << color_qi::to_string(s._M_fill_color) << space;
  stream << "fill-opacity:" << s._M_fill_opacity << space;
  stream << "stroke:" << color_qi::to_string(s._M_stroke_color) << space;
  stream << "stroke-opacity:" << s._M_stroke_opacity << space;
  stream << "stroke-width:" << s._M_stroke_size << k::quote;
  return stream.str();
}


/// Datum consolidating transform possibilities.
/// https://developer.mozilla.org/en-US/docs/Web/SVG/Attribute/transform
struct transform
{
  static string
  matrix(double a, double b, double c, double d, double e, double f)
  {
    std::ostringstream stream;
    stream << "matrix(" << a << k::comma << b << k::comma << c << k::comma
	   << d << k::comma << e << k::comma << f << k::comma << ")";
    return stream.str();
  }

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
  scale(double factor1, double factor2)
  {
    std::ostringstream stream;
    stream << "scale(" << factor1 << k::comma << factor2 << ")";
    return stream.str();
  }

  static string
  skew_x(double factor)
  {
    std::ostringstream stream;
    stream << "skewX(" << factor << ")";
    return stream.str();
  }

  static string
  skew_y(double factor)
  {
    std::ostringstream stream;
    stream << "skewY(" << factor << ")";
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
   https://developer.mozilla.org/docs/Web/SVG/Tutorial/SVG_fonts
*/
struct typography
{
  /// Text alignment.
  enum class align
    {
      left,		///< Left-most part of text block
      center,		///< Center part of text block
      right,		///< Right part of text block
      justify,
      justifyall,
      start,
      end,
      inherit,
      matchparent,
      initial,
      unset
    };

  /// How to align text to a given point.
  enum class anchor
    {
      start,		///< Start the text block at point.
      middle,		///< Center the middle of the text block at point.
      end,		///< End the text block at point.
      inherit		///< Enclosing object or group
    };

  /// How to align text to the dominant-baseline.
  /// https://developer.mozilla.org/docs/Web/SVG/Attribute/dominant-baseline
  enum class baseline
    {
      none,		///< Ignore this attribute.
      automatic,	///< Auto
      ideographic,
      alphabetic,
      hanging,
      mathematical,
      central,		///< For rotated text
      middle,
      text_after_edge,
      text_before_edge,
      text_top
    };

  /// Weight as per CSS property.
  enum class weight
    {
      xlight,
      light,		///< CSS 300
      normal,		///< CSS 400
      medium,		///< CSS 500
      bold,
      xbold
    };

  /// Face style variant.
  /// https://developer.mozilla.org/docs/Web/CSS/font-style
  enum class property
    {
      normal,		///< Book
      italic		///< Italic
    };

  // Find all installed fonts on linux with `fc-list`
  std::string		_M_face;	// System font name
  space_type		_M_size;	// Display size
  style			_M_style;

  // Style attributes for text
  // font-weight, transform,
  // https://developer.mozilla.org/en-US/docs/Web/SVG/Element/font-face
  anchor		_M_anchor;
  align			_M_align;
  baseline		_M_baseline;
  weight		_M_w;
  property		_M_p;

  // Compile time expression.
  typography
  use_style(const style s) const
  { return { _M_face, _M_size, s, _M_anchor, _M_align, _M_baseline, _M_w, _M_p }; }

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
  to_string(const baseline b) const
  {
    using enum_map_type = std::map<baseline, std::string>;

    static enum_map_type enum_map;
    if (enum_map.empty())
      {
	enum_map[baseline::none] = "";
	enum_map[baseline::automatic] = "auto";
	enum_map[baseline::ideographic] = "ideographic";
	enum_map[baseline::alphabetic] = "alphabetic";
	enum_map[baseline::hanging] = "hanging";
	enum_map[baseline::mathematical] = "mathematical";
	enum_map[baseline::central] = "central";
	enum_map[baseline::middle] = "middle";
	enum_map[baseline::text_after_edge] = "text-after-edge";
	enum_map[baseline::text_before_edge] = "text-before-edge";
	enum_map[baseline::text_top] = "text-top";
      }
    return enum_map[b];
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
  add_attribute(const svg::unit utype = svg::unit::pixel) const
  {
    const string name("__name");
    const string size("__size");
    const string anchor("__anchor");
    const string align("__align");
    const string len("__lentype");

    std::string strip1 =					\
      R"(font-family="__name" font-size="__size__lentype" text-anchor="__anchor" text-align="__align" )";

    string_replace(strip1, name, _M_face);
    string_replace(strip1, size, std::to_string(_M_size));
    string_replace(strip1, anchor, to_string(_M_anchor));
    string_replace(strip1, align, to_string(_M_align));
    string_replace(strip1, len, svg::to_string(utype));

    const std::string weight("__weight");
    const std::string property("__prop");

    std::string strip2 =					\
      R"(font-weight="__weight" font-style="__prop")";

    string_replace(strip2, weight, to_string(_M_w));
    string_replace(strip2, property, to_string(_M_p));

    // Add dominant baseline only if necessary.
    string stripb;
    if (_M_baseline != baseline::none)
      {
	stripb += "dominant-baseline=";
	stripb += k::quote;
	stripb += to_string(_M_baseline);
	stripb += k::quote;
	stripb += k::space;
      }
    return strip1 + stripb + strip2;
  }
};

} // namespace svg

#endif
