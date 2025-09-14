 #include "a60-svg.h"

void
test_hex(std::string ofile)
{
  using namespace std;
  using namespace svg;

  area<> a = k::letter_096_v;
  svg_element obj(ofile, a);
  point_2t cp = obj.center_point();
  auto [ x, y ] = cp;

  const auto len = 20;
  const double opc = 0.2;

  // Hexagon honeycomb 1
  const style styl1 = { color::red, opc, color::black, 0, 0};
  auto honey1 = make_hexagon_honeycomb(cp, len, 21, false, styl1);

  // Hexagon honeycomb 2
  const style styl2 = { color::blue, opc, color::black, 0, 0};
  string xformr2 = svg::transform::rotate(15, x, y);
  auto honey2 = make_hexagon_honeycomb(cp, len, 20, false, styl2, xformr2);

  // Hexagon honeycomb 3
  const style styl3 = { color::green, opc, color::black, 0, 0};
  string xformr3 = svg::transform::rotate(30, x, y);
  auto honey3 = make_hexagon_honeycomb(cp, len, 19, false, styl3, xformr3);

  // Hexagon honeycomb 4
  const style styl4 = { color::green, opc, color::wcag_gray, 0, 0};
  string xformr4 = svg::transform::rotate(45, x, y);
  auto honey4 = make_hexagon_honeycomb(cp, len, 18, false, styl4, xformr4);

  obj.add_element(honey1);
  obj.add_element(honey2);
  obj.add_element(honey3);
  obj.add_element(honey4);
}


int main()
{
  test_hex("hexagon-honeycomb-2-vector");
  return 0;
}
