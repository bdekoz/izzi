#include "a60-svg.h"
#include "a60-svg-radial.h"


using namespace std;
using namespace svg;


namespace {
  // Default style is 2-pixel black color stroke, 50% opacity 1 pixel stroke.
  const style dstyl = { color::white, 1.0, color::black, 0.5, 1 };
  const style mstyl = { color::wcag_lgray, 1, color::wcag_lgray, 0, 0.66 };
}


void
init_render_state()
{
  // Set radial range for this viz.
  point_2t& rrange = get_radial_range();
  rrange = { 0, 359 };

  // Default layer selections are  vector, glyph and text.
  using select = svg::k::select;
  const select dviz = select::glyph | select::vector | select::text;

  // Set empty identifiers (aka "") to default layer selection and style.
  add_to_id_rstate_cache("", dstyl, dviz);
}


void
test_text(std::string ofile)
{
  using namespace std;
  using namespace svg;

  area<> a = svg::k::square8bis_096;
  svg_element obj(ofile, a);

  typography typo = k::apercu_typo;
  typo._M_size = 6;

  point_2t cp = obj.center_point();
  auto [ x, y ] = cp;

  // Background color
  constexpr bool backgroundp = false;
  if (backgroundp)
    {
      auto [ width, height ] = a;
      point_to_rect(obj, { 0, 0 }, dstyl, width, height);
    }

  const int radius = 16; // 50 if engc
  auto rspace = radius / 5;

  svg::set_label_spaces(12); // For 6 point, space 6 min, all but pose 18
  svg::set_min_ring_size(2); // For 6 point
  svg::set_min_satellite_distance(2.85);

  // Direction glyph.
  if (true)
    {
      style stylarc = svg::k::b_style;
      stylarc.set_colors(color::wcag_lgray);
      stylarc._M_stroke_size = 1;
      const auto rrangeold = set_radial_range(20, 340);
      direction_arc_at(obj, cp, radius, stylarc, radius * 0.1);
      set_radial_range(std::get<0>(rrangeold), std::get<1>(rrangeold));
    }
  else
    {
      // Small center of radius circle
      point_to_circle(obj, { x, y }, k::b_style, radius);
    }

  // Put together data set.
  svg::id_value_umap ivm;
  ivm.insert(make_pair("a", 1));
  ivm.insert(make_pair("b", 2));
  ivm.insert(make_pair("c", 3));
  ivm.insert(make_pair("d", 4));
  ivm.insert(make_pair("e", 5));
  ivm.insert(make_pair("f", 6));
  ivm.insert(make_pair("g", 7));
  ivm.insert(make_pair("h", 8));

  // Render.
  kusama_ids_per_uvalue_on_arc(obj, cp, typo, ivm, 8, radius, rspace,
			       true, false);
}


int main()
{
  init_render_state();
  test_text("radial-graph-kusama-2");
  return 0;
}
