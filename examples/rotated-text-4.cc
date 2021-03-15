#include "a60-svg.h"

// https://developer.mozilla.org/en-US/docs/Web/SVG/Attribute/alignment-baseline
// rotate centerline

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
  point_2t top = { x, y - 200 };
  point_2t bottom = { x, y + 200 };

  // Three text items.
  // use alignment-baseline="middle"
  typography typo(typobase);
  styled_text(obj, "top", top, typo);
  styled_text_r(obj, "cp 90", cp, typo, 90, cp);
  styled_text_r(obj, "bottom -90", bottom, typo, -90, bottom);

  // Red marks at placement points.
  const style rstyl = { color::red, 1.0, color::red, 1.0, 2 };
  point_to_plus_lines(obj, rstyl, cp, 10);
  point_to_plus_lines(obj, rstyl, top, 10);
  point_to_plus_lines(obj, rstyl, bottom, 10);
}


int main()
{
  test_text("rotated-text-4");
  return 0;
}
