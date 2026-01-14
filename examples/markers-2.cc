#include "a60-svg.h"
#include "a60-svg-graphs-line.h"

// static local
namespace {
const svg::area<> a = { 1920, 1080 };
svg::svg_element obj("markers-2", a);
svg::point_2t cp = obj.center_point();
const auto [ cx, cy ] = cp;

const auto spacer = 150;
}


void
test_markers(const double x, const double y, const svg::style styl)
{
  using namespace std;
  using namespace svg;

  // Center dots.
  circle_element c1 = make_circle({cx, cy + 5}, styl, 20);
  circle_element c2 = make_circle({cx, cy - 5}, styl, 20);
  obj.add_element(c1);
  obj.add_element(c2);

  double yp(y);
  const double radius = 50;

  // 5
  yp += spacer;

  point_2t p10 = { x, yp };
  auto m10 = make_sunburst(p10, styl, radius, 1);
  obj.add_element(m10);

  point_2t p11 = { x - spacer, yp };
  auto m1 = make_sunburst(p11, styl, radius, 10);
  obj.add_element(m1);

  point_2t p12 = { x + spacer, yp };
  auto m2 = make_sunburst(p12, styl, radius, 20);
  obj.add_element(m2);

  // 9
  yp += spacer;
  point_2t p9 = { x, yp };
  path_element l2 = make_lauburu(p9, styl, radius);
  obj.add_element(l2);

  // 1
  yp += spacer;
  point_2t p3 = { x, yp };
  path_element l3 = make_lauburu(p3, styl, radius, 0.1, 1);
  obj.add_element(l3);

  point_2t p32 = { x - spacer, yp };
  path_element l32 = make_lauburu(p32, styl, radius, 0.8, 1);
  obj.add_element(l32);

  point_2t p33 = { x + spacer, yp };
  path_element l33 = make_lauburu(p33, styl, radius, 1.5, 1);
  obj.add_element(l33);


  // 3
  yp += spacer;
  point_2t p5 = { x, yp };
  path_element l5 = make_lauburu(p5, styl, radius, 1.0, 2);
  obj.add_element(l5);

  point_2t p52 = { x - spacer, yp };
  path_element l52 = make_lauburu(p52, styl, radius, 1.5, 3);
  obj.add_element(l52);

  point_2t p53 = { x + spacer, yp };
  path_element l53 = make_lauburu(p53, styl, radius, 2.0, 6);
  obj.add_element(l53);

  point_2t p54 = { x + spacer * 2, yp };
  path_element l54 = make_lauburu(p54, styl, radius / 3, 4.0, 6);  // 4, 6
  obj.add_element(l54);


  // 4
  yp += spacer;
  point_2t p4 = { x, yp };
  path_element l4 = make_lauburu(p4, styl, radius, 0.5, 4);
  obj.add_element(l4);


  // 10
  yp += spacer;
  point_2t p6 = { x, yp };
  path_element l6 = make_lauburu(p6, styl, radius, 1.8, 8);
  obj.add_element(l6);

}


int main()
{
  using namespace svg;

  const style stylo = { color::wcag_lgray, 0.0, color::wcag_lgray, 1.0, 2 };
  test_markers(cx * .5, 50, stylo);

  const style stylf = { color::wcag_lgray, 1.0, color::wcag_lgray, 0.0, 4 };
  test_markers(cx * 1.5, 50, stylf);

  return 0;
}
