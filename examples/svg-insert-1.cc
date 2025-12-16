#include <iostream>
#include "a60-svg.h"

using namespace std;
using namespace svg;


color bkgklr = color::wcag_lgray;
const style gstyl = { color::green, 1.0, color::green, 0.0, 0 };
const style bkgstyl = { bkgklr, 1.0, bkgklr, 0.0, 0 };


// svg outermost -> group <isvg file>
void
test_svg_insert(svg_element& obj)
{
  // Add background.
  auto r = make_rect({ 0, 0 }, bkgstyl,
		     {obj._M_area._M_width, obj._M_area._M_height});
  const point_2t cp = obj.center_point();

  // a60_DATA_DIR
  const string prefix("/home/bkoz/src/alpha60-data");
  const string glyphf("/identity/a60-identity-transgender-glyph.svg");
  string isvg = file_to_svg_insert(prefix + glyphf);

  const auto isize = 100;
  const auto osize = 100;
  const auto angle = 0;
  insert_svg_at(obj, isvg, cp, isize, osize, angle);
}


int main()
{
  area<> a = k::a2_096_h;
  svg_element obj("svg-insert-1", a);

  test_svg_insert(obj);
  return 0;
}
