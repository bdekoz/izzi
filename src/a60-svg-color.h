// svg color -*- mode: C++ -*-

// Copyright (C) 2014-2023 Benjamin De Kosnik <b.dekosnik@gmail.com>

// This file is part of the alpha60-MiL SVG library.  This library is
// free software; you can redistribute it and/or modify it under the
// terms of the GNU General Public License as published by the Free
// Software Foundation; either version 3, or (at your option) any
// later version.

// This library is distributed in the hope that it will be useful, but
// WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// General Public License for more details.

#ifndef MiL_SVG_COLOR_H
#define MiL_SVG_COLOR_H 1

#include <string>
#include <map>
#include <algorithm>
#include <random>


namespace svg {

/**
   HUE

   color = color enum (and name string conversion)
   color_qi = color quantified as RGB integral values 0-255 (similar to Scalar in OpenCV).
   color_qf = color quantified as Hue Saturation Value (HSV) decimal values 0-1
   spectrum = color spectrum as finite array of color enum values
*/

/// Color enumerated as types.
enum class color
{
  none,

  // 13 tints / shades
  white,
  black,
  gray10,
  gray20,
  gray25,		// gainsboro
  gray30,
  gray40,
  gray50,
  gray60,
  gray75,		// slategray
  gray70,
  gray80,
  gray90,

  wcag_lgray,
  wcag_gray,
  wcag_dgray,

  // 15 yellow
  kanzoiro,		// daylily light orange
  kohakuiro,		// amber
  kinsusutake,		// golden-gray bamoo
  daylily,
  goldenyellow,
  hellayellow,
  antiquewhite,
  lemonchiffon,
  goldenrod,
  navajowhite,
  ivory,
  gold,
  duboisyellow1,
  duboisyellow2,
  duboisyellow3,

  // 7 orange
  orange,
  darkorange,
  asamaorange,
  redorange,
  orangered,
  dutchorange,
  internationalorange,

  // 17 red
  red,
  foreigncrimson,	// red
  ginshu,		// gray red
  akabeni,		// pure crimson
  akebonoiro,		// dawn color
  ochre,
  sohi,
  benikaba,		// red birch
  benitobi,		// red kite bird
  ake,			// scarlet/blood
  crimson,
  tomato,
  coral,
  salmon,
  duboisred1,
  duboisred2,
  duboisred3,

  // 4 brown
  duboisbrown1,
  duboisbrown2,
  duboisbrown3,
  duboisbrown4,
  duboisbrown5,

  // 20 green
  byakuroku,		// whitish green
  usumoegi,		// pale onion
  moegi,			// onion green
  hiwamoegi,		// siskin sprout
  midori,
  rokusho,
  aotakeiro,		// green bamboo
  seiheki,		// blue green
  seijiiro,		// celadon
  yanagizome,		// willow dye
  green,
  chartreuse,
  greenyellow,
  limegreen,
  springgreen,
  aquamarine,
  duboisgreen1,
  duboisgreen2,
  duboisgreen3,
  duboisgreen4,
  duboisgreen5,

  // 35 blue
  blue,
  ultramarine,
  shinbashiiro,
  hanada,		// blue silk
  ruriiro,		// lapis
  bellflower,
  navy,
  asagiiro,		// light blue
  indigo,
  rurikon,		// dark blue lapis
  asamablue,
  cyan,
  lightcyan,
  powderblue,
  steelblue,
  cornflowerblue,
  deepskyblue,
  dodgerblue,
  lightblue,
  skyblue,
  lightskyblue,
  midnightblue,
  mediumblue,
  royalblue,
  darkslateblue,
  slateblue,
  azure,
  crayolacerulean,
  duboisblue1,
  duboisblue2,
  duboisblue3,
  duboisblue4,
  blueprintlight,
  blueprint,
  blueprintdark,

  // 32 purple
  wisteria,
  murasaki,		// purple
  ayameiro,
  peony,
  futaai,		// dark indigo
  benimidori,		// stained red/violet
  redwisteria,		// dusty rose
  botan,			// tree peony
  kokimurasaki,		// deep purple
  usuiro,		// thin
  asamapink,
  blueviolet,
  darkmagenta,
  darkviolet,
  thistle,
  plum,
  violet,
  magenta,
  dfuschia,
  deeppink,
  hotpink,
  pink,
  kissmepink,
  palevioletred,
  mediumvioletred,
  lavender,
  orchid,
  mediumorchid,
  darkestmagenta,
  mediumpurple,
  purple,
  dustyrose,
  atmosphericp,

  // STOS
  command,
  science,
  engineering,

  // Must be last, so can be cast to int for size.
  last
};

/// Total number of enumerated colors.
constexpr uint color_max_size = static_cast<uint>(color::last);

/// Convert color to string.
const std::string&
to_string(const color e)
{
  using enum_map_type = std::map<color, std::string>;

  static enum_map_type enum_map;
  if (enum_map.empty())
    {
      enum_map[color::white] = "rgb(255, 255, 255)";
      enum_map[color::black] = "rgb(0, 0, 0)";
      enum_map[color::gray90] = "rgb(25, 25, 25)";
      enum_map[color::gray80] = "rgb(50, 50, 50)";
      enum_map[color::gray75] = "rgb(64, 64, 64)";
      enum_map[color::gray70] = "rgb(77, 77, 77)";
      enum_map[color::gray60] = "rgb(100, 100, 100)";
      enum_map[color::gray50] = "rgb(128, 128, 128)";
      enum_map[color::gray40] = "rgb(150, 150, 150)";
      enum_map[color::gray30] = "rgb(180, 180, 180)";
      enum_map[color::gray25] = "rgb(191, 191, 191)";
      enum_map[color::gray20] = "rgb(200, 200, 200)";
      enum_map[color::gray10] = "rgb(230, 230, 230)";

      enum_map[color::wcag_lgray] = "rgb(148, 148, 148)"; // LG TXT on white 3:1
      enum_map[color::wcag_gray] = "rgb(118, 118, 118)"; // min on white 4.5:1
      enum_map[color::wcag_dgray] = "rgb(46, 46, 46)"; // on white 13.6:1

      enum_map[color::command] = "rgb(255, 0, 171)";
      enum_map[color::science] = "rgb(150, 230, 191)";
      enum_map[color::engineering] = "rgb(161, 158, 178)";

      enum_map[color::kissmepink] = "rgb(255, 59, 241)";

      enum_map[color::red] = "rgb(255, 0, 0)";
      enum_map[color::green] = "rgb(0, 255, 0)";
      enum_map[color::blue] = "rgb(0, 0, 255)";

      enum_map[color::asamablue] = "rgb(1, 137, 255)";
      enum_map[color::asamaorange] = "rgb(236, 75, 37)";
      enum_map[color::asamapink] = "rgb(200, 56, 81)";

      // Yellows
      enum_map[color::kanzoiro] = "rgb(255, 137, 54)";
      enum_map[color::kohakuiro] = "rgb(202, 105, 36)";
      enum_map[color::kinsusutake] = "rgb(125, 78, 45)";
      enum_map[color::daylily] = "rgb(255, 137, 54)";
      enum_map[color::goldenyellow] = "rgb(255, 164, 0)";
      enum_map[color::hellayellow] = "rgb(255, 255, 0)";
      enum_map[color::antiquewhite] = "rgb(250, 235, 215)";
      enum_map[color::lemonchiffon] = "rgb(255, 250, 205)";
      enum_map[color::goldenrod] = "rgb(250, 250, 210)";
      enum_map[color::navajowhite] = "rgb(255, 222, 173)";

      enum_map[color::ivory] = "rgb(255, 255, 240)";
      enum_map[color::gold] = "rgb(255, 215, 0)";

      enum_map[color::duboisyellow1] = "rgb(255, 255, 5)";
      enum_map[color::duboisyellow2] = "rgb(255, 234, 18)";
      enum_map[color::duboisyellow3] = "rgb(255, 215, 1)";

      // Orange
      enum_map[color::orange] = "rgb(255, 165, 0)";
      enum_map[color::orangered] = "rgb(255, 69, 0)";
      enum_map[color::redorange] = "rgb(220, 48, 35)";
      enum_map[color::darkorange] = "rgb(255, 140, 17)";
      enum_map[color::dutchorange] = "rgb(250, 155, 30)";
      enum_map[color::internationalorange] = "rgb(255, 79, 0)";

      // Brown
      enum_map[color::duboisbrown1] = "rgb(128, 5, 5)";
      enum_map[color::duboisbrown2] = "rgb(134, 90, 61)";
      enum_map[color::duboisbrown3] = "rgb(81, 55, 42)";
      enum_map[color::duboisbrown4] = "rgb(197, 146, 37)";
      enum_map[color::duboisbrown5] ="rgb(255, 240, 200)";

      // Reds
      enum_map[color::foreigncrimson] = "rgb(201, 31, 55)";
      enum_map[color::ginshu] = "rgb(188, 45, 41)";
      enum_map[color::akabeni] = "rgb(195, 39,43)";
      enum_map[color::akebonoiro] = "rgb(250, 123, 98)";

      enum_map[color::ochre] = "rgb(255, 78, 32)";
      enum_map[color::sohi] = "rgb(227, 92, 56)";
      enum_map[color::benikaba] = "rgb(157, 43, 34)";
      enum_map[color::benitobi] = "rgb(145, 50, 40)";
      enum_map[color::ake] = "rgb(207, 58, 36)";

      enum_map[color::crimson] = "rgb(220, 20, 60)";
      enum_map[color::tomato] = "rgb(255, 99, 71)";
      enum_map[color::coral] = "rgb(255, 127, 80)";
      enum_map[color::salmon] = "rgb(250, 128, 114)";

      enum_map[color::duboisred1] = "rgb(255, 29, 16)";
      enum_map[color::duboisred2] = "rgb(249,110, 11)";
      enum_map[color::duboisred3] = "rgb(215, 25, 50)";

      // Greens
      enum_map[color::byakuroku] = "rgb(165, 186, 147)";
      enum_map[color::usumoegi] = "rgb(141, 178, 85)";
      enum_map[color::moegi] = "rgb(91, 137, 48)";
      enum_map[color::hiwamoegi] = "rgb(122, 148, 46)";
      enum_map[color::midori] = "rgb(42, 96, 59)";
      enum_map[color::rokusho] = "rgb(64, 122, 82)";
      enum_map[color::aotakeiro] = "rgb(0, 100, 66)";
      enum_map[color::seiheki] = "rgb(58, 105, 96)";
      enum_map[color::seijiiro] = "rgb(129, 156, 139)";
      enum_map[color::yanagizome] = "rgb(140, 158, 94)";

      enum_map[color::chartreuse] = "rgb(127, 255, 0)";
      enum_map[color::greenyellow] = "rgb(173, 255, 47)";
      enum_map[color::limegreen] = "rgb(50, 205, 50)";
      enum_map[color::springgreen] = "rgb(0, 255, 127)";
      enum_map[color::aquamarine] = "rgb(127, 255, 212)";

      enum_map[color::duboisgreen1] = "rgb(5, 255, 5)";
      enum_map[color::duboisgreen2] = "rgb(127, 225, 15)";
      enum_map[color::duboisgreen3] = "rgb(16, 114, 9)";
      enum_map[color::duboisgreen4] = "rgb(0, 148, 16)";
      enum_map[color::duboisgreen5] = "rgb(24, 57, 30)";

      // Blues
      enum_map[color::ultramarine] = "rgb(93, 140, 174)";
      enum_map[color::shinbashiiro] = "rgb(0, 108, 127)";
      enum_map[color::hanada] = "rgb(4, 79, 103)";
      enum_map[color::ruriiro] = "rgb(31, 71, 136)";
      enum_map[color::bellflower] = "rgb(25, 31, 69)";
      enum_map[color::navy] = "rgb(0, 49, 113)";
      enum_map[color::asagiiro] = "rgb(72, 146, 155)";
      enum_map[color::indigo] = "rgb(38, 67, 72)";
      enum_map[color::rurikon] = "rgb(27, 41, 75)";
      enum_map[color::cyan] = "rgb(0, 255, 255)";

      enum_map[color::lightcyan] = "rgb(224, 255, 255)";
      enum_map[color::powderblue] = "rgb(176, 224, 230)";
      enum_map[color::steelblue] = "rgb(70, 130, 237)";
      enum_map[color::cornflowerblue] = "rgb(100, 149, 237)";
      enum_map[color::deepskyblue] = "rgb(0, 191, 255)";
      enum_map[color::dodgerblue] = "rgb(30, 144, 255)";
      enum_map[color::lightblue] = "rgb(173, 216, 230)";
      enum_map[color::skyblue] = "rgb(135, 206, 235)";
      enum_map[color::lightskyblue] = "rgb(173, 206, 250)";
      enum_map[color::midnightblue] = "rgb(25, 25, 112)";

      enum_map[color::mediumblue] = "rgb(0, 0, 205)";
      enum_map[color::royalblue] = "rgb(65, 105, 225)";
      enum_map[color::darkslateblue] = "rgb(72, 61, 139)";
      enum_map[color::slateblue] = "rgb(106, 90, 205)";
      enum_map[color::azure] = "rgb(240, 255, 255)";
      enum_map[color::crayolacerulean] = "rgb(29, 172, 214)";

      enum_map[color::duboisblue1] = "rgb(37, 42, 255)";
      enum_map[color::duboisblue2] = "rgb(100, 150, 245)";
      enum_map[color::duboisblue3] = "rgb(74, 87, 129)";
      enum_map[color::duboisblue4] = "rgb(49, 64, 103)";

      enum_map[color::blueprintlight] = "rgb(0, 25, 166)";
      enum_map[color::blueprint] = "rgb(0, 20, 132)";
      enum_map[color::blueprintdark] = "rgb(0, 16, 106)";

      // Purples
      enum_map[color::wisteria] = "rgb(135, 95, 154)";
      enum_map[color::murasaki] = "rgb(79, 40, 75)";
      enum_map[color::ayameiro] = "rgb(118, 53, 104)";
      enum_map[color::peony] = "rgb(164, 52, 93)";
      enum_map[color::futaai] = "rgb(97, 78, 110)";
      enum_map[color::benimidori] = "rgb(120, 119, 155)";
      enum_map[color::redwisteria] = "rgb(187, 119, 150)";
      enum_map[color::botan] = "rgb(164, 52, 93)";
      enum_map[color::kokimurasaki] = "rgb(58, 36, 59)";
      enum_map[color::usuiro] = "rgb(168, 124, 160)";

      enum_map[color::blueviolet] = "rgb(138, 43, 226)";
      enum_map[color::darkmagenta] = "rgb(139, 0, 139)";
      enum_map[color::darkviolet] = "rgb(148, 0, 211)";
      enum_map[color::thistle] = "rgb(216, 191, 216)";
      enum_map[color::plum] = "rgb(221, 160, 221)";
      enum_map[color::violet] = "rgb(238, 130, 238)";
      enum_map[color::magenta] = "rgb(255, 0, 255)";
      enum_map[color::dfuschia] = "rgb(255, 35, 255)";
      enum_map[color::deeppink] = "rgb(255, 20, 147)";
      enum_map[color::hotpink] = "rgb(255, 105, 180)";
      enum_map[color::pink] = "rgb(255, 192, 203)";

      enum_map[color::palevioletred] = "rgb(219, 112, 147)";
      enum_map[color::mediumvioletred] = "rgb(199, 21, 133)";
      enum_map[color::lavender] = "rgb(230, 230, 250)";
      enum_map[color::orchid] = "rgb(218, 112, 214)";
      enum_map[color::mediumorchid] = "rgb(186, 85, 211)";
      enum_map[color::darkestmagenta] = "rgb(180, 0, 180)";
      enum_map[color::mediumpurple] = "rgb(147, 112, 219)";
      enum_map[color::purple] = "rgb(128, 0, 128)";
      enum_map[color::dustyrose] = "rgb(191, 136, 187)";
      enum_map[color::atmosphericp] = "rgb(228, 210, 231)";

      enum_map[color::none] = "rgb(1, 0, 0)";
      enum_map[color::last] = "rgb(0, 0, 1)";

      // Error check to make sure all the colors have names/values.
      if (enum_map.size() != color_max_size + 1)
	{
	  string m("to_string(color)::color map size fail ");
	  m += k::newline;
	  m += std::to_string(enum_map.size());
	  m += " not equal to named colors of size ";
	  m += k::newline;
	  m += std::to_string(color_max_size);
	  throw std::runtime_error(m);
	}
    }
  return enum_map[e];
}


/// Color quantified as integral RGB components in the range [0,255].
/// aka like Scalar in OpenCV.
struct color_qi
{
  using itype = unsigned short;

  itype	r;
  itype	g;
  itype	b;

  // Return "rgb(64, 64, 64)";

  static string
  to_string(color_qi s)
  {
    std::ostringstream oss;
    oss << "rgb(" << s.r << ',' << s.g << ',' << s.b << ")";
    return oss.str();
  }

  // From "rgb(64, 64, 64)";
  static color_qi
  from_string(string s)
  {
    // Kill rgb() enclosing, if be.
    if (s.empty() || s.size() < 5 || s[0] != 'r')
      {
	string m("color_qi::from_string input is not in rbg form: ");
	m += s;
	m += k::newline;
	throw std::runtime_error(m);
	}
    else
      {
	s.pop_back();
	s = s.substr(4);
      }

    // String stream which eats whitespace and knows number separation.
    std::istringstream iss(s);
    iss >> std::skipws;

    char c(0);

    ushort rs(0);
    iss >> rs;
    itype r = static_cast<itype>(rs);
    iss >> c;

    ushort gs(0);
    iss >> gs;
    itype g = static_cast<itype>(gs);
    iss >> c;

    ushort bs(0);
    iss >> bs;
    itype b = static_cast<itype>(bs);

    return color_qi(r, g, b);
  }

  color_qi() = default;
  color_qi(const color_qi&) = default;
  color_qi& operator=(const color_qi&) = default;

  // auto operator<=>(const color_qi&) const = default;

  color_qi(itype ra, itype ga, itype ba) : r(ra), g(ga), b(ba) { }

  color_qi(const color e)
  {
    color_qi klr = from_string(svg::to_string(e));
    r = klr.r;
    b = klr.b;
    g = klr.g;
  }
};


/// Convert color_qi to string.
const std::string
to_string(const color_qi klr)
{ return color_qi::to_string(klr); }


/**
   Color quantified as floating point HSV components in the range [0,1].

   https://web.archive.org/web/20150303174723/
   http://en.literateprograms.org/RGB_to_HSV_color_space_conversion_(C)
*/
struct color_qf
{
  using ftype = float;

  ftype	h; /// Hue
  ftype	s; /// Saturation
  ftype	v; /// Value

  static string
  to_string(color_qf s)
  {
    std::ostringstream oss;
    oss << "hsv(" << s.h << ',' << s.s << ',' << s.v << ")";
    return oss.str();
  }

  // From "rgb(64, 64, 64)";
  static color_qf
  from_string(string)
  {
    ftype i(0);
    return color_qf(i, i, i);
  }

  color_qf() = default;
  color_qf(const color_qf&) = default;
  color_qf& operator=(const color_qf&) = default;

  // auto operator<=>(const color_qf&) const = default;

  color_qf(ftype vh, ftype vs, ftype vv) : h(vh), s(vs), v(vv) { }

  color_qf(const color e)
  {
    color_qf klr = from_string(svg::to_string(e));
    h = klr.h;
    s = klr.s;
    v = klr.v;
  }
};


/// Convert color_qf to string.
const std::string
to_string(const color_qf klr)
{ return color_qf::to_string(klr); }


/**
  Combine color a with color b in percentages ad and ab, respectively.

  To average, constrain paramters ad and ab such that: ad + ab == 2.

  Like so:
  ushort ur = (a.r + b.r) / 2;
  ushort ug = (a.g + b.g) / 2;
  ushort ub = (a.b + b.b) / 2;
*/
color_qi
combine_two_color_qi(const color_qi& a, const double ad,
		   const color_qi& b, const double bd)
{
  double denom = ad + bd;
  double ur = ((a.r * ad) + (b.r * bd)) / denom;
  double ug = ((a.g * ad) + (b.g * bd)) / denom;
  double ub = ((a.b * ad) + (b.b * bd)) / denom;

  using itype = color_qi::itype;
  itype cr = static_cast<itype>(ur);
  itype cg = static_cast<itype>(ug);
  itype cb = static_cast<itype>(ub);
  return color_qi { cr, cg, cb };
}

/// Average two colors, return the result.
color_qi
average_two_color_qi(const color_qi& a, const color_qi& b)
{ return combine_two_color_qi(a, 1.0, b, 1.0); }


/// Color iteration and combinatorics.
using colors = std::vector<color>;
using color_qis = std::vector<color_qi>;

using color_array = std::array<color, color_max_size>;


/// Color spectrum.
color_array spectrum =
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


/// Random entry from array above.
color
random_color(uint startoffset = 0)
{
  const uint maxc = spectrum.size();
  static std::mt19937_64 rg(std::random_device{}());
  auto disti = std::uniform_int_distribution<>(startoffset, maxc - 1);
  uint index = disti(rg);
  return spectrum[index];
}


/// Loop through color array starting at position c.
color
next_color(color klr)
{
  color cnext = spectrum.front();
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
color_start_at_specified(color klr)
{
  static color _S_klr = klr;
  color_qi retk = _S_klr;
  _S_klr = next_color(_S_klr);
  return retk;
}


// Specific regions of spectrum as bands of color.
// Each band has a starting hue and a number of known good samples.
// This is then used to seed a generator that computes more of similar hues.
using colorband = std::tuple<color, ushort>;
constexpr colorband cband_bw = std::make_tuple(color::white, 2);
constexpr colorband cband_gray = std::make_tuple(color::white, 16);
constexpr colorband cband_y = std::make_tuple(color::hellayellow, 10);
constexpr colorband cband_r = std::make_tuple(color::red, 17);
constexpr colorband cband_g = std::make_tuple(color::green, 21);
constexpr colorband cband_b = std::make_tuple(color::blue, 34);
constexpr colorband cband_p = std::make_tuple(color::purple, 33);
constexpr colorband cband_o = std::make_tuple(color::orange, 10);
constexpr colorband cband_brown = std::make_tuple(color::duboisbrown1, 7);


/// Add white to tint in density % (0 to 1)
color_qi
tint_to(const color_qi c, const double density)
{
  color_qi klr;
  klr.r = c.r + (255 - c.r) * density;
  klr.g = c.g + (255 - c.g) * density;
  klr.b = c.b + (255 - c.b) * density;
  return klr;
}

/// Add black to shade in density % (0 to 1)
color_qi
shade_to(const color_qi c, const double density)
{
  color_qi klr;
  klr.r = c.r * (1.0 - density);
  klr.g = c.r * (1.0 - density);
  klr.b = c.b * (1.0 - density);
  return klr;
}

/**
  Generate a color band from starting hue and seeds.

  Algorithm is average two known good, where two picked randomly.

  Return type is a vector of generated color_qi types.
*/
color_qis
make_color_band(const colorband& cb, const ushort neededh)
{
  // Find starting hue and number of samples in the color band.
  color c = std::get<0>(cb);
  ushort hn = std::get<1>(cb);

  // Find initial offset.
  auto itr = std::find(spectrum.begin(), spectrum.end(), c);
  if (itr == spectrum.end())
    {
      string m("collection::make_color_band: " + to_string(c));
      throw std::runtime_error(m);
    }

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
      color c1 = *(itr + o1);
      color c2 = *(itr + o2);

      // Combine.
      double c1r = distr(rg);
      double c2r = 2.0 - c1r;
      color_qi cgen = combine_two_color_qi(c1, c1r, c2, c2r);
      cband.push_back(cgen);
    }

  return cband;
}


/// Flip through color band colors.
/// @bandn is the number of colors in the colorband.
color_qi
next_in_color_band(const colorband& cb, const ushort bandn = 400)
{
  // Generate bands.
  static color_qis gband_bw = make_color_band(cband_bw, bandn);
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
