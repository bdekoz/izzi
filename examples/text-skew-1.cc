#include "a60-svg.h"


void
point_to_line_centered(svg::svg_element& obj, const svg::style styl,
		       const svg::point_2t p, uint n)
{
  auto [ x, y ] = p;
  svg::point_2t pb(x - n, y);
  svg::point_2t pe(x + n, y);
  svg::points_to_line(obj, styl, pb, pe);
}


// http://tutorials.jenkov.com/svg/svg-transformation.html
void
test_text(std::string ofile)
{
  using namespace std;
  using namespace svg;

  // Inject text characteristics.
  using baseline = svg::typography::baseline;

  area<> a = k::letter_096_h;
  svg_element obj(ofile, a);
  point_2t cp = obj.center_point();
  auto [ x, y ] = cp;

  typography typobase = k::apercu_typo;
  typobase._M_size = 24;

  const style rstyl = { color::red, 1.0, color::red, 1.0, 2 };
  const uint n = 15;
  const uint delta = 200;
  x = 0;

  // 1
  {
    x = 100;
    typography typo(typobase);
    typo._M_baseline = baseline::alphabetic;
    point_2t p(x, y);
    point_to_line_centered(obj, rstyl, p, n);
    styled_text(obj, "alphabetic", p, typo, transform::skew_x(30));
  }

  // 2
  {
    x += delta;
    typography typo(typobase);
    typo._M_baseline = baseline::text_top;
    point_2t p(x, y);
    point_to_line_centered(obj, rstyl, p, n);
    styled_text(obj, "text_top", p, typo, transform::skew_x(40));
  }

  // 3
  {
    x += delta;
    typography typo(typobase);
    typo._M_baseline = baseline::text_before_edge;
    point_2t p(x, y);
    point_to_line_centered(obj, rstyl, p, n);
    styled_text(obj, "text_before_edge", p, typo, transform::skew_x(50));
  }

  // 4
  {
    x += delta;
    typography typo(typobase);
    typo._M_baseline = baseline::text_after_edge;
    point_2t p(x, y);
    point_to_line_centered(obj, rstyl, p, n);
    styled_text(obj, "text_after_edge", p, typo, transform::skew_x(60));
  }

  // 5
  {
    x += delta;
    typography typo(typobase);
    typo._M_baseline = baseline::central;
    point_2t p(x, y);
    point_to_line_centered(obj, rstyl, p, n);
    styled_text(obj, "central", p, typo, transform::skew_x(80));
  }
}


int main()
{
  test_text("text-skew-1");
  return 0;
}
