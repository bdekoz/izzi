#include "a60-svg.h"

void
test_text(std::string ofile)
{
  using namespace std;
  using namespace svg;

  // Inject text characteristics.
  using align = svg::typography::align;

  area<> a = k::letter_096_v;
  svg_element obj(ofile, a);

  typography typobase = k::apercu_typo;
  typobase._M_size = 24;

  point_2t cp = obj.center_point();
  auto [ x, y ] = cp;

  // 1 align left
  {
    typography typo(typobase);
    typo._M_align = align::left;
    sized_text_r(obj, typo, 24, "asama", x, y + 100, 90);
  }

  // 2 align center
  {
    typography typo(typobase);
    typo._M_align = align::center;
    sized_text_r(obj, typo, 24, "asama", x, y, 90);
  }

  // 3 align right
  {
    typography typo(typobase);
    typo._M_align = align::right;
    sized_text_r(obj, typo, 24, "asama", x, y - 100, 90);
  }

  const style rstyl = { color::red, 1.0, color::red, 1.0, 2 };
  obj.add_element(make_path_center_mark(cp, rstyl, 300, 2));
}


int main()
{
  test_text("rotated-text-2");
  return 0;
}
