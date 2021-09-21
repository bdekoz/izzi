#include "a60-svg.h"

void
test_gblur(std::string ofile)
{
  using namespace std;
  using namespace svg;
  using atype = svg_element::atype;

  area<> a = k::letter_096_v;
  svg_element obj(ofile, a);

  point_2t cp = obj.center_point();
  auto [ xd, yd ] = cp;

  auto height = 100;
  auto width = 100;

  // Center of rectangle
  const atype x(xd - double(width / 2));
  const atype y(yd - double(height / 2));

  const string filter_name("gblur5zero");
  const atype blur_size = 10;
  const area<> blur_area = { width + 2 * blur_size, height + 2 * blur_size };
  const point_2t blur_origin = { -blur_size, -blur_size };

  filter_element f;
  f.start_element(filter_name, blur_area, blur_origin);
  f.add_data(f.gaussian_blur(std::to_string(blur_size)));
  f.finish_element();
  obj.add_element(f);

  // rect
  rect_element r1;
  rect_element::data drb1 = { x, y, width, height };
  r1.start_element();
  r1.add_data(drb1);
  r1.add_style(k::b_style);
  r1.add_filter(filter_name);
  r1.finish_element();
  obj.add_element(r1);

  rect_element r2;
  rect_element::data drb2 = { x, y + (height * 2), width, height };
  r2.start_element();
  r2.add_data(drb2);
  r2.add_style(k::b_style);
  r2.finish_element();
  obj.add_element(r2);
}


int main()
{
  test_gblur("filter-gaussian-blur-2");
  return 0;
}
