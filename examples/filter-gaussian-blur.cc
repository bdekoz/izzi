#include "a60-svg.h"

void
test_gblur(std::string ofile)
{
  using namespace std;
  using namespace svg;

  const auto offset = 100;

  area<> a = k::letter_096_v;
  svg_element obj(ofile, a);

  point_2t cp = obj.center_point();
  auto [ xd, yd ] = cp;
  const size_type x(xd);
  const size_type y(yd);

  auto height = 25;
  auto width = 50;
  auto radius = std::max( height, width);

  // using builtin filters gblur10, gblur20, gblur10y, gblur20y
  filter_element fdefault;

  // 1 rect
  rect_element r1;
  rect_element::data drb1 = { x - width /2, y, width, height };
  r1.start_element();
  r1.add_data(drb1);
  r1.add_filter("gblur20y");
  r1.add_style(k::b_style);
  r1.finish_element();
  obj.add_element(r1);

  // 2 rect
  rect_element r2;
  rect_element::data drb2 = { x - width / 2, y + offset, width, height };
  r2.start_element();
  r2.add_data(drb2);
  r2.add_filter("gblur10y");
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

  // 4 named filter + circle
  const string filter_name("gblur5zero");
  const int blur_size = 20;
  const area<> blur_area = { radius + 2 * blur_size, radius + 2 * blur_size };
  const point_2t blur_origin = { -blur_size, -blur_size };

  filter_element f;
  f.start_element(filter_name, blur_area, blur_origin);
  f.add_data(f.gaussian_blur(std::to_string(blur_size)));
  f.finish_element();
  obj.add_element(f);

  circle_element c2;
  circle_element::data dc2 = { x - 2 * offset, y, radius };
  c2.start_element();
  c2.add_data(dc2);
  c2.add_style(k::b_style);
  c2.add_filter(filter_name);
  c2.finish_element();
  obj.add_element(c2);
}


int main()
{
  test_gblur("test-gblur-1");
  return 0;
}
