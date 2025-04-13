#include "a60-svg.h"
#include "a60-json-basics.h"
#include "a60-svg-graphs-line.h"

// static local
namespace {

  //const svg::area<> a = { 1920, 1080 };
const svg::area<> a = { 900, 600 };
svg::svg_element obj("line-graph-1", a);
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
  const style styl1 = { color::wcag_lgray, 0.0, color::wcag_lgray, 1.0, 4 };
  //const style styl2 = { color::wcag_gray, 0.0, color::wcag_gray, 1.0, 4 };
  const style styl3 = { color::wcag_dgray, 0.0, color::wcag_dgray, 1.0, 4 };

  const string jdir("/home/bkoz/src/mozilla-a11y-data-visual-forms/data/");
  const string jfile(jdir + "2025-01-27-minimal.json");
  const string afieldpost("/metrics/SpeedIndexProgress");
  const string afx("/firefox" + afieldpost);
  const string achrome("/chrome" + afieldpost);
  const string f1("timestamp");
  const string f2("percent");

  vrange vr1 = deserialize_json_array_object_field_n(jfile, afx, f1, f2);
  graph_state gs1 { "firefox", f1, f2, styl1, "2", "t4wcagg"};
  svg_element chart1 = make_line_graph(a, vr1, gs1);
  obj.add_element(chart1);

  vrange vr2 = deserialize_json_array_object_field_n(jfile, achrome, f1, f2);
  graph_state gs2 { "chrome", f1, f2, styl3, "2", "c4wcaglg"};
  svg_element chart2 = make_line_graph(a, vr2, gs2);
  obj.add_element(chart2);
}


int main()
{
  svg::make_markers(obj);
  test_chart();
  return 0;
}
