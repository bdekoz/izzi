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
  test_color("color-palette-6.3-colorbrewer2-3", svg::colorbrewer2s3s_palette);
  test_color("color-palette-6.3-colorbrewer2-9", svg::colorbrewer2s9s_palette);
  return 0;
}
