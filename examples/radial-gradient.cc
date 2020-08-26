#include <iostream>
#include "a60-svg.h"

// https://developer.mozilla.org/en-US/docs/Web/SVG/Element/radialGradient
// https://www.digitalocean.com/community/tutorials/svg-radial-gradients
// https://www.w3.org/TR/SVG11/pservers.html
// http://www.svgbasics.com/radial_gradients.html
void
test_radial_gradient(std::string ofile)
{
  using namespace std;
  using namespace svg;

  const auto offset = 100;

  area<> a = k::letter_096_v;
  svg_element obj(ofile, a);
  style s = { color::white, 0.0, color::black, 1.0, 3 };

  point_2t cp = obj.center_point();
  auto [ xd, yd ] = cp;
  const size_type x(xd);
  const size_type y(yd);

  const auto height = 25;
  const auto width = 50;

  const auto radius = std::max( height, width);
  const auto variance = 20;

  // outer ring == upper bound, radius + variance.
  const double oring = 100 * (double(radius) / double(radius + variance));
  const string oringpercent = to_string(oring) + "%";

  // inner ring == lower bound, radius - variance.
  const double iring = 100 * (double(radius - variance) / double(radius));
  const string iringpercent = to_string(iring) + "%";

  // Some basic diagnostics.
  clog << "radius: " << radius << k::newline;
  clog << "variance: " << variance << k::space
       << "(+" << oringpercent << ')' << k::newline
       << "(-" << iringpercent << ')' << k::newline;


  // Set default circle style is black ring.
  style cstyle = k::w_style;
  cstyle._M_fill_opacity = 0;
  cstyle._M_stroke_opacity = 1.0;
  cstyle._M_stroke_size = 2.0;


  // 1 circle
  circle_element c1;
  circle_element::data dc1 = { x, y, radius };
  c1.start_element();
  c1.add_data(dc1);
  c1.add_style(cstyle);
  c1.finish_element();
  obj.add_element(c1);


  // 2 circle with gradient inside, left
  const string rgrad1_name("radialin");
  radial_gradient rgrad1;
  rgrad1.start_element(rgrad1_name);
  rgrad1.stop("10%", color::white);
  rgrad1.stop("90%", color::crimson);
  rgrad1.finish_element();
  obj.add_element(rgrad1);

  circle_element c2;
  circle_element::data dc2 = { x - 2 * offset, y, radius };
  c2.start_element();
  c2.add_data(dc2);
  c2.add_fill(rgrad1_name);
  c2.finish_element();
  obj.add_element(c2);


  // 3 circle with gradient inside, right
  const string rgrad2_name("radialinblack");
  radial_gradient rgrad2;
  rgrad2.start_element(rgrad2_name);
  rgrad2.stop("30%", color::black);
  rgrad2.stop("70%", color::crimson);
  rgrad2.finish_element();
  obj.add_element(rgrad2);

  circle_element c3;
  circle_element::data dc3 = { x + 2 * offset, y, radius };
  c3.start_element();
  c3.add_data(dc3);
  c3.add_fill(rgrad2_name);
  c3.finish_element();
  obj.add_element(c3);


  // 4 circle with gradient outside and hollow center, down

  // outer
  const string rgrad3o_name("radialout_down_out");
  radial_gradient rgrad3o;
  rgrad3o.start_element(rgrad3o_name);
  rgrad3o.stop(oringpercent, color::crimson);
  rgrad3o.stop("100%", color::white);
  rgrad3o.finish_element();
  obj.add_element(rgrad3o);

  circle_element c4o;
  circle_element::data dc4o = { x, y + 2 * offset, radius + variance };
  c4o.start_element();
  c4o.add_data(dc4o);
  c4o.add_fill(rgrad3o_name);
  c4o.finish_element();
  obj.add_element(c4o);

  // inner
  const string rgrad3i_name("radialout_down_in");
  radial_gradient rgrad3i;
  rgrad3i.start_element(rgrad3i_name);
  rgrad3i.stop(iringpercent, color::white);
  rgrad3i.stop("100%", color::crimson);
  rgrad3i.finish_element();
  obj.add_element(rgrad3i);

  circle_element c4i;
  circle_element::data dc4i = { x, y + 2 * offset, radius };
  c4o.start_element();
  c4o.add_data(dc4i);
  c4o.add_fill(rgrad3i_name);
  c4o.finish_element();
  obj.add_element(c4o);

  // overlay ghost circle, original size.
  circle_element c5;
  circle_element::data dc5 = { x, y + 2 * offset, radius };
  c5.start_element();
  c5.add_data(dc5);
  c5.add_style(cstyle);
  c5.finish_element();
  obj.add_element(c5);


  // 5 circle with gradient outside, up
  const string rgrad4_name("radialout_up");
  radial_gradient rgrad4;
  rgrad4.start_element(rgrad4_name, oringpercent, "50%", "50%");
  rgrad4.stop("0%", color::crimson);
  rgrad4.stop("100%", color::white);
  rgrad4.finish_element();
  obj.add_element(rgrad4);

  circle_element c6;
  circle_element::data dc6 = { x, y - 2 * offset, radius + variance };
  c6.start_element();
  c6.add_data(dc6);
  c6.add_fill(rgrad4_name);
  c6.finish_element();
  obj.add_element(c6);

  // Overlay ghost circle outline, original size.
  circle_element c7;
  circle_element::data dc7 = { x, y - 2 * offset, radius };
  c7.start_element();
  c7.add_data(dc7);
  c7.add_style(cstyle);
  c7.finish_element();
  obj.add_element(c7);
}


int main()
{
  test_radial_gradient("test-radial-gradient-1");
  return 0;
}
