// svg element parts -*- mode: C++ -*-

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

#ifndef MiL_SVG_ELEMENTS_COMPONENTS_H
#define MiL_SVG_ELEMENTS_COMPONENTS_H 1


namespace svg {

void
element_base::add_title(const string& t)
{
  title_element te;
  te.start_element(t);
  te.finish_element();
  add_element(te);
}

void
svg_element::write()
{
  try
    {
      string filename(_M_name + ".svg");
      std::ofstream f(filename);
      if (!f.is_open() || !f.good())
	throw std::runtime_error("svg_element::write fail");

      f << _M_sstream.str() << std::endl;
    }
  catch(std::exception& e)
    {
      throw e;
    }
}

/// SVG element beginning boilerplate for outermost (containing) svg_element.
/// Variable: unit, x=0, y=0, width, height
void
svg_element::start_element()
{
  const string start = R"_delimiter_(<?xml version="1.0" encoding="utf-8"?>
<!DOCTYPE svg PUBLIC "-//W3C//DTD SVG 1.1//EN" "http://www.w3.org/Graphics/SVG/1.1/DTD/svg11.dtd">
<svg version="1.1"
     id="svg2" xml:space="preserve"
     xmlns:dc="http://purl.org/dc/elements/1.1/"
     xmlns:cc="http://creativecommons.org/ns#"
     xmlns:svg="http://www.w3.org/2000/svg"
     xmlns="http://www.w3.org/2000/svg"
     xmlns:xlink="http://www.w3.org/1999/xlink"
)_delimiter_";

  const string unit("__unit");
  const string width("__width");
  const string height("__height");

  string strip = R"_delimiter_(x="0__unit" y="0__unit"
width="__width__unit" height="__height__unit"
viewBox="0 0 __width __height" enable-background="new 0 0 __width __height">
)_delimiter_";

  string_replace(strip, unit, to_string(_M_unit));
  string_replace(strip, width, std::to_string(_M_area._M_width));
  string_replace(strip, height, std::to_string(_M_area._M_height));

  _M_sstream << start;
  _M_sstream << strip << std::endl;
}


/// SVG element for nested svg_element.
/// Use name, area, unit, typo for viewport frame
/// @origin x,y position
/// @desta height, width of destination. (If > that _M_area) then enlarge.
void
svg_element::start_element(const point_2t p, const area destarea,
			   const style& styl)
{
  const string id("__id");
  string start = R"_delimiter_(<svg id="__id" )_delimiter_";
  string_replace(start, id, _M_name);

  const string unit("__unit");
  const string width("__width");
  const string height("__height");
  const string dwidth("__dwidth");
  const string dheight("__dheight");
  const string px("__x");
  const string py("__y");
  string strip = R"_delimiter_(viewBox="0 0 __width __height" x="__x__unit" y="__y__unit" width="__dwidth__unit" height="__dheight__unit" )_delimiter_";

  string_replace(strip, unit, to_string(_M_unit));
  string_replace(strip, width, std::to_string(_M_area._M_width));
  string_replace(strip, height, std::to_string(_M_area._M_height));

  const auto [ dw, dh ] = destarea;
  const auto [ x, y ] = p;
  string_replace(strip, dwidth, std::to_string(dw));
  string_replace(strip, dheight, std::to_string(dh));
  string_replace(strip, px, std::to_string(int(x)));
  string_replace(strip, py, std::to_string(int(y)));

  // Check to make sure stream starts empty.
  if (!this->empty())
    {
      string m("svg_element::start_element error buffer is not empty: " );
      m += k::newline;
      m += this->str();
      m += k::newline;
      throw std::runtime_error(m);
    }

  _M_sstream << start;
  _M_sstream << strip << std::endl;

  // Only add style if it is not the default argument.
  const string nostr = to_string(color::none);
  const string fillstr = to_string(styl._M_fill_color);
  const bool colornonep = nostr == fillstr;
  if (!colornonep && (styl._M_fill_opacity != 0 || styl._M_stroke_opacity != 0))
    {
      add_style(styl);
      _M_sstream << k::space;
    }
  _M_sstream << '>' << k::newline;
}


void
svg_element::add_filters()
{
  const string f = R"_delimiter_(<defs>
    <filter id="gblur10" x="0" y="0">
      <feGaussianBlur in="SourceGraphic" stdDeviation="10" />
      <feOffset dx="0" dy="0" />
     </filter>
    <filter id="gblur20" x="0" y="0">
      <feGaussianBlur in="SourceGraphic" stdDeviation="20" />
      <feOffset dx="0" dy="0" />
    </filter>
    <filter id="gblur10y" x="0" y="0">
      <feGaussianBlur in="SourceGraphic" stdDeviation="0, 10" />
      <feOffset dx="0" dy="0" />
    </filter>
    <filter id="gblur20y" x="0" y="0">
      <feGaussianBlur in="SourceGraphic" stdDeviation="0, 20" />
      <feOffset dx="0" dy="0" />
    </filter>
  </defs>
)_delimiter_";

  _M_sstream << f << std::endl;
}


/// SVG element end boilerplate.
void
svg_element::finish_element()
{
  _M_sstream << "</svg>" << std::endl;
}

} // namespace svg

#endif
