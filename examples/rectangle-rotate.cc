#include "a60-svg.h"

// https://developer.mozilla.org/en-US/docs/Web/SVG/Attribute/transform
// rotate square examples

void
test_rectangles(std::string ofile)
{
  using namespace std;
  using namespace svg;

  area<> a = k::letter_096_v;
  svg_element obj(ofile, a);
  point_2t cp = obj.center_point();
  auto [ x, y ] = cp;

  // Black
  rect_element r1;
  rect_element::data dr1 = { size_type(x), size_type(y), 10, 10 };
  r1.start_element();
  r1.add_data(dr1);
  r1.add_style(k::b_style);
  r1.finish_element();
  obj.add_element(r1);

  // Red
  const style rstyl = { color::red, 1.0, color::red, 1.0, 2 };
  rect_element r2;
  rect_element::data dr2 = { size_type(x), size_type(y), 10, 10 };
  r2.start_element();
  r2.add_data(dr2);
  r2.add_style(rstyl);
  r2.add_transform(transform::rotate(100));
  r2.finish_element();
  obj.add_element(r2);

  // Green
  const style gstyl = { color::green, 1.0, color::green, 1.0, 2 };
  rect_element r3;
  rect_element::data dr3 = { size_type(x), size_type(y), 10, 10 };
  r3.start_element();
  r3.add_data(dr3);
  r3.add_style(gstyl);
  r3.add_transform(transform::rotate(100, x + 10, y + 10));
  r3.finish_element();
  obj.add_element(r3);
}


int main()
{
  test_rectangles("rectangle-rotate");
  return 0;
}
