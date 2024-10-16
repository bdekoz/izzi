#include "a60-svg.h"

void
test_color(std::string ofile, auto spectrum)
{
  using namespace std;
  using namespace svg;

  const area<> a = svg::k::square8bis_096;
  const auto [ awidth, aheight ] = a;

  svg_element obj(ofile, a);

  typography typo = k::apercu_typo;
  typo._M_style = k::b_style;
  typo._M_size = 24;
  typo._M_align = typography::align::left;
  typo._M_anchor = typography::anchor::start;

  // Draw out colors.
  auto rwidth = 20;
  auto rheight = 100;
  auto rspace = 4;

  auto x = rwidth, y = rheight;
  auto xoffset = 0;
  for (const auto& klr : spectrum)
    {
      // Color block
      const style s = { klr, 1.0, klr, 0.0, 2 };
      point_2t p = { x + xoffset, y };
      point_to_rect_centered(obj, p, s, rwidth, rheight);

      // Label.
      sized_text_r(obj, typo, 9, to_string(klr),
		   x + xoffset, y - rheight / 2 + rspace, 90);

      if (xoffset + rwidth + rspace < awidth - rwidth - rspace)
	xoffset += rwidth + rspace;
      else
	{
	  xoffset = 0;
	  y += (rheight + rspace + rspace);
	}
    }
}


int main()
{
  test_color("color-palette-6.3-colorbrewer2-3", svg::colorbrewer2s3s_palette);
  test_color("color-palette-6.3-colorbrewer2-9", svg::colorbrewer2s9s_palette);
  return 0;
}
