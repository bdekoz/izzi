#include "a60-svg.h"

void
test_title(std::string ofile)
{
  using namespace std;
  using namespace svg;

  area<> a = k::a5_096_v;
  svg_element obj(ofile, a);
  point_2t cp = obj.center_point();
  auto [ x, y ] = cp;

  // Black
  //const style styl1 = { color::wcag_lgray, 1.0, color::black, 0.0, 2 };
  const style styl1 = { color::wcag_lgray, 1.0, color::black, 1.0, .5 };

  const string finish_hard(string { element_base::finish_tag } + k::newline);

  double r1 = 150;
  circle_element c;
  circle_element::data dc = { x - r1/2, y, r1 };
  c.start_element();
  c.add_data(dc);
  c.add_style(styl1);
  c.add_raw(finish_hard);
  c.add_title("uss callister into infinity");
  c.add_raw(string { circle_element::tag_closing } + k::newline);

  obj.add_element(c);
}


int main()
{
  test_title("circle-title");
  return 0;
}
