#include "a60-svg.h"

void
test_title_desc(std::string ofile)
{
  using namespace std;
  using namespace svg;
  using atype = area<>::atype;

  area<> a = k::a4_096_v;
  const string desc = "Test file demonstrating title and description for AT-enabled setups. It contains a while background with one black circle in the center left.";
  svg_element obj(ofile, desc, a);
  point_2t cp = obj.center_point();
  auto [ x, y ] = cp;

  const auto radius = 50;

  // circle 1
  atype x1 = 2 * radius;
  circle_element c1;
  circle_element::data dc1 = { x1, y, radius};
  c1.start_element();
  c1.add_data(dc1);
  c1.add_style(k::b_style);
  c1.finish_element();
  obj.add_element(c1);
}


int main()
{
  test_title_desc("svg-title-desc-test");
  return 0;
}
