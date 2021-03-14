#include "a60-svg.h"

using namespace std;
using namespace svg;

const style s = { color::gray50, 1.0, color::black, 1.0, 2 };
const auto r = 120;

void
test_path_demi_circle(svg::svg_element& obj)
{
  const point_2t origin = obj.center_point();
  auto [ x, y ] = origin;

  // Top.
  const point_2t topo = { x, y - (2 * r) };
  path_element pdct(true);
  string dcpatht = make_path_arc_closed(topo, 90, 270, r);
  path_element::data pdcdatat = { dcpatht, 0 };
  pdct.start_element("demi circle top");
  pdct.add_data(pdcdatat);
  pdct.add_style(s);
  pdct.finish_element();
  obj.add_element(pdct);

  // Bottom.
  const point_2t bottomo = { x, y + (2 * r) };
  path_element pdcb(true);
  string dcpathb = make_path_arc_closed(bottomo, zero_angle_north_cw(270),
					zero_angle_north_cw(90), r);
  path_element::data pdcdatab = { dcpathb, 0 };
  pdcb.start_element("demi circle bottom");
  pdcb.add_data(pdcdatab);
  pdcb.add_style(s);
  pdcb.finish_element();
  obj.add_element(pdcb);  
}

void
test_center_mark(svg::svg_element& obj)
{
  const point_2t origin = obj.center_point();
  auto [ x, y ] = origin;

  //sized_text(obj, k::apercu_typo, 2 * r, "+", x, y);

  const auto width = 20;
  
  path_element pth(true);
  string pstr = make_path_center_mark(origin, r, width);
  path_element::data pthdata = { pstr, 0 };
  pth.start_element("centermark out");
  pth.add_data(pthdata); 
  pth.add_style(s);
  pth.finish_element();
  obj.add_element(pth);  
  
  style cstyl = k::w_style;
  cstyl._M_stroke_size = 2;
  cstyl._M_stroke_opacity = 1.0;  
  point_2d_to_circle(obj, x, y, cstyl, 4);
}

int main()
{
  area<> a = k::letter_096_v;
  svg_element obj("path-demi-circle-center-mark", a);

  test_path_demi_circle(obj);
  test_center_mark(obj);  
  return 0;
}
