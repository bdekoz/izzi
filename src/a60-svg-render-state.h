// svg render basics -*- mode: C++ -*-

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

#ifndef MiL_SVG_RENDER_STATE_H
#define MiL_SVG_RENDER_STATE_H 1


namespace svg {

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

  /// Forwarding functions.
  inline void
  set_select(select& a, const select& b)
  { a = b; }

  inline void
  clear_select(select& a, const select& b)
  { a &= ~b; }

  inline void
  flip_select(select& a, const select& b)
  { a |= b; }


  /**
     Some high-level nobs for rendering: scale tuning.

     small		== defense distributed == 70k-300k
     medium		== westworld == 2-4M
     large		== stranger things == 10M+

     [1-5]xsmall        == reduction from least to greatest
     1x                 == baseline
     [1-5]xlarge        == enlargement from least to greatest
  */
  enum class scale
    {
     r5s, r4s, r3s, r2s, r1s,
     xxsmall, xsmall,
     small,
     medium, baseline,
     large,
     xlarge, xxlarge,
     e1s, e2s , e3s,  e4s, e5s,
    };


  // Convenience function for mapping values to strings.
  string
  to_string(const scale e)
  {
    using enum_map_type = std::map<scale, std::string>;

    static enum_map_type enum_map;
    if (enum_map.empty())
      {
	enum_map[scale::r5s] = "reduce-5s";
	enum_map[scale::r4s] = "reduce-4s";
	enum_map[scale::r3s] = "reduce-3s";
	enum_map[scale::r2s] = "reduce-2s";
	enum_map[scale::r1s] = "reduce-1s";
	enum_map[scale::e5s] = "enlarge-5s";
	enum_map[scale::e4s] = "enlarge-4s";
	enum_map[scale::e3s] = "enlarge-3s";
	enum_map[scale::e2s] = "enlarge-2s";
	enum_map[scale::e1s] = "enlarge-1s";
	enum_map[scale::medium] = "medium";
	enum_map[scale::baseline] = "baseline";
	enum_map[scale::small] = "small";
	enum_map[scale::xsmall] = "xsmall";
	enum_map[scale::xxsmall] = "xxsmall";
	enum_map[scale::large] = "large";
	enum_map[scale::xlarge] = "xlarge";
	enum_map[scale::xxlarge] = "xxlarge";
      }
    return enum_map[e];
  }


/// Settings for glyph, graphic, chart, and collection object render.
struct render_state_base
{
  select	visible_mode;

  bool
  is_visible(const select v) const
  {
    using __utype = typename std::underlying_type<select>::type;
    __utype isv(static_cast<__utype>(visible_mode & v));
    return isv > 0;
  }
};


/// Collection derived states.
struct collection_rstate : public render_state_base
{
  select	outline_mode;
  scale		scale_mode;

  double	opacity;
  int		text_scale_max;

  // Values with similar geo coordinates counted in as one meta coordinate.
  bool		weigh;

  // Genderate colors from color bands (true) or use finite colors (false).
  bool		color_generated;

  // Alternate output naming mode via scheme (true = info, false = map)
  bool		alt;

  collection_rstate(const double o = 0.10,
		    const scale rscale = scale::medium)
  : render_state_base(select::none), outline_mode(select::none),
    scale_mode(rscale), opacity(o), text_scale_max(0),
    weigh(false), color_generated(true), alt(false)
  { }

  collection_rstate(const collection_rstate&) = default;

  string
  mangle() const
  {
    string mangled;

    bool foundp = false;

    const uint vi = static_cast<uint>(visible_mode);
    if (vi > 0)
      {
	mangled += "visible-mode-";
	mangled += std::to_string(vi);
	foundp = true;
      }

    const uint oi = static_cast<uint>(outline_mode);
    if (oi > 0)
      {
	if (foundp)
	  mangled += k::hyphen;
	else
	  foundp = true;
	mangled += "outline-mode-";
	mangled += std::to_string(oi);
      }

    if (opacity > 0)
      {
	if (foundp)
	  mangled += k::hyphen;
	else
	foundp = true;
	mangled += "opacity-";
	const double o(opacity * 100);
	const uint oi = static_cast<uint>(o);
	mangled += std::to_string(oi);
      }

    return mangled;
  }
};


///  Render settings for collections.
struct color_rstate : public collection_rstate
{
  using	colormap = std::unordered_map<string, color_qi>;

  colormap	klrs;

  color_qi
  get_color(const string& s) const
  {
    auto i = klrs.find(s);
    if (i != klrs.end())
      return i->second;
    else
      throw std::runtime_error("render_state::get_color " + s + " not found");
  }

  color_rstate() = default;
};


/// Global state.
color_rstate&
get_render_state()
{
  static color_rstate state;
  return state;
}


/// Named render state.
/// Datum to take id string and tie it to visual representation.
struct id_rstate: public render_state_base
{
  style		styl; // Overrides render_state_base.opacity
  string	name;
  double	rotate;   // Degrees to rotate to origin (CW), if any.
  double	multiple; // Scale factor, if any.

  static style	dstyl;

  explicit
  id_rstate(const style s = dstyl, const string f = "",
		  const double angle = 0.0, const double scale = 1.0)
  : styl(s), name(f), rotate(angle), multiple(scale) { }

  id_rstate(const id_rstate&) = default;
  id_rstate& operator=(const id_rstate&) = default;
};

style id_rstate::dstyl = { color::black, 0.5, color::black, 0.5, 2 };

using id_rstate_umap = std::unordered_map<string, id_rstate>;


id_rstate_umap&
get_id_rstate_cache()
{
  static id_rstate_umap cache;
  return cache;
}


/// Add value to cache with base style of styl, colors klr, visibility vis.
void
add_to_id_rstate_cache(const string id, const style styl,
			     const select vis)
{
  id_rstate_umap& cache = get_id_rstate_cache();

  id_rstate state(styl, id);
  set_select(state.visible_mode, vis);
  cache.insert(std::make_pair(id, state));
}


/// Given identifier/name/id, get corresponding id_rstate from cache.
const id_rstate
get_id_rstate(const string id)
{
  const id_rstate_umap& cache = get_id_rstate_cache();

  id_rstate ret;
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
const id_rstate
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
  return get_id_rstate(value);
}

} // namespace svg

#endif
