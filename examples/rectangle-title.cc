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

  const auto space = 150;

  // Black
  const style styl1 = { color::wcag_lgray, 1, color::black, 1.0, .5 };

  double r1 = 150;
  rect_element r;
  rect_element::data dr = { x - space, y, r1, r1 };
  r.start_element();
  r.add_data(dr);
  r.add_style(styl1);
  r.add_raw(element_base::finish_tag_hard);
  r.add_title("uss callister into infinity");
  r.add_raw(string { rect_element::pair_finish_tag } + k::newline);

  obj.add_element(r);
}


int main()
{
  test_title("rectangle-title");
  return 0;
}
