// svg render basics -*- mode: C++ -*-

// Copyright (C) 2014-2021 Benjamin De Kosnik <b.dekosnik@gmail.com>

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


namespace svg {

namespace constants {

  /**
     Selected or Active in render area.
     Make discrete element or layer (visible, outline, etc) if true.
     Used as a (visibility, outline, etc.) bitmask
  */
  enum class select : uint
    {
     none		= 1u << 0, ///< nothing
     cartography	= 1u << 1, ///< cartographic elements
     vector		= 1u << 2, ///< svg path, circle, rectangle, etc.
     cloud		= 1u << 3, ///< opacity color fill version of vector
     blur		= 1u << 4, ///< blur or gradient version of vector
     echo		= 1u << 5, ///< b & w outline version of vector
     text		= 1u << 6, ///< title, metadata, header
     legend		= 1u << 7, ///< text and symbol legends
     exitnode		= 1u << 8, ///< tor exit node infrastructure
     telecom		= 1u << 9, ///< telecom infrastructure
     glyph		= 1u << 10, ///< glyph
     image		= 1u << 11, ///< image
     svg		= 1u << 12, ///< svg element, perhaps nested
     alt		= 1u << 13, ///< alternate use specified in situ
     background		= 1u << 14, ///< background elements
     raster		= 1u << 15, ///< raster (pixel) elements
     top		= 1u << 16, ///< top
     bottom		= 1u << 17, ///< bottom
     inner		= 1u << 18, ///< inner
     outer		= 1u << 19, ///< outer
     next		= 1u << 20, ///< next
     previous		= 1u << 21, ///< previous
     even		= 1u << 22, ///< even
     odd		= 1u << 23, ///< odd
     all		= 1u << 24, ///< all elements and layers
     _S_end		= 1u << 25  ///< last
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
  {
    using __utype = typename std::underlying_type<select>::type;
    return select(static_cast<__utype>(__a) | static_cast<__utype>(__b));
  }

  inline constexpr select
  operator^(select __a, select __b)
  {
    using __utype = typename std::underlying_type<select>::type;
    return select(static_cast<__utype>(__a) ^ static_cast<__utype>(__b));
  }

  inline constexpr select
  operator~(select __a)
  {
    using __utype = typename std::underlying_type<select>::type;
    return select(~static_cast<__utype>(__a));
  }

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

  render_state_base(const render_state_base&) = default;

  bool
  is_visible(const k::select v) const
  {
    using __utype = typename std::underlying_type<k::select>::type;
    __utype isv(static_cast<__utype>(visible_mode & v));
    return isv > 0;
  }

  void
  set(k::select& a, const k::select& b)
  { a = b; }

  void
  clear(k::select& a, const k::select& b)
  { a &= ~b; }

  void
  flip(k::select& a, const k::select& b)
  { a |= b; }
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


/// Named render state.
/// Datum to take id string and tie it to visual representation.
struct id_render_state: public render_state_base
{
  style		styl; // Overrides render_state_base.opacity
  string	name;
  double	rotate;   // Degrees to rotate to origin (CW), if any.
  double	multiple; // Scale factor, if any.

  static style	dstyl;

  explicit
  id_render_state(const style s = dstyl, const string f = "",
		  const double angle = 0.0, const double scale = 1.0)
  : styl(s), name(f), rotate(angle), multiple(scale) { }

  id_render_state(const id_render_state&) = default;
  id_render_state& operator=(const id_render_state&) = default;
};

style id_render_state::dstyl = { color::black, 0.5, color::black, 0.5, 2 };

using id_render_state_umap = std::unordered_map<string, id_render_state>;


id_render_state_umap&
get_id_render_state_cache()
{
  static id_render_state_umap cache;
  return cache;
}


/// Add value to cache with base style of styl, colors klr, visibility vis.
void
add_to_id_render_state_cache(const string id, const style styl,
			     const k::select vis)
{
  id_render_state_umap& cache = get_id_render_state_cache();

  id_render_state state(styl, id);
  state.set(state.visible_mode, vis);
  cache.insert(std::make_pair(id, state));
}


/// Given identifier/name/id, get corresponding id_render_state from cache.
const id_render_state
get_id_render_state(const string id)
{
  const id_render_state_umap& cache = get_id_render_state_cache();

  id_render_state ret;
  if (cache.count(id) == 1)
    {
      auto iter = cache.find(id);
      ret = iter->second;
    }
  else
    {
      // If default string is set in the cache, use it.
      if (cache.count("") == 1)
	{
	  auto iter = cache.find("");
	  ret = iter->second;
	}
    }
  return ret;
}


/// Roll through render states given in values squentially,
/// index starts with zero.
const id_render_state
traverse_states(const strings& values)
{
  static uint indx(0);
  string value;
  if (indx < values.size())
    {
      value = values[indx];
      ++indx;
    }
  else
    {
      // Start over.
      indx = 0;
      value = values[indx];
    }
  return get_id_render_state(value);
}

} // namespace svg

#endif
