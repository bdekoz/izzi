#include "a60-svg.h"

void
test_color(std::string s)
{
  using namespace svg;

  const colorband& cbp = cband_p;
  color_qis klrs = make_color_band_v2(cbp, std::get<1>(cbp) * 3);

  const svg::area<> a = { 1920, 1080 };
  svg_element emb = display_color_qis(klrs, a, k::apercu_typo);
  svg::svg_element obj(s, a);
  obj.add_element(emb);
}


int main()
{
  test_color("color-band-expand-to-larger");
  return 0;
}

