#include "a60-svg.h"
#include "a60-svg-radial.h"

void
init_render_state()
{
  using namespace std;
  using namespace svg;

  using select = svg::k::select;

  // Set radial range for this viz.
  point_2t& rrange = get_radial_range();
  rrange = { 0, 359 };

  // Default style is 2-pixel black color stroke, 50% opacity 1 pixel stroke.
  const style dstyl = { color::white, 1.0, color::black, 0.5, 1 };

  // Default layer selections are  vector, glyph and text.
  const select dviz = select::glyph | select::vector | select::text;

  // Set empty identifiers (aka "") to default layer selection and style.
  add_to_id_render_state_cache("", dstyl, dviz);
}


void
test_text(std::string ofile)
{
  using namespace std;
  using namespace svg;

  area<> a = svg::k::square8bis_096_v;
  svg_element obj(ofile, a);

  typography typo = k::apercu_typo;
  typo._M_size = 24;

  point_2t cp = obj.center_point();
  auto [ x, y ] = cp;

  // Background color
  //const style dstyl = { color::gray20, 1.0, color::black, 0, 2 };
  //auto [ width, height ] = a;
  //point_2d_to_rect(obj, 0, 0, dstyl, width, height);

  // Default style is 2-pixel black color stroke, 50% opacity 1 pixel stroke.
  const style dstyl = { color::wcag_lgray, 1, color::wcag_lgray, 0, 0.66 };

  // Default layer selections are  vector, glyph and text.
  using select = svg::k::select;
  const select dviz = select::glyph | select::vector | select::text;
  add_to_id_render_state_cache("", dstyl, dviz);


  // Set radial range for this viz.
  point_2t& rrange = get_radial_range();
  rrange = { 0, 359 };

  const int radius = 32; // 50 if engc
  auto rspace = radius / 5;

  svg::set_label_spaces(12); // For 6 point, space 6 min, all but pose 18
  svg::set_min_ring_size(2); // For 6 point
  svg::set_min_satellite_distance(2.85);

  // Direction glyph.
  if (true)
    {
      style stylarc = svg::k::b_style;
      stylarc.set_colors(color::wcag_lgray);
      stylarc._M_stroke_opacity = 1.0;
      stylarc._M_stroke_size = 3;
      auto cradius = radius;
      const auto rrangeold = set_radial_range(20, 340);
      direction_arc_at(obj, cp, cradius, stylarc, cradius * 0.25);
      set_radial_range(std::get<0>(rrangeold), std::get<1>(rrangeold));
    }
  else
    {
      // Small center of radius circle
      point_2d_to_circle(obj, x, y, k::b_style, radius);
    }

  // Put together data set.
  svg::id_value_umap ivm;
  ivm.insert(make_pair("one a", 1));
  ivm.insert(make_pair("one b", 1));
  ivm.insert(make_pair("one c", 1));

  ivm.insert(make_pair("three d", 3));
  ivm.insert(make_pair("three e", 3));

  ivm.insert(make_pair("six f", 6));

  // Render.
  kusama_ids_per_uvalue_on_arc(obj, cp, typo, ivm, 8, radius, rspace, true,
			       false);
}


int main()
{
  init_render_state();
  test_text("radial-graph-kusama-3");
  return 0;
}
