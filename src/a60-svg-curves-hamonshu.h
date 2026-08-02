// Izzi Hamonshu wave-pattern curves -*- mode: C++ -*-

// Copyright (c) 2026, Benjamin De Kosnik <b.dekosnik@gmail.com>
//
// The source studies are Mori Yuzan's 1903 Hamonshu, volume 2. The
// Smithsonian Libraries digitization is CC0. The English motif names below
// are descriptive catalogue labels because the source has no captions.

// This file is part of the alpha60 library.  This library is free
// software; you can redistribute it and/or modify it under the terms
// of the GNU General Public License as published by the Free Software
// Foundation; either version 3, or (at your option) any later
// version.

// This library is distributed in the hope that it will be useful, but
// WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// General Public License for more details.

#ifndef a60_SVG_CURVES_HAMONSHU_H
#define a60_SVG_CURVES_HAMONSHU_H 1

#include <algorithm>
#include <array>
#include <cctype>
#include <cmath>
#include <cstddef>
#include <limits>
#include <stdexcept>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

#include "a60-svg.h"

namespace svg::hamonshu {

/**
 * Deterministic procedural interpretations of the wave studies in Mori
 * Yuzan's 1903 Hamonshu, volume 2.
 *
 * `pattern_specs` is the source-indexed catalogue. Pass any entry and a
 * finite, positive `pattern_box` to `make_motif_path()` to obtain SVG path
 * data suitable for `svg::make_path()`. Motifs use normalized coordinates,
 * then scale independently in x and y to the requested box. A seeded rotation
 * of at most four degrees may carry linework just outside that box; callers
 * that require a hard boundary should apply an SVG clip path.
 *
 * The routines construct only motif linework. Projection, geographic data,
 * clipping, fill color, stroke style, and document/layer assembly remain the
 * caller's responsibility.
 */

inline constexpr double pi
  = 3.141592653589793238462643383279502884;

struct pattern_spec
{
  unsigned first_page;
  unsigned last_page;
  unsigned motif;
  std::string_view name;
};

// The digitized volume has 28 PDF scans but 51 numbered book pages.  The
// first illustrated page is alone on scan 2; scans 3-27 hold two book pages.
// Page 50 is the colophon.  The catalogue uses book-page numbers and records
// each bounded specimen separately; the generated SVG title also records the
// corresponding PDF scan number.
inline constexpr std::array pattern_specs {
#include "a60-svg-curves-hamonshu-v2.inc"
};

static_assert(pattern_specs.size() == 153);

struct pattern_box
{
  double left = std::numeric_limits<double>::infinity();
  double top = std::numeric_limits<double>::infinity();
  double right = -std::numeric_limits<double>::infinity();
  double bottom = -std::numeric_limits<double>::infinity();

  void
  include(const svg::point_2t point)
  {
    const auto [x, y] = point;
    left = std::min(left, x);
    top = std::min(top, y);
    right = std::max(right, x);
    bottom = std::max(bottom, y);
  }

  bool
  valid() const
  {
    return std::isfinite(left) && std::isfinite(top)
           && std::isfinite(right) && std::isfinite(bottom)
           && right > left && bottom > top;
  }

  double width() const { return right - left; }
  double height() const { return bottom - top; }
};

enum class motif_kind
{
  waterline,
  crest,
  spiral,
  spray,
  arc,
  lattice,
  bubble,
  scroll,
  fan,
  breaker,
  braid,
  cascade,
  ripple,
  fountain,
  cloud,
  cell,
};

inline void
require(const bool condition, const std::string& message)
{
  if (!condition)
    throw std::runtime_error(message);
}

inline std::string
zero_padded(const unsigned value, const unsigned width)
{
  std::string result = std::to_string(value);
  if (result.size() < width)
    result.insert(result.begin(), width - result.size(), '0');
  return result;
}

inline std::string
pattern_id(const pattern_spec& spec)
{
  std::string id = "hamonshu-page-" + zero_padded(spec.first_page, 3);
  if (spec.last_page != spec.first_page)
    id += "-" + zero_padded(spec.last_page, 3);
  id += "-motif-" + zero_padded(spec.motif, 2);
  id += "-" + std::string(spec.name);
  return id;
}

inline std::string
display_name(const std::string_view slug)
{
  std::string result(slug);
  std::replace(result.begin(), result.end(), '-', ' ');
  if (!result.empty())
    result.front() = static_cast<char>(std::toupper(
      static_cast<unsigned char>(result.front())));
  return result;
}

inline unsigned
pdf_scan_page(const unsigned illustrated_page)
{
  if (illustrated_page == 1)
    return 2;
  return (illustrated_page + 4) / 2;
}

inline std::string
pattern_title(const pattern_spec& spec)
{
  std::string title = "Hamonshu volume 2, illustrated page ";
  title += zero_padded(spec.first_page, 3);
  if (spec.last_page != spec.first_page)
    title += "-" + zero_padded(spec.last_page, 3);
  title += " (PDF scan " + std::to_string(pdf_scan_page(spec.first_page));
  if (pdf_scan_page(spec.last_page) != pdf_scan_page(spec.first_page))
    title += "-" + std::to_string(pdf_scan_page(spec.last_page));
  title += "), motif " + zero_padded(spec.motif, 2) + ": ";
  title += display_name(spec.name);
  title += " [descriptive English title; the source has no pattern caption]";
  return title;
}

inline void
validate_pattern_spec(const pattern_spec& spec)
{
  require(spec.first_page >= 1 && spec.first_page <= spec.last_page
          && spec.last_page <= 51,
          "Hamonshu catalogue contains an invalid illustrated-page range");
  require(!(spec.first_page <= 50 && spec.last_page >= 50),
          "Hamonshu colophon page 50 cannot contain a wave motif");
  require(spec.motif != 0 && !spec.name.empty(),
          "Hamonshu catalogue contains an incomplete motif entry");
  require(std::all_of(spec.name.begin(), spec.name.end(), [](const char value) {
            const unsigned char character
              = static_cast<unsigned char>(value);
            return (character >= 'a' && character <= 'z')
                   || (character >= '0' && character <= '9')
                   || character == '-';
          }),
          "Hamonshu catalogue motif names must be lowercase ASCII slugs");
}

inline bool
contains(const std::string_view text, const std::string_view token)
{ return text.find(token) != std::string_view::npos; }

inline motif_kind
classify_pattern(const pattern_spec& spec)
{
  const std::string_view name = spec.name;
  if (contains(name, "bubble") || contains(name, "droplet"))
    return motif_kind::bubble;
  if (contains(name, "cell"))
    return motif_kind::cell;
  if (contains(name, "fountain") || contains(name, "reed")
      || contains(name, "spear"))
    return motif_kind::fountain;
  if (contains(name, "chevron") || contains(name, "herringbone")
      || contains(name, "diamond") || contains(name, "lattice")
      || contains(name, "crosshatch"))
    return motif_kind::lattice;
  if (contains(name, "spiral") || contains(name, "eddy")
      || contains(name, "whirlpool"))
    return motif_kind::spiral;
  if (contains(name, "ripple") || contains(name, "ring")
      || contains(name, "pool"))
    return motif_kind::ripple;
  if (contains(name, "scallop") || contains(name, "scale")
      || contains(name, "arc"))
    return motif_kind::arc;
  if (contains(name, "spray") || contains(name, "foam"))
    return motif_kind::spray;
  if (contains(name, "breaking") || contains(name, "breaker"))
    return motif_kind::breaker;
  if (contains(name, "cloud"))
    return motif_kind::cloud;
  if (contains(name, "braid") || contains(name, "interwoven")
      || contains(name, "linked") || contains(name, "knot"))
    return motif_kind::braid;
  if (contains(name, "cascade") || contains(name, "folded"))
    return motif_kind::cascade;
  if (contains(name, "fan"))
    return motif_kind::fan;
  if (contains(name, "scroll") || contains(name, "curl")
      || contains(name, "hook"))
    return motif_kind::scroll;
  if (contains(name, "crest") || contains(name, "ridge")
      || contains(name, "swell") || contains(name, "wave")
      || contains(name, "sea"))
    return motif_kind::crest;
  return motif_kind::waterline;
}

inline unsigned
pattern_seed(const pattern_spec& spec)
{
  unsigned seed = spec.first_page * 131 + spec.last_page * 17
                  + spec.motif * 43;
  for (const unsigned char character : spec.name)
    seed = seed * 33U ^ character;
  return seed;
}

struct pattern_context
{
  pattern_box box;
  unsigned seed;

  svg::point_2t
  point(const double u, const double v) const
  {
    const double angle
      = (static_cast<int>(seed % 9) - 4) * (pi / 180);
    const double du = u - 0.5;
    const double dv = v - 0.5;
    const double rotated_u = 0.5 + std::cos(angle) * du
                             - std::sin(angle) * dv;
    const double rotated_v = 0.5 + std::sin(angle) * du
                             + std::cos(angle) * dv;
    return {
      box.left + rotated_u * box.width(),
      box.top + rotated_v * box.height(),
    };
  }
};

inline void
append_polyline(std::string& path_data, const svg::vrange& points,
                const bool close = false)
{
  if (points.size() < (close ? 3U : 2U))
    return;
  path_data += svg::make_path_data_from_points(points);
  if (close)
    path_data += "Z ";
}

template<typename Function>
inline void
append_curve(std::string& path_data, const pattern_context& context,
             const int samples, Function function)
{
  svg::vrange points;
  points.reserve(static_cast<std::size_t>(samples + 1));
  for (int index = 0; index <= samples; ++index)
    {
      const double t = static_cast<double>(index) / samples;
      const auto [u, v] = function(t);
      points.push_back(context.point(u, v));
    }
  append_polyline(path_data, points);
}

inline void
append_ellipse(std::string& path_data, const pattern_context& context,
               const double center_u, const double center_v,
               const double radius_u, const double radius_v,
               const int samples = 28)
{
  svg::vrange points;
  points.reserve(static_cast<std::size_t>(samples));
  for (int index = 0; index != samples; ++index)
    {
      const double angle = 2 * pi * index / samples;
      points.push_back(context.point(
        center_u + radius_u * std::cos(angle),
        center_v + radius_v * std::sin(angle)));
    }
  append_polyline(path_data, points, true);
}

inline void
append_spiral(std::string& path_data, const pattern_context& context,
              const double center_u, const double center_v,
              const double radius, const double phase,
              const double turns = 1.8)
{
  append_curve(
    path_data, context, 42,
    [=](const double t) {
      const double angle = phase + turns * 2 * pi * t;
      const double r = radius * (1 - 0.82 * t);
      return std::pair {
        center_u + r * std::cos(angle),
        center_v + r * std::sin(angle),
      };
    });
}

inline void
make_waterlines(std::string& data, const pattern_context& context)
{
  const int rows = 5 + static_cast<int>(context.seed % 4);
  const double phase = (context.seed % 29) * 0.19;
  for (int row = 0; row != rows; ++row)
    append_curve(
      data, context, 36,
      [=](const double t) {
        const double center = (row + 1.0) / (rows + 1.0);
        const double wave = 0.035 * std::sin((2.0 + row % 3) * 2 * pi * t
                                             + phase + row * 0.7);
        const double ripple = 0.012 * std::sin(11 * pi * t + phase);
        return std::pair {t, center + wave + ripple};
      });
}

inline void
make_crests(std::string& data, const pattern_context& context)
{
  const int rows = 3 + static_cast<int>(context.seed % 2);
  const int repeats = 3 + static_cast<int>((context.seed / 3) % 3);
  for (int row = 0; row != rows; ++row)
    append_curve(
      data, context, 48,
      [=](const double t) {
        const double local = std::fmod(t * repeats + 0.5 * (row % 2), 1.0);
        const double arch = std::sin(pi * local);
        const double y = 0.2 + row * 0.22 - 0.13 * arch * arch;
        return std::pair {t, y};
      });
}

inline void
make_spirals(std::string& data, const pattern_context& context)
{
  const double phase = (context.seed % 31) * 0.2;
  const int count = 4 + static_cast<int>(context.seed % 3);
  for (int index = 0; index != count; ++index)
    {
      const double u = 0.18 + (index % 3) * 0.32;
      const double v = 0.28 + (index / 3) * 0.42;
      append_spiral(data, context, u, v, 0.16, phase + index * 0.8,
                    1.4 + 0.2 * (index % 3));
    }
}

inline void
make_spray(std::string& data, const pattern_context& context)
{
  const double phase = (context.seed % 17) * 0.11;
  const int branches = 5 + static_cast<int>(context.seed % 3);
  for (int branch = 0; branch != branches; ++branch)
    {
      const double base = 0.1 + 0.8 * branch / (branches - 1.0);
      append_curve(
        data, context, 24,
        [=](const double t) {
          const double u = base + 0.10 * std::sin(pi * t + phase + branch);
          const double v = 0.88 - 0.70 * t
                           + 0.08 * std::sin(3 * pi * t + branch);
          return std::pair {u, v};
        });
      append_ellipse(data, context,
                     base + 0.08 * std::sin(phase + branch),
                     0.12 + 0.03 * (branch % 2), 0.018, 0.025, 16);
    }
}

inline void
make_arcs(std::string& data, const pattern_context& context)
{
  const int rows = 4 + static_cast<int>(context.seed % 2);
  const int columns = 5;
  for (int row = 0; row != rows; ++row)
    for (int column = -1; column <= columns; ++column)
      append_curve(
        data, context, 16,
        [=](const double t) {
          const double center = (column + 0.5 * (row % 2)) / columns;
          const double angle = pi + pi * t;
          return std::pair {
            center + 0.13 * std::cos(angle),
            0.12 + row * 0.21 + 0.10 * std::sin(angle),
          };
        });
}

inline void
make_lattice(std::string& data, const pattern_context& context)
{
  const int lines = 7 + static_cast<int>(context.seed % 3);
  for (int line = -2; line < lines; ++line)
    for (const double direction : {-1.0, 1.0})
      append_curve(
        data, context, 22,
        [=](const double t) {
          const double offset = line / static_cast<double>(lines);
          const double u = t;
          const double v = 0.5 + direction * (t - 0.5) + offset - 0.35;
          return std::pair {u, v};
        });
}

inline void
make_bubbles(std::string& data, const pattern_context& context)
{
  const int count = 8 + static_cast<int>(context.seed % 5);
  for (int index = 0; index != count; ++index)
    {
      const unsigned mixed = context.seed + static_cast<unsigned>(index * 97);
      const double u = 0.12 + (mixed % 73) / 73.0 * 0.76;
      const double v = 0.12 + ((mixed / 73) % 67) / 67.0 * 0.76;
      const double radius = 0.035 + ((mixed / 491) % 5) * 0.012;
      append_ellipse(data, context, u, v, radius, radius * 0.8, 20);
    }
}

inline void
make_scrolls(std::string& data, const pattern_context& context)
{
  const int rows = 3 + static_cast<int>(context.seed % 2);
  const double phase = (context.seed % 23) * 0.13;
  for (int row = 0; row != rows; ++row)
    {
      append_curve(
        data, context, 38,
        [=](const double t) {
          const double u = t;
          const double v = 0.2 + row * 0.25
                           + 0.09 * std::sin(2 * pi * (2 * t) + phase);
          return std::pair {u, v};
        });
      append_spiral(data, context, 0.2 + row * 0.25,
                    0.2 + row * 0.22, 0.10, phase + row, 1.25);
    }
}

inline void
make_fans(std::string& data, const pattern_context& context)
{
  const bool reverse = context.seed % 2 != 0;
  for (int line = 0; line != 8; ++line)
    append_curve(
      data, context, 30,
      [=](const double t) {
        double u = 0.05 + 0.9 * t;
        if (reverse)
          u = 1 - u;
        const double height = 0.10 + line * 0.045;
        const double v = 0.88 - height * std::sin(pi * t)
                         - 0.28 * t;
        return std::pair {u, v};
      });
}

inline void
make_breakers(std::string& data, const pattern_context& context)
{
  const bool reverse = context.seed % 2 != 0;
  const double phase = (context.seed % 19) * 0.17;
  for (int line = 0; line != 6; ++line)
    append_curve(
      data, context, 38,
      [=](const double t) {
        double u = 0.04 + 0.78 * t;
        if (reverse)
          u = 1 - u;
        const double v = 0.84 - 0.55 * std::sin(0.72 * pi * t)
                         + line * 0.025 + 0.02 * std::sin(phase + 5 * t);
        return std::pair {u, v};
      });
  append_spiral(data, context, reverse ? 0.22 : 0.78, 0.30,
                0.17, reverse ? pi : 0, 1.4);
  for (int drop = 0; drop != 4; ++drop)
    append_ellipse(data, context, 0.42 + drop * 0.10,
                   0.16 + 0.04 * (drop % 2), 0.012, 0.018, 14);
}

inline void
make_braids(std::string& data, const pattern_context& context)
{
  const int strands = 4 + static_cast<int>(context.seed % 3);
  const double phase = (context.seed % 13) * 0.23;
  for (int strand = 0; strand != strands; ++strand)
    append_curve(
      data, context, 44,
      [=](const double t) {
        const double v = 0.5 + 0.22 * std::sin(
          2 * pi * (1.5 + strand % 2) * t + phase
          + strand * 2 * pi / strands);
        return std::pair {t, v};
      });
}

inline void
make_cascade(std::string& data, const pattern_context& context)
{
  for (int line = 0; line != 9; ++line)
    append_curve(
      data, context, 32,
      [=](const double t) {
        const double u = 0.12 + line * 0.09
                         + 0.07 * std::sin(pi * t + line * 0.4);
        const double v = 0.05 + 0.9 * t;
        return std::pair {u, v};
      });
}

inline void
make_ripples(std::string& data, const pattern_context& context)
{
  const int centers = 2 + static_cast<int>(context.seed % 2);
  for (int center = 0; center != centers; ++center)
    for (int ring = 1; ring <= 4; ++ring)
      append_ellipse(data, context,
                     0.28 + center * 0.43,
                     0.35 + 0.25 * (center % 2),
                     0.035 * ring, 0.022 * ring, 28);
}

inline void
make_fountains(std::string& data, const pattern_context& context)
{
  for (int line = 0; line != 7; ++line)
    {
      const double spread = 0.12 + line * 0.055;
      for (const double direction : {-1.0, 1.0})
        append_curve(
          data, context, 28,
          [=](const double t) {
            const double u = 0.5 + direction * spread * std::sin(pi * t / 2);
            const double v = 0.92 - 0.78 * t + 0.18 * t * t;
            return std::pair {u, v};
          });
    }
  append_spiral(data, context, 0.24, 0.72, 0.10, pi, 1.1);
  append_spiral(data, context, 0.76, 0.72, 0.10, 0, 1.1);
}

inline void
make_clouds(std::string& data, const pattern_context& context)
{
  const double phase = (context.seed % 31) * 0.1;
  for (int index = 0; index != 7; ++index)
    {
      const double u = 0.12 + (index % 4) * 0.25;
      const double v = 0.30 + (index / 4) * 0.38;
      append_spiral(data, context, u, v, 0.13, phase + index * 0.7, 1.35);
    }
  make_waterlines(data, context);
}

inline void
make_cells(std::string& data, const pattern_context& context)
{
  for (int row = 0; row != 4; ++row)
    for (int column = 0; column != 5; ++column)
      {
        const double u = 0.10 + column * 0.20 + 0.10 * (row % 2);
        const double v = 0.14 + row * 0.24;
        const double r = 0.07 + 0.01 * ((context.seed + row + column) % 3);
        append_ellipse(data, context, u, v, r, r * 0.75, 22);
      }
}

inline std::string
make_motif_path(const pattern_spec& spec, const pattern_box& box)
{
  validate_pattern_spec(spec);
  require(box.valid(), "Hamonshu motif box must be finite and positive");
  pattern_context context {box, pattern_seed(spec)};
  std::string data;
  switch (classify_pattern(spec))
    {
    case motif_kind::waterline: make_waterlines(data, context); break;
    case motif_kind::crest: make_crests(data, context); break;
    case motif_kind::spiral: make_spirals(data, context); break;
    case motif_kind::spray: make_spray(data, context); break;
    case motif_kind::arc: make_arcs(data, context); break;
    case motif_kind::lattice: make_lattice(data, context); break;
    case motif_kind::bubble: make_bubbles(data, context); break;
    case motif_kind::scroll: make_scrolls(data, context); break;
    case motif_kind::fan: make_fans(data, context); break;
    case motif_kind::breaker: make_breakers(data, context); break;
    case motif_kind::braid: make_braids(data, context); break;
    case motif_kind::cascade: make_cascade(data, context); break;
    case motif_kind::ripple: make_ripples(data, context); break;
    case motif_kind::fountain: make_fountains(data, context); break;
    case motif_kind::cloud: make_clouds(data, context); break;
    case motif_kind::cell: make_cells(data, context); break;
    }
  require(!data.empty(), "Hamonshu motif generated no SVG path data");
  return data;
}


} // namespace svg::hamonshu

#endif
