#include "a60-svg.h"
#include "a60-svg-graphs-line.h"

// static local
namespace {
const svg::area<> a = { 1920, 1080 };
svg::svg_element obj("markers-3", a);
svg::point_2t cp = obj.center_point();
const auto [ cx, cy ] = cp;

const auto spacer = 150;
}


void
test_markers(const double x, const double y, const svg::style styl)
{
  using namespace std;
  using namespace svg;

  const double radius = 250;

  point_2t p10 = { x, y };
  double amp = spacer; // 10-100 hight of wave
  double decay = 2.5; // 0.1-5.0  Rate of amplitude decrease 
  int cycles = 2;
  auto m10 = make_path_ripple(p10, styl, radius, amp, decay, cycles);
  obj.add_element(m10);

}


int main()
{
  using namespace svg;

  const style stylo = { color::wcag_lgray, 0.0, color::wcag_lgray, 1.0, 2 };
  test_markers(cx * .5, cy, stylo);

  const style stylf = { color::wcag_lgray, 1.0, color::wcag_lgray, 0.0, 4 };
  test_markers(cx * 1.5, cy, stylf);

  return 0;
}
