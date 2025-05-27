#include "a60-svg.h"

void
test_color(std::string ofile)
{
  using namespace std;
  using namespace svg;

  area<> a = svg::k::square8bis_096;
  svg_element obj(ofile, a);

  typography typo = k::apercu_typo;
  typo._M_style = k::w_style;
  typo._M_size = 24;
  typo._M_align = typography::align::left;
  typo._M_anchor = typography::anchor::start;

  const uint wcag_n = 3;
  using bw_array = std::array<color, wcag_n>;

  bw_array spectrum =
    {
      color::wcag_lgray, color::wcag_gray, color::wcag_dgray
    };

  svg_element emb = display_color_qis(spectrum, a, k::apercu_typo);
  obj.add_element(emb);
}


int main()
{
  test_color("color-palette-4");
  return 0;
}
