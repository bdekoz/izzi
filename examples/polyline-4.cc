#include "a60-svg.h"
#include "a60-svg-graphs-line.h"
#include "izzi-json-basics.h"

// static local
namespace {

const svg::area<> a = { 1920, 1080 };
svg::svg_element obj("polyline-4", a);
svg::point_2t cp = obj.center_point();

const auto spacer = 50;
const auto linelen = 150;

}


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

  // Line Style.
  const uint lsize = 2;
  const style lstyl = { color::none, 0.0, color::black, 1.0, lsize };

  // Stroke Style
  // NB: dasharrays and offsets must take into account the width of
  // the line. A 2 point line with a 1 point dasharray won't be seen.

  // { markerspoints, dasharray, dashoffset, linecap, path_length }
  // dasharray == 3 (3 stroke dash and 3 stroke space)
  //              2, 1 (2 stroke dash and 1 stroke space)
  // dashoffset == -1, 1, 3, etc. Offset of dashes from start/end points.
  // linecap == butt, round, square

  // Line 1
  auto xo = linelen;
  auto yo = linelen;

  vrange points5 = make_points_at({xo, yo + 3 * linelen});
  const stroke_style sstyl5 = {"", 0, "", "", "", ""};
  polyline_element pl5 = make_polyline(points5, lstyl, sstyl5);
  obj.add_element(pl5);

  const uint dasize = lsize + 1;
  const string llstr = to_string(dasize);
  vrange points3 = make_points_at({xo, yo + 2 * linelen});
  const stroke_style sstyl3 = {"", 0, llstr, "", "round", ""};
  polyline_element pl3 = make_polyline(points3, lstyl, sstyl3);
  obj.add_element(pl3);

  const string llstr2 = llstr + k::space + to_string(dasize * 2);
  vrange points2 = make_points_at({xo, yo + linelen});
  const stroke_style sstyl2 = {"", 0, llstr2, "", "round", ""};
  polyline_element pl2 = make_polyline(points2, lstyl, sstyl2);
  obj.add_element(pl2);

  const string llstr3 = llstr + k::space + to_string(dasize * 4);
  vrange points1 = make_points_at({xo, yo});
  const stroke_style slstyl = {"", 0, llstr3, "", "round", ""};
  polyline_element pl1 = make_polyline(points1, lstyl, slstyl);
  obj.add_element(pl1);
}


int main()
{
  svg::make_markers(obj);
  test_polyline();
  return 0;
}
