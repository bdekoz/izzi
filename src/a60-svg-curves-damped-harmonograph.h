// izzi harmonograph  -*- mode: C++ -*-

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

#ifndef a60_SVG_CURVES_DAMPED_HARMONOGRAPH_H
#define a60_SVG_CURVES_DAMPED_HARMONOGRAPH_H 1

#include <iostream>
#include <string>
#include <format>
#include <cmath>
#include <numbers>
#include <tuple>
#include <vector>

using point_2t = std::tuple<double, double>;

/**
 * Generates a Damped Harmonograph SVG path.
 * @param pt     Origin tuple {x, y}
 * @param r      Initial Radius (scale)
 * @param n      Frequency ratio (number of waves)
 * @param d      Damping coefficient (0.01 to 0.1)
 * @param cycles Total rotations (2 * pi * cycles)
 */
std::string
generate_damped_harmonograph(point_2t pt, double r, double n, double d,
			     double cycles = 10.0)
{
  auto [ox, oy] = pt;

  auto getPos = [&](double t) -> point_2t {
    double decay = r * std::exp(-d * t);
    return {
      ox + decay * std::sin(t),
      oy + decay * std::sin(n * t + std::numbers::pi / 2.0)
    };
  };

  auto getVelocity = [&](double t) -> point_2t {
    double e_dt = std::exp(-d * t);
    // Derivatives accounting for product rule with damping
    double vx = r * e_dt * (std::cos(t) - d * std::sin(t));
    double vy = r * e_dt * (n * std::cos(n * t + std::numbers::pi / 2.0) - d * std::sin(n * t + std::numbers::pi / 2.0));
    return { vx, vy };
  };

  // Calculate segments: more cycles require more steps to maintain smoothness
  int steps = static_cast<int>(cycles * 64);
  double max_t = cycles * 2.0 * std::numbers::pi;
  double dt = max_t / steps;
  double kappa = dt / 3.0;

  auto [start_x, start_y] = getPos(0);
  std::string path = std::format("M {:.2f} {:.2f}", start_x, start_y);

  for (int i = 0; i < steps; ++i)
    {
      double t0 = i * dt;
      double t1 = (i + 1) * dt;

      auto [p0x, p0y] = getPos(t0);
      auto [v0x, v0y] = getVelocity(t0);
      auto [p1x, p1y] = getPos(t1);
      auto [v1x, v1y] = getVelocity(t1);

      // Control points
      double c1x = p0x + kappa * v0x;
      double c1y = p0y + kappa * v0y;
      double c2x = p1x - kappa * v1x;
      double c2y = p1y - kappa * v1y;

      path += std::format(" C {:.2f} {:.2f}, {:.2f} {:.2f}, {:.2f} {:.2f}",
			  c1x, c1y, c2x, c2y, p1x, p1y);
    }

  return path;
}

/**
 * Generates a Triple-Frequency Damped Harmonograph.
 * @param n1 Primary Vertical Frequency
 * @param n2 Secondary Vertical Frequency
 * @param p2 Phase shift for the second frequency
 */
std::string
generate_triple_harmonograph(point_2t origin, double r, double n1, double n2,
			     double p2, double d, double cycles)
{
  auto [ox, oy] = origin;

  auto getPos = [&](double t) -> point_2t {
    double decay = r * std::exp(-d * t);
    // X is a single pendulum
    double x = ox + decay * std::sin(t);
    // Y is the sum of two pendulums (interference)
    double y = oy + (decay / 2.0) * (std::sin(n1 * t + std::numbers::pi / 2.0) + std::sin(n2 * t + p2));
    return {x, y};
  };

  auto getVelocity = [&](double t) -> point_2t {
    double e_dt = std::exp(-d * t);
    double vx = r * e_dt * (std::cos(t) - d * std::sin(t));
    // Derivative of the sum of two sines
    double vy = (r / 2.0) * e_dt * (
				    (n1 * std::cos(n1 * t + std::numbers::pi / 2.0) - d * std::sin(n1 * t + std::numbers::pi / 2.0)) +
				    (n2 * std::cos(n2 * t + p2) - d * std::sin(n2 * t + p2))
				    );
    return {vx, vy};
  };

  int steps = static_cast<int>(cycles * 120);
  double dt = (cycles * 2.0 * std::numbers::pi) / steps;
  double kappa = dt / 3.0;

  auto [sx, sy] = getPos(0);
  std::string path = std::format("M {:.2f} {:.2f}", sx, sy);

  for (int i = 0; i < steps; ++i) {
    double t0 = i * dt, t1 = (i + 1) * dt;
    auto [p0x, p0y] = getPos(t0); auto [v0x, v0y] = getVelocity(t0);
    auto [p1x, p1y] = getPos(t1); auto [v1x, v1y] = getVelocity(t1);

    path += std::format(" C {:.2f} {:.2f}, {:.2f} {:.2f}, {:.2f} {:.2f}",
			p0x + kappa * v0x, p0y + kappa * v0y,
			p1x - kappa * v1x, p1y - kappa * v1y, p1x, p1y);
  }
  return path;
}



#endif
