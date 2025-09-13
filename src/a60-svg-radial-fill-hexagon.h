#include <vector>
#include <tuple>
#include <cmath>
#include <numbers>
#include <functional>
#include <queue>
#include <unordered_set>
#include <iostream>

#include "a60-svg.h"

namespace svg {

/**
   write a c++ function that tessalates hexagons, named
   generate_radial_hexagons_v3. The arguments are (const int n, const
   double r, const point_2t origin, const bool centerfilledp = false)
   where n is the number of hexagons total, r is the radius a hexagon
   (all hexagons same size), point_2t is a tuple of (x,y) points, and
   centerfilledp when true starts filling in the center and when false
   leaves the center empty

   the function returns a std::vector<points> of hexagon centerpoints.
 */

//using point_2t = std::tuple<double, double>;

// Hash function for point_2t to use in unordered_set
// Use a simple hash combining x and y coordinates
struct PointHash
{
  std::size_t
  operator()(const point_2t& p) const
  {
    auto [x, y] = p;
    return std::hash<double>()(x) ^ (std::hash<double>()(y) << 1);
  }
};

// Equality function for point_2t
struct PointEqual
{
  bool
  operator()(const point_2t& a, const point_2t& b) const
  {
    // Compare with a small epsilon for floating point precision
    auto [ax, ay] = a;
    auto [bx, by] = b;
    return std::abs(ax - bx) < 1e-9 && std::abs(ay - by) < 1e-9;
  }
};

vrange
radiate_hexagon_honeycomb(const uint n, const double r, const point_2t origin,
			  const bool centerfilledp)
{
  vrange hexagon_centers;
  if (n == 0)
    return hexagon_centers;

  auto [origin_x, origin_y] = origin;


  // For hexagonal grid, compute the horizontal and vertical spacing.

  // Width of hexagon (flat-to-flat)
  const double hex_width = 2.0 * r;

  // Height of hexagon (point-to-point)
  const double hex_height = std::sqrt(3.0) * r;

  // Direction vectors for the 6 neighbors in a hexagonal grid
  const vrange directions =
    {
      { hex_width, 0.0 },                    // Right
      { hex_width / 2.0, hex_height },       // Up-right
      { -hex_width / 2.0, hex_height },      // Up-left
      { -hex_width, 0.0 },                   // Left
      { -hex_width / 2.0, -hex_height },     // Down-left
      { hex_width / 2.0, -hex_height }       // Down-right
  };

  // Use BFS to generate hexagons radially
  std::queue<point_2t> q;
  std::unordered_set<point_2t, PointHash, PointEqual> visited;

  // Ring 0 is the center hexagon.
  if (centerfilledp)
    {
      q.push(origin);
      hexagon_centers.push_back(origin);
    }
  visited.insert(origin);

  // Ring 1 (6 hexagons around center)
  for (const auto& movement : directions)
    {
      auto [dx, dy] = movement;
      point_2t neighbor = {origin_x + dx, origin_y + dy};
      q.push(neighbor);
      visited.insert(neighbor);
      hexagon_centers.push_back(neighbor);
      if (hexagon_centers.size() == n)
	break;
    }

  // Generate hexagons until we reach n total
  while (hexagon_centers.size() < n && !q.empty())
    {
      point_2t current = q.front();
      q.pop();

      auto [current_x, current_y] = current;

      // Explore all 6 neighbors
      for (const auto& dir : directions)
	{
	  auto [dx, dy] = dir;
	  point_2t neighbor = {current_x + dx, current_y + dy};

	  // Check if we haven't visited this point and haven't reached n yet
	  if (visited.find(neighbor) == visited.end()
	      && hexagon_centers.size() < n)
	    {
	      visited.insert(neighbor);
	      hexagon_centers.push_back(neighbor);
	      q.push(neighbor);
	    }
	}
    }

  return hexagon_centers;
}


vspace
get_honeycomb_angles(const vrange& hexagon_centers, const point_2t origin)
{
  vspace angles;
  angles.reserve(hexagon_centers.size());

  auto [origin_x, origin_y] = origin;
  for (const auto& center : hexagon_centers)
    {
      auto [hex_x, hex_y] = center;
      double dx = hex_x - origin_x;
      double dy = hex_y - origin_y;
      double angle_rad = std::atan2(dy, dx);
      double angle_d(180 * (angle_rad / k::pi));
      angles.push_back(angle_d);
    }

  return angles;
}

} // namespace svg
