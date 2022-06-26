#include "a60-svg.h"

void
test_text(std::string ofile)
{
  using namespace std;
  using namespace svg;

  // Inject text characteristics.
  using anchor = svg::typography::anchor;
  //  using align = svg::typography::align;

  area<> a = k::letter_096_v;
  svg_element obj(ofile, a);

  typography typobase = k::apercu_typo;
  typobase._M_size = 24;

  point_2t cp = obj.center_point();
  auto [ x, y ] = cp;

  // 1 anchor start
  {
    typography typo(typobase);
    typo._M_anchor = anchor::start;
    sized_text_r(obj, typo, 24, "asama", x, y + 50, 90);
  }

  // 2 anchor middle
  {
    typography typo(typobase);
    typo._M_anchor = anchor::middle;
    sized_text_r(obj, typo, 24, "asama", x, y, 90);
  }

  // 3 anchor end
  {
    typography typo(typobase);
    typo._M_anchor = anchor::end;
    sized_text_r(obj, typo, 24, "asama", x, y - 50, 90);
  }

  const style rstyl = { color::red, 1.0, color::red, 1.0, 2 };
  point_to_crossed_lines(obj, rstyl, cp, 300);
}


int main()
{
  test_text("rotated-text-1");
  return 0;
}
