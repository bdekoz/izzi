#include "a60-svg.h"

void
test_color(std::string ofile)
{
  using namespace std;
  using namespace svg;

  area<> a = k::letter_096_v;
  auto [ width, height ] = a;
  svg_element obj(ofile, a);

  const style sgray = { color::gray10, 1.0, color::gray10, 0.0, 2 };
  
  typography typo = k::apercu_typo;
  typo._M_size = 24;
  typo._M_style = sgray;
  typo._M_align = typography::align::left;
  typo._M_anchor = typography::anchor::start;

  point_2t cp = obj.center_point();
  auto [ x, y ] = cp;

  // Gray background.
  point_to_rect_centered(obj, cp, sgray, width, height);

  const uint bw_n = 2;
  using bw_array = std::array<color, bw_n>;

  bw_array spectrum =
    {
      color::black, color::white
    };

  // Draw out colors.
  auto rwidth = 100;
  auto rheight = 400;
  auto rspace = 10;

  auto xoffset = 0;
  for (const auto& klr : spectrum)
    {
      // Color block
      const style s = { klr, 1.0, klr, 1.0, 2 };
      point_2t p = { x + xoffset, y };
      point_to_rect_centered(obj, p, s, rwidth, rheight);

      // Label.
      sized_text_r(obj, typo, 24, to_string(klr),
		   x + xoffset, y - rheight / 2 + rspace, 90);

      xoffset += rwidth + rspace;
    }
}


int main()
{
  test_color("color-palette-1");
  return 0;
}
