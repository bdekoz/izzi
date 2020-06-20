// -*- mode: C++ -*-
// Copyright (C) 2014-2019 Benjamin De Kosnik <b.dekosnik@gmail.com>

#ifndef MiL_SVG_CONSTANTS_H
#define MiL_SVG_CONSTANTS_H 1


namespace svg::constants {

  /// Useful area constants.
  constexpr area<float> mm_letter = { 215.9, 279.4 };
  constexpr area<float> mm_a4 = { 210, 297 };

  constexpr area<> px_letter_area = { 765, 990 };
  constexpr area<> px_a4_area = { 765, 990 };
  constexpr area<> px_720p_area = { 1280, 720 };
  constexpr area<> px_instagram_area = { 1080, 1350 };
  constexpr area<> px_1080p_area = { 1920, 1080 };

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
     characters is used, ie some variant of Source Sans, Source Han
     Sans XX, etc. Otherwise, blank spaces in rendered output are
     likely.
  */
  const typography d_typo = shsans_typo;

} // namespace svg::constants

#endif
