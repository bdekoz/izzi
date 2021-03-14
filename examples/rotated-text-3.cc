#include "a60-svg.h"

void
test_text(std::string ofile)
{
  using namespace std;
  using namespace svg;

  area<> a = k::letter_096_v;
  svg_element obj(ofile, a);

  typography typobase = k::apercu_typo;
  typobase._M_size = 24;

  point_2t cp = obj.center_point();
  auto [ x, y ] = cp;

  // 1 top
  {
    typography typo(typobase);
    point_2t origin = { x, y - 100 };
    styled_text(obj, "top", origin, typo);
    styled_text_r(obj, "top-90-cp", origin, typo, 90, cp);
    styled_text_r(obj, "top-90-origin", origin, typo, 90, origin);
  }

  const style rstyl = { color::red, 1.0, color::red, 1.0, 2 };
  point_to_plus_lines(obj, rstyl, cp, 300);
}


int main()
{
  test_text("rotated-text-3");
  return 0;
}
