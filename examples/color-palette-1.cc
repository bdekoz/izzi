#include "a60-svg.h"

void
test_color(std::string ofile)
{
  using namespace std;
  using namespace svg;

  area<> a = k::letter_096_v;
  auto [ width, height ] = a;
  svg_element obj(ofile, a);
  point_2t cp = obj.center_point();

  const style sgray = { color::gray10, 1.0, color::gray10, 0.0, 2 };

  const uint bw_n = 2;
  using bw_array = std::array<color, bw_n>;
  bw_array spectrum =
    {
      color::black, color::white
    };

  // Gray background.
  auto r = make_rect_centered(cp, sgray, {width, height});
  obj.add_element(r);
  svg_element emb = display_color_qis(spectrum, a, k::apercu_typo);
  obj.add_element(emb);
}


int main()
{
  test_color("color-palette-1");
  return 0;
}
