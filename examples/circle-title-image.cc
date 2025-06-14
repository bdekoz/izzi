#include "a60-svg.h"

using namespace svg;

void
test_title(svg_element& obj)
{
  using namespace std;
  using namespace svg;

  point_2t cp = obj.center_point();
  auto [ x, y ] = cp;
  double r = 10;

  // Black
  //const style styl1 = { color::wcag_lgray, 1.0, color::black, 0.0, 2 };
  const style styl1 = { color::wcag_lgray, 1.0, color::black, 1.0, .5 };

  // 1 : left text tooltip
  circle_element c;
  circle_element::data dc = { x - 2 * r, y, r };
  c.start_element();
  c.add_data(dc);
  c.add_style(styl1);
  c.add_raw(element_base::finish_hard);

  link_element l;
  l.start_element();
  l.add_data("https://www.sfgate.com/");
  text_element t = style_text("SF", { x - r, y}, k::hyperl_typo);
  l.add_element(t);
  l.finish_element();
  c.add_element(l);

  c.add_title("uss callister into infinity");
  c.add_raw(circle_element::pair_finish_tag);
  obj.add_element(c);

  // 2 : right image tooltip
  circle_element c2;
  circle_element::data dc2 = { x + 2 * r, y, r };
  c2.start_element();
  c2.add_data(dc2);
  c2.add_style(styl1);
  c2.add_raw(element_base::finish_hard);

  link_element l2;
  l2.start_element();
  l2.add_data("../docs/image/floor-angle-shadow-blur.jpg", "preload", "image");
  c2.add_element(l2);

  c2.add_title("uss enterprise");
  c2.add_raw(circle_element::pair_finish_tag);
  obj.add_element(c2);
}


int main()
{
  area<> a = k::a5_096_v;
  svg_element obj("circle-title-image", a);

  test_title(obj);
  return 0;
}
