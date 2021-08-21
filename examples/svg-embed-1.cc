#include "a60-svg.h"

void
test_embed(std::string ofile)
{
  using namespace std;
  using namespace svg;

  area<> a = k::a4_096_v;
  svg_element obj(ofile, a);
  point_2t cp = obj.center_point();
  auto [ x, y ] = cp;

  const auto radius = 50;
  const style bstyl = { color::blue, 1.0, color::green, 1.0, 2 };

  // circle 1
  size_type x1 = 2 * radius;
  circle_element c1;
  circle_element::data dc1 = { x1, size_type(y), radius};
  c1.start_element();
  c1.add_data(dc1);
  c1.add_style(k::b_style);
  c1.finish_element();
  obj.add_element(c1);

  // circle 2
  size_type x2 = a._M_width - 2 * radius;
  circle_element c2;
  circle_element::data dc2 = { x2, size_type(y), radius};
  c2.start_element();
  c2.add_data(dc2);
  c2.add_style(k::b_style);
  c2.finish_element();
  obj.add_element(c2);

  // insert nested svg with circle.
  area<> destarea = { 2 * radius, 2 * radius };
  svg_element nested_obj("nested svg", destarea, false);
  point_2t ncp = nested_obj.center_point();
  auto [ nx, ny ] = ncp;

  nested_obj.start_element(cp, destarea, bstyl);

  // circle 3
  circle_element c3;
  circle_element::data dc3 = { size_type(nx), size_type(ny), radius};
  c3.start_element();
  c3.add_data(dc3);
  c3.finish_element();
  nested_obj.add_element(c3);

  nested_obj.finish_element();

  obj.add_element(nested_obj);
}


int main()
{
  test_embed("svg-embed-1");
  return 0;
}
