#include "a60-svg.h"

  const svg::area<> a = { 1920, 1080 };
svg::svg_element obj("line-1", a);
svg::point_2t cp = obj.center_point();
  auto [ x, y ] = cp;

  const auto linelen = 150;

void
test_line()
{
  using namespace std;
  using namespace svg;

  // WCAG Black/White/Gray
  const style styl1 = { color::wcag_lgray, 0.0, color::wcag_lgray, 1.0, 4 };
  const style styl2 = { color::wcag_gray, 0.0, color::wcag_gray, 1.0, 4 };
  const style styl3 = { color::wcag_dgray, 0.0, color::wcag_dgray, 1.0, 4 };

  // Line 1
  const point_2t p11 { x, y };
  const point_2t p12 { x, y + 2 * linelen };
  line_element l1 = make_line(p11, p12, styl1);
  obj.add_element(l1);

  // Line 2
  const point_2t p21 { x, y };
  const point_2t p22 { x + linelen, y + linelen};
  line_element l2 = make_line(p21, p22, styl2);
  obj.add_element(l2);

  // Line 3
  const point_2t p31 { x, y };
  const point_2t p32 { x - linelen, y - linelen};
  line_element l3 = make_line(p31, p32, styl3);
}


// https://developer.mozilla.org/en-US/docs/Web/CSS/stroke-dasharray
void
test_line_2()
{
  using namespace std;
  using namespace svg;

  // Line 1
  const point_2t p11 { x - linelen, y };
  const point_2t p12 { x + linelen, y + linelen };
  const style styl = { color::black, 1.0, color::black, 1.0, 2 };
  //line_element l1 = make_line(p11, p12, styl, "40%, 60%");
  line_element l1 = make_line(p11, p12, styl, "10 5");
  obj.add_element(l1);

  const point_2t p21 { linelen, 0 };
  const point_2t p22 { linelen, linelen };
  line_element l2 = make_line(p21, p22, styl, "2 2");
  obj.add_element(l2);

  const point_2t p31 { linelen * 2, 0 };
  const point_2t p32 { linelen * 2, linelen };
  line_element l3 = make_line(p31, p32, styl, "3");
  obj.add_element(l3);
};


int main()
{
  test_line();
  test_line_2();
  return 0;
}
