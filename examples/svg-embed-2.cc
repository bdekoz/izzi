#include <iostream>
#include "a60-svg.h"

using namespace std;
using namespace svg;


color bkgklr = color::wcag_lgray;
const style bkgstyl = { bkgklr, 1.0, color::white, 0.0, 0 };


void
test_halo_insert(svg_element& obj)
{
  // Add background.
  point_to_rect(obj, { 0, 0 }, bkgstyl,
		obj._M_area._M_width, obj._M_area._M_height);

  const point_2t cp = obj.center_point();

  area<>::atype len = 100;
  area<> destarea = { len, len };
  svg_element nobj("nested-halo", destarea, false);
  nobj.start_element(cp, destarea);

  point_2t ncp = nobj.center_point();
  const ssize_type radius = len / 3;
  const double blurspace = radius * 0.5;
  point_to_ring_halo(nobj, ncp, radius, blurspace, color::white, bkgklr);

  nobj.finish_element();

  obj.add_element(nobj);
}


int main()
{
  area<> a = k::letter_096_v;
  svg_element obj("svg-embed-2", a);

  test_halo_insert(obj);
  return 0;
}
