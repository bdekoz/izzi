// Izzi roulette curves -*- mode: C++ -*-

// Copyright (c) 2026, Benjamin De Kosnik <b.dekosnik@gmail.com>

// This file is part of the alpha60 library.  This library is free
// software; you can redistribute it and/or modify it under the terms
// of the GNU General Public License as published by the Free Software
// Foundation; either version 3, or (at your option) any later
// version.

// This library is distributed in the hope that it will be useful, but
// WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// General Public License for more details.

// Formula source and geometric explanation:
// Keith Peters, "Coding Curves 09: Roulette Curves", December 15, 2022.
// https://www.bit-101.com/2017/2022/12/coding-curves-09-roulette-curves/

#ifndef a60_SVG_CURVES_ROULETTE_H
#define a60_SVG_CURVES_ROULETTE_H 1

#include <cmath>
#include <cstddef>
#include <limits>
#include <numeric>
#include <stdexcept>
#include <string>
#include <tuple>

#include "a60-svg.h"

namespace svg {

/// Whether the moving circle rolls outside or inside the fixed circle.
enum class roulette_kind
{
  epitrochoid,  ///< Moving circle rolls around the fixed circle's exterior.
  hypotrochoid, ///< Moving circle rolls around the fixed circle's interior.
};

/// Parameters for a circle rolling along a straight line.
struct trochoid_config
{
  double rolling_radius = 1.0; ///< Radius `a` of the rolling circle.
  double point_distance = 1.0; ///< Center-to-tracing-point distance `b`.
  double turns = 4.0; ///< Number of rolling-circle revolutions to sample.
  double phase = 0.0; ///< Initial tracing-point angle, in radians.
  std::size_t samples_per_turn = 160; ///< Polyline samples per revolution.
};

/// Parameters for epi- and hypotrochoids with an automatically closed path.
struct roulette_config
{
  /// Radius of the fixed circle, expressed as an integer ratio term.
  std::size_t fixed_radius = 3;
  /// Radius of the rolling circle, expressed as an integer ratio term.
  std::size_t rolling_radius = 1;
  /// Distance from the rolling-circle center to the tracing point.
  double point_distance = 1.0;
  /// Initial tracing-point angle relative to the rolling circle, in radians.
  double phase = 0.0;
  /// Polyline samples for each complete orbit of the rolling-circle center.
  std::size_t samples_per_turn = 192;
};

/// Internal validation and point-sampling helpers.
namespace roulette_detail {

inline constexpr double pi
  = 3.141592653589793238462643383279502884;
inline constexpr std::size_t minimum_samples_per_turn = 8;
inline constexpr std::size_t maximum_sample_count = 2'000'000;

inline void
require(const bool condition, const std::string& message)
{
  if (!condition)
    throw std::invalid_argument(message);
}

inline void
validate_output_transform(const svg::point_2t origin, const double scale)
{
  const auto [origin_x, origin_y] = origin;
  require(std::isfinite(origin_x) && std::isfinite(origin_y),
          "roulette origin must contain finite coordinates");
  require(std::isfinite(scale) && scale > 0,
          "roulette scale must be finite and positive");
}

inline std::size_t
sample_count(const double turns, const std::size_t samples_per_turn)
{
  require(std::isfinite(turns) && turns > 0,
          "roulette turn count must be finite and positive");
  require(samples_per_turn >= minimum_samples_per_turn,
          "roulette samples_per_turn must be at least eight");
  const double requested = std::ceil(
    turns * static_cast<double>(samples_per_turn));
  require(std::isfinite(requested)
            && requested <= static_cast<double>(maximum_sample_count),
          "roulette sample count exceeds the supported limit");
  return static_cast<std::size_t>(requested);
}

inline svg::point_2t
to_svg_point(const svg::point_2t origin, const double scale,
             const double x, const double y)
{
  const auto [origin_x, origin_y] = origin;
  const svg::point_2t result {
    origin_x + scale * x,
    origin_y - scale * y,
  };
  const auto [result_x, result_y] = result;
  require(std::isfinite(result_x) && std::isfinite(result_y),
          "roulette calculation produced a non-finite coordinate");
  return result;
}

inline std::string
path_from_points(const svg::vrange& points, const bool close)
{
  require(points.size() >= 2,
          "roulette calculation produced too few path points");
  std::string path_data = svg::make_path_data_from_points(points);
  if (close)
    path_data += "Z ";
  return path_data;
}

} // namespace roulette_detail

/// Return the number of fixed-circle orbits required to close a centered
/// roulette whose two radius terms are integers.
/// @param config Centered roulette parameters.
/// @return Denominator of the reduced fixed-to-rolling radius ratio.
/// @throws std::invalid_argument if either radius is zero.
inline std::size_t
roulette_completion_turns(const roulette_config& config)
{
  roulette_detail::require(config.fixed_radius != 0,
                           "roulette fixed_radius must be positive");
  roulette_detail::require(config.rolling_radius != 0,
                           "roulette rolling_radius must be positive");
  return config.rolling_radius
         / std::gcd(config.fixed_radius, config.rolling_radius);
}

/// Generate a trochoid from a circle rolling along a straight line.
///
/// The canonical cycloid has `point_distance == rolling_radius`. A smaller
/// distance produces a curtate trochoid; a larger distance produces a prolate
/// trochoid. Cartesian y coordinates are reflected into SVG's downward y axis.
/// @param origin SVG location corresponding to the formula's Cartesian origin.
/// @param scale Multiplier applied to every generated coordinate.
/// @param config Rolling circle, tracing point, duration, and sampling data.
/// @return SVG path data containing one open polyline.
/// @throws std::invalid_argument for non-finite or unsupported parameters.
inline std::string
make_trochoid_path(const svg::point_2t origin, const double scale,
                   const trochoid_config& config = {})
{
  roulette_detail::validate_output_transform(origin, scale);
  roulette_detail::require(
    std::isfinite(config.rolling_radius) && config.rolling_radius > 0,
    "trochoid rolling_radius must be finite and positive");
  roulette_detail::require(
    std::isfinite(config.point_distance) && config.point_distance >= 0,
    "trochoid point_distance must be finite and nonnegative");
  roulette_detail::require(std::isfinite(config.phase),
                            "trochoid phase must be finite");

  const std::size_t samples = roulette_detail::sample_count(
    config.turns, config.samples_per_turn);
  const double maximum_t = 2 * roulette_detail::pi * config.turns;
  svg::vrange points;
  points.reserve(samples + 1);
  for (std::size_t index = 0; index <= samples; ++index)
    {
      const double t = maximum_t * static_cast<double>(index) / samples;
      const double angle = t + config.phase;
      const double x = config.rolling_radius * t
                       - config.point_distance * std::sin(angle);
      const double y = config.rolling_radius
                       - config.point_distance * std::cos(angle);
      points.push_back(roulette_detail::to_svg_point(origin, scale, x, y));
    }
  return roulette_detail::path_from_points(points, false);
}

/// Generate a cycloid, the common trochoid traced at the circle's edge.
/// @param origin SVG location corresponding to the formula's Cartesian origin.
/// @param scale Multiplier applied to every generated coordinate.
/// @param config Trochoid settings; `point_distance` is replaced by the radius.
/// @return SVG path data containing one open polyline.
inline std::string
make_cycloid_path(const svg::point_2t origin, const double scale,
                  trochoid_config config = {})
{
  config.point_distance = config.rolling_radius;
  return make_trochoid_path(origin, scale, config);
}

/// Generate an automatically closed epi- or hypotrochoid.
///
/// Integer radius terms make the closure period exact: the path samples the
/// denominator of their reduced ratio in complete turns, as described by the
/// source article.
/// @param origin Center of the fixed circle in SVG coordinates.
/// @param scale Multiplier applied to the radius terms and point distance.
/// @param kind Selects outside-circle or inside-circle rolling.
/// @param config Radius ratio, tracing point, phase, and sampling data.
/// @return Closed SVG path data.
/// @throws std::invalid_argument for invalid geometry or sampling parameters.
inline std::string
make_roulette_path(const svg::point_2t origin, const double scale,
                   const roulette_kind kind,
                   const roulette_config& config = {})
{
  roulette_detail::validate_output_transform(origin, scale);
  const std::size_t turns = roulette_completion_turns(config);
  roulette_detail::require(
    std::isfinite(config.point_distance) && config.point_distance >= 0,
    "roulette point_distance must be finite and nonnegative");
  roulette_detail::require(std::isfinite(config.phase),
                            "roulette phase must be finite");
  if (kind == roulette_kind::hypotrochoid)
    roulette_detail::require(
      config.fixed_radius > config.rolling_radius,
      "hypotrochoid fixed_radius must exceed rolling_radius");

  const std::size_t samples = roulette_detail::sample_count(
    static_cast<double>(turns), config.samples_per_turn);
  const double fixed = static_cast<double>(config.fixed_radius);
  const double rolling = static_cast<double>(config.rolling_radius);
  const double signed_rolling = kind == roulette_kind::epitrochoid
                                  ? rolling : -rolling;
  const double center_radius = fixed + signed_rolling;
  const double frequency = center_radius / rolling;
  const double maximum_t = 2 * roulette_detail::pi * turns;

  svg::vrange points;
  points.reserve(samples + 1);
  for (std::size_t index = 0; index <= samples; ++index)
    {
      const double t = maximum_t * static_cast<double>(index) / samples;
      const double tracing_angle = frequency * t + config.phase;
      double x = center_radius * std::cos(t);
      double y = center_radius * std::sin(t);
      if (kind == roulette_kind::epitrochoid)
        {
          x -= config.point_distance * std::cos(tracing_angle);
          y -= config.point_distance * std::sin(tracing_angle);
        }
      else
        {
          x += config.point_distance * std::cos(tracing_angle);
          y -= config.point_distance * std::sin(tracing_angle);
        }
      points.push_back(roulette_detail::to_svg_point(origin, scale, x, y));
    }
  return roulette_detail::path_from_points(points, true);
}

/// Generate an epitrochoid from a circle rolling outside a fixed circle.
/// @param origin Center of the fixed circle in SVG coordinates.
/// @param scale Multiplier applied to the radius terms and point distance.
/// @param config Radius ratio, tracing point, phase, and sampling data.
/// @return Closed SVG path data.
inline std::string
make_epitrochoid_path(const svg::point_2t origin, const double scale,
                      const roulette_config& config = {})
{
  return make_roulette_path(
    origin, scale, roulette_kind::epitrochoid, config);
}

/// Generate an epicycloid by placing the tracing point on the rolling circle.
/// @param origin Center of the fixed circle in SVG coordinates.
/// @param scale Multiplier applied to the radius terms.
/// @param config Centered roulette settings; `point_distance` is replaced.
/// @return Closed SVG path data.
inline std::string
make_epicycloid_path(const svg::point_2t origin, const double scale,
                     roulette_config config = {})
{
  config.point_distance = static_cast<double>(config.rolling_radius);
  return make_epitrochoid_path(origin, scale, config);
}

/// Generate a hypotrochoid from a circle rolling inside a fixed circle.
/// @param origin Center of the fixed circle in SVG coordinates.
/// @param scale Multiplier applied to the radius terms and point distance.
/// @param config Radius ratio, tracing point, phase, and sampling data.
/// @return Closed SVG path data.
inline std::string
make_hypotrochoid_path(const svg::point_2t origin, const double scale,
                       const roulette_config& config = {})
{
  return make_roulette_path(
    origin, scale, roulette_kind::hypotrochoid, config);
}

/// Generate a hypocycloid by placing the tracing point on the rolling circle.
/// @param origin Center of the fixed circle in SVG coordinates.
/// @param scale Multiplier applied to the radius terms.
/// @param config Centered roulette settings; `point_distance` is replaced.
/// @return Closed SVG path data.
inline std::string
make_hypocycloid_path(const svg::point_2t origin, const double scale,
                      roulette_config config = {})
{
  config.point_distance = static_cast<double>(config.rolling_radius);
  return make_hypotrochoid_path(origin, scale, config);
}

} // namespace svg

#endif
