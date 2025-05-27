#include "a60-svg.h"

void
test_color(std::string s, const svg::colorband& cbp, const uint sizen)
{
  using namespace svg;
  const svg::area<> a = { 1920, 1080 };

  color_qis klrs = make_color_band(cbp, sizen);
  svg_element emb = display_color_qis(klrs, a, k::apercu_typo);
  svg::svg_element obj(s, a);
  obj.add_element(emb);
}


int main()
{
  test_color("color-band-expand-to-larger-p", svg::cband_p, 100); // purple
  test_color("color-band-expand-to-larger-o", svg::cband_o, 100); // orange
  test_color("color-band-expand-to-larger-r", svg::cband_r, 100); // red
  return 0;
}
