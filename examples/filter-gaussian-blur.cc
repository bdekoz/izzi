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

  // 1
  rect_element r1;
  rect_element::data drb1 = { x, y, width, height };
  r1.start_element();
  r1.add_data(drb1);
  r1.add_filter("20y");
  r1.add_style(k::b_style);
  r1.finish_element();
  obj.add_element(r1);

  // 2
  rect_element r2;
  rect_element::data drb2 = { x, y + offset, width, height };
  r2.start_element();
  r2.add_data(drb2);
  r2.add_filter("10y");
  r2.add_style(k::b_style);
  r2.finish_element();
  obj.add_element(r2);
}


int main()
{
  test_gblur("test-gblur-1");
  return 0;
}
