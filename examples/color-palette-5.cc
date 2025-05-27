#include "a60-svg.h"

void
test_color_spectrum(std::string ofile)
{
  using namespace std;
  using namespace svg;

  area<> a = svg::k::square8bisb_096_v;
  svg_element obj(ofile, a);

  svg_element emb = display_color_qis(active_spectrum(), a, k::apercu_typo);
  obj.add_element(emb);

  // Text label.
  auto rspace = 10;
  const string title = "color spectrum size: " + std::to_string(color_max_size);
  sized_text(obj, k::apercu_typo, 24, title, rspace, rspace);
}


int main()
{
  test_color_spectrum("color-palette-5");
  return 0;
}
