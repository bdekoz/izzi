// izzi 2d points  -*- mode: C++ -*-

// Copyright (c) 2025, Benjamin De Kosnik <b.dekosnik@gmail.com>

// This file is part of the alpha60 library.  This library is free
// software; you can redistribute it and/or modify it under the terms
// of the GNU General Public License as published by the Free Software
// Foundation; either version 3, or (at your option) any later
// version.

// This library is distributed in the hope that it will be useful, but
// WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// General Public License for more details.

#ifndef izzi_POINTS_H
#define izzi_POINTS_H 1

namespace svg {

/// Point (x,y) in 2D space, space_type defaults to double.
using point_2t = std::tuple<space_type, space_type>;

/// Point (x,y) in 2D space with weight n.
using point_2tn = std::tuple<ulong, point_2t>;

/// Named Point (x,y) in 2D space.
using point_2ts = std::tuple<string, point_2t>;

/// Convert point_2t to string.
string
to_string(point_2t p)
{
  auto [ x, y ] = p;
  std::ostringstream oss;
  oss << x << ',' << y;
  return oss.str();
}

/// Split range, so one dimension of (x,y) cartesian plane.
using vspace = std::vector<space_type>;

/// Latitude and Longitude Ranges.
using srange = std::set<point_2t>;
using srangen = std::set<point_2tn>;
using vrange = std::vector<point_2t>;
using vrangen = std::vector<point_2tn>;
using vvranges = std::vector<vrange>;

using vrangenamed = std::vector<point_2ts>;


/// Decompose/split 2D ranges to 1D spaces, perhaps with scaling.
void
split_vrange(const vrange& cpoints, vspace& xpoints, vspace& ypoints,
	     const double xscale = 1, const double yscale = 1)
{
 for (const auto& [x, y] : cpoints)
   {
     xpoints.push_back(x / xscale);
     ypoints.push_back(y / yscale);
   }
}


/// Union two ranges.
vrange
union_vrange(const vrange& r1, const vrange& r2)
{
  vrange vr;
  vr.insert(vr.end(), r1.begin(), r1.end());
  vr.insert(vr.end(), r2.begin(), r2.end());
  return vr;
}


/// For each dimension of vrnage, find min/max and return (xmax, ymax)
/// NB: Assumes zero is min.
point_2t
max_vrange(vspace& xpoints, vspace& ypoints, const uint pown)
{
  point_2t rangemaxx = { 0, 0 };
  if (!xpoints.empty() && !ypoints.empty())
    {
      sort(xpoints.begin(), xpoints.end());
      sort(ypoints.begin(), ypoints.end());

      // For x axis, need to insert padding iff axes are scaled down
      // and/or have values with truncated significant digits.
      const bool padp(true);
      if (padp)
	{
	  const double sigd = pow(10, pown);

	  const double dx = xpoints.back();
	  double ix = std::round(dx * sigd) / sigd;
	  if (ix > dx)
	    xpoints.push_back(ix);

	  const double dy = ypoints.back();
	  uint iy = std::round(dy * sigd) / sigd;
	  if (iy > dy)
	    ypoints.push_back(iy);
	}

      // Find combined ranges, assume zero start.
      rangemaxx = std::make_tuple(xpoints.back(), ypoints.back());
    }
  return rangemaxx;
}


/// Just the range info, none of the temporary objects.
point_2t
max_vrange(const vrange& points, const uint pown,
	   const double xscale = 1, const double yscale = 1)
{
  vspace pointsx;
  vspace pointsy;
  split_vrange(points, pointsx, pointsy, xscale, yscale);
  point_2t ret = max_vrange(pointsx, pointsy, pown);
  return ret;
}


/// Truncate double to double with pown signifigant digits.
vspace
narrow_vspace(const vspace& points, uint pown)
{
  const double sigd = pow(10, pown);
  vspace npoints;
  for (const double& d : points)
    {
      double dn(d);
      if (dn > 0)
	{
	  uint itrunc(dn * sigd);
	  npoints.push_back(itrunc / sigd);
	}
      else
	npoints.push_back(dn);
    }
  return npoints;
}

/// Find cartesian distance between two 2D points.
space_type
distance_cartesian(const point_2t& p1, const point_2t& p2)
{
  auto [ x1, y1 ] = p1;
  auto [ x2, y2 ] = p2;
  auto distancex = (x2 - x1) * (x2 - x1);
  auto distancey = (y2 - y1) * (y2 - y1);
  space_type distance = sqrt(distancex + distancey);
  return distance;
}


// Does point p1 of radius r1 intersect point p2 with radius r2?
// https://developer.mozilla.org x 2D_collision_detection
bool
detect_collision(const point_2t& p1, const int r1,
		 const point_2t& p2, const int r2)
{
  bool ret(false);
  if (distance_cartesian(p1, p2) < r1 + r2)
    ret = true;
  return ret;
}

} // namespace svg

struct Point
{
  double x, y;
  std::string name;

  Point(double x = 0, double y = 0) : x(x), y(y) { }

  double
  distance(const Point& other) const
  { return std::hypot(x - other.x, y - other.y); }

  bool
  operator==(const Point& other) const
  { return x == other.x && y == other.y; }

  Point
  operator+(const Point& other) const
  { return Point(x + other.x, y + other.y); }

  Point
  operator/(double scalar) const
  { return Point(x / scalar, y / scalar); }
};

struct WeightedPoint
{
  double x, y;
  size_t weight;

  WeightedPoint(double x, double y, size_t w) : x(x), y(y), weight(w) {}
  WeightedPoint(const Point& p, size_t w) : x(p.x), y(p.y), weight(w) {}
};

using vpoints = std::vector<Point>;
using vwpoints = std::vector<WeightedPoint>;


namespace svg {

/// Convert point_2t to Point.

vpoints
vrange_to_points(const vrange& inpoints)
{
  std::vector<Point> outpoints;
  outpoints.reserve(inpoints.size());

  auto lconv = [](const point_2t& p){ auto [ x, y ] = p; return Point(x, y); };
  std::ranges::transform(inpoints, std::back_inserter(outpoints), lconv);
  return outpoints;
}

/// Convert point_2ts to Point.
vpoints
vrangenamed_to_points(const vrangenamed& inpoints)
{
  std::vector<Point> outpoints;
  outpoints.reserve(inpoints.size());

  auto lconv = [](const point_2ts& p)
  { auto [ s, pt ] = p; auto [x, y] = pt; return Point(x, y); };
  std::ranges::transform(inpoints, std::back_inserter(outpoints), lconv);
  return outpoints;
}

/// Convert Point to point_2t
vrange
points_to_vrange(const vpoints& inpoints)
{
  vrange outpoints;
  outpoints.reserve(inpoints.size());

  auto lconv = [](const Point& p){ return std::make_tuple(p.x, p.y); };
  std::ranges::transform(inpoints, std::back_inserter(outpoints), lconv);
  return outpoints;
}

} // namespace svg

#endif
