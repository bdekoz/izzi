#include "a60-svg-radial.h"

void
test_arc(std::string ofile)
{
  using namespace std;
  using namespace svg;

  area<> a = k::letter_096_v;
  svg_element obj(ofile, a);

  style sarc = { color::black, 1.0, color::black, 0.0, 2 };

  point_2t cp = obj.center_point();
  const int r = 120;

  set_radial_range(90, 180);
  direction_arc_at(obj, cp, r, sarc);
}


int main()
{
  test_arc("direction-arc-3");
  return 0;
}
