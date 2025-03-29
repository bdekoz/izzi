#include "a60-svg.h"

// static local
namespace {
const svg::area<> a = { 1920, 1080 };
svg::svg_element obj("polyline-2", a);
svg::point_2t cp = obj.center_point();
auto [ x, y ] = cp;
const auto spacer = 50;
const auto linelen = 150;
}

void
make_marker_circle(const std::string id,
		   const svg::area<> ma, const svg::point_2t mcp, uint radius)
{
  svg::marker_element mkr;
  mkr.start_element(id, ma, mcp);

  using svg::color::red;
  const svg::style r_style = { red, 1.0, red, 0.0, 0.5 };

  svg::circle_element c = make_circle(mcp, r_style, radius);
  mkr.add_raw(c.str());
  mkr.finish_element();
  obj.add_element(mkr);
}

void
make_markers()
{
  make_marker_circle("c4", { 4, 4}, { 2, 2}, 2);
  make_marker_circle("c8", { 8, 8}, { 4, 4}, 4);
};

void
test_polyline_1()
{
  using namespace std;
  using namespace svg;

  // WCAG Black/White/Gray
  const style styl1 = { color::wcag_lgray, 0.0, color::wcag_lgray, 1.0, 4 };
  //const style styl2 = { color::wcag_gray, 0.0, color::wcag_gray, 1.0, 4 };
  //const style styl3 = { color::wcag_dgray, 0.0, color::wcag_dgray, 1.0, 4 };

  // Line 1
  const point_2t p1 { x + spacer * 0, y };
  const point_2t p2 { x + spacer * 1, y - linelen };
  const point_2t p3 { x + spacer * 2, y + linelen};
  const point_2t p4 { x + spacer * 3, y };
  const point_2t p5 { x + spacer * 4, y + spacer };
  const point_2t p6 { x + spacer * 5, y };
  const point_2t p7 { x + spacer * 6, y + spacer };
  const point_2t p8 { x + spacer * 7, y };
  vrange points { p1, p2, p3, p4, p5, p6, p7, p8 };
  polyline_element pl = make_polyline(points, styl1, "", "c8");
  obj.add_element(pl);
}

void
test_polyline_2()
{
  using namespace std;
  using namespace svg;

  // WCAG Black/White/Gray
  //const style styl1 = { color::wcag_lgray, 0.0, color::wcag_lgray, 1.0, 4 };
  //const style styl2 = { color::wcag_gray, 0.0, color::wcag_gray, 1.0, 4 };
  const style styl3 = { color::wcag_dgray, 0.0, color::wcag_dgray, 1.0, 4 };

  // Line 1
  x -= 2 * linelen;
  y += linelen;
  const point_2t p1 { x + spacer * 0, y };
  const point_2t p2 { x + spacer * 1, y - linelen };
  const point_2t p3 { x + spacer * 2, y + linelen};
  const point_2t p4 { x + spacer * 3, y };
  const point_2t p5 { x + spacer * 4, y + spacer };
  const point_2t p6 { x + spacer * 5, y };
  const point_2t p7 { x + spacer * 6, y + spacer };
  const point_2t p8 { x + spacer * 7, y };
  vrange points { p1, p2, p3, p4, p5, p6, p7, p8 };
  polyline_element pl = make_polyline(points, styl3, "5", "c4");
  obj.add_element(pl);
}


int main()
{
  make_markers();
  test_polyline_1();
  test_polyline_2();
  return 0;
}
