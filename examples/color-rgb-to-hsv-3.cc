#include "a60-svg.h"


void
draw_spectrum(svg::svg_element& obj, const svg::color_qis& spktrm,
	      const svg::point_2t po)
{
  using namespace std;
  using namespace svg;

  typography typo = k::apercu_typo;
  typo._M_size = 24;
  typo._M_style = { color::wcag_lgray, 1.0, color::white, 0.0, 0.5 };
  typo._M_align = typography::align::left;
  typo._M_anchor = typography::anchor::start;

  // Draw out colors.
  auto rwidth = 25;
  auto rheight = 400;
  auto rspace = 5;

  auto [ x, y ] = po;
  auto xoffset = 0;
  for (const auto& klr : spktrm)
    {
      // Color block
      const style s = { klr, 1.0, klr, 1.0, 2 };
      point_2t p = { x + xoffset, y + (rheight / 2)};
      point_to_rect_centered(obj, p, s, rwidth, rheight);

      // Label.
      string klrtxt = to_string(klr) + k::hyphen + to_string(color_qf(klr));
      sized_text_r(obj, typo, 12, klrtxt,
		   x + xoffset, y + rheight / 2 + rspace, 90);

      xoffset += rwidth + rspace;
    }
}


void
test_color(std::string ofile)
{
  using namespace std;
  using namespace svg;

  // Output file.
  svg_element obj(ofile, k::letter_096_v);

  // Color specturm slice.
  auto& spectrum = active_spectrum();
  color_qis klrs(spectrum.begin(), spectrum.end());

  // 1 draw as-is.
  point_2t p1 = make_tuple(0,0);
  draw_spectrum(obj, klrs, p1);

  // 2 draw sorted
  auto [ x, y ] = obj.center_point();
  point_2t p2 = make_tuple(0, y);
  //  sort(klrs.begin(), klrs.end(), color_qf_lt);
  sort(klrs.begin(), klrs.end(), color_qf_lt_hue_v2);
  draw_spectrum(obj, klrs, p2);
}


int main()
{
  test_color("color-rgb-to-hsv-3");
  return 0;
}
