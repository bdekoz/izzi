#include "a60-svg.h"

void
test_color(std::string ofile)
{
  using namespace std;
  using namespace svg;

  area<> a = svg::k::square8bis_096;
  svg_element obj(ofile, a);

  typography typo = k::apercu_typo;
  typo._M_style = k::w_style;
  typo._M_size = 24;
  typo._M_align = typography::align::left;
  typo._M_anchor = typography::anchor::start;

  point_2t cp = obj.center_point();
  auto [ x, y ] = cp;

  const uint wcag_n = 3;
  using bw_array = std::array<color, wcag_n>;

  bw_array spectrum =
    {
      color::wcag_lgray, color::wcag_gray, color::wcag_dgray
    };

  // Draw out colors.
  auto rwidth = 100;
  auto rheight = 400;
  auto rspace = 10;

  x -= (rwidth + rspace);
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
  test_color("color-palette-4");
  return 0;
}
