// Example of a Microtonal "Flat Octave"
// n = 2.01 (An octave plus a tiny offset)
// d = 0.005 (Very light damping to allow for many rotations)
// cycles = 40 (Many rotations to see the phase shift)

#include "a60-svg-curves-damped-harmonograph.h"
#include <iostream>


void test02()
{
  point_2t origin = {500, 500};
  std::string microtonalPath = generate_damped_harmonograph(origin, 400.0, 2.01, 0.005, 40.0);
  
  std::cout << "\n";
  std::cout << std::format("<path d='{}' fill='none' stroke='purple' stroke-width='0.5' />", microtonalPath);
}

int main()
{
  test02();
  return 0;
}

