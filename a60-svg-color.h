// svg color -*- mode: C++ -*-

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

#ifndef MiL_SVG_COLOR_H
#define MiL_SVG_COLOR_H 1

#include <string>
#include <map>
#include <algorithm>
#include <random>


namespace svg {

/**
   HUE

   colore = color enum (and name string conversion)
   colorq = color quantified, similar to Scalar in OpenCV.
   colors = color spectrum as array of color enums
*/

/// Color enumerated as types.
enum class colore
{
 // 13 tints
 white,
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
 black,

 // 12 yellow
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

 // 7 orange
 orange,
 darkorange,
 asamaorange,
 redorange,
 orangered,
 dutchorange,
 internationalorange,

 // 14 red
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

 // 16 green
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

 // 27 blue
 ultramarine,
 shinbashiiro,
 hanada,		// blue silk
 ruriiro,		// lapis
 bellflower,
 navy,
 asagiiro,		// light blue
 indigo,
 rurikon,		// dark blue lapis
 blue,
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

 none
};


/// Convert colore to string.
const std::string
to_string(const colore e)
{
  using enum_map_type = std::map<colore, std::string>;

  static enum_map_type enum_map;
  if (enum_map.empty())
    {
      enum_map[colore::white] = "rgb(255, 255, 255)";
      enum_map[colore::black] = "rgb(0, 0, 0)";
      enum_map[colore::gray90] = "rgb(25, 25, 25)";
      enum_map[colore::gray80] = "rgb(50, 50, 50)";
      enum_map[colore::gray75] = "rgb(64, 64, 64)";
      enum_map[colore::gray70] = "rgb(77, 77, 77)";
      enum_map[colore::gray60] = "rgb(100, 100, 100)";
      enum_map[colore::gray50] = "rgb(128, 128, 128)";
      enum_map[colore::gray40] = "rgb(150, 150, 150)";
      enum_map[colore::gray30] = "rgb(180, 180, 180)";
      enum_map[colore::gray25] = "rgb(191, 191, 191)";
      enum_map[colore::gray20] = "rgb(200, 200, 200)";
      enum_map[colore::gray10] = "rgb(230, 230, 230)";

      enum_map[colore::command] = "rgb(255, 0, 171)";
      enum_map[colore::science] = "rgb(150, 230, 191)";
      enum_map[colore::engineering] = "rgb(161, 158, 178)";

      enum_map[colore::kissmepink] = "rgb(255, 59, 241)";

      enum_map[colore::red] = "rgb(255, 0, 0)";
      enum_map[colore::green] = "rgb(0, 255, 0)";
      enum_map[colore::blue] = "rgb(0, 0, 255)";

      enum_map[colore::asamablue] = "rgb(1, 137, 255)";
      enum_map[colore::asamaorange] = "rgb(236, 75, 37)";
      enum_map[colore::asamapink] = "rgb(200, 56, 81)";

      // Reds
      enum_map[colore::foreigncrimson] = "rgb(201, 31, 55)";
      enum_map[colore::ginshu] = "rgb(188, 45, 41)";
      enum_map[colore::akabeni] = "rgb(195, 39,43)";
      enum_map[colore::akebonoiro] = "rgb(250, 123, 98)";

      enum_map[colore::ochre] = "rgb(255, 78, 32)";
      enum_map[colore::sohi] = "rgb(227, 92, 56)";
      enum_map[colore::benikaba] = "rgb(157, 43, 34)";
      enum_map[colore::benitobi] = "rgb(145, 50, 40)";
      enum_map[colore::ake] = "rgb(207, 58, 36)";

      enum_map[colore::crimson] = "rgb(220, 20, 60)";
      enum_map[colore::tomato] = "rgb(255, 99, 71)";
      enum_map[colore::coral] = "rgb(255, 127, 80)";
      enum_map[colore::salmon] = "rgb(250, 128, 114)";

      // Orange
      enum_map[colore::orange] = "rgb(255, 165, 0)";
      enum_map[colore::orangered] = "rgb(255, 69, 0)";
      enum_map[colore::redorange] = "rgb(220, 48, 35)";
      enum_map[colore::darkorange] = "rgb(255, 140, 17)";
      enum_map[colore::dutchorange] = "rgb(250, 155, 30)";
      enum_map[colore::internationalorange] = "rgb(255, 79, 0)";

      // Yellows
      enum_map[colore::kanzoiro] = "rgb(255, 137, 54)";
      enum_map[colore::kohakuiro] = "rgb(202, 105, 36)";
      enum_map[colore::kinsusutake] = "rgb(125, 78, 45)";
      enum_map[colore::daylily] = "rgb(255, 137, 54)";
      enum_map[colore::goldenyellow] = "rgb(255, 164, 0)";
      enum_map[colore::hellayellow] = "rgb(255, 255, 0)";
      enum_map[colore::antiquewhite] = "rgb(250, 235, 215)";
      enum_map[colore::lemonchiffon] = "rgb(255, 250, 205)";
      enum_map[colore::goldenrod] = "rgb(250, 250, 210)";
      enum_map[colore::navajowhite] = "rgb(255, 222, 173)";

      enum_map[colore::ivory] = "rgb(255, 255, 240)";
      enum_map[colore::gold] = "rgb(255, 215, 0)";

      // Greens
      enum_map[colore::byakuroku] = "rgb(165, 186, 147)";
      enum_map[colore::usumoegi] = "rgb(141, 178, 85)";
      enum_map[colore::moegi] = "rgb(91, 137, 48)";
      enum_map[colore::hiwamoegi] = "rgb(122, 148, 46)";
      enum_map[colore::midori] = "rgb(42, 96, 59)";
      enum_map[colore::rokusho] = "rgb(64, 122, 82)";
      enum_map[colore::aotakeiro] = "rgb(0, 100, 66)";
      enum_map[colore::seiheki] = "rgb(58, 105, 96)";
      enum_map[colore::seijiiro] = "rgb(129, 156, 139)";
      enum_map[colore::yanagizome] = "rgb(140, 158, 94)";

      enum_map[colore::chartreuse] = "rgb(127, 255, 0)";
      enum_map[colore::greenyellow] = "rgb(173, 255, 47)";
      enum_map[colore::limegreen] = "rgb(50, 205, 50)";
      enum_map[colore::springgreen] = "rgb(0, 255, 127)";
      enum_map[colore::aquamarine] = "rgb(127, 255, 212)";

      // Blues
      enum_map[colore::ultramarine] = "rgb(93, 140, 174)";
      enum_map[colore::shinbashiiro] = "rgb(0, 108, 127)";
      enum_map[colore::hanada] = "rgb(4, 79, 103)";
      enum_map[colore::ruriiro] = "rgb(31, 71, 136)";
      enum_map[colore::bellflower] = "rgb(25, 31, 69)";
      enum_map[colore::navy] = "rgb(0, 49, 113)";
      enum_map[colore::asagiiro] = "rgb(72, 146, 155)";
      enum_map[colore::indigo] = "rgb(38, 67, 72)";
      enum_map[colore::rurikon] = "rgb(27, 41, 75)";
      enum_map[colore::cyan] = "rgb(0, 255, 255)";

      enum_map[colore::lightcyan] = "rgb(224, 255, 255)";
      enum_map[colore::powderblue] = "rgb(176, 224, 230)";
      enum_map[colore::steelblue] = "rgb(70, 130, 237)";
      enum_map[colore::cornflowerblue] = "rgb(100, 149, 237)";
      enum_map[colore::deepskyblue] = "rgb(0, 191, 255)";
      enum_map[colore::dodgerblue] = "rgb(30, 144, 255)";
      enum_map[colore::lightblue] = "rgb(173, 216, 230)";
      enum_map[colore::skyblue] = "rgb(135, 206, 235)";
      enum_map[colore::lightskyblue] = "rgb(173, 206, 250)";
      enum_map[colore::midnightblue] = "rgb(25, 25, 112)";

      enum_map[colore::mediumblue] = "rgb(0, 0, 205)";
      enum_map[colore::royalblue] = "rgb(65, 105, 225)";
      enum_map[colore::darkslateblue] = "rgb(72, 61, 139)";
      enum_map[colore::slateblue] = "rgb(106, 90, 205)";
      enum_map[colore::azure] = "rgb(240, 255, 255)";

      // Purples
      enum_map[colore::wisteria] = "rgb(135, 95, 154)";
      enum_map[colore::murasaki] = "rgb(79, 40, 75)";
      enum_map[colore::ayameiro] = "rgb(118, 53, 104)";
      enum_map[colore::peony] = "rgb(164, 52, 93)";
      enum_map[colore::futaai] = "rgb(97, 78, 110)";
      enum_map[colore::benimidori] = "rgb(120, 119, 155)";
      enum_map[colore::redwisteria] = "rgb(187, 119, 150)";
      enum_map[colore::botan] = "rgb(164, 52, 93)";
      enum_map[colore::kokimurasaki] = "rgb(58, 36, 59)";
      enum_map[colore::usuiro] = "rgb(168, 124, 160)";

      enum_map[colore::blueviolet] = "rgb(138, 43, 226)";
      enum_map[colore::darkmagenta] = "rgb(139, 0, 139)";
      enum_map[colore::darkviolet] = "rgb(148, 0, 211)";
      enum_map[colore::thistle] = "rgb(216, 191, 216)";
      enum_map[colore::plum] = "rgb(221, 160, 221)";
      enum_map[colore::violet] = "rgb(238, 130, 238)";
      enum_map[colore::magenta] = "rgb(255, 0, 255)";
      enum_map[colore::deeppink] = "rgb(255, 20, 147)";
      enum_map[colore::hotpink] = "rgb(255, 105, 180)";
      enum_map[colore::pink] = "rgb(255, 192, 203)";

      enum_map[colore::palevioletred] = "rgb(219, 112, 147)";
      enum_map[colore::mediumvioletred] = "rgb(199, 21, 133)";
      enum_map[colore::lavender] = "rgb(230, 230, 250)";
      enum_map[colore::orchid] = "rgb(218, 112, 214)";
      enum_map[colore::mediumorchid] = "rgb(186, 85, 211)";
      enum_map[colore::darkestmagenta] = "rgb(180, 0, 180)";
      enum_map[colore::mediumpurple] = "rgb(147, 112, 219)";
      enum_map[colore::purple] = "rgb(128, 0, 128)";
      enum_map[colore::dustyrose] = "rgb(191, 136, 187)";
      enum_map[colore::atmosphericp] = "rgb(228, 210, 231)";

      enum_map[colore::none] = "none";
    }
  return enum_map[e];
}


/// Color quantified as components in aggregate type with RGB values.
/// aka like Scalar in OpenCV.
struct colorq
{
  using itype = unsigned short;

  itype	r;
  itype	g;
  itype	b;

  // Return "rgb(64, 64, 64)";
  string
  static to_string(colorq s)
  {
    std::ostringstream oss;
    oss << "rgb(" << s.r << ',' << s.g << ',' << s.b << ")";
    return oss.str();
  }

  // From "rgb(64, 64, 64)";
  colorq
  static from_string(string s)
  {
    // Kill rgb() enclosing.
    s.pop_back();
    s = s.substr(4);

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

    return colorq(r, g, b);
  }

  colorq() = default;
  colorq(const colorq&) = default;
  colorq& operator=(const colorq&) = default;

  colorq(itype ra, itype ga, itype ba) : r(ra), g(ga), b(ba) { }

  colorq(const colore e)
  {
    colorq klr = from_string(svg::to_string(e));
    r = klr.r;
    b = klr.b;
    g = klr.g;
  }
};


/**
  Combine color a with color b in percentages ad and ab, respectively.

  To average, constrain paramters ad and ab such that: ad + ab == 2.

  Like so:
  ushort ur = (a.r + b.r) / 2;
  ushort ug = (a.g + b.g) / 2;
  ushort ub = (a.b + b.b) / 2;
*/
colorq
combine_two_colorq(const colorq& a, double ad, const colorq& b, double bd)
{
  using itype = colorq::itype;

  auto denom = ad + bd;
  auto ur = ((a.r * ad) + (b.r * bd)) / denom;
  auto ug = ((a.g * ad) + (b.g * bd)) / denom;
  auto ub = ((a.b * ad) + (b.b * bd)) / denom;
  itype cr = static_cast<itype>(ur);
  itype cg = static_cast<itype>(ug);
  itype cb = static_cast<itype>(ub);
  return colorq { cr, cg, cb };
}

/// Average two colors, return the result.
colorq
average_two_colorq(const colorq& a, const colorq& b)
{ return combine_two_colorq(a, 1.0, b, 1.0); }


/// Color iteration and combinatorics.
using colores = std::vector<colore>;
using colorqs = std::vector<colorq>;

using color_array = std::array<colore, 122>;


/// Color spectrum.
color_array colors =
{
 // black to gray to white in 10% and 25% increments
 colore::white,
 colore::gray10, colore::gray20, colore::gray25, colore::gray30,
 colore::gray40, colore::gray50, colore::gray60, colore::gray70,
 colore::gray75, colore::gray80, colore::gray90,

 // yellow
 colore::hellayellow, colore::goldenyellow,
 colore::navajowhite, colore::ivory, colore::gold,
 colore::antiquewhite, colore::lemonchiffon, colore::goldenrod,
 colore::kanzoiro, colore::kohakuiro, colore::kinsusutake, colore::daylily,

 // red
 colore::red,  colore::coral, colore::salmon, colore::akabeni,
 colore::akebonoiro, colore::ochre, colore::sohi, colore::benikaba,
 colore::benitobi, colore::ake, colore::crimson, colore::tomato,
 colore::foreigncrimson, colore::ginshu,

 // orange
 colore::orange, colore::darkorange, colore::orangered, colore::redorange,
 colore::asamaorange, colore::dutchorange, colore::internationalorange,

 // green
 colore::green, colore::limegreen, colore::springgreen, colore::byakuroku,
 colore::usumoegi, colore::aquamarine, colore::midori, colore::rokusho,
 colore::aotakeiro, colore::seiheki, colore::seijiiro, colore::yanagizome,
 colore::hiwamoegi,colore::chartreuse, colore::greenyellow, colore::moegi,

 // blue
 colore::ultramarine, colore::shinbashiiro, colore::hanada, colore::ruriiro,
 colore::cornflowerblue, colore::lightblue, colore::skyblue,
 colore::lightskyblue, colore::bellflower, colore::navy, colore::asagiiro,
 colore::indigo, colore::rurikon, colore::blue, colore::asamablue, colore::cyan,
 colore::lightcyan, colore::powderblue, colore::steelblue, colore::dodgerblue,
 colore::royalblue, colore::mediumblue, colore::deepskyblue,
 colore::midnightblue, colore::darkslateblue, colore::slateblue, colore::azure,

 // purple (magenta, violet, pink)
 colore::wisteria, colore::asamapink, colore::pink, colore::peony,
 colore::violet, colore::magenta, colore::deeppink, colore::hotpink,
 colore::dustyrose, colore::atmosphericp, colore::kissmepink,
 colore::futaai, colore::benimidori, colore::redwisteria, colore::botan,
 colore::kokimurasaki, colore::usuiro, colore::murasaki, colore::ayameiro,
 colore::blueviolet, colore::darkmagenta, colore::darkviolet,
 colore::thistle, colore::plum,
 colore::palevioletred, colore::mediumvioletred, colore::lavender,
 colore::orchid, colore::mediumorchid, colore::darkestmagenta,
 colore::mediumpurple, colore::purple,

 colore::black
};


/// Random entry from array above.
colore
random_color()
{
  const uint maxc = colors.size();
  static std::mt19937_64 rg(std::random_device{}());
  auto disti = std::uniform_int_distribution<>(0, maxc - 1);
  uint index = disti(rg);
  return colors[index];
}


/// Loop through color array starting at position c.
colore
next_color(colore c)
{
  colore cnext = colors.front();
  auto itr = std::find(colors.begin(), colors.end(), c);
  if (itr != colors.end())
    {
      ++itr;
      if (itr == colors.end())
	itr = colors.begin();
      cnext = *itr;
    }
  return cnext;
}


/// Start at specified color bar entry point.
colorq
color_start_at_specified(colore klr)
{
  static colore _S_klr = klr;
  colorq retk = _S_klr;
  _S_klr = next_color(_S_klr);
  return retk;
}


// Specific regions of spectrum as bands of color.
// Each band has a starting hue and a number of known good samples.
// This is then used to seed a generator that computes more of similar hues.
using colorband = std::tuple<colore, ushort>;
constexpr colorband cband_bw = std::make_tuple(colore::white, 13);
constexpr colorband cband_y = std::make_tuple(colore::hellayellow, 12);
constexpr colorband cband_r = std::make_tuple(colore::red, 14);
constexpr colorband cband_g = std::make_tuple(colore::green, 16);
constexpr colorband cband_b = std::make_tuple(colore::ultramarine, 27);
constexpr colorband cband_p = std::make_tuple(colore::wisteria, 32);
constexpr colorband cband_o = std::make_tuple(colore::orange, 7);


/// Add white to tint in r ammount.
colorq
tint_to(const colorq c, const double density)
{
  colorq klr;
  klr.r = c.r + (255 - c.r) * density;
  klr.g = c.g + (255 - c.g) * density;
  klr.b = c.b + (255 - c.b) * density;
  return klr;
}

/// Add black to shade in r ammount.
colorq
shade_to(const colorq c, const double density)
{
  colorq klr;
  klr.r = c.r * (1.0 - density);
  klr.g = c.r * (1.0 - density);
  klr.b = c.b * (1.0 - density);
  return klr;
}

/**
  Generate a color band from starting hue and seeds.

  Algorithm is average two known good, where two picked randomly.

  Return type is a vector of generated colorq types.
*/
colorqs
make_color_band(const colorband& cb, const ushort neededh)
{
  // Find starting hue and number of samples in the color band.
  colore c = std::get<0>(cb);
  ushort hn = std::get<1>(cb);

  // Find initial offset.
  auto itr = std::find(colors.begin(), colors.end(), c);
  if (itr == colors.end())
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
  colorqs cband;
  for (ushort i = 0; i < neededh; ++i)
    {
      // New color.
      ushort o1 = disti(rg);
      ushort o2 = disti(rg);
      colore c1 = *(itr + o1);
      colore c2 = *(itr + o2);

      // Combine.
      double c1r = distr(rg);
      double c2r = 2.0 - c1r;
      colorq cgen = combine_two_colorq(c1, c1r, c2, c2r);
      cband.push_back(cgen);
    }

  return cband;
}

colorq
next_in_color_band(const colorband& cb)
{
  // Generate bands.
  // For now, just generate 100 each.
  const ushort bandn = 100;
  static colorqs gband_bw = make_color_band(cband_bw, bandn);
  static colorqs gband_y = make_color_band(cband_y, bandn);
  static colorqs gband_r = make_color_band(cband_r, bandn);
  static colorqs gband_g = make_color_band(cband_g, bandn);
  static colorqs gband_b = make_color_band(cband_b, bandn);
  static colorqs gband_p = make_color_band(cband_p, bandn);
  static colorqs gband_o = make_color_band(cband_o, bandn);

  // XXX No out of bounds error checking.
  colorq ret;
  const colore c = std::get<0>(cb);
  switch (c)
    {
    case colore::white:
      ret = gband_bw.back();
      gband_bw.pop_back();
      break;
    case colore::hellayellow:
      ret = gband_y.back();
      gband_y.pop_back();
      break;
    case colore::orange:
      ret = gband_o.back();
      gband_o.pop_back();
      break;
    case colore::red:
      ret = gband_r.back();
      gband_r.pop_back();
      break;
    case colore::green:
      ret = gband_g.back();
      gband_g.pop_back();
      break;
    case colore::ultramarine:
      ret = gband_b.back();
      gband_b.pop_back();
      break;
    case colore::wisteria:
      ret = gband_p.back();
      gband_p.pop_back();
      break;
    default:
      throw std::runtime_error("collection::next_in_color_band");
      break;
    }
  return ret;
}

} // namespace svg

#endif
