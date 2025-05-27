// svg color bands-*- mode: C++ -*-

// alpha60
// bittorrent x scrape x data + analytics

// Copyright (c) 2024-2025, Benjamin De Kosnik <b.dekosnik@gmail.com>

// This file is part of the alpha60 library.  This library is free
// software; you can redistribute it and/or modify it under the terms
// of the GNU General Public License as published by the Free Software
// Foundation; either version 3, or (at your option) any later
// version.

// This library is distributed in the hope that it will be useful, but
// WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// General Public License for more details.

#ifndef MiL_SVG_COLOR_BAND_H
#define MiL_SVG_COLOR_BAND_H 1

//#include "a60-svg-color-palette.h"
#include <iostream>


namespace svg {


/// Specific regions of spectrum as bands of color.
/// Each band has a starting hue and a number of known good samples.
/// This is then used to seed a generator that computes more of similar hues.
using colorband = std::tuple<color, ushort>;

/// Izzi palette-specific offsets for colorbands.
constexpr colorband cband_bw = std::make_tuple(color::white, 2);
constexpr colorband cband_gray = std::make_tuple(color::white, 16);
constexpr colorband cband_brown = std::make_tuple(color::duboisbrown5, 7);
constexpr colorband cband_r = std::make_tuple(color::red, 17);
constexpr colorband cband_o = std::make_tuple(color::orange, 10);
constexpr colorband cband_y = std::make_tuple(color::hellayellow, 10);
constexpr colorband cband_g = std::make_tuple(color::green, 21);
constexpr colorband cband_b = std::make_tuple(color::blue, 34);
constexpr colorband cband_p = std::make_tuple(color::purple, 33);


/**
  Generate a color band from starting hue and seeds.

  Algorithm is average two known good, where two picked randomly.

  Return type is a vector of generated color_qi types.
*/
color_qis
make_color_band_v1(const colorband& cb, const ushort neededh,
		   auto& spectrum)
{
  // Find starting hue and number of samples in the color band.
  color c = std::get<0>(cb);
  ushort hn = std::get<1>(cb);

  // Find initial offset.
  auto itr = std::find(spectrum.begin(), spectrum.end(), c);
  if (itr == spectrum.end())
    {
      string m("collection::make_color_band_v1: color " + to_string(c));
      m += " not found in spectrum of size ";
      m += std::to_string(spectrum.size());
      throw std::runtime_error(m);
    }
  const ulong offset = std::distance(spectrum.begin(), itr);

  // Randomness.
  static std::mt19937_64 rg(std::random_device{}());

  // Setup random picker of sample hues in band.
  auto disti = std::uniform_int_distribution<>(0, hn - 1);

  // Set up random percentage for combining two colors.
  auto distr = std::uniform_real_distribution<>(0, 1);

  // Generate new from averaging random samples, cache in return vector.
  color_qis cband;
  for (ushort i = 0; i < neededh; ++i)
    {
      // New color.
      ushort o1 = disti(rg);
      ushort o2 = disti(rg);
      color_qi c1 = spectrum[offset + o1];
      color_qi c2 = spectrum[offset + o2];

      // Combine.
      double c1r = distr(rg);
      double c2r = 2.0 - c1r;
      color_qi cgen = combine_color_qi(c1, c1r, c2, c2r);
      cband.push_back(cgen);
    }

  return cband;
}


/// Algorightm is HSV generation.
color_qis
make_color_band_v2(const colorband& cb, const ushort neededh)
{
  // Find starting hue and number of samples in the color band.
  color_qi h = std::get<0>(cb);

  std::set<color_qi> uklrs;
  while (uklrs.size() < neededh)
    {
      std::cout << to_string(h) << std::endl;

      color_qf hhsv = mutate_color_qf(h);
      uklrs.insert(hhsv.to_color_qi());
      h = next_color(h);
    }

  color_qis cband(uklrs.begin(), uklrs.end());
  std::sort(cband.begin(), cband.end(), svg::color_qf_lt_v);
  std::reverse(cband.begin(), cband.end());

  return cband;
}


/// Forwarding function.
color_qis
make_color_band(const colorband& cb, const ushort neededh)
{
  return make_color_band_v1(cb, neededh, active_spectrum());
  //return make_color_band_v2(cb, neededh);
}


/// Flip through color band colors.
/// @bandn is the number of colors in the colorband.
color_qi
next_in_color_band(const colorband& cb, const ushort bandn = 400)
{
  // Generate bands.
  static color_qis gband_bw = make_color_band_v1(cband_bw, bandn,
						 svg::izzi_palette);
  static color_qis gband_y = make_color_band(cband_y, bandn);
  static color_qis gband_r = make_color_band(cband_r, bandn);
  static color_qis gband_g = make_color_band(cband_g, bandn);
  static color_qis gband_b = make_color_band(cband_b, bandn);
  static color_qis gband_p = make_color_band(cband_p, bandn);
  static color_qis gband_o = make_color_band(cband_o, bandn);
  static color_qis gband_brown = make_color_band(cband_brown, bandn);

  color_qi ret;
  const color c = std::get<0>(cb);
  switch (c)
    {
    case color::white:
      ret = gband_bw.back();
      gband_bw.pop_back();
      break;
    case color::hellayellow:
      ret = gband_y.back();
      gband_y.pop_back();
      break;
    case color::orange:
      ret = gband_o.back();
      gband_o.pop_back();
      break;
    case color::duboisbrown1:
      ret = gband_brown.back();
      gband_brown.pop_back();
      break;
    case color::red:
      ret = gband_r.back();
      gband_r.pop_back();
      break;
    case color::green:
      ret = gband_g.back();
      gband_g.pop_back();
      break;
    case color::blue:
      ret = gband_b.back();
      gband_b.pop_back();
      break;
    case color::purple:
      ret = gband_p.back();
      gband_p.pop_back();
      break;
    default:
      string m("next_in_color_band:: error");
      m += k::newline;
      m += "color is: ";
      m += to_string(c);
      throw std::runtime_error(m);
      break;
    }
  return ret;
}

} // namespace svg

#endif
