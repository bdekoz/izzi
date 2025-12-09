#include "a60-svg.h"

using namespace std;
using namespace svg;

auto rwidth = 25;
auto rheight = 200;
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


const palette_qi<7> star_wars_palette =
  {
    color_qi(215,25,50),	// acolyte red
    color_qi(148,0,211),	// ahsoka purple
    color_qi(37,43,255),	// andor blue
    color_qi(16,114,9),		// mandalorian green
    color_qi(255,69,0),		// boba fett orange
    color_qi(128,5,5),		// kenobi brown
    color::none
  };

void
test_colors(std::string ofile)
{
  // Output file.
  svg_element obj(ofile, k::a4_096_v);
  uint x = rwidth;
  uint y = rheight;

  // Set up palette, pick color per series.
  // 8 bands, 11 colors in each for 88 colors total.
  // In each band, lightest is 1 darkest is 11.
  auto spectrum = star_wars_palette;
  for (uint i = 0; i < spectrum.size(); i++)
    {
      auto klr = spectrum[i];
      point_2t p = { x + rspace + i * rwidth, y };
      draw_klr(obj, klr, p, i);
    }

  x = rwidth;
  y += rheight + rspace;
  
  for (const auto klrbase : spectrum)
    {
      for (uint i = 0; i < 11; i++)
	{
	  auto klr = color_qf(klrbase).tint_perceptual(i);
	  point_2t p = { x + rspace + i * rwidth, y };
	  draw_klr(obj, klr, p, i);
	}
      y += rheight + rspace;
    }
}


int main()
{
  test_colors("color-qf-tint-perceptual-2");
  return 0;
}
