#include <iostream>
#include "a60-svg.h"

using namespace std;
using namespace svg;
using atype = svg_element::atype;

void
make_ring_ranges(svg_element& obj, const point_2t origin,
		 const atype ri, const atype r, const atype ro)
{
  auto [ dx, dy ] = origin;
  const atype x(dx);
  const atype y(dy);

  // Set default circle style is black ring.
  style cstyle = k::w_style;
  cstyle._M_fill_opacity = 0;
  cstyle._M_stroke_opacity = 1.0;
  cstyle._M_stroke_size = 1.0;

  // inner ring
  circle_element cughosti;
  circle_element::data dcug = { x, y, ri };
  cughosti.start_element();
  cughosti.add_data(dcug);
  cughosti.add_style(cstyle);
  cughosti.finish_element();
  obj.add_element(cughosti);

  // outer ring
  circle_element cughosto;
  dcug._M_radius = ro;
  cughosto.start_element();
  cughosto.add_data(dcug);
  cughosto.add_style(cstyle);
  cughosto.finish_element();
  obj.add_element(cughosto);

  // overlay ghost circle, original size.
  circle_element cughost;
  dcug._M_radius = r;
  cstyle._M_stroke_color = color::white;
  cstyle._M_stroke_size = 1.5;
  cughost.start_element();
  cughost.add_data(dcug);
  cughost.add_style(cstyle);
  cughost.finish_element();
  obj.add_element(cughost);
}


// https://developer.mozilla.org/en-US/docs/Web/SVG/Element/radialGradient
// https://www.digitalocean.com/community/tutorials/svg-radial-gradients
// https://www.w3.org/TR/SVG11/pservers.html
// http://www.svgbasics.com/radial_gradients.html
void
test_radial_gradient(std::string ofile)
{
  area<> a = k::letter_096_v;
  svg_element obj(ofile, a);

  point_2t cp = obj.center_point();
  auto [ x, y ] = cp;

  const auto radius = 60;
  const double fuzzpercentage = 0.33;
  const ssize_type variance = std::round(radius * fuzzpercentage);

  // outer ring == upper bound, radius + variance.
  const ssize_type oring = radius + variance;
  const double oratio = static_cast<double>(oring)/static_cast<double>(radius);
  const ssize_type oringperc(round(oratio * 100));
  const string oringpercent = to_string(oringperc) + "%";

  // inner ring == lower bound, radius - variance.
  const ssize_type iring = radius - variance;
  const double iratio = static_cast<double>(iring)/static_cast<double>(radius);
  const ssize_type iringperc(round(iratio * 100));
  const string iringpercent = to_string(iringperc) + "%";

  // Some basic diagnostics.
  clog << "radius: " << radius << k::newline;
  clog << "variance: " << variance <<  k::newline
       << "(+" << oring << k::comma << oringpercent << ')' << k::newline
       << "(-" << iring << k::comma << iringpercent << ')' << k::newline;


  // 1 circumference of circle with gradient both inside and outside

  // outer
  // strategy: make a bigger circle cprime, then do a radial gradient to it
  // starting gradient from color at radius to 100% white/tranparent at cprime.
  const string rgrad3o_name("radialout");
  radial_gradient rgrad3o;
  rgrad3o.start_element(rgrad3o_name);
  rgrad3o.stop(rgrad3o.offset_percentage(radius, oring), color::crimson);
  rgrad3o.stop("100%", color::white, 0);
  rgrad3o.finish_element();
  obj.add_element(rgrad3o);

  circle_element c4o;
  circle_element::data dc4o = { x, y, oring };
  c4o.start_element();
  c4o.add_data(dc4o);
  c4o.add_fill(rgrad3o_name);
  c4o.finish_element();
  obj.add_element(c4o);

  // inner
  // strategy: make a smaller circle cprime, then do a radial gradient from it
  // starting gradient from white/transparent at cprime to color at r.
  const string rgrad3i_name("radialin");
  radial_gradient rgrad3i;
  rgrad3i.start_element(rgrad3i_name);
  rgrad3i.stop(rgrad3i.offset_percentage(iring, radius), color::white);
  rgrad3i.stop("100%", color::crimson);
  rgrad3i.finish_element();
  obj.add_element(rgrad3i);

  circle_element c4i;
  circle_element::data dc4i = { x, y, radius };
  c4i.start_element();
  c4i.add_data(dc4i);
  c4i.add_fill(rgrad3i_name);
  c4i.finish_element();
  obj.add_element(c4i);


  // circle registration marks.
  make_ring_ranges(obj, cp, iring, radius, oring);
}


int main()
{
  test_radial_gradient("radial-gradient-ring-halo-1");
  return 0;
}
