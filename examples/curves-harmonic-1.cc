
#include "a60-svg-curves-damped-harmonograph.h"


int main() {
  
  point_2t origin = {400, 400};
  double radius = 300.0;
  
  // Example 1: The Damped Octave
  std::string octavePath = generate_damped_harmonograph(origin, radius, 2.0, 0.02, 12.0);
  
  // Example 2: The Damped Perfect Fifth
  std::string fifthPath = generate_damped_harmonograph({800, 400}, radius, 1.5, 0.015, 20.0);
  
  std::cout << "\n<path d=\"" << octavePath << "\" stroke=\"blue\" fill=\"none\" />\n";
  std::cout << "\n<path d=\"" << fifthPath << "\" stroke=\"red\" fill=\"none\" />\n";
  
  return 0;
}
