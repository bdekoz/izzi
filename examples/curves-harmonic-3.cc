#include "a60-svg.h"
#include "a60-svg-curves-damped-harmonograph.h"


int main()
{
  std::ofstream file("triple_explorer.svg");
  file << "<svg viewBox='0 0 1200 800' xmlns='http://www.w3.org/2000/svg' style='background:#000'>\n";

  struct Config { std::string label; double n1; double n2; std::string color; };

  std::vector<Config> configs = {
    {"Pure Triad (1:2:3)", 2.0, 3.0, "#00d4ff"},
    {"Major Third Interference (1:1.25:1.5)", 1.25, 1.5, "#00ff88"},
    {"Microtonal Cluster (1:2.01:3.02)", 2.01, 3.02, "#ff00ff"}
    };

  for (ulong i = 0; i < configs.size(); ++i)
    {
      double x = 200.0 + i * 400.0;

      // Row 1: Low Damping (Structure)
      file << std::format("<text x='{}' y='60' fill='white' text-anchor='middle'>{}</text>", x, configs[i].label);
      file << "<path d='" << generate_triple_harmonograph({x, 250}, 150, configs[i].n1, configs[i].n2, 0, 0.01, 30)
	   << "' fill='none' stroke='" << configs[i].color << "' stroke-width='0.4' />";

      // Row 2: High Damping (Spiral collapse)
      file << "<path d='" << generate_triple_harmonograph({x, 600}, 150, configs[i].n1, configs[i].n2, 1.2, 0.04, 15)
	   << "' fill='none' stroke='" << configs[i].color << "' stroke-width='0.6' stroke-opacity='0.5' />";
    }

  file << "</svg>";
  return 0;
}
