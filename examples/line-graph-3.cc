#include "a60-svg.h"
#include "a60-svg-graphs-line.h"
#include "izzi-json-basics.h"

// static local
namespace {

  //const svg::area<> a = { 1920, 1080 };
const svg::area<> a = { 900, 600 };
svg::svg_element obj("line-graph-3", a);
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
  const string afieldpost("/metrics/SpeedIndexProgress");
  const string afx("/firefox" + afieldpost);
  const string achrome("/chrome" + afieldpost);
  const string f1("timestamp");
  const string f2("percent");

  // Color constants.
  // WCAG Black/White/Gray
  const style styl1 = { color::wcag_lgray, 0.0, color::wcag_lgray, 1.0, 1 };
  const style styl2 = { color::wcag_gray, 0.0, color::wcag_gray, 1.0, 1 };
  const style styl3 = { color::wcag_dgray, 0.0, color::wcag_dgray, 1.0, 1 };

  // Set graph styles.
  //svg::select glayers { select::ticks | select::axis };
  //svg::select glayers { select::ticks | select::vector };

  graph_rstate gs1 { select::vector,
		     "firefox", a, chart_line_style_2,
		     f1, f2, "ms", "%", styl3,
		     { "r2wcadg", "1 2", "", "triangle", "" },
		     {0,0}, "" };

  graph_rstate gs2 { select::vector | select::echo,
		     "chrome", a, chart_line_style_2,
		     f1, f2, "ms", "%", styl1,
		     { "c2wcaglg", "3", "", "round", "" },
		     {0,0}, "" };

  graph_rstate gsa { select::ticks | select::linex | select::alt,
		     "annotation", a, chart_line_style_2,
		     f1, f2, "s", "%", styl2,
		     { "", "", "", "", "" },
		     {0,0}, "" };

  // Deserialize A/B data.
  vrange vr1a = deserialize_json_array_object_field_n(jfile, afx, f1, f2);
  vrange vr1 = find_change_points(vr1a);
  vrange vr2a = deserialize_json_array_object_field_n(jfile, achrome, f1, f2);
  vrange vr2 = find_change_points(vr2a);
  vrange vunion = union_vrange(vr1, vr2);

  // Draw axis, ticks, etc.
  // NB scale x axis from milliseconds in json to seconds in display.
  svg_element anno = make_line_graph_annotations(vunion, gsa, 1000);
  obj.add_element(anno);

  // Find combined ranges, assume zero start.
  auto [ maxx, maxy ] = max_vrange(vunion, graph_rstate::xticdigits, 1000);
  point_2t rangex = make_tuple(0, maxx * 1000);
  point_2t rangey = make_tuple(0, maxy);

  // Draw graph(s).
  svg_element chart1 = make_line_graph(vr1, gs1, rangex, rangey);
  svg_element chart2 = make_line_graph(vr2, gs2, rangex, rangey);
  obj.add_element(chart1);
  obj.add_element(chart2);
}


int main()
{
  test_chart();
  return 0;
}
