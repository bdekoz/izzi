#include <iostream>
#include <vector>
#include <string>
#include <format>
#include <fstream>
#include <cmath>
#include <numbers>
#include <tuple>

// [The generateDampedHarmonograph function from previous steps goes here]
#include "a60-svg-curves-damped-harmonograph.h"

using point_2t = std::tuple<double, double>;



int main()
{
  std::ofstream file("parameter_explorer.svg");
  file << "<svg viewBox='0 0 1000 1000' xmlns='http://www.w3.org/2000/svg' style='background:#050505'>\n";
  
  // Setup a 3x3 grid
  // Rows: Different Intervals (Unison, Fifth, Octave)
  // Cols: Different Microtonal Drifts (+0.0, +0.02, +0.1)
  
  double intervals[] = { 1.0, 1.5, 2.0 };
  double drifts[]    = { 0.0, 0.02, 0.1 };
  std::string colors[] = { "#00ffcc", "#ffcc00", "#ff0066" };
  
  for (int row = 0; row < 3; ++row)
    {
      for (int col = 0; col < 3; ++col)
	{
	  double base_n = intervals[row];
	  double drift  = drifts[col];
	  double final_n = base_n + drift;
          
	  double x = 200 + col * 300;
	  double y = 200 + row * 300;
          
	  // Adjust damping: less damping for more "drift" visualization
	  double damping = (drift == 0.0) ? 0.05 : 0.015;
	  double cycles  = (drift == 0.0) ? 10.0 : 40.0;
	  
	  file << std::format("  <text x='{}' y='{}' fill='white' font-family='sans-serif' font-size='14' text-anchor='middle'>n: {:.2f}</text>\n", 
			      x, y - 160, final_n);
	  
	  file << "  <path d='" << generate_damped_harmonograph({x, y}, 120, final_n, damping, cycles) 
	       << "' fill='none' stroke='" << colors[row] << "' stroke-width='0.5' stroke-opacity='0.7' />\n";
        }
    }
  
  file << "</svg>";
  file.close();
  return 0;
}
