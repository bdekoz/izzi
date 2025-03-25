#include <iostream>
#include "a60-svg.h"

using namespace std;
using namespace svg;


color bkgklr = color::wcag_lgray;
const style bkgstyl = { bkgklr, 1.0, color::white, 0.0, 0 };
const style gstyl = { color::green, 1.0, color::green, 0.0, 0 };


// Group style not working
// svg outermost -> group x style <isvg file>
void
test_svg_insert(svg_element& obj)
{
  // Add background.
  point_to_rect(obj, { 0, 0 }, bkgstyl,
		obj._M_area._M_width, obj._M_area._M_height);
  const point_2t cp = obj.center_point();

  // a60_DATA_DIR
  const string prefix("/home/bkoz/src/alpha60-data");
  const string glyphf("/identity/a60-identity-transgender-glyph.svg");
  string isvg = file_to_svg_insert(prefix + glyphf);

  // XXX style not applied in group insertion.
  const auto isize = 100;
  const auto osize = 200;
  const auto angle = 45;
  insert_svg_at(obj, isvg, cp, isize, osize, angle, gstyl);
}


int main()
{
  area<> a = k::a2_096_h;
  svg_element obj("svg-insert-2", a);

  test_svg_insert(obj);
  return 0;
}
