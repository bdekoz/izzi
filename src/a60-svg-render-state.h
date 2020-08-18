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

#ifndef MiL_SVG_RENDER_STATE_H
#define MiL_SVG_RENDER_STATE_H 1

//#include <unordered_map>

namespace svg {

namespace constants {

  /**
     Selected or Active in render area.
     Make discrete element or layer (visible, outline, etc) if true.
     Used as a (visibility, outline, etc.) bitmask
  */
  enum class select : unsigned
    {
     none		= 1u << 0, ///> nothing
     cartography	= 1u << 1, ///> cartographic elements
     vector		= 1u << 2, ///> svg path, circle, rectangle, etc.
     cloud		= 1u << 3, ///> color fill version of vector
     echo		= 1u << 4, ///> b & w outline version of vector
     text		= 1u << 5, ///> title, metadata, header
     legend		= 1u << 6, ///> torrent collection glyph
     exitnode		= 1u << 7, ///> tor exit node infrastructure
     telecom		= 1u << 8, ///> telecom infrastructure
     glyph		= 1u << 9, ///> glyph
     image		= 1u << 10, ///> image
     svg		= 1u << 11, ///> svg element, perhaps nested
     all		= 1u << 12, ///> all elements and layers
     _S_end		= 1u << 16 ///> future use 10-16
    };

  inline constexpr select
  operator&(select __a, select __b)
  {
    using __utype = typename std::underlying_type<select>::type;
    __utype r = static_cast<__utype>(__a) & static_cast<__utype>(__b);
    return static_cast<select>(r);
  }

  inline constexpr select
  operator|(select __a, select __b)
  { return select(static_cast<int>(__a) | static_cast<int>(__b)); }

  inline constexpr select
  operator^(select __a, select __b)
  { return select(static_cast<int>(__a) ^ static_cast<int>(__b)); }

  inline constexpr select
  operator~(select __a)
  { return select(~static_cast<int>(__a)); }

  inline const select&
  operator|=(select& __a, select __b)
  { return __a = __a | __b; }

  inline const select&
  operator&=(select& __a, select __b)
  { return __a = __a & __b; }

  inline const select&
  operator^=(select& __a, select __b)
  { return __a = __a ^ __b; }


  /**
     Some high-level nobs for rendering: scale tuning.

     small		== defense distributed == 70k-300k
     medium		== westworld == 2-4M
     large		== stranger things == 10M+
  */
  enum class scale
    {
     xxsmall, xsmall, small,
     medium,
     large, xlarge, xxlarge
    };

} // namespace constants


/// Settings for rendering collection-derived objects.
struct render_state_base
{
  k::select	visible_mode;
  k::select	outline_mode;
  k::scale	scale_mode;

  double	opacity;

  // Values with similar geo coordinates counted in as one meta coordinate.
  bool		weigh;

  // Genderate colors from color bands (true) or use finite colors (false).
  bool		color_generated;

  // Alternate output naming mode via scheme (true = info, false = map)
  bool		alt;

  render_state_base(const double o = 0.10,
		    const k::scale rscale = k::scale::medium)
  : visible_mode(k::select::none), outline_mode(k::select::none),
    scale_mode(rscale), opacity(o),
    weigh(false), color_generated(true), alt(false)
  { }

  bool
  is_visible(const k::select v) const
  { return static_cast<bool>(visible_mode & v); }

  void
  set(k::select& a, const k::select& b)
  { a |= b; }

  void
  clear(k::select& a, const k::select& b)
  { a &= ~b; }
};



///  Render settings for collections.
struct render_state : public render_state_base
{
  using	colormap = std::unordered_map<string, colorq>;

  colormap	klrs;

  colorq
  get_color(const string& s) const
  {
    auto i = klrs.find(s);
    if (i != klrs.end())
      return i->second;
    else
      throw std::runtime_error("render_state::get_color " + s + " not found");
  }

  render_state() = default;
};


render_state&
get_render_state()
{
  static render_state state;
  return state;
}

} // namespace svg

#endif
