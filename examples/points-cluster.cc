#include <iostream>
#include "a60-svg.h"
#include "izzi-points-cluster.h"

using namespace std;
using namespace svg;



void
test()
{
  // Sample data with clear clusters
  std::vector<Point> points = {
    {1.0, 1.0}, {1.1, 1.1}, {1.2, 0.9}, {0.9, 1.2},
    {5.0, 5.0}, {5.1, 5.2}, {5.3, 4.9}, {4.9, 5.1},
    {10.0, 10.0}, {10.2, 10.1}, {9.9, 10.2},
    {1.5, 1.6},  // Should form its own cluster or join first one
    {15.0, 15.0}, {15.5, 15.5}  // Isolated cluster
  };

  double radius = 0.5;

  std::cout << "Original points: " << points.size() << "\n\n";

  // Test different algorithms
  auto grid_clusters = cluster_points_by(points, radius, "grid");
  std::cout << "Grid-based clustering:\n";
  print_clusters(grid_clusters);

  auto hierarchical_clusters = cluster_points_by(points, radius, "hierarchical");
  std::cout << "Hierarchical clustering:\n";
  print_clusters(hierarchical_clusters);

  auto kmeans_clusters = cluster_points_by(points, radius, "kmeans");
  std::cout << "Constrained K-Means clustering:\n";
  print_clusters(kmeans_clusters);

  auto voronoi_clusters = cluster_points_by(points, radius, "voronoi");
  std::cout << "Voronoi-based clustering:\n";
  print_clusters(voronoi_clusters);
}


int main()
{
  test();
  return 0;
}
