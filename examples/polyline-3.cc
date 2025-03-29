#include "a60-svg.h"

// static local
namespace {

const svg::area<> a = { 1920, 1080 };
svg::svg_element obj("polyline-3", a);
svg::point_2t cp = obj.center_point();

const auto spacer = 50;
const auto linelen = 150;

using svg::color::red;
using svg::color::wcag_lgray;
using svg::color::wcag_dgray;
const svg::style style_r = { red, 1.0, red, 0.0, 0.5 };
const svg::style style_wcaglg = { wcag_lgray, 1.0, wcag_lgray, 0.0, 0.5 };
const svg::style style_wcagdg = { wcag_dgray, 1.0, wcag_dgray, 0.0, 0.5 };

}

void
make_marker_circle(const std::string id,
		   const svg::area<> ma, const svg::point_2t mcp,
		   const uint radius, const svg::style styl)
{
  svg::marker_element mkr;
  mkr.start_element(id, ma, mcp);

  svg::circle_element c = make_circle(mcp, styl, radius);
  mkr.add_raw(c.str());
  mkr.finish_element();
  obj.add_element(mkr);
}

void
make_markers()
{
  make_marker_circle("c4red", {4, 4}, {2, 2}, 2, style_r);
  make_marker_circle("c4wcaglg", {4, 4}, {2, 2}, 2, style_wcaglg);
  make_marker_circle("c4wcagdg", {4, 4}, {2, 2}, 2, style_wcagdg);
  make_marker_circle("c4black", {4, 4}, {2, 2}, 2, svg::k::b_style);    
};

svg::vrange
make_points_at(const svg::point_2t p)
{
  using svg::point_2t;
  
  auto [ x, y ] = p;  
  const point_2t p0 { x + spacer * 0, y };
  const point_2t p1 { x + spacer * 1, y - linelen };
  const point_2t p2 { x + spacer * 2, y + linelen};
  const point_2t p3 { x + spacer * 3, y };
  const point_2t p4 { x + spacer * 4, y + spacer };
  const point_2t p5 { x + spacer * 5, y };
  const point_2t p6 { x + spacer * 6, y + spacer };
  const point_2t p7 { x + spacer * 7, y };
  const point_2t p8 { x + spacer * 8, y + 2 * spacer };
  const point_2t p9 { x + spacer * 9, y - spacer };    
  svg::vrange points { p0, p1, p2, p3, p4, p5, p6, p7, p8, p9 };
  return points;
}

void
test_polyline()
{
  using namespace std;
  using namespace svg;

  // WCAG Black/White/Gray
  const style styl1 = { color::wcag_lgray, 0.0, color::wcag_lgray, 1.0, 4 };
  const style styl2 = { color::wcag_gray, 0.0, color::wcag_gray, 1.0, 4 };
  const style styl3 = { color::wcag_dgray, 0.0, color::wcag_dgray, 1.0, 4 };

  // Line 1
  auto xo = 0;
  auto yo = linelen;

  vrange points1 = make_points_at({xo, yo});
  polyline_element pl1 = make_polyline(points1, styl1, "2", "c4wcaglg");
  obj.add_element(pl1);

  vrange points2 = make_points_at({xo, yo + linelen});
  polyline_element pl2 = make_polyline(points2, styl2, "1", "c4red");
  obj.add_element(pl2);

  vrange points3 = make_points_at({xo, yo + 2 * linelen});
  polyline_element pl3 = make_polyline(points3, styl3, "5", "c4wcagdg");
  obj.add_element(pl3);
}


int main()
{
  make_markers();
  test_polyline();
  return 0;
}
