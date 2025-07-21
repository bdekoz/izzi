#include "a60-svg.h"
#include "a60-svg-graphs-line.h"
#include "izzi-json-basics.h"

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
  const style styl1 = { color::wcag_lgray, 0.0, color::wcag_lgray, 1.0, 3 };
  //const style styl2 = { color::wcag_gray, 0.0, color::wcag_gray, 1.0, 4 };
  //const style styl3 = { color::wcag_dgray, 0.0, color::wcag_dgray, 1.0, 3 };

  const string jdir("/home/bkoz/src/mozilla-a11y-data-visual-forms/data/");
  const string jfile(jdir + "2025-01-27-minimal.json");
  const string afieldpost("/metrics/SpeedIndexProgress");
  const string afx("/firefox" + afieldpost);
  const string achrome("/chrome" + afieldpost);
  const string f1("timestamp");
  const string f2("percent");

  //svg::select glayers { select::ticks | select::axis };
  //svg::select glayers { select::ticks | select::vector };
  svg::select glayers { select::vector };

  // Deserialize data.
  vrange vr1 = deserialize_json_array_object_field_n(jfile, afx, f1, f2);
  auto [ maxx, maxy ] = max_vrange(vr1, graph_rstate::xticdigits, 1000);
  point_2t rangex = make_tuple(0, maxx * 1000);
  point_2t rangey = make_tuple(0, maxy);

  graph_rstate gs1 {
    		     glayers, "firefox", a, chart_line_style_1,  
		     f1, "% complete", "s", "%",
		     styl1, { "t2wcagg", "2", "", "round", "" },
		     {0,0}, "", ""
		   };
  svg_element chart1 = make_line_graph(vr1, gs1, rangex, rangey);
  obj.add_element(chart1);

  gs1.visible_mode = select::ticks;
  svg_element anno = make_line_graph_annotations(vr1, gs1, 1000);
  obj.add_element(anno);
}


int main()
{
  svg::make_markers(obj);
  test_chart();
  return 0;
}
