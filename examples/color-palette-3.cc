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

  // type
  typography typo = k::apercu_typo;
  typo._M_size = 24;
  typo._M_style._M_stroke_color = color::white;
  typo._M_style._M_fill_color = color::white;
  typo._M_align = typography::align::left;
  typo._M_anchor = typography::anchor::start;

  // green colors band size
  const uint green_n = 21;

  point_2t cp = obj.center_point();
  auto [ x, y ] = cp;

  // Gray background.
  point_to_rect_centered(obj, cp, sgray, width, height);

  // Draw out colors.
  auto rwidth = 40;
  auto rheight = 400;
  auto rspace = 10;
  auto xoffset = 0;

  color klr = color::green;
  for (uint i = 0; i < green_n; ++i)
    {
      // Color block
      const style s = { klr, 1.0, klr, 0.0, 0 };
      point_2t p = { x + xoffset, y };
      point_to_rect_centered(obj, p, s, rwidth, rheight);

      // Label.
      sized_text_r(obj, typo, 24, to_string(klr),
		   x + xoffset, y - rheight / 2 + rspace, 90);

      xoffset += rwidth + rspace;
      klr = next_color(klr);
    }
}


int main()
{
  test_color("color-palette-3");
  return 0;
}
