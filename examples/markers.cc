#include "a60-svg.h"
#include "a60-svg-graphs-line.h"

// static local
namespace {
const svg::area<> a = { 1920, 1080 };
svg::svg_element obj("markers", a);
svg::point_2t cp = obj.center_point();
double cx = std::get<0>(cp);
const auto spacer = 100;
const auto linelen = 150;
}


void
test_markers_1(const double x, const svg::style styl)
{
  using namespace std;
  using namespace svg;

  const double radius = 30;

  // 1
  double yp = spacer;
  point_2t p1 = { x, yp };
  string m1 = make_marker_instance(marker_shape::circle, p1, styl, radius);
  obj.add_raw(m1);

  // 2
  yp += spacer;
  point_2t p2 = { x, yp };
  string m2 = make_marker_instance(marker_shape::triangle, p2, styl, radius);
  obj.add_raw(m2);

  // 3
  yp += spacer;
  point_2t p3 = { x, yp };
  string m3 = make_marker_instance(marker_shape::square, p3, styl, radius);
  obj.add_raw(m3);

  // 4
  yp += spacer;
  point_2t p4 = { x, yp };
  string m4 = make_marker_instance(marker_shape::hexagon, p4, styl, radius);
  obj.add_raw(m4);

  // 5
  yp += spacer;
  point_2t p5 = { x, yp };
  string m5 = make_marker_instance(marker_shape::sunburst, p5, styl, radius);
  obj.add_raw(m5);

  // 6
  yp += spacer;
  point_2t p6 = { x, yp };
  string m6 = make_marker_instance(marker_shape::x, p6, styl, radius);
  obj.add_raw(m6);

  // 7
  yp += spacer;
  point_2t p7 = { x, yp };
  string m7 = make_marker_instance(marker_shape::blob, p7, styl, radius);
  obj.add_raw(m7);

  // 8
  yp += spacer;
  point_2t p8 = { x, yp };
  string m8 = make_marker_instance(marker_shape::wave, p8, styl, radius);
  obj.add_raw(m8);
}



int main()
{
  using namespace svg;

  const style stylo = { color::wcag_lgray, 0.0, color::wcag_lgray, 1.0, 2 };
  test_markers_1(cx, stylo);

  const style stylf = { color::wcag_lgray, 1.0, color::wcag_lgray, 0.0, 4 };
  test_markers_1(cx + spacer, stylf);

  return 0;
}
