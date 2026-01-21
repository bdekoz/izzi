// izzi ribbon curves  -*- mode: C++ -*-

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

#ifndef a60_SVG_CURVES_GRIGNANI_H
#define a60_SVG_CURVES_GRIGNANI_H 1

#include <iostream>
#include <vector>
#include <string>
#include <numbers>
#include <cmath>
#include <format>
#include <tuple>
#include <algorithm>
#include <array>

namespace svg {

/// Define types using std::tuple as requested
using point_2d = std::tuple<double, double>;
using point_3d = std::tuple<double, double, double>;


/// Helper: Normalize a vector tuple
point_3d
normalize(point_3d v)
{
  auto [x, y, z] = v;
  double len = std::hypot(x, y, z);
  if (len == 0.0)
    return {0, 0, 0};
  return {x / len, y / len, z / len};
}

//// Helper: Cross product of two tuples
point_3d
cross_product(point_3d a, point_3d b)
{
  auto [ax, ay, az] = a;
  auto [bx, by, bz] = b;
  return { ay * bz - az * by, az * bx - ax * bz, ax * by - ay * bx };
}


/**
 * Rotates a 3D point tuple to simulate camera orientation.
 */
point_3d
rotate_point(point_3d p, double angle_x, double angle_y)
{
  auto [x, y, z] = p;

  // Rotate around X axis
  double y1 = y * std::cos(angle_x) - z * std::sin(angle_x);
  double z1 = y * std::sin(angle_x) + z * std::cos(angle_x);
  double x1 = x;

  // Rotate around Y axis
  double x2 = x1 * std::cos(angle_y) + z1 * std::sin(angle_y);
  double z2 = -x1 * std::sin(angle_y) + z1 * std::cos(angle_y);
  double y2 = y1;

  return {x2, y2, z2};
}


/**
 * Configuration for the 3D ribbon folding algorithm.
 * Identifiers are now strictly snake_case.
 */
struct ribbon_config
{
  double fold_amplitude = 1.0;  // Radius of the main loops
  double fold_frequency = 3.0;  // How many times it loops
  double torsion = 0.8;         // The vertical depth (Z-height)
  double roll_speed = 1.5;      // How fast the ribbon twists
  double view_tilt_x = 0.6;     // Camera angle X (radians)
  double view_tilt_y = 0.7;     // Camera angle Y (radians)
};


/**
   * Generates an SVG path by simulating a 3D rolling ribbon and
   * projecting it to 2D.
 */
string
make_rolling_ribbon(double origin_x, double origin_y, double scale,
		    int ribbon_strands, double ribbon_width,
		    ribbon_config config)
{
  using std::numbers::pi;

  // Higher steps = smoother curves
  const int steps = 360;

  double gap = ribbon_width;
  double stride = ribbon_width + gap;
  double total_bundle_width = (ribbon_strands * stride) - gap;

  std::string path_data;

  // --- 1. Define the 3D Spine Function ---
  auto get_spine_3d = [&](double t) -> point_3d
  {
    double angle_k = config.fold_frequency * t;

    // Modulate radius for loops
    double r = config.fold_amplitude + 0.4 * std::cos(angle_k);

    double x = r * std::cos(t);
    double y = r * std::sin(t);
    double z = config.torsion * std::sin(angle_k);
    return { x, y, z };
  };

  // --- 2. Generate Each Strand ---
  for (int s = 0; s < ribbon_strands; ++s)
    {
      double offset = (s * stride) - (total_bundle_width / 2.0);

      std::vector<point_2d> edge_left_2d;
      std::vector<point_2d> edge_right_2d;

      for (int i = 0; i <= steps; ++i)
	{
	  double t = (static_cast<double>(i) / steps) * 2.0 * pi;
	  double epsilon = 0.005;

	  // A. Get Spine Point & Tangent
	  point_3d p0 = get_spine_3d(t);
	  point_3d p1 = get_spine_3d(t + epsilon);

	  auto [p0x, p0y, p0z] = p0;
	  auto [p1x, p1y, p1z] = p1;

	  point_3d tangent = normalize({ p1x - p0x, p1y - p0y, p1z - p0z });

	  // B. Calculate Surface Normal
	  // Arbitrary "Up" vector to calculate frame
	  point_3d up = {0, 0, 1};
	  if (std::abs(std::get<2>(tangent)) > 0.95)
	    up = {1, 0, 0};

	  point_3d binormal = normalize(cross_product(tangent, up));
	  point_3d normal   = cross_product(binormal, tangent); // Already normalized

	  // C. Apply "Roll" (Twist)
	  // Rotate the Normal vector around the Tangent axis
	  double current_roll = config.roll_speed * t * 2.0;

	  // Unpack vectors for rotation math
	  auto [nx, ny, nz] = normal;
	  // auto [tx, ty, tz] = tangent; unused

	  // Calculate (Tangent x Normal) for Rodrigues formula
	  auto [kx, ky, kz] = cross_product(tangent, normal);

	  double c = std::cos(current_roll);
	  double s_val = std::sin(current_roll); // 's' is taken by loop var

	  // Rodrigues rotation formula: V_rot = V*cos + (K x V)*sin
	  point_3d surface_vec = {
	    nx * c + kx * s_val,
	    ny * c + ky * s_val,
	    nz * c + kz * s_val
	  };
	  auto [svx, svy, svz] = surface_vec;

	  // D. Compute 3D positions
	  point_3d p_start_3d = {
	    p0x + svx * offset,
	    p0y + svy * offset,
	    p0z + svz * offset
	  };

	  point_3d p_end_3d = {
	    p0x + svx * (offset + ribbon_width),
	    p0y + svy * (offset + ribbon_width),
	    p0z + svz * (offset + ribbon_width)
	  };

	  // E. Project to 2D
	  auto [rx_start, ry_start, rz_start] = rotate_point(p_start_3d, config.view_tilt_x, config.view_tilt_y);
	  auto [rx_end, ry_end, rz_end]       = rotate_point(p_end_3d,   config.view_tilt_x, config.view_tilt_y);

	  edge_left_2d.emplace_back(origin_x + rx_start * scale, origin_y + ry_start * scale);
	  edge_right_2d.emplace_back(origin_x + rx_end * scale,   origin_y + ry_end * scale);
	}

      // --- 3. Construct SVG Path String ---
      auto [start_x, start_y] = edge_left_2d[0];
      path_data += std::format("M {:.2f} {:.2f} ", start_x, start_y);

      for (size_t k = 1; k < edge_left_2d.size(); ++k)
	{
	  auto [lx, ly] = edge_left_2d[k];
	  path_data += std::format("L {:.2f} {:.2f} ", lx, ly);
	}

      auto [last_rx, last_ry] = edge_right_2d.back();
      path_data += std::format("L {:.2f} {:.2f} ", last_rx, last_ry);

      for (int k = static_cast<int>(edge_right_2d.size()) - 2; k >= 0; --k)
	{
	  auto [rx, ry] = edge_right_2d[k];
	  path_data += std::format("L {:.2f} {:.2f} ", rx, ry);
	}

      path_data += "Z ";
    }

  return std::format("<path d=\"{}\" fill=\"black\" stroke=\"none\" />", path_data);
}



/// Config struct.
struct ripple_config
{
  double amplitude = 1.0;
  double frequency = 1.0;
  double phase_shift = 0.0;
  double decay = 0.0;
  double view_tilt_x = 0.5;
  double view_tilt_y = 0.6;
};


std::string
make_ripple_ribbon(double origin_x, double origin_y, double length,
		   int ribbon_strands, double ribbon_width, ripple_config config)
{
  using std::numbers::pi;

  const int steps = 180;
  double gap = ribbon_width;
  double stride = ribbon_width + gap;
  double total_bundle_width = (ribbon_strands * stride) - gap;

  // Local lambda
  auto get_spine_3d = [&](double t) -> point_3d
  {
    // t: -1.0 to 1.0
    double x = t * (length / 2.0);
    double wave_arg = (t * pi * config.frequency) + config.phase_shift;

    double amp_mod = 1.0;
    if (config.decay > 0.0)
      {
	// Linear decay: 1.0 at start, decreasing based on decay factor
	// To make it look nice, we decay based on progress 0..1
	double progress = (t + 1.0) / 2.0;
	amp_mod = std::max(0.0, 1.0 - (config.decay * progress));
      }

    double z = config.amplitude * std::sin(wave_arg) * amp_mod;
    return {x, 0.0, z};
  };

  std::string path_data;  
  for (int s = 0; s < ribbon_strands; ++s)
    {
      double offset_val = (s * stride) - (total_bundle_width / 2.0);
      std::vector<point_2d> edge_left, edge_right;
      
      for (int i = 0; i <= steps; ++i)
	{
	  double t = -1.0 + (2.0 * static_cast<double>(i) / steps);
	  double epsilon = 0.01;
	  
	  point_3d p0 = get_spine_3d(t);
	  point_3d p1 = get_spine_3d(t + epsilon);
	  auto [p0x, p0y, p0z] = p0;
	  auto [p1x, p1y, p1z] = p1;
	  
	  point_3d tangent = normalize({ p1x - p0x, p1y - p0y, p1z - p0z });
	  point_3d up = {0, 0, 1};
	  point_3d binormal = normalize(cross_product(tangent, up));
	  
	  // Ensure consistent orientation
	  if (std::get<1>(binormal) < 0)
	    {
	      binormal = { -std::get<0>(binormal), -std::get<1>(binormal), -std::get<2>(binormal) };
	    }
	  auto [bx, by, bz] = binormal;

      point_3d p_start_3d = { p0x + bx * offset_val, p0y + by * offset_val, p0z + bz * offset_val };
      point_3d p_end_3d   = { p0x + bx * (offset_val + ribbon_width), p0y + by * (offset_val + ribbon_width), p0z + bz * (offset_val + ribbon_width) };

      auto [rx_s, ry_s, rz_s] = rotate_point(p_start_3d, config.view_tilt_x, config.view_tilt_y);
      auto [rx_e, ry_e, rz_e] = rotate_point(p_end_3d,   config.view_tilt_x, config.view_tilt_y);

      edge_left.emplace_back(origin_x + rx_s, origin_y + ry_s);
      edge_right.emplace_back(origin_x + rx_e, origin_y + ry_e);
    }

    auto [sx, sy] = edge_left[0];
    path_data += std::format("M {:.1f} {:.1f} ", sx, sy);
    for (size_t k = 1; k < edge_left.size(); ++k)
      {
	auto [lx, ly] = edge_left[k];
	path_data += std::format("L {:.1f} {:.1f} ", lx, ly);
      }
    auto [ex, ey] = edge_right.back();
    path_data += std::format("L {:.1f} {:.1f} ", ex, ey);
    for (int k = static_cast<int>(edge_right.size()) - 2; k >= 0; --k)
      {
	auto [rx, ry] = edge_right[k];
	path_data += std::format("L {:.1f} {:.1f} ", rx, ry);
      }
    path_data += "Z ";
  }
  return std::format("<path d=\"{}\" fill=\"black\" />", path_data);
}


} // namespace svg

#endif
