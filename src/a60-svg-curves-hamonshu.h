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
 * `pattern_specs` is the source-indexed catalogue. Pass any entry, a finite
 * positive `pattern_box`, and an optional `motif_config` to
 * `make_motif_path()` to obtain SVG path data suitable for
 * `svg::make_path()`. The configuration changes line density, curvature,
 * phase, orientation, reflection, and sampling without changing styling.
 * Motifs use normalized coordinates, then scale independently in x and y to
 * the requested box. Rotation may carry linework outside that box; callers
 * that require a hard boundary should apply an SVG clip path.
 *
 * The routines construct only motif linework. Projection, geographic data,
 * clipping, fill color, stroke style, and document/layer assembly remain the
 * caller's responsibility. `curated_motif_selections` and
 * `curated_curvature_ratios` define the shared 13-by-7 form set rendered by
 * the parameter-grid example and cartofreako's ocean maps.
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

/// Source coordinates for one motif in the curated form-exploration set.
struct curated_motif_selection
{
  unsigned first_page;
  unsigned last_page;
  unsigned motif;
};

// These thirteen motifs are the deliberately selected rows in
// examples/curves-hamonshu.cc. Source coordinates, rather than catalogue
// offsets, keep the selection stable when unrelated entries are inserted.
inline constexpr std::array curated_motif_selections {
  curated_motif_selection {1, 1, 1},
  curated_motif_selection {2, 2, 1},
  curated_motif_selection {3, 3, 1},
  curated_motif_selection {3, 3, 2},
  curated_motif_selection {6, 6, 2},
  curated_motif_selection {9, 9, 1},
  curated_motif_selection {17, 17, 3},
  curated_motif_selection {20, 20, 4},
  curated_motif_selection {23, 23, 2},
  curated_motif_selection {39, 39, 2},
  curated_motif_selection {40, 40, 1},
  curated_motif_selection {46, 47, 2},
  curated_motif_selection {51, 51, 5},
};

// Each value changes only wave height, curl radius, and transverse
// displacement. The value 1.0 is the catalogue motif's canonical form.
inline constexpr std::array curated_curvature_ratios {
  0.25, 0.45, 0.70, 1.0, 1.30, 1.65, 2.10,
};

inline constexpr std::size_t curated_variation_count
  = curated_motif_selections.size() * curated_curvature_ratios.size();

static_assert(curated_variation_count == 91);

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

/// Form and sampling parameters shared by every Hamonshu motif family.
struct motif_config
{
  /// Multiplier for the number of repeated lines or constituent shapes.
  double density = 1.0;
  /// Multiplier for wave height, curl radius, and transverse displacement.
  double curvature = 1.0;
  /// Angular offset added to the catalogue motif's canonical phase, radians.
  double phase = 0.0;
  /// Rotation added to the catalogue motif's canonical orientation, radians.
  double rotation = 0.0;
  /// Reflect normalized motif coordinates around the box's vertical axis.
  bool reflected = false;
  /// Relative sampling resolution; 48 preserves the canonical resolution.
  std::size_t samples_per_curve = 48;
};

inline void
require(const bool condition, const std::string& message)
{
  if (!condition)
    throw std::invalid_argument(message);
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

/// Resolve one stable source-coordinate selection into the full catalogue.
inline const pattern_spec&
curated_pattern(const curated_motif_selection selection)
{
  const auto match = std::find_if(
    pattern_specs.begin(), pattern_specs.end(),
    [=](const pattern_spec& spec) {
      return spec.first_page == selection.first_page
        && spec.last_page == selection.last_page
        && spec.motif == selection.motif;
    });
  require(match != pattern_specs.end(),
          "curated Hamonshu motif is not in the catalogue");
  validate_pattern_spec(*match);
  return *match;
}

inline bool
has_token_prefix(const std::string_view text, const std::string_view prefix)
{
  std::size_t begin = 0;
  while (begin < text.size())
    {
      const std::size_t end = text.find('-', begin);
      const std::string_view token = text.substr(
        begin, end == std::string_view::npos ? end : end - begin);
      if (token.size() >= prefix.size()
          && token.substr(0, prefix.size()) == prefix)
        return true;
      if (end == std::string_view::npos)
        break;
      begin = end + 1;
    }
  return false;
}

inline motif_kind
classify_pattern(const pattern_spec& spec)
{
  const std::string_view name = spec.name;
  if (has_token_prefix(name, "bubble")
      || has_token_prefix(name, "droplet"))
    return motif_kind::bubble;
  if (has_token_prefix(name, "cell"))
    return motif_kind::cell;
  if (has_token_prefix(name, "fountain")
      || has_token_prefix(name, "reed")
      || has_token_prefix(name, "spear"))
    return motif_kind::fountain;
  if (has_token_prefix(name, "chevron")
      || has_token_prefix(name, "herringbone")
      || has_token_prefix(name, "diamond")
      || has_token_prefix(name, "lattice")
      || has_token_prefix(name, "crosshatch"))
    return motif_kind::lattice;
  if (has_token_prefix(name, "spiral")
      || has_token_prefix(name, "eddy")
      || has_token_prefix(name, "whirlpool"))
    return motif_kind::spiral;
  if (has_token_prefix(name, "ripple")
      || has_token_prefix(name, "ring")
      || has_token_prefix(name, "pool"))
    return motif_kind::ripple;
  if (has_token_prefix(name, "scallop")
      || has_token_prefix(name, "scale")
      || has_token_prefix(name, "arc"))
    return motif_kind::arc;
  if (has_token_prefix(name, "spray")
      || has_token_prefix(name, "foam"))
    return motif_kind::spray;
  if (has_token_prefix(name, "break"))
    return motif_kind::breaker;
  if (has_token_prefix(name, "cloud"))
    return motif_kind::cloud;
  if (has_token_prefix(name, "braid")
      || has_token_prefix(name, "interwoven")
      || has_token_prefix(name, "link")
      || has_token_prefix(name, "knot"))
    return motif_kind::braid;
  if (has_token_prefix(name, "cascade")
      || has_token_prefix(name, "fold"))
    return motif_kind::cascade;
  if (has_token_prefix(name, "fan"))
    return motif_kind::fan;
  if (has_token_prefix(name, "scroll")
      || has_token_prefix(name, "curl")
      || has_token_prefix(name, "hook"))
    return motif_kind::scroll;
  if (has_token_prefix(name, "crest")
      || has_token_prefix(name, "ridge")
      || has_token_prefix(name, "swell")
      || has_token_prefix(name, "wave")
      || has_token_prefix(name, "sea"))
    return motif_kind::crest;
  return motif_kind::waterline;
}

namespace detail {

inline unsigned
catalogue_key(const pattern_spec& spec)
{
  unsigned key = spec.first_page * 131 + spec.last_page * 17
                 + spec.motif * 43;
  for (const unsigned char character : spec.name)
    key = key * 33U ^ character;
  return key;
}

inline constexpr std::size_t minimum_samples_per_curve = 8;
inline constexpr std::size_t maximum_samples_per_curve = 4096;
inline constexpr int maximum_repetition_count = 128;

inline void
validate_config(const motif_config& config)
{
  require(std::isfinite(config.density) && config.density >= 0.25
            && config.density <= 4.0,
          "Hamonshu density must be finite and between 0.25 and 4");
  require(std::isfinite(config.curvature) && config.curvature >= 0.20
            && config.curvature <= 4.0,
          "Hamonshu curvature must be finite and between 0.20 and 4");
  require(std::isfinite(config.phase),
          "Hamonshu phase must be finite");
  require(std::isfinite(config.rotation),
          "Hamonshu rotation must be finite");
  require(config.samples_per_curve >= minimum_samples_per_curve
            && config.samples_per_curve <= maximum_samples_per_curve,
          "Hamonshu samples_per_curve must be between 8 and 4096");
}

struct pattern_context
{
  pattern_box box;
  unsigned key;
  motif_config config;

  int
  repetition_count(const int canonical, const int minimum = 1) const
  {
    const double requested = canonical * config.density;
    require(std::isfinite(requested)
              && requested <= maximum_repetition_count,
            "Hamonshu repetition count exceeds the supported limit");
    return std::max(minimum, static_cast<int>(std::lround(requested)));
  }

  int
  sample_count(const int canonical) const
  {
    const double requested
      = canonical * static_cast<double>(config.samples_per_curve) / 48.0;
    require(std::isfinite(requested)
              && requested <= maximum_samples_per_curve,
            "Hamonshu sample count exceeds the supported limit");
    return std::max(
      static_cast<int>(minimum_samples_per_curve),
      static_cast<int>(std::lround(requested)));
  }

  svg::point_2t
  point(double u, const double v) const
  {
    if (config.reflected)
      u = 1 - u;
    const double angle
      = (static_cast<int>(key % 9) - 4) * (pi / 180)
        + config.rotation;
    const double du = u - 0.5;
    const double dv = v - 0.5;
    const double rotated_u = 0.5 + std::cos(angle) * du
                             - std::sin(angle) * dv;
    const double rotated_v = 0.5 + std::sin(angle) * du
                             + std::cos(angle) * dv;
    const svg::point_2t result {
      box.left + rotated_u * box.width(),
      box.top + rotated_v * box.height(),
    };
    const auto [x, y] = result;
    require(std::isfinite(x) && std::isfinite(y),
            "Hamonshu calculation produced a non-finite coordinate");
    return result;
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
             const int canonical_samples, Function function)
{
  const int samples = context.sample_count(canonical_samples);
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
               const int canonical_samples = 28)
{
  const int samples = context.sample_count(canonical_samples);
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
  const int rows = context.repetition_count(
    5 + static_cast<int>(context.key % 4));
  const double phase = (context.key % 29) * 0.19
                       + context.config.phase;
  for (int row = 0; row != rows; ++row)
    append_curve(
      data, context, 36,
      [=](const double t) {
        const double center = (row + 1.0) / (rows + 1.0);
        const double wave = context.config.curvature * 0.035
          * std::sin((2.0 + row % 3) * 2 * pi * t
                     + phase + row * 0.7);
        const double ripple = context.config.curvature * 0.012
          * std::sin(11 * pi * t + phase);
        return std::pair {t, center + wave + ripple};
      });
}

inline void
make_crests(std::string& data, const pattern_context& context)
{
  const int rows = context.repetition_count(
    3 + static_cast<int>(context.key % 2));
  const int repeats = context.repetition_count(
    3 + static_cast<int>((context.key / 3) % 3));
  const double phase = context.config.phase / (2 * pi);
  for (int row = 0; row != rows; ++row)
    append_curve(
      data, context, 48,
      [=](const double t) {
        double local = std::fmod(
          t * repeats + 0.5 * (row % 2) + phase, 1.0);
        if (local < 0)
          local += 1;
        const double arch = std::sin(pi * local);
        const double center = (row + 1.0) / (rows + 1.0);
        const double y = center
          - context.config.curvature * 0.13 * arch * arch;
        return std::pair {t, y};
      });
}

inline void
make_spirals(std::string& data, const pattern_context& context)
{
  const double phase = (context.key % 31) * 0.2
                       + context.config.phase;
  const int count = context.repetition_count(
    4 + static_cast<int>(context.key % 3));
  const int rows = (count + 2) / 3;
  const double radius = 0.16 * context.config.curvature
                        / std::sqrt(context.config.density);
  for (int index = 0; index != count; ++index)
    {
      const double u = 0.18 + (index % 3) * 0.32;
      const int row = index / 3;
      const double v = rows <= 2
        ? 0.28 + row * 0.42
        : 0.16 + row * 0.68 / (rows - 1.0);
      append_spiral(data, context, u, v, radius, phase + index * 0.8,
                    1.4 + 0.2 * (index % 3));
    }
}

inline void
make_spray(std::string& data, const pattern_context& context)
{
  const double phase = (context.key % 17) * 0.11
                       + context.config.phase;
  const int branches = context.repetition_count(
    5 + static_cast<int>(context.key % 3), 2);
  for (int branch = 0; branch != branches; ++branch)
    {
      const double base = 0.1 + 0.8 * branch / (branches - 1.0);
      append_curve(
        data, context, 24,
        [=](const double t) {
          const double u = base + context.config.curvature * 0.10
            * std::sin(pi * t + phase + branch);
          const double v = 0.88 - 0.70 * t
            + context.config.curvature * 0.08
              * std::sin(3 * pi * t + branch + context.config.phase);
          return std::pair {u, v};
        });
      append_ellipse(data, context,
                     base + 0.08 * std::sin(phase + branch),
                     0.12 + 0.03 * (branch % 2),
                     context.config.curvature * 0.018,
                     context.config.curvature * 0.025, 16);
    }
}

inline void
make_arcs(std::string& data, const pattern_context& context)
{
  const int rows = context.repetition_count(
    4 + static_cast<int>(context.key % 2));
  const int columns = context.repetition_count(5);
  for (int row = 0; row != rows; ++row)
    for (int column = -1; column <= columns; ++column)
      append_curve(
        data, context, 16,
        [=](const double t) {
          const double center = (column + 0.5 * (row % 2)) / columns;
          const double angle = pi + pi * t + context.config.phase;
          return std::pair {
            center + context.config.curvature * 0.13 * std::cos(angle),
            (row + 1.0) / (rows + 1.0)
              + context.config.curvature * 0.10 * std::sin(angle),
          };
        });
}

inline void
make_lattice(std::string& data, const pattern_context& context)
{
  const int lines = context.repetition_count(
    7 + static_cast<int>(context.key % 3));
  for (int line = -2; line < lines; ++line)
    for (const double direction : {-1.0, 1.0})
      append_curve(
        data, context, 22,
        [=](const double t) {
          const double offset = line / static_cast<double>(lines);
          const double u = t;
          const double v = 0.5
            + context.config.curvature * direction * (t - 0.5)
            + offset - 0.35
            + 0.05 * std::sin(context.config.phase);
          return std::pair {u, v};
        });
}

inline void
make_bubbles(std::string& data, const pattern_context& context)
{
  const int count = context.repetition_count(
    8 + static_cast<int>(context.key % 5));
  for (int index = 0; index != count; ++index)
    {
      const unsigned mixed = context.key + static_cast<unsigned>(index * 97);
      const double u = 0.12 + (mixed % 73) / 73.0 * 0.76;
      const double v = 0.12 + ((mixed / 73) % 67) / 67.0 * 0.76;
      const double radius = context.config.curvature
        * (0.035 + ((mixed / 491) % 5) * 0.012)
        / std::sqrt(context.config.density);
      append_ellipse(data, context, u, v, radius,
                     radius * (0.8 + 0.12 * std::sin(context.config.phase)),
                     20);
    }
}

inline void
make_scrolls(std::string& data, const pattern_context& context)
{
  const int rows = context.repetition_count(
    3 + static_cast<int>(context.key % 2));
  const double phase = (context.key % 23) * 0.13
                       + context.config.phase;
  for (int row = 0; row != rows; ++row)
    {
      append_curve(
        data, context, 38,
        [=](const double t) {
          const double u = t;
          const double v = (row + 1.0) / (rows + 1.0)
            + context.config.curvature * 0.09
              * std::sin(2 * pi * (2 * t) + phase);
          return std::pair {u, v};
        });
      append_spiral(data, context,
                    0.18 + 0.64 * row / std::max(1, rows - 1),
                    (row + 1.0) / (rows + 1.0),
                    context.config.curvature * 0.10,
                    phase + row, 1.25);
    }
}

inline void
make_fans(std::string& data, const pattern_context& context)
{
  const bool reverse = context.key % 2 != 0;
  const int lines = context.repetition_count(8, 2);
  for (int line = 0; line != lines; ++line)
    append_curve(
      data, context, 30,
      [=](const double t) {
        double u = 0.05 + 0.9 * t;
        if (reverse)
          u = 1 - u;
        const double height = 0.10 + line * 0.315 / (lines - 1.0);
        const double v = 0.88
          - context.config.curvature * height
            * std::sin(pi * t + context.config.phase)
          - 0.28 * t;
        return std::pair {u, v};
      });
}

inline void
make_breakers(std::string& data, const pattern_context& context)
{
  const bool reverse = context.key % 2 != 0;
  const double phase = (context.key % 19) * 0.17
                       + context.config.phase;
  const int lines = context.repetition_count(6, 2);
  for (int line = 0; line != lines; ++line)
    append_curve(
      data, context, 38,
      [=](const double t) {
        double u = 0.04 + 0.78 * t;
        if (reverse)
          u = 1 - u;
        const double v = 0.84
          - context.config.curvature * 0.55
            * std::sin(0.72 * pi * t)
          + line * 0.125 / (lines - 1.0)
          + context.config.curvature * 0.02 * std::sin(phase + 5 * t);
        return std::pair {u, v};
      });
  append_spiral(data, context, reverse ? 0.22 : 0.78, 0.30,
                context.config.curvature * 0.17,
                (reverse ? pi : 0) + context.config.phase, 1.4);
  const int drops = context.repetition_count(4, 2);
  for (int drop = 0; drop != drops; ++drop)
    append_ellipse(data, context,
                   0.42 + drop * 0.30 / (drops - 1.0),
                   0.16 + 0.04 * (drop % 2),
                   context.config.curvature * 0.012,
                   context.config.curvature * 0.018, 14);
}

inline void
make_braids(std::string& data, const pattern_context& context)
{
  const int strands = context.repetition_count(
    4 + static_cast<int>(context.key % 3), 2);
  const double phase = (context.key % 13) * 0.23
                       + context.config.phase;
  for (int strand = 0; strand != strands; ++strand)
    append_curve(
      data, context, 44,
      [=](const double t) {
        const double v = 0.5 + context.config.curvature * 0.22 * std::sin(
          2 * pi * (1.5 + strand % 2) * t + phase
          + strand * 2 * pi / strands);
        return std::pair {t, v};
      });
}

inline void
make_cascade(std::string& data, const pattern_context& context)
{
  const int lines = context.repetition_count(9, 2);
  for (int line = 0; line != lines; ++line)
    append_curve(
      data, context, 32,
      [=](const double t) {
        const double u = 0.12 + line * 0.72 / (lines - 1.0)
          + context.config.curvature * 0.07
            * std::sin(pi * t + line * 0.4 + context.config.phase);
        const double v = 0.05 + 0.9 * t;
        return std::pair {u, v};
      });
}

inline void
make_ripples(std::string& data, const pattern_context& context)
{
  const int centers = context.repetition_count(
    2 + static_cast<int>(context.key % 2), 2);
  const int rings = context.repetition_count(4);
  for (int center = 0; center != centers; ++center)
    for (int ring = 1; ring <= rings; ++ring)
      append_ellipse(data, context,
                     0.28 + center * 0.43 / (centers - 1.0),
                     0.35 + 0.25 * (center % 2),
                     context.config.curvature * 0.035 * ring,
                     context.config.curvature * 0.022 * ring
                       * (1 + 0.12 * std::sin(context.config.phase)),
                     28);
}

inline void
make_fountains(std::string& data, const pattern_context& context)
{
  const int lines = context.repetition_count(7, 2);
  for (int line = 0; line != lines; ++line)
    {
      const double spread = context.config.curvature
        * (0.12 + line * 0.33 / (lines - 1.0));
      for (const double direction : {-1.0, 1.0})
        append_curve(
          data, context, 28,
          [=](const double t) {
            const double u = 0.5 + direction * spread * std::sin(pi * t / 2);
            const double v = 0.92 - 0.78 * t
              + context.config.curvature * 0.18 * t * t;
            return std::pair {u, v};
          });
    }
  append_spiral(data, context, 0.24, 0.72,
                context.config.curvature * 0.10,
                pi + context.config.phase, 1.1);
  append_spiral(data, context, 0.76, 0.72,
                context.config.curvature * 0.10,
                context.config.phase, 1.1);
}

inline void
make_clouds(std::string& data, const pattern_context& context)
{
  const double phase = (context.key % 31) * 0.1
                       + context.config.phase;
  const int count = context.repetition_count(7);
  const int rows = (count + 3) / 4;
  for (int index = 0; index != count; ++index)
    {
      const double u = 0.12 + (index % 4) * 0.25;
      const int row = index / 4;
      const double v = rows <= 2
        ? 0.30 + row * 0.38
        : 0.18 + row * 0.64 / (rows - 1.0);
      append_spiral(data, context, u, v,
                    context.config.curvature * 0.13
                      / std::sqrt(context.config.density),
                    phase + index * 0.7, 1.35);
    }
  make_waterlines(data, context);
}

inline void
make_cells(std::string& data, const pattern_context& context)
{
  const int rows = context.repetition_count(4, 2);
  const int columns = context.repetition_count(5, 2);
  for (int row = 0; row != rows; ++row)
    for (int column = 0; column != columns; ++column)
      {
        const double u = 0.10 + column * 0.80 / (columns - 1.0)
                         + 0.10 * (row % 2);
        const double v = 0.14 + row * 0.72 / (rows - 1.0);
        const double r = context.config.curvature
          * (0.07 + 0.01 * ((context.key + row + column) % 3))
          / std::sqrt(context.config.density);
        append_ellipse(data, context, u, v, r, r * 0.75, 22);
      }
}

inline void
make_motif(std::string& data, const motif_kind kind,
           const pattern_context& context)
{
  switch (kind)
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
}

} // namespace detail

/// Generate one source-indexed Hamonshu motif with explicit form parameters.
/// @param spec Catalogue identity and descriptive source label.
/// @param box Finite SVG bounds with positive width and height.
/// @param config Density, curvature, phase, orientation, and sampling data.
/// @return SVG path data containing the motif's constituent polylines.
/// @throws std::invalid_argument for invalid catalogue or form parameters.
inline std::string
make_motif_path(const pattern_spec& spec, const pattern_box& box,
                const motif_config& config = {})
{
  validate_pattern_spec(spec);
  require(box.valid(), "Hamonshu motif box must be finite and positive");
  detail::validate_config(config);
  const detail::pattern_context context {
    box, detail::catalogue_key(spec), config,
  };
  std::string data;
  detail::make_motif(data, classify_pattern(spec), context);
  require(!data.empty(), "Hamonshu motif generated no SVG path data");
  return data;
}


} // namespace svg::hamonshu

#endif
