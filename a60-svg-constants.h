// -*- mode: C++ -*-
// Copyright (C) 2014-2019 Benjamin De Kosnik <b.dekosnik@gmail.com>

#ifndef MiL_SVG_CONSTANTS_H
#define MiL_SVG_CONSTANTS_H 1


namespace svg::constants {

/// Useful area constants.
/// h == horizontal aka landscape
/// v == vertical aka portrait
constexpr area<float> letter_mm_h = { 216, 279 };

constexpr area<float> a2_mm_h = { 420, 594 };
constexpr area<float> a4_mm_h = { 210, 297 };
constexpr area<float> a5_mm_h = { 148, 210 };
constexpr area<float> a6_mm_h = { 105, 148 };

/// Pixel areas are using 96 PPI unless otherwise noted.
constexpr area<> letter_096_v = { 816, 1056 };
constexpr area<> letter_096_h = { 1056, 816 };

constexpr area<> a2_096_h = { 2245, 1587 };
constexpr area<> a4_096_h = { 1123, 794 };
constexpr area<> a5_096_h = { 794, 559 };
constexpr area<> a6_096_h = { 559, 397 };

constexpr area<> a2_096_v = { 1587, 2245 };
constexpr area<> a4_096_v = { 794, 1123 };
constexpr area<> a5_096_v = { 559, 794 };
constexpr area<> a6_096_v = { 397, 559 };

constexpr area<> instagram_090_v = { 1080, 1350 };
constexpr area<> instagram_096_h = { 1440, 1152 };
constexpr area<> instagram_096_v = { 1152, 1440 };

constexpr area<> v1080p_096_h = { 2048, 1152 };
constexpr area<> v1080p_096_v = { 1152, 2048 };


/// Style constants.
const colorq whiteq(colore::white);
const colorq blackq(colore::black);
const style w_style = { whiteq, 1.0, blackq, 0.0, 0 };
const style b_style = { blackq, 1.0, whiteq, 0.0, 0 };


/// Useful typography constants.
typography::anchor a = typography::anchor::middle;
typography::align al = typography::align::center;
typography::weight w = typography::weight::normal;
typography::property p = typography::property::normal;

const char* sserif = "Source Serif Pro";
const char* ssans = "Source Sans Pro";
const char* smono = "Source Code Pro";
const char* shsans = "Source Han Sans CN";
const char* aprcu = "Apercu";
const char* ccode = "Calling Code";
const char* zslab = "Zilla Slab";
const char* mexcellent = "Mexcellent";
const typography smono_typo = { smono, 12, b_style, a, al, w, p };
const typography sserif_typo = { sserif, 12, b_style, a, al, w, p };
const typography ssans_typo = { ssans, 12, b_style, a, al, w, p };
const typography shsans_typo = { shsans, 12, b_style, a, al, w, p };
const typography apercu_typo = { aprcu, 4, b_style, a, al, w, p };
const typography ccode_typo = { ccode, 12, b_style, a, al, w, p };
const typography zslab_typo = { zslab, 12, b_style, a, al, w, p };
const typography mxcllnt_typo = { mexcellent, 12, b_style, a, al, w, p };

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
