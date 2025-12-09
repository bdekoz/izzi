#include "a60-svg.h"

using namespace std;
using namespace svg;

auto rwidth = 25;
auto rheight = 400;
auto rspace = 5;

void
draw_klr(svg::svg_element& obj, const svg::color_qi& klr,
	 const svg::point_2t po, uint i)
{
  const uint tsize = 24;
  typography typo = k::apercu_typo;
  typo._M_style = { color::wcag_lgray, 1.0, color::white, 0.0, 0.5 };
  typo._M_align = typography::align::center;
  typo._M_anchor = typography::anchor::start;

  // Draw out colors.
  // Color block
  const style s = { klr, 1.0, klr, 0, 2 };
  point_to_rect_centered(obj, po, s, rwidth, rheight);

  auto [ x, y ] = po;
  auto xo = x - rwidth / 2;
  auto yo = y + rheight / 2 + rspace + tsize;

  // Number.
  typo._M_size = tsize / 2;
  typo._M_anchor = typography::anchor::middle;
  typo._M_align = typography::align::left;
  text_element t = style_text(to_string(i), { xo, yo }, typo);
  obj.add_element(t);

  // Color Label.
  typo._M_size = tsize;
  typo._M_anchor = typography::anchor::start;
  typo._M_align = typography::align::center;
  string klrtxt = to_string(klr) + k::hyphen + to_string(color_qf(klr));
  sized_text_r(obj, typo, 12, klrtxt, xo, yo + tsize, 90);
}


void
test_color(std::string ofile)
{
  // Output file.
  svg_element obj(ofile, k::a4_096_v);
  auto cp = obj.center_point();
  auto [ x, y ] = cp;

  // Set up color per series.
  // 8 bands, 11 colors in each for 88 colors total.
  // In each band, lightest is 1 darkest is 11.
  auto spectrum = svg::ciecam16j70_palette;
  for (uint i = 0; i < 35; i++)
    {
      //auto klr = spectrum[10 + i * 10];
      auto klr = spectrum[i];
      point_2t p = { x + rspace + i * rwidth, y };
      draw_klr(obj, klr, p, i);
    }
}


int main()
{
  test_color("color-qf-tint-perceptual-1");
  return 0;
}
