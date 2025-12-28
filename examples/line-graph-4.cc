#include "a60-svg.h"
#include "a60-svg-graphs-line.h"
#include "izzi-json-basics.h"

// static local
namespace {

  //const svg::area<> a = { 1920, 1080 };
const svg::area<> a = { 900, 600 };
const svg::area<> aimg = { 200, 150 };

svg::svg_element obj("line-graph-4", a);
svg::point_2t cp = obj.center_point();

const auto spacer = 50;
const auto linelen = 150;

}

void
test_chart()
{
  using namespace std;
  using namespace svg;

  // JSON location, offsets, fields.
  const string jdir("/home/bkoz/src/mozilla-a11y-data-visual-forms/data/");
  const string jfile(jdir + "2025-01-27-minimal.json");
  const string bfirefox("firefox");
  const string bchrome("chrome");
  const string afieldpost("/metrics/SpeedIndexProgress");
  const string afx("/" + bfirefox + afieldpost);
  const string achrome("" + bchrome + afieldpost);
  const string f1("timestamp");
  const string f2("percent");

  const string fxidbase = bfirefox + "-img-";
  const string chidbase = bchrome + "-img-";

  // Color constants.
  // WCAG Black/White/Gray
  const style styl1 = { color::wcag_lgray, 0.0, color::wcag_lgray, 1.0, 1 };
  const style styl2 = { color::wcag_gray, 0.0, color::wcag_gray, 1.0, 1 };
  const style styl3 = { color::wcag_dgray, 0.0, color::wcag_dgray, 1.0, 1 };

  // Set graph styles.
  //svg::select glayers { select::ticks | select::axis };
  //svg::select glayers { select::ticks | select::vector };

  graph_rstate gs1 {
		     select::vector,
		     bfirefox, a, chart_line_style_3,
		     f1, f2, "ms", "%",
		     styl3, { "r2wcadg", marker_shape::none, 0, "1 2", "", "triangle", "" },
		     aimg, "", ""
		   };

  graph_rstate gs2 { select::vector | select::echo,
		     bchrome, a, chart_line_style_3,
		     f1, f2, "ms", "%",
		     styl1, { "c2wcaglg", marker_shape::none, 0, "3", "", "round", "" },
		     aimg, "", "" };

  graph_rstate gsa { select::ticks | select::linex | select::alt,
		     "annotation", a, chart_line_style_3,
		     f1, f2, "s", "%",
		     styl2, { "", marker_shape::none, 0, "", "", "", "" },
		     aimg, "", "" };

  // Deserialize A/B data.
  vrange vr1a = deserialize_json_array_object_field_n(jfile, afx, f1, f2);
  vrange vr1 = find_change_points(vr1a);
  vrange vr2a = deserialize_json_array_object_field_n(jfile, achrome, f1, f2);
  vrange vr2 = find_change_points(vr2a);
  vrange vunion = union_vrange(vr1, vr2);

  // Find combined ranges, assume zero start.
  auto [ maxx, maxy ] = max_vrange(vunion, graph_rstate::xticdigits, 1000);
  point_2t rangex = make_tuple(0, maxx * 1000);
  point_2t rangey = make_tuple(0, maxy);

  // Draw axis, ticks, etc.
  // NB scale x axis from milliseconds in json to seconds in display.
  svg_element anno = make_line_graph_annotations(vunion, gsa, rangex, rangey,
						 1000);
  obj.add_element(anno);

  // Draw graph(s).
  const string tpmeta = "2025-06-13-android-15-ptablet-youtube_COU5T_Wafa4-";
  svg_element chart1 = make_line_graph(vr1, vr1, gs1, rangex, rangey,
				       tpmeta, script_element::scope::element);

  svg_element chart2 = make_line_graph(vr2, vr2, gs2, rangex, rangey,
				       tpmeta, script_element::scope::element);

  obj.add_element(chart1);
  obj.add_raw(gs1.tooltip_images);
  obj.add_element(chart2);
  obj.add_raw(gs2.tooltip_images);

  // Add js to control visibility of images.
  script_element::scope context = script_element::scope::element;
  obj.add_element(script_element::tooltip_script(context));
}


int main()
{
  test_chart();
  return 0;
}
