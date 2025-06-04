#include "a60-svg.h"
#include "a60-svg-grid-matrix-systems.h"


/// Metric name index as it appears in data JSON file.
constexpr const char* metrics[] =
  {
    "redirectStart", "redirectEnd", "fetchStart",
    "domainLookupStart", "connectStart", "requestStart",
    "responseStart", "domInteractive", "domComplete",
    "loadEventEnd", "fcp", "largestContentfulPaintrenderTime",
    "lcp_element", "SpeedIndex", "VisualComplete85",
    "LastVisualChange"
  };

/// If true, metric above is active in this analysis.
constexpr const bool activep[] =
  {
    false, true, false,
    true, true, true,
    true, true, false,
    true, true, true,
    false, true, false,
    true
  };

const uint metrics_active_n = std::count(std::begin(activep), std::end(activep), true);

using namespace std;
using namespace svg;

/// Render progressive pageload glyph of expanding boxes, numbers
void
render_fade_in_glyph(svg_element& obj, const double py)
{
  //const double wscale = 0.9;
  const double wscale = 1.0;
  const auto margin = 50;
  const auto delta = 10;
  auto a = obj._M_area;
  auto [ awidth, aheight ] = a;

  // Guide Lines
  style rlstyl = k::r_style;
  rlstyl._M_stroke_opacity = 1.0;
  line_element l1 = make_line({margin, 0}, {margin, aheight}, rlstyl);
  obj.add_element(l1);
  line_element l2 = make_line({awidth - margin, 0}, {awidth - margin, aheight}, rlstyl);
  obj.add_element(l2);

  group_element gimg;
  gimg.start_element("progressive-glyph-" + to_string(py));
  obj.add_element(gimg);

  // Grid
  uint i = 0;
  uint j = 0;
  for (const string s : metrics)
    {
      const double height = 25;

      if (activep[i])
	{
	  const point_2t p = to_point_in_1xn_matrix(a, metrics_active_n,
						    j, margin, py, false);
	  const auto [ x, y ] = p;
	  const double basewidth = (a._M_width - (2 * margin)) / metrics_active_n;
	  const double wgrow = ((1.0 / metrics_active_n) * j);
	  double width = basewidth * wscale * wgrow;
	  if (width < 1.0)
	    width = 1;

	  // Fade in Rect.
	  rect_element r1;
	  rect_element::data drb1 = { x, y, width, height };
	  r1.start_element();
	  r1.add_data(drb1);
	  r1.add_style(svg::k::b_style);
	  r1.finish_element();
	  obj.add_element(r1);

	  // Metric number
	  svg::typography typo = svg::k::hyperl_typo;
	  typo._M_size = 11;
	  typo._M_align = svg::typography::align::left;
	  typo._M_anchor = svg::typography::anchor::start;
	  styled_text(obj, to_string(j + 1),
		      make_tuple(x, y + height + 5 * delta), typo);

	  ++j;
	}

      ++i;
    }

  group_element gimgend;
  gimgend.finish_element();
  obj.add_element(gimgend);
}


/// Render progressive pageload glyph of expanding boxes, numbers
void
render_box_glyph(svg::svg_element& obj, const double py)
{
  uint n = 10;
  const auto margin = 50;
  const style styl = { color::red, 1.0, color::white, 1.0, 1 };

  auto a = obj._M_area;
  auto [ awidth, aheight ] = a;
  const double rwidth = (awidth - (2 * margin)) / n;
  const double rheight = 100;

  for (uint i = 0; i < n; i++)
    {
      const point_2t p = to_point_in_1xn_matrix(a, n, i, margin, py, true);
      rect_element r = make_rect_centered(p, styl, { rwidth, rheight });
      obj.add_element(r);
      styled_text(obj, to_string(i), make_tuple(get<0>(p), py + 10), k::hyperl_typo);
    }
}

void
test_grid()
{
  const svg::area<> a = { 1920, 1080 };
  svg::svg_element obj("grid-matrix-1", a);
  render_fade_in_glyph(obj, 100);

  render_box_glyph(obj, 500);
};


int main()
{
  test_grid();
  return 0;
}
