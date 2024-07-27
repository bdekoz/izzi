 #include "a60-svg.h"

void
test_oct(std::string ofile)
{
  using namespace std;
  using namespace svg;

  area<> a = k::letter_096_v;
  svg_element obj(ofile, a);
  point_2t cp = obj.center_point();

  const auto len = 50;

  // Black
  const style bstyl = { color::black, 1.0, color::black, 0.0, 2 };
  path_element t1 = make_path_octogon(cp, bstyl, len * 3);
  obj.add_element(t1);

  // Red
  const style rstyl = { color::red, 1.0, color::red, 0.0, 2 };
  path_element t2 = make_path_octogon(cp, rstyl, len * 2);
  obj.add_element(t2);

  // Green
  const style gstyl = { color::green, 1.0, color::green, 0.0, 2 };
  path_element t3 = make_path_octogon(cp, gstyl, len * 1);
  obj.add_element(t3);
}


int main()
{
  test_oct("octogon");
  return 0;
}
