#include "a60-svg.h"
#include "a60-svg-graphs-line.h"
#include "izzi-json-basics.h"

// static local
namespace {

  //const svg::area<> a = { 1920, 1080 };
const svg::area<> a = { 900, 600 };
svg::svg_element obj("line-graph-2", a);
svg::point_2t cp = obj.center_point();

const auto spacer = 50;
const auto linelen = 150;

}

void
test_chart()
{
  using namespace std;
  using namespace svg;

  // WCAG Black/White/Gray
  const style styl1 = { color::wcag_lgray, 0.0, color::wcag_lgray, 1.0, 1 };
  const style styl2 = { color::wcag_gray, 0.0, color::wcag_gray, 1.0, 1 };
  const style styl3 = { color::wcag_dgray, 0.0, color::wcag_dgray, 1.0, 1 };

  const string jdir("/home/bkoz/src/mozilla-a11y-data-visual-forms/data/");
  const string jfile(jdir + "2025-01-27-minimal.json");
  const string afieldpost("/metrics/SpeedIndexProgress");
  const string afx("/firefox" + afieldpost);
  const string achrome("/chrome" + afieldpost);
  const string f1("timestamp");
  const string f2("percent");

  //svg::select glayers { select::ticks | select::axis };
  //svg::select glayers { select::ticks | select::vector };


  // Deserialize A/B data.
  vrange vr1 = deserialize_json_array_object_field_n(jfile, afx, f1, f2);
  vrange vr2 = deserialize_json_array_object_field_n(jfile, achrome, f1, f2);

  vspace xpoints;
  vspace ypoints;
  split_vrange(vr1, xpoints, ypoints);
  split_vrange(vr2, xpoints, ypoints);
  sort(xpoints.begin(), xpoints.end());
  sort(ypoints.begin(), ypoints.end());

  // Find combined ranges, assume zero start.
  point_2t rangex = make_tuple(0, xpoints.back());
  point_2t rangey = make_tuple(0, ypoints.back());

  // Set graph styles.
  svg::select vlayer { select::vector };
  graph_rstate gs1 { vlayer, "firefox", f1, f2, "ms", "%",
		     styl1, { "r2wcadg", "3", "", "square", "" } };
  graph_rstate gs2 { vlayer, "chrome", f1, f2, "ms", "%",
		     styl3, { "c2wcaglg", "1 2", "", "round", "" } };
  graph_rstate gsa { {select::ticks}, "chrome", f1, f2, "ms", "%",
		     styl2, { "", "", "", "", "" } };


  // Draw axis, ticks, etc.
  make_line_graph_annotations(a, xpoints, ypoints, gsa, obj);

  // Draw graph(s).
  svg_element chart1 = make_line_graph(a, vr1, gs1, rangex, rangey);
  svg_element chart2 = make_line_graph(a, vr2, gs2, rangex, rangey);
  obj.add_element(chart1);
  obj.add_element(chart2);
}


int main()
{
  test_chart();
  return 0;
}
