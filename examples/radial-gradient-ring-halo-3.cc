#include <iostream>
#include "a60-svg.h"

using namespace std;
using namespace svg;

const uint rectw = 100;

// https://developer.mozilla.org/en-US/docs/Web/SVG/Element/radialGradient
// https://www.digitalocean.com/community/tutorials/svg-radial-gradients
// https://www.w3.org/TR/SVG11/pservers.html
// http://www.svgbasics.com/radial_gradients.html
void
test_radial_gradient(svg_element& obj)
{
  point_2t cp = obj.center_point();

  auto [ xd, yd ] = cp;
  const ssize_type x(xd);
  const ssize_type y(yd);
  point_to_ring_halo(obj, make_tuple(x, y + rectw), 60, 0.66, color::blue);
  point_to_ring_halo(obj, make_tuple(x, y - rectw), 60, 0.66, color::blue);

  point_to_ring_halo(obj, make_tuple(x + rectw, y), 80, 0.8, color::gray50);
  point_to_ring_halo(obj, make_tuple(x - rectw, y), 80, 0.8, color::gray50);

  point_to_ring_halo(obj, cp, 30, 0.15, color::crimson);
}


int main()
{
  area<> a = k::letter_096_v;
  svg_element obj("radial-gradient-ring-halo-3", a);

  // Add background so that checking transparency is easier with naked eye.
  auto r = make_rect({ a._M_width / 2 - rectw / 2, 0 }, k::b_style,
		     {rectw, a._M_height});
  obj.add_element(r);

  test_radial_gradient(obj);
  return 0;
}
