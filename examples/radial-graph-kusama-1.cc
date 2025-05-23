#include "a60-svg.h"
#include "a60-svg-radial.h"

void
init_render_state()
{
  using namespace std;
  using namespace svg;

  using select = svg::select;

  // Set radial range for this viz.
  point_2t& rrange = get_radial_range();
  rrange = { 0, 359 };

  // Default style is 2-pixel black color stroke, 50% opacity 1 pixel stroke.
  const style dstyl = { color::white, 1.0, color::black, 0.5, 1 };

  // Default layer selections are  vector, glyph and text.
  const select dviz = select::glyph | select::vector | select::text;

  // Set empty identifiers (aka "") to default layer selection and style.
  add_to_id_rstate_cache("", dstyl, dviz);
}


void
test_text(std::string ofile)
{
  using namespace std;
  using namespace svg;

  area<> a = k::letter_096_h;
  auto [ width, height ] = a;
  svg_element obj(ofile, a);

  typography typo = k::apercu_typo;
  typo._M_size = 24;

  point_2t cp = obj.center_point();
  auto [ x, y ] = cp;

  auto radius = 80;
  auto rspace = radius / 5;

  // Background color
  const style dstyl = { color::gray20, 1.0, color::black, 0, 2 };
  point_to_rect(obj, { 0, 0 }, dstyl, width, height);

  // Small center of radius circle
  point_to_circle(obj, { x, y }, k::b_style, radius);

  // Put together data set.
  svg::id_value_umap ivm;
  for (uint i = 0; i < 9; i++)
    ivm.insert(make_pair(to_string(i), i));

  // Render.
  kusama_ids_per_uvalue_on_arc(obj, cp, typo, ivm, 8, radius, rspace);
}


int main()
{
  init_render_state();
  test_text("radial-graph-kusama-1");
  return 0;
}
