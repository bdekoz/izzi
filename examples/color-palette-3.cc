#include "a60-svg.h"

void
test_color(std::string ofile)
{
  using namespace std;
  using namespace svg;

  area<> a = k::letter_096_v;
  svg_element obj(ofile, a);

  auto [ klr, sz ] = cband_g;

  color_qis greens;
  for (uint i = 0; i < sz; ++i)
    greens.push_back(start_at_color(klr));

  svg_element emb = display_color_qis(greens, a, k::apercu_typo);
  obj.add_element(emb);
}


int main()
{
  test_color("color-palette-3");
  return 0;
}
