// -*- mode: C++ -*-

// Copyright (C) 2014-2022 Benjamin De Kosnik <b.dekosnik@gmail.com>

// This file is part of the alpha60-MiL SVG library.  This library is
// free software; you can redistribute it and/or modify it under the
// terms of the GNU General Public License as published by the Free
// Software Foundation; either version 3, or (at your option) any
// later version.

// This library is distributed in the hope that it will be useful, but
// WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// General Public License for more details.

#ifndef MiL_SVG_CONSTANTS_H
#define MiL_SVG_CONSTANTS_H 1


namespace svg::constants {

/**
   Area constants use [h, v] suffix as per
   - h == horizontal aka landscape
   - v == vertical aka portrait
*/
constexpr area<> no_area = { 0, 0 };

/// Area Constants in millimeter units.
constexpr area<> letter_mm_h = { 216, 279 };

constexpr area<> a2_mm_h = { 420, 594 };
constexpr area<> a4_mm_h = { 210, 297 };
constexpr area<> a5_mm_h = { 148, 210 };
constexpr area<> a6_mm_h = { 105, 148 };

/// Area Constants in pixel units.
/// Pixel constants are using 96 PPI unless otherwise noted.
constexpr area<> a2_096_h = { 2245, 1587 };
constexpr area<> a4_096_h = { 1123, 794 };
constexpr area<> a5_096_h = { 794, 559 };
constexpr area<> a6_096_h = { 559, 397 };

constexpr area<> a2_096_v = { 1587, 2245 };
constexpr area<> a4_096_v = { 794, 1123 };
constexpr area<> a5_096_v = { 559, 794 };
constexpr area<> a6_096_v = { 397, 559 };

constexpr area<> letter_096_v = { 816, 1056 };
constexpr area<> letter_096_h = { 1056, 816 };

/// Letter bleed (8.625 w x 11.25 h)
constexpr area<> letterb_096_v = { 816, 1056 };
constexpr area<> letterb_096_h = { 1056, 816 };

/// Square 8 (8 w x 8 h)
constexpr area<> square8_096 = { 768, 768 };

/// Square 8bis (8.5 w x 8.5 h)
constexpr area<> square8bis_096 = { 816, 816 };

/// Square 8bis bleed (8.625 w x 8.75 h) (219 x 222 mm)
constexpr area<> square8bisb_096_v = { 828, 840 };
constexpr area<> square8bisb_096_h = { 840, 826 };

/// Square 17 bleed 2 x (8.625 w x 8.75 h) for above at 2x
constexpr area<> square17b_096_v = { 1656, 1680 };
constexpr area<> square17b_096_h = { 1680, 1656 };

/// Engineering C, aka 22 x 17 letter quattro
constexpr area<> p22x17_096_h = { 2112, 1632 };
constexpr area<> p17x22_096_v = { 1632, 2112 };

constexpr area<> instagram_v = { 1080, 1350 };
constexpr area<> instagram_s = { 1080, 1080 };
constexpr area<> instagram_4ks = { 2160, 2160 };

constexpr area<> v1080p_h = { 1920, 1080 };
constexpr area<> v1080p_v = { 1080, 1920 };

constexpr area<> v4k_h = { 3840, 2160 };
constexpr area<> v4k_v = { 2160, 3840 };


/// Style constants.
const style no_style = { color::none, 0.0, color::none, 0.0, 0 };
const style b_style = { color::black, 1.0, color::white, 0.0, 0.5 };
const style w_style = { color::white, 1.0, color::black, 0.0, 0.5 };

const style r_style = { color::red, 1.0, color::red, 0.0, 0.5 };
const style wcaglg_style = { color::wcag_lgray, 1.0, color::wcag_lgray, 0.0, 0.5 };
const style wcagg_style = { color::wcag_gray, 1.0, color::wcag_gray, 0.0, 0.5 };
const style wcagdg_style = { color::wcag_dgray, 1.0, color::wcag_dgray, 0.0, 0.5 };


/// Useful typography constants.
typography::anchor a = typography::anchor::middle;
typography::align al = typography::align::center;
typography::weight w = typography::weight::normal;
typography::property p = typography::property::normal;
typography::baseline b = typography::baseline::none;

const char* sserif = "Source Serif Pro";
const char* ssans = "Source Sans Pro";
const char* smono = "Source Code Pro";
const char* shsans = "Source Han Sans CN";
const char* aprcu = "Apercu";
const char* aprcumo = "Apercu Mono";
const char* ccode = "Calling Code";
const char* zslab = "Zilla Slab";
const char* mexcellent = "Mexcellent";
const char* hyperl = "Atkinson Hyperlegible";

const typography smono_typo = { smono, 12, b_style, a, al, b, w, p };
const typography sserif_typo = { sserif, 12, b_style, a, al, b, w, p };
const typography ssans_typo = { ssans, 12, b_style, a, al, b, w, p };
const typography shsans_typo = { shsans, 12, b_style, a, al, b, w, p };
const typography apercu_typo = { aprcu, 12, b_style, a, al, b, w, p };
const typography apercumo_typo = { aprcumo, 12, b_style, a, al, b, w, p };
const typography ccode_typo = { ccode, 12, b_style, a, al, b, w, p };
const typography zslab_typo = { zslab, 12, b_style, a, al, b, w, p };
const typography mxcllnt_typo = { mexcellent, 12, b_style, a, al, b, w, p };
const typography hyperl_typo = { hyperl, 12, b_style, a, al, b, w, p };


/**
   Default typography.

   NB: Using a global locale. So, filenames are in multiple
   languages, not just en_US.

   Make sure that the font with the largest support for random
   characters is used, ie some variant of Source Sans, Source Han Sans
   XX, etc. Otherwise, blank spaces in rendered output are likely.
*/
const typography d_typo = shsans_typo;

} // namespace svg::constants

#endif
