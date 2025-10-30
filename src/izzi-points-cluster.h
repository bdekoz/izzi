// izzi 2d point clustering  -*- mode: C++ -*-

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

#ifndef izzi_POINTS_CLUSTER_H
#define izzi_POINTS_CLUSTER_H 1

#include <vector>
#include <cmath>
#include <unordered_map>
#include <queue>
#include <algorithm>
#include <random>
#include <ranges>
#include <concepts>
#include <limits>
#include <memory>
#include <iostream>
#include <numeric>


/// Voronoi diagram related structures
struct voronoi_cell
{
  Point		site;
  vpoints	points;
  vpoints	vertices;

  voronoi_cell(const Point& p) : site(p) { }
};


/// Cluster of points.
/// Reduce a set of points via a given cluster algorithm.
class point_cluster
{
  double	radius;
  vpoints	points;

public:
  point_cluster(const vpoints& input_points, const double r)
  : radius(r), points(input_points) { }

  /// Algorithm 1: Grid-based Clustering (Simple and Fast)
  vwpoints
  reduce_grid()
  {
    if (points.empty()) return {};

    // Create grid cells
    std::unordered_map<std::pair<int, int>, vpoints,
		       decltype([](const auto& p) {
			 return std::hash<int>{}(p.first) ^ (std::hash<int>{}(p.second) << 1);
		       })> grid;

    for (const auto& point : points)
      {
	int grid_x = static_cast<int>(point.x / radius);
	int grid_y = static_cast<int>(point.y / radius);
	grid[{grid_x, grid_y}].push_back(point);
      }

    vwpoints result;
    for (const auto& [cell, cell_points] : grid)
      {
	if (!cell_points.empty())
	  {
	    auto centroid = calculate_centroid(cell_points);
	    result.emplace_back(centroid.x, centroid.y, cell_points.size());
	  }
      }

    return result;
  }

  /// Algorithm 2: Hierarchical Agglomerative Clustering (More Accurate)
  vwpoints
  reduce_hierarchical_agglomerative()
  {
    if (points.empty()) return {};

    std::vector<vpoints> clusters;
    std::vector<bool> assigned(points.size(), false);

    for (size_t i = 0; i < points.size(); ++i) {
      if (assigned[i]) continue;

      vpoints current_cluster;
      std::queue<size_t> to_process;
      to_process.push(i);
      assigned[i] = true;

      while (!to_process.empty()) {
	size_t current_idx = to_process.front();
	to_process.pop();
	current_cluster.push_back(points[current_idx]);

	// Find all unassigned points within cluster radius
	for (size_t j = 0; j < points.size(); ++j) {
	  if (!assigned[j] && points[current_idx].distance(points[j]) <= radius) {
	    assigned[j] = true;
	    to_process.push(j);
	  }
	}
      }

      if (!current_cluster.empty()) {
	clusters.push_back(std::move(current_cluster));
      }
    }

    return weigh_cluster(clusters);
  }

  /// Algorithm 3: K-Means Inspired with Distance Constraint
  vwpoints
  reduce_constrained_k_means(size_t max_iterations = 100)
  {
    if (points.empty()) return {};

    auto centroids = initialize_centroids();
    std::vector<vpoints> clusters(centroids.size());

    for (size_t iter = 0; iter < max_iterations; ++iter)
      {
	// Assign points to nearest centroid within cluster radius
	for (const auto& point : points)
	  {
	    double min_dist = std::numeric_limits<double>::max();
	    int best_cluster = -1;

	    for (size_t i = 0; i < centroids.size(); ++i)
	      {
		double dist = point.distance(centroids[i]);
		if (dist <= radius && dist < min_dist)
		  {
		    min_dist = dist;
		    best_cluster = static_cast<int>(i);
		  }
	      }

	    if (best_cluster != -1)
	      clusters[best_cluster].push_back(point);
	  }

      // Update centroids
      vpoints new_centroids;
      for (size_t i = 0; i < clusters.size(); ++i)
	{
	  if (!clusters[i].empty())
	    new_centroids.push_back(calculate_centroid(clusters[i]));
      }

      // Check for convergence
      if (new_centroids.size() == centroids.size())
	{
	  bool converged = true;
	  for (size_t i = 0; i < centroids.size(); ++i)
	    {
	      if (centroids[i].distance(new_centroids[i]) > 1e-6)
		{
		  converged = false;
		  break;
		}
	    }
	  if (converged)
	    break;
	}

      centroids = std::move(new_centroids);
      clusters.resize(centroids.size());
      }

    return weigh_cluster(clusters);
  }

  /// Algorithm 4: Voronoi-Based Clustering with Radius Constraint
  vwpoints
  reduce_voronoi(size_t max_iterations = 50)
  {
    if (points.empty()) return {};

    // Step 1: Initialize sites using farthest point sampling
    auto sites = initialize_centroids();
    std::vector<voronoi_cell> cells;

    for (const auto& site : sites) {
      cells.emplace_back(site);
    }

    for (size_t iter = 0; iter < max_iterations; ++iter) {
      // Clear current cell points
      for (auto& cell : cells) {
	cell.points.clear();
      }

      // Step 2: Assign each point to nearest site (Voronoi partitioning)
      for (const auto& point : points) {
	double min_dist = std::numeric_limits<double>::max();
	size_t best_cell = 0;

	for (size_t i = 0; i < cells.size(); ++i) {
	  double dist = point.distance(cells[i].site);
	  if (dist < min_dist) {
	    min_dist = dist;
	    best_cell = i;
	  }
	}

	// Only assign if within cluster radius of the site
	if (min_dist <= radius) {
	  cells[best_cell].points.push_back(point);
	}
      }

      // Step 3: Update sites to centroids of their Voronoi cells
      bool converged = true;
      for (size_t i = 0; i < cells.size(); ++i) {
	if (!cells[i].points.empty()) {
	  Point new_site = calculate_centroid(cells[i].points);
	  if (cells[i].site.distance(new_site) > 1e-6) {
	    converged = false;
	  }
	  cells[i].site = new_site;
	}
      }

      if (converged) break;

      // Step 4: Remove empty cells and merge nearby sites
      cells.erase(std::remove_if(cells.begin(), cells.end(),
				 [](const voronoi_cell& cell) { return cell.points.empty(); }),
		  cells.end());

      // Merge sites that are too close
      merge_close_sites(cells);
    }

    // Create final clusters by ensuring radius constraint
    return constrain_voronoi_clusters(cells);
  }

private:
  vpoints
  initialize_centroids()
  {
    vpoints centroids;
    if (!points.empty())
      {
	std::vector<bool> selected(points.size(), false);

	// Start with a random point
	std::random_device rd;
	std::mt19937 gen(rd());
	std::uniform_int_distribution<size_t> dist(0, points.size() - 1);

	size_t first_idx = dist(gen);
	centroids.push_back(points[first_idx]);
	selected[first_idx] = true;

	// Add centroids that are at least radius away from existing ones
	for (size_t i = 0; i < points.size(); ++i)
	  {
	    if (selected[i])
	      continue;

	    bool far_enough = true;
	    for (const auto& centroid : centroids)
	      {
		if (points[i].distance(centroid) < radius)
		  {
		    far_enough = false;
		    break;
		  }
	      }

	    if (far_enough)
	      {
		centroids.push_back(points[i]);
		selected[i] = true;
	      }
	  }
      }
    return centroids;
  }

  Point
  calculate_centroid(const vpoints& cluster)
  {
    Point cp(0,0);
    if (!cluster.empty())
      {
	double sum_x = 0, sum_y = 0;
	for (const auto& point : cluster)
	  {
	    sum_x += point.x;
	    sum_y += point.y;
	  }
	cp = Point(sum_x / cluster.size(), sum_y / cluster.size());
      }
    return cp;
  }

  vwpoints
  weigh_cluster(const std::vector<vpoints>& clusters)
  {
    vwpoints result;
    for (const auto& cluster : clusters) {
      if (!cluster.empty()) {
	auto centroid = calculate_centroid(cluster);
	result.emplace_back(centroid.x, centroid.y, cluster.size());
      }
    }
    return result;
  }

  void
  merge_close_sites(std::vector<voronoi_cell>& cells)
  {
    for (size_t i = 0; i < cells.size(); ++i)
      {
	for (size_t j = i + 1; j < cells.size(); )
	  {
	    if (cells[i].site.distance(cells[j].site) < radius * 0.5)
	      {
		// Merge cell j into cell i
		auto& cpoints = cells[i].points;
		cpoints.insert(cpoints.end(),
			       cells[j].points.begin(), cells[j].points.end());
		cells[i].site = calculate_centroid(cpoints);

		// Remove cell j
		cells.erase(cells.begin() + j);
	      }
	    else
	      ++j;
	  }
      }
  }

  vwpoints
  constrain_voronoi_clusters(const std::vector<voronoi_cell>& cells)
  {
    vwpoints result;
    for (const auto& cell : cells)
      {
	if (!cell.points.empty())
	  {
	    // Further split cells that violate radius constraint
	    auto subclusters = split_large_cell(cell);
	    for (const auto& subcluster : subclusters)
	      {
		if (!subcluster.empty())
		  {
		    auto centroid = calculate_centroid(subcluster);
		    result.emplace_back(centroid, subcluster.size());
		  }
	      }
	  }
      }

    return result;
  }

  std::vector<vpoints>
  split_large_cell(const voronoi_cell& cell)
  {
    std::vector<vpoints> subclusters;

    if (cell.points.empty()) return subclusters;

    // Check if cell needs splitting (points too far from centroid)
    std::vector<bool> assigned(cell.points.size(), false);

    for (size_t i = 0; i < cell.points.size(); ++i) {
      if (assigned[i]) continue;

      vpoints current_subcluster;
      std::queue<size_t> to_process;
      to_process.push(i);
      assigned[i] = true;

      while (!to_process.empty()) {
	size_t current_idx = to_process.front();
	to_process.pop();
	current_subcluster.push_back(cell.points[current_idx]);

	// Find all unassigned points within cluster radius
	for (size_t j = 0; j < cell.points.size(); ++j) {
	  if (!assigned[j] &&
	      cell.points[current_idx].distance(cell.points[j]) <= radius) {
	    assigned[j] = true;
	    to_process.push(j);
	  }
	}
      }

      if (!current_subcluster.empty()) {
	subclusters.push_back(std::move(current_subcluster));
      }
    }

    return subclusters;
  }
};

// Main function interface
vwpoints
clusterPoints(const vpoints& points, double radius,
	      const std::string& method)
{
  point_cluster clusterer(points, radius);

  if (method == "grid") {
    return clusterer.reduce_grid();
  } else if (method == "hierarchical") {
    return clusterer.reduce_hierarchical_agglomerative();
  } else if (method == "kmeans") {
    return clusterer.reduce_constrained_k_means();
  } else if (method == "voronoi") {
    return clusterer.reduce_voronoi();
  } else {
    throw std::invalid_argument("Unknown clustering method");
  }
}

// Example usage and test function
void
printClusters(const vwpoints& clusters)
{
  std::cout << "Clusters found: " << clusters.size() << "\n";
  for (size_t i = 0; i < clusters.size(); ++i)
    {
      const auto& wp = clusters[i];
      std::cout << "Cluster " << i + 1 << ": (" << wp.x << ", " << wp.y
		<< ") weight: " << wp.weight << "\n";
    }

  auto lklstr = [](size_t sum, const WeightedPoint& wp)
  { return sum + wp.weight; };

  std::cout << "Total points represented: "
	    << std::accumulate(clusters.begin(), clusters.end(), 0ULL, lklstr)
	    << "\n";
}

#endif
