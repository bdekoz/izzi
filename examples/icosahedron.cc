 #include "a60-svg.h"

void
test_geometry(std::string ofile)
{
  using namespace std;
  using namespace svg;

  area<> a = k::letter_096_v;
  svg_element obj(ofile, a);
  point_2t cp = obj.center_point();
  auto [ x, y] = cp;

  const auto len = 50;

  // Black
  const style bstyl = { color::black, 0.0, color::black, 1.0, .33 };
  auto t1 = make_icosahedron(cp, bstyl, len * 3);
  obj.add_element(t1);

  // Red
  const style rstyl = { color::red, 1.0, color::black, 1.0, 1 };
  auto t2 = make_icosahedron({ x, y + 200 }, rstyl, len * 2);
  obj.add_element(t2);

  // Green
  const style gstyl = { color::green, 0.5, color::green, 0.5, 0.5 };
  auto t3 = make_icosahedron({ x, y + 400 }, gstyl, len);
  obj.add_element(t3);
}


int main()
{
  test_geometry("icosahedron");
  return 0;
}
