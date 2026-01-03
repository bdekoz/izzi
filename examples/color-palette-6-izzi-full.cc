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
  auto spectrum = svg::izzi_palette;
  test_color("color-palette-6.izzi.1", spectrum);

  auto spectrums(spectrum);
  std::sort(spectrums.begin(), spectrums.end(), svg::color_qf_lt);
  std::reverse(spectrums.begin(), spectrums.end());
  test_color("color-palette-6.izzi.2", spectrums);
  return 0;
}
