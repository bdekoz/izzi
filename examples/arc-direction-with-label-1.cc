#include "a60-svg-radial.h"

void
test_arc(std::string ofile)
{
  using namespace std;
  using namespace svg;

  area<> a = k::letter_096_v;
  svg_element obj(ofile, a);
  style s = { color::white, 0.0, color::black, 1.0, 2 };

  typography typoleft = k::apercu_typo;
  typoleft._M_size = 10;
  typoleft._M_a = typography::anchor::start;
  typoleft._M_align = typography::align::left;

  typography typoright(typoleft);
  typoright._M_a = typography::anchor::end;
  typoright._M_align = typography::align::right;

  point_2t cp = obj.center_point();
  const int r = 120;

  direction_arc_at(obj, cp, r, k::b_style);
  direction_arc_title_at(obj, cp, r, s, "web vitals 2020");
}


int main()
{
  test_arc("test-arc-direction-with-label-1");
  return 0;
}
