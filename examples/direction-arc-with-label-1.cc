#include "a60-svg-radial.h"

void
test_arc(std::string ofile)
{
  using namespace std;
  using namespace svg;

  area<> a = k::letter_096_v;
  svg_element obj(ofile, a);
  style stext = { color::white, 0.0, color::black, 1.0, 2 };
  style sarc = { color::black, 1.0, color::black, 0.0, 2 };

  point_2t cp = obj.center_point();
  const int r = 120;

  direction_arc_at(obj, cp, r, sarc);
  direction_arc_title_at(obj, cp, r + 20, stext, "web vitals 2020");
}


int main()
{
  test_arc("direction-arc-with-label-1");
  return 0;
}
