#include "a60-svg.h"

void
test_color(std::string ofile, auto spectrum)
{
  using namespace std;
  using namespace svg;

  const area<> a = svg::k::square8bis_096;
  svg_element obj(ofile, a);

  svg_element emb = display_color_qis(spectrum, a, k::apercu_typo);
  obj.add_element(emb);
}


int main()
{
  auto spectrum = svg::ciecam16j70_palette;
  test_color("color-palette-6.ciecam16j70", spectrum);

  auto spectrums(spectrum);

  test_color("color-palette-6.ciecam02", spectrums);
  return 0;
}
