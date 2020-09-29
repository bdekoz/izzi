#include <iostream>
#include "a60-svg.h"

using namespace std;
using namespace svg;

#if 0
void
make_ring_halo(svg_element& obj, const point_2t origin,
	       size_type radius, double fuzzpercentage)
{

}
#endif


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
  auto [ xd, yd ] = cp;
  const size_type x(xd);
  const size_type y(yd);

  const auto radius = 60;
  const double fuzzpercentage = 0.33;
  const size_type variance = std::round(radius * fuzzpercentage);

  // outer ring == upper bound, radius + variance.
  const double oring = radius + variance;
  const string oringpercent = to_string(oring/radius) + "%";

  // inner ring == lower bound, radius - variance.
  const double iring = radius - variance;
  const string iringpercent = to_string(iring/radius) + "%";

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
  rgrad3o.stop(to_string(radius/oring) + "%", color::crimson);
  rgrad3o.stop("100%", color::white);
  rgrad3o.finish_element();
  obj.add_element(rgrad3o);

  circle_element c4o;
  circle_element::data dc4o = { x, y, radius + variance };
  c4o.start_element();
  c4o.add_data(dc4o);
  c4o.add_fill(rgrad3o_name);
  c4o.finish_element();
  obj.add_element(c4o);

#if 0  
  // inner
  const string rgrad3i_name("radialin");
  radial_gradient rgrad3i;
  rgrad3i.start_element(rgrad3i_name, radius, x, y, iring, x, y); 
  rgrad3i.stop("0%", color::white);
  rgrad3i.stop("100%", color::crimson);
  rgrad3i.finish_element();
  obj.add_element(rgrad3i);

  circle_element c4i;
  circle_element::data dc4i = { x, y, radius };
  c4o.start_element();
  c4o.add_data(dc4i);
  c4o.add_fill(rgrad3i_name);
  c4o.finish_element();
  obj.add_element(c4o);


  // 2 & 3 pre
  
  // Set default circle style is black ring.
  style cstyle = k::w_style;
  cstyle._M_fill_opacity = 0;
  cstyle._M_stroke_opacity = 1.0;
  cstyle._M_stroke_size = 2.0;

  // Layout distance between variations, if multiple on canvas.
  const auto offset = 200;
  
  
  // 2 upper inner gradient with tracking circle, (y - offset)
  circle_element cu;
  circle_element::data dcu = { x, y - offset, radius };
  cu.start_element();
  cu.add_data(dcu);
  cu.add_fill(rgrad3i_name);
  cu.finish_element();
  obj.add_element(cu);

  // overlay ghost circle, original size.
  circle_element cughost;
  circle_element::data dcug = { x, y - offset, radius };
  cughost.start_element();
  cughost.add_data(dcug);
  cughost.add_style(cstyle);
  cughost.finish_element();
  obj.add_element(cughost);

  circle_element cughosti;
  dcug._M_radius = iring;
  cughosti.start_element();
  cughosti.add_data(dcug);
  cughosti.add_style(cstyle);
  cughosti.finish_element();
  obj.add_element(cughosti);

  circle_element cughosto;
  dcug._M_radius = oring;
  cughosto.start_element();
  cughosto.add_data(dcug);
  cughosto.add_style(cstyle);
  cughosto.finish_element();
  obj.add_element(cughosto);
  
  // 3 lower, outer gradient with tracking circle, (y + offset)
  circle_element cl;
  circle_element::data dcl = { x, y + offset, radius + variance };
  cl.start_element();
  cl.add_data(dcl);
  cl.add_fill(rgrad3o_name);
  cl.finish_element();
  obj.add_element(cl);

  // overlay ghost circles at radius and radius prime.
  circle_element clghostr;
  circle_element::data dclg = { x, y + offset, radius };
  clghostr.start_element();
  clghostr.add_data(dclg);
  clghostr.add_style(cstyle);
  clghostr.finish_element();
  obj.add_element(clghostr);

  circle_element clghostrp;
  circle_element::data dclgp = { x, y + offset, radius + variance};
  clghostrp.start_element();
  clghostrp.add_data(dclgp);
  clghostrp.add_style(cstyle);
  clghostrp.finish_element();
  obj.add_element(clghostrp);
#endif
}


int main()
{
  test_radial_gradient("test-radial-gradient-ring-halo");
  return 0;
}
