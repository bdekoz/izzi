#include <iostream>
#include "a60-svg.h"

using namespace std;
using namespace svg;

void
test_radial_gradient(svg_element& obj)
{
  // Add background.
  point_2d_to_rect(obj, 0, 0, k::b_style,
		   obj._M_area._M_width, obj._M_area._M_height);

  point_2t cp = obj.center_point();
  const size_type radius = obj._M_area._M_width / 3;
  const double blurspace = radius * 0.5;
  point_to_ring_halo(obj, cp, radius, blurspace, color::blue, color::black);
}


int main()
{
  area<> a = k::letter_096_v;
  svg_element obj("radial-gradient-ring-halo-4", a);

  test_radial_gradient(obj);
  return 0;
}
