#include <iostream>
#include "a60-svg.h"

using namespace std;
using namespace svg;


color bkgklr = color::wcag_lgray;
const style bkgstyl = { bkgklr, 1.0, color::white, 0.0, 0 };


void
test_halo(svg_element& obj)
{
  // Add background.
  auto r = make_rect({ 0, 0 }, bkgstyl,
		     {obj._M_area._M_width, obj._M_area._M_height});
  obj.add_element(r);  

  point_2t cp = obj.center_point();
  const ssize_type radius = obj._M_area._M_width / 3;
  const double blurspace = radius * 0.5;
  point_to_ring_halo(obj, cp, radius, blurspace, color::white, bkgklr);
}


int main()
{
  area<> a = k::letter_096_v;
  svg_element obj("radial-gradient-ring-halo-5", a);

  test_halo(obj);
  return 0;
}
