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

/// Render progressive pageload glyph of expanding boxes, numbers
void
render_fade_in_glyph(svg::svg_element& obj, const double py)
{
  using namespace std;
  using namespace svg;

  const auto margin = 50;
  const auto delta = 10;
  auto a = obj._M_area;

  group_element gimg;
  gimg.start_element("progressive-page-load-glyph");
  obj.add_element(gimg);

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

	  const double wscale = 0.9;
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

void
test_grid()
{
  const svg::area<> a = { 1920, 1080 };
  svg::svg_element obj("grid-matrix-1", a);
  render_fade_in_glyph(obj, 100);
};


int main()
{
  test_grid();
  return 0;
}
