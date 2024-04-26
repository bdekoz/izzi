// svg custom palette-*- mode: C++ -*-

// alpha60
// bittorrent x scrape x data + analytics

// Copyright (c) 2024, Benjamin De Kosnik <b.dekosnik@gmail.com>

// This file is part of the alpha60 library.  This library is free
// software; you can redistribute it and/or modify it under the terms
// of the GNU General Public License as published by the Free Software
// Foundation; either version 3, or (at your option) any later
// version.

// This library is distributed in the hope that it will be useful, but
// WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// General Public License for more details.

#ifndef MiL_SVG_COLOR_PALETTE_H
#define MiL_SVG_COLOR_PALETTE_H 1

//#include "a60-svg-color.h"


namespace svg {

/// Palette, finite set of colors used. Must end with color::none.
template<typename T, std::size_t N>
using palette = std::array<T, N>;

template<std::size_t N>
using palette_qi = palette<color_qi, N>;

template<std::size_t N>
using palette_qf = palette<color_qf, N>;

/// Default colors for izzi.
palette_qi<color_max_size> izzi_palette =
{
 // black to gray to white in 10% and 25% increments
 color::white,
 color::black,
 color::gray10, color::gray20, color::gray25, color::gray30,
 color::gray40, color::gray50, color::gray60, color::gray70,
 color::gray75, color::gray80, color::gray90,

 color::wcag_lgray, color::wcag_gray, color::wcag_dgray,

 // yellow
 color::hellayellow, color::navajowhite,
 color::ivory, color::gold, color::antiquewhite,
 color::lemonchiffon, color::goldenrod,
 color::duboisyellow1, color::duboisyellow2, color::duboisyellow3,

 // orange
 color::orange, color::darkorange, color::asamaorange, color::dutchorange,
 color::internationalorange, color::goldenyellow, color::kanzoiro,
 color::daylily,  color::orangered, color::duboisred2,

 // red
 color::red,  color::coral, color::salmon, color::akabeni,
 color::akebonoiro, color::ochre, color::sohi, color::benikaba,
 color::benitobi, color::ake, color::crimson, color::tomato,
 color::foreigncrimson, color::ginshu,
 color::duboisred1, color::duboisred3,
 color::redorange,

 // brown
 color::duboisbrown5, color::duboisbrown4,
 color::duboisbrown1, color::duboisbrown2, color::duboisbrown3,
 color::kohakuiro, color::kinsusutake,

 // green
 color::green, color::limegreen, color::springgreen, color::byakuroku,
 color::usumoegi, color::aquamarine, color::midori, color::rokusho,
 color::aotakeiro, color::seiheki, color::seijiiro, color::yanagizome,
 color::hiwamoegi, color::chartreuse, color::greenyellow, color::moegi,
 color::duboisgreen1, color::duboisgreen2, color::duboisgreen3,
 color::duboisgreen4, color::duboisgreen5,

 // blue
 color::blue, color::ultramarine, color::shinbashiiro, color::hanada,
 color::ruriiro, color::cornflowerblue, color::lightblue, color::skyblue,
 color::lightskyblue, color::bellflower, color::navy, color::asagiiro,
 color::indigo, color::rurikon, color::asamablue, color::cyan,
 color::lightcyan, color::powderblue, color::steelblue, color::dodgerblue,
 color::royalblue, color::mediumblue, color::deepskyblue,
 color::midnightblue, color::darkslateblue, color::slateblue,
 color::azure, color::crayolacerulean,
 color::duboisblue1, color::duboisblue2, color::duboisblue3,
 color::blueprintlight, color::blueprint, color::blueprintdark,

 // purple (magenta, violet, pink)
 color::purple, color::wisteria, color::asamapink, color::pink, color::peony,
 color::violet, color::magenta, color::dfuschia, color::deeppink, color::hotpink,
 color::dustyrose, color::atmosphericp, color::kissmepink,
 color::futaai, color::benimidori, color::redwisteria, color::botan,
 color::kokimurasaki, color::usuiro, color::murasaki, color::ayameiro,
 color::blueviolet, color::darkmagenta, color::darkviolet,
 color::thistle, color::plum,
 color::palevioletred, color::mediumvioletred, color::lavender,
 color::orchid, color::mediumorchid, color::darkestmagenta, color::mediumpurple,

 color::none
};


/// Japan colors (67)
palette_qi<76> jp_palette =
{
 // yellow
 color::hellayellow, color::gold,

 // orange
 color::orange, color::internationalorange,

 // red
 color::red,  color::akabeni, color::benitobi,

 // brown
 color::duboisbrown1, color::kohakuiro, color::kinsusutake,

 // green
 color::aotakeiro, color::duboisgreen4, color::rokusho,
 color::yanagizome, color::hiwamoegi, color::duboisgreen2, color::chartreuse,
 color::green, color::springgreen, color::aquamarine,


 // blue
 color::blue, color::ultramarine, color::shinbashiiro, color::hanada,
 color::cornflowerblue, color::lightblue, color::skyblue,
 color::asagiiro, color::rurikon, color::asamablue, color::cyan,
 color::lightcyan, color::powderblue, color::steelblue, color::dodgerblue,
 color::royalblue, color::mediumblue, color::deepskyblue,
 color::azure, color::crayolacerulean,
 color::duboisblue1, color::duboisblue2,
 color::blueprintlight, color::blueprint,

 // purple (magenta, violet, pink)
 color::purple, color::wisteria, color::asamapink, color::pink, color::peony,
 color::violet, color::magenta, color::dfuschia, color::deeppink,
 color::hotpink, color::dustyrose, color::atmosphericp, color::kissmepink,
 color::futaai, color::redwisteria, color::botan,
 color::kokimurasaki, color::usuiro, color::murasaki, color::ayameiro,
 color::blueviolet, color::darkmagenta, color::darkviolet,
 color::thistle, color::plum,
 color::palevioletred, color::mediumvioletred, color::orchid,
 color::mediumorchid, color::darkestmagenta, color::mediumpurple,

 color::none
};


/**
   CIECAM02-UCS color space, perceptually different colors, equally spaced out.

   Hand-picked values for a 67-68 BTIHA, aka cyberwar.
   Start with color constraints, generate 264 options, hand-pick to n=68.
   http://jnnnnn.github.io/category-colors-constrained.html
*/
palette_qi<73> ciecam02_palette =
{
  // SE, red to orange/yellow (18)
  color_qi(251,225,217), color_qi(254,231,192), color_qi(253,196,189),
  color_qi(212,130,149), color_qi(253,225,137), color_qi(252,205,149),
  color_qi(217,167,66), color_qi(254,205,15), color_qi(246,166,2),
  color_qi(205,129,38), color_qi(233,112,10), color_qi(208,73,7),
  color_qi(253,65,9), color_qi(243,28,78), color_qi(201,8,10),
  color_qi(202,105,73), color_qi(242,92,88), color_qi(175,77,68),
  /*
    color_qi(253,176,130), color_qi(253,162,71), color_qi(246,131,146),
    color_qi(254,103,148), color_qi(250,45,142), color_qi(213,9,126),
  */

  // SW, yellow, chartreuse, green, aquamarine (18)
  color_qi(204,246,233), color_qi(155,254,227), color_qi(253,233,69),
  color_qi(214,222,15), color_qi(211,254,20), color_qi(158,247,24),
  color_qi(10,216,17), color_qi(10,235,101), color_qi(27,232,149),
  color_qi(46,246,197), color_qi(224,248,106), color_qi(183,255,128),
  color_qi(97,189,41), color_qi(163,238,148), color_qi(207,229,137),
  color_qi(129,255,173), color_qi(158,227,189), color_qi(206,248,196),

  // NE lavender to red violet (18)
  color_qi(91,90,208), color_qi(125,91,240), color_qi(167,26,253),
  color_qi(181,157,251), color_qi(214,202,254), color_qi(178,86,249),
  color_qi(214,148,253), color_qi(188,121,216), color_qi(159,32,195),
  color_qi(245,17,253), color_qi(251,80,225), color_qi(206,98,203),
  color_qi(252,18,201), color_qi(235,125,171), color_qi(252,184,211),
  color_qi(255,226,241), color_qi(254,199,248), color_qi(221,177,238),

  // NW sky blue to indigo (18)
  color_qi(11,240,233), color_qi(16,224,248), color_qi(152,252,255),
  color_qi(199,236,249), color_qi(231,234,253), color_qi(187,217,253),
  color_qi(126,216,254), color_qi(95,153,253), color_qi(57,87,255),
  color_qi(22,130,251), color_qi(16,176,255), color_qi(48,199,253),
  color_qi(5,169,209), color_qi(204,214,228), color_qi(100,214,203),
  color_qi(146,166,254), color_qi(123,159,212), color_qi(105,154,179),

  color::none
};


/// Set and Get working spectrum, aka default palette.
auto&
active_spectrum()
{
  static auto spectrum = izzi_palette;
  //static auto  spectrum = jp_palette;
  //static auto spectrum = ciecam02_palette;
  return spectrum;
}


/// Random entry from array above.
color_qi
random_color(const uint startoffset = 0)
{
  auto& spectrum = active_spectrum();
  const uint maxc = spectrum.size();
  static std::mt19937_64 rg(std::random_device{}());
  auto disti = std::uniform_int_distribution<>(startoffset, maxc - 1);
  uint index = disti(rg);
  return spectrum[index];
}

/// Loop through color array starting at position c.
color_qi
next_color(const color_qi klr)
{
  auto& spectrum = active_spectrum();
  color_qi cnext = spectrum.front();
  auto itr = std::find(spectrum.begin(), spectrum.end(), klr);
  if (itr != spectrum.end())
    {
      ++itr;
      if (itr == spectrum.end())
	itr = spectrum.begin();
      cnext = *itr;
    }
  return cnext;
}
  
/// Start at specified color bar entry point.
color_qi
start_at_color(const color_qi klr)
{
  static color_qi _S_klr = klr;
  color_qi retk = _S_klr;
  _S_klr = next_color(_S_klr);
  return retk;
}
  
} // namespace svg

#endif
