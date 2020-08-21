#include "a60-svg.h"

void
test_gblur(std::string ofile)
{
  using namespace std;
  using namespace svg;

  const auto offset = 100;

  area<> a = k::letter_096_v;
  svg_element obj(ofile, a);
  style s = { color::white, 0.0, color::black, 1.0, 3 };

  point_2t cp = obj.center_point();
  auto [ xd, yd ] = cp;
  const size_type x(xd);
  const size_type y(yd);

  auto height = 25;
  auto width = 50;

  // 1 rect
  rect_element r1;
  rect_element::data drb1 = { x + width /2, y, width, height };
  r1.start_element();
  r1.add_data(drb1);
  r1.add_filter("20y");
  r1.add_style(k::b_style);
  r1.finish_element();
  obj.add_element(r1);

  // 2 rect
  rect_element r2;
  rect_element::data drb2 = { x + width / 2, y + offset, width, height };
  r2.start_element();
  r2.add_data(drb2);
  r2.add_filter("10y");
  r2.add_style(k::b_style);
  r2.finish_element();
  obj.add_element(r2);

  // 3 circle
  circle_element c1;
  circle_element::data dc1 = { x + 2 * offset, y, width };
  c1.start_element();
  c1.add_data(dc1);
  c1.add_style(k::b_style);
  c1.finish_element();
  obj.add_element(c1);

  // 4 circle
  circle_element c2;
  circle_element::data dc2 = { x - 2 * offset, y, width };
  c2.start_element();
  c2.add_data(dc2);
  c2.add_style(k::b_style);
  c2.add_filter("10");
  c2.finish_element();
  obj.add_element(c2);


}


int main()
{
  test_gblur("test-gblur-1");
  return 0;
}
