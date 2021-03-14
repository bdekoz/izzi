#include "a60-svg-radial.h"

void
test_arc(std::string ofile)
{
  using namespace std;
  using namespace svg;

  area<> a = k::letter_096_v;
  svg_element obj(ofile, a);
  style s = { color::white, 0.0, color::black, 1.0, 3 };

  point_2t cp = obj.center_point();
  const int r = 80;

  double dn = zero_angle_north_cw(0);
  double ds = zero_angle_north_cw(180);
  double de = zero_angle_north_cw(90);
  double dw = zero_angle_north_cw(270);

  point_2t pn = get_circumference_point_d(dn, r, cp);
  point_2t ps = get_circumference_point_d(ds, r, cp);
  point_2t pe = get_circumference_point_d(de, r, cp);
  point_2t pw = get_circumference_point_d(dw, r, cp);

  string ne = make_path_arc_circumference(pn, pe, r);
  string se = make_path_arc_circumference(pe, ps, r);
  string sw = make_path_arc_circumference(ps, pw, r);
  string nw = make_path_arc_circumference(pw, pn, r);

  const bool is_visible = true;
  const string arc_name("arc-v");

  // Make arc path 1
  path_element parc1(is_visible);
  path_element::data da1 = { nw, 0 };
  parc1.start_element(arc_name + "1");
  parc1.add_data(da1);
  parc1.add_style(s);
  parc1.finish_element();
  obj.add_element(parc1);

  // Make arc path 2
  const int rlarge = r * 2;
  style s2 = { color::green, 0.0, color::green, 1.0, 1.5 };
  path_element parc2(is_visible);
  const string arc2 = make_path_arc_closed(cp, get_circumference_point_d(dn, rlarge, cp),
					   get_circumference_point_d(dw, rlarge, cp),
					   rlarge, 1, 1);
  path_element::data da2 = { arc2, 0 };
  parc2.start_element(arc_name + "2");
  parc2.add_data(da2);
  parc2.add_style(s2);
  parc2.finish_element();
  obj.add_element(parc2);


}


int main()
{
  test_arc("arc-1");
  return 0;
}
