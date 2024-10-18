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
static const palette_qi<color_max_size - 4> izzi_palette =
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

/// Only color no black and white or gray
static const palette_qi<color_max_size - 20> izzi_hue_palette =
{
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

/// Japan colors (118)
/// https://en.wikipedia.org/wiki/Traditional_colors_of_Japan
static const palette_qi<118> jp_palette =
{
  color_qi(240,143,144), color_qi(219,90,107), color_qi(252,201,185),
  color_qi(242,102,108), color_qi(245,143,132), color_qi(185,87,84),
  color_qi(157,41,51), color_qi(247,102,90), color_qi(151,100,90),
  color_qi(195,39,43), color_qi(103,36,34), color_qi(94,40,36),
  color_qi(250,123,98), color_qi(220,48,35), color_qi(147,67,55),
  color_qi(145,50,40), color_qi(53,30,28), color_qi(211,78,54),
  color_qi(161,61,45), color_qi(117,46,35), color_qi(255,121,82),
  color_qi(230,131,100), color_qi(227,92,56), color_qi(179,92,68),
  color_qi(155,83,63), color_qi(96,40,30), color_qi(76,34,27),
  color_qi(255,53,0), color_qi(159,116,98), color_qi(89,43,31),
  color_qi(245,127,79), color_qi(159,82,51), color_qi(152,85,56),
  color_qi(255,162,107), color_qi(255,137,54), color_qi(251,129,54),
  color_qi(46,33,27), color_qi(202,105,36), color_qi(213,120,53),
  color_qi(169,98,50), color_qi(140,89,57), color_qi(89,58,39),
  color_qi(190,127,81), color_qi(183,112,45), color_qi(219,132,73),
  color_qi(247,187,125), color_qi(255,166,49), color_qi(203,126,31),
  color_qi(120,94,73), color_qi(250,169,69), color_qi(187,129,65),
  color_qi(255,185,78), color_qi(230,155,58), color_qi(176,146,122),
  color_qi(127,107,93), color_qi(102,83,67), color_qi(161,121,23),
  color_qi(92,72,39), color_qi(226,177,60), color_qi(211,177,125),
  color_qi(149,123,56), color_qi(100,85,48), color_qi(189,169,40),
  color_qi(156,138,77), color_qi(71,63,45), color_qi(82,75,42),
  color_qi(133,124,85), color_qi(122,148,46), color_qi(188,181,140),
  color_qi(140,158,94), color_qi(82,89,59), color_qi(140,156,118),
  color_qi(129,123,105), color_qi(55,66,49), color_qi(165,186,147),
  color_qi(64,122,82), color_qi(61,64,53), color_qi(101,98,85),
  color_qi(45,68,54), color_qi(90,100,87), color_qi(129,156,139),
  color_qi(58,64,59), color_qi(53,78,75), color_qi(117,125,117),
  color_qi(43,55,54), color_qi(106,127,122), color_qi(72,146,155),
  color_qi(69,88,89), color_qi(38,67,72), color_qi(29,105,124),
  color_qi(77,100,108), color_qi(52,77,86), color_qi(77,143,172),
  color_qi(93,140,174), color_qi(24,27,38), color_qi(0,49,113),
  color_qi(120,119,155), color_qi(118,105,128), color_qi(137,114,158),
  color_qi(135,95,154), color_qi(151,110,154), color_qi(43,32,40),
  color_qi(168,124,160), color_qi(91,50,86), color_qi(35,25,30),
  color_qi(187,119,150), color_qi(117,93,91), color_qi(109,43,80),
  color_qi(164,52,93), color_qi(67,36,42), color_qi(126,38,57),
  color_qi(68,49,46), color_qi(255,221,202), color_qi(151,134,124),
  color_qi(75,60,57), color_qi(53,41,37), color_qi(23,20,18),
  color::none
};


/// ColorBrewer 2.0
/// https://colorbrewer2.org

/// Single hue 3-class sequential, low to high
static const palette_qi<19> colorbrewer2s3s_palette =
{
  /// red
  color_qi(254,224,210), color_qi(252,146,114), color_qi(222,45,38),

  /// orange
  color_qi(254,230,206), color_qi(253,174,107), color_qi(230,85,13),

  /// green
  color_qi(229,245,224), color_qi(161,217,155), color_qi(49,163,84),

  /// blue
  color_qi(222,235,247), color_qi(158,202,225), color_qi(49,130,189),

  /// purple
  color_qi(239,237,245), color_qi(188,189,220), color_qi(117,107,177),

  /// gray
  color_qi(240,240,240), color_qi(189,189,189), color_qi(99,99,99),
  color::none
};

/// Single hue 9-class sequential, low to high
static const palette_qi<55> colorbrewer2s9s_palette =
{
  /// reds
  color_qi(255,245,240), color_qi(254,224,210), color_qi(252,187,161),
  color_qi(252,146,114), color_qi(251,106,74), color_qi(239,59,44),
  color_qi(203,24,29), color_qi(165,15,21), color_qi(103,0,13),

  /// orange
  color_qi(255,245,235), color_qi(254,230,206), color_qi(253,208,162),
  color_qi(253,174,107), color_qi(253,141,60), color_qi(241,105,19),
  color_qi(217,72,1), color_qi(166,54,3), color_qi(127,39,4),

  /// green
  color_qi(247,252,245), color_qi(229,245,224), color_qi(199,233,192),
  color_qi(161,217,155), color_qi(116,217,155), color_qi(65,171,93),
  color_qi(35,139,69), color_qi(0,109,44), color_qi(0,68,27),

  /// blue
  color_qi(247,251,255), color_qi(222,235,247), color_qi(198,219,239),
  color_qi(158,202,225), color_qi(107,174,214), color_qi(66,146,198),
  color_qi(33,113,181), color_qi(8,81,156), color_qi(8,48,107),

  /// purple
  color_qi(252,251,253), color_qi(239,237,245), color_qi(218,218,235),
  color_qi(188,189,220), color_qi(158,154,200), color_qi(128,125,186),
  color_qi(106,81,163), color_qi(84,39,143), color_qi(63,0,125),

  /// gray
  color_qi(255,255,255), color_qi(240,240,240), color_qi(217,217,217),
  color_qi(189,189,189), color_qi(150,150,150), color_qi(115,115,115),
  color_qi(82,82,82), color_qi(37,37,37), color_qi(0,0,0),

  color::none
};

/// Single hue 7-class sequential, low to high
static const palette_qi<43> colorbrewer2s7s_palette =
{
  /// orange
  color_qi(254,237,222), color_qi(253,208,162), color_qi(253,174,107),
  color_qi(253,141,60), color_qi(241,105,19), color_qi(217,72,1),
  color_qi(140,45,4),

  /// reds
  color_qi(254,229,217), color_qi(252,187,161), color_qi(252,146,114),
  color_qi(251,106,74), color_qi(239,59,44), color_qi(203,24,29),
  color_qi(153,0,13),

  /// purple
  color_qi(242,240,247), color_qi(218,218,235), color_qi(188,189,220),
  color_qi(158,154,200), color_qi(128,125,186), color_qi(106,81,163),
  color_qi(74,20,134),

  /// blue
  color_qi(239,243,255), color_qi(198,219,239), color_qi(158,202,225),
  color_qi(107,174,214), color_qi(66,146,198), color_qi(33,113,181),
  color_qi(8,69,148),

  /// green
  color_qi(237,248,233), color_qi(199,233,192), color_qi(161,217,155),
  color_qi(116,196,118), color_qi(65,171,93), color_qi(35,139,69),
  color_qi(0,90,50),

  /// gray
  color_qi(247,247,247), color_qi(217,217,217), color_qi(189,189,189),
  color_qi(150,150,150), color_qi(115,115,115), color_qi(82,82,82),
  color_qi(37,37,37),

  color::none
};


/**
   CIECAM02-UCS color space, perceptually different colors, equally spaced out.

   Hand-picked values for a 67-68 BTIHA, aka cyberwar.
   Start with color constraints, generate 264 options, hand-pick to n=68.
   http://jnnnnn.github.io/category-colors-constrained.html
*/
static const palette_qi<73> ciecam02_palette =
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
  //static auto spectrum = izzi_palette;
  static auto spectrum = izzi_hue_palette;
  //static auto  spectrum = jp_palette;
  //static auto spectrum = ciecam02_palette;

  static bool initp(false);
  if (!initp)
    {
      std::sort(spectrum.begin(), spectrum.end(), svg::color_qf_lt_hue);
      std::reverse(spectrum.begin(), spectrum.end());
      initp = true;
    }
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

template<typename _Spectrm>
color_qi
random_color(const _Spectrm& spectrm, const uint startoffset = 0)
{
  const uint maxc = spectrm.size();
  static std::mt19937_64 rg(std::random_device{}());
  auto disti = std::uniform_int_distribution<>(startoffset, maxc - 1);
  uint index = disti(rg);
  return spectrm[index];
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
