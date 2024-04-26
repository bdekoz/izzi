#include "a60-svg.h"

void
test_color_spectrum(std::string ofile)
{
  using namespace std;
  using namespace svg;

  area<> a = svg::k::square8bisb_096_v;
  svg_element obj(ofile, a);

  typography typo = k::apercu_typo;
  typo._M_style = k::w_style;
  typo._M_size = 24;
  typo._M_align = typography::align::left;
  typo._M_anchor = typography::anchor::start;

  point_2t cp = obj.center_point();
  auto [ x, y ] = cp;

  // Draw out colors.
  auto rwidth = 100;
  auto rheight = 400;
  auto rspace = 10;

  x -= (rwidth + rspace);
  auto xoffset = 0;
  auto& spectrum = active_spectrum();
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

  // Text label.
  typo._M_style = k::b_style;
  const string title = "color spectrum size: " + std::to_string(color_max_size);
  sized_text(obj, typo, 24, title, rspace, rspace);
}


int main()
{
  test_color_spectrum("color-palette-5");
  return 0;
}
