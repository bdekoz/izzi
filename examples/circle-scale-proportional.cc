#include "a60-svg.h"

void
test_proportional(std::string ofile)
{
  using namespace std;
  using namespace svg;

  area<> a = k::letter_096_v;
  svg_element obj(ofile, a);
  point_2t cp = obj.center_point();
  auto [ x, y ] = cp;

  const auto len = 10;
  const auto space = 150;

  // Black
  const style styl1 = { color::wcag_lgray, 1.0, color::black, 0.0, 2 };
  const style styl2 = { color::wcag_gray, 1.0, color::black, 0.0, 2 };
  const style styl3 = { color::wcag_dgray, 1.0, color::black, 0.0, 2 };

  // loki
  auto r1 = scale_proportional_to_area(len, 1.68);
  circle_element c1;
  circle_element::data dc1 = { x - space, y, r1 };
  c1.start_element();
  c1.add_data(dc1);
  c1.add_style(styl1);
  c1.finish_element();
  obj.add_element(c1);

  // barbie
  auto r2 = scale_proportional_to_area(len, 4.18);
  circle_element c2;
  circle_element::data dc2 = { x, y, r2 };
  c2.start_element();
  c2.add_data(dc2);
  c2.add_style(styl2);
  c2.finish_element();
  obj.add_element(c2);

  // ddos
  auto r3 = scale_proportional_to_area(len, 116.79);
  circle_element c3;
  circle_element::data dc3 = { x + space, y, r3 };
  c3.start_element();
  c3.add_data(dc3);
  c3.add_style(styl3);
  c3.finish_element();
  obj.add_element(c3);
}


int main()
{
  test_proportional("circle-scale-proportional");
  return 0;
}
