#include "a60-svg-radial.h"

void
test_arc(std::string ofile)
{
  using namespace std;
  using namespace svg;

  area<> a = k::letter_096_v;
  svg_element obj(ofile, a);
  style s = { color::white, 0.0, color::black, 1.0, 3 };

  typography typoleft = k::apercu_typo;
  typoleft._M_size = 10;
  typoleft._M_a = typography::anchor::start;
  typoleft._M_align = typography::align::left;

  typography typoright(typoleft);
  typoright._M_a = typography::anchor::end;
  typoright._M_align = typography::align::right;

  point_2t cp = obj.center_point();
  const int r = 80;

  point_2t p0 = get_circumference_point_d(zero_angle_north_cw(0), r, cp);
  point_2t p1 = get_circumference_point_d(zero_angle_north_cw(90), r, cp);
  point_2t p2 = get_circumference_point_d(zero_angle_north_cw(180), r, cp);
  point_2t p3 = get_circumference_point_d(zero_angle_north_cw(270), r, cp);

  string ne = make_path_arc_circumference(p0, p1, r);
  string se = make_path_arc_circumference(p1, p2, r);
  string sw = make_path_arc_circumference(p2, p3, r);
  string nw = make_path_arc_circumference(p3, p0, r);

  // Make arc 1 vector path
  string arc_namev("arc-v");
  path_element parcv(true);
  path_element::data dav = { make_path_arc_circumference(p0, p3, r, 1), 0 };
  parcv.start_element(arc_namev);
  parcv.add_data(dav);
  parcv.add_style(s);
  parcv.finish_element();
  obj.add_element(parcv);

  const bool is_visible = false;
  string arc_name("arc-text-");

  // Make arc 1 text path
  path_element parc(is_visible);
  path_element::data da = { nw, 0 };
  parc.start_element(arc_name + to_string(1));
  parc.add_data(da);
  parc.add_style(s);
  parc.finish_element();
  obj.add_element(parc);

  // Make label text 1 and style it.
  const string text("web vitals 2020");

  // Put it together 1.
  text_element::data dt = { 0, 0, text, typoleft };
  text_path_element tp(arc_name + to_string(1), "2%", "left");
  tp.start_element();
  tp.add_data(dt);
  tp.finish_element();
  obj.add_element(tp);


  // Make arc 2 text path
  point_2t p20 = get_circumference_point_d(zero_angle_north_cw(0), r * 2, cp);
  point_2t p21 = get_circumference_point_d(zero_angle_north_cw(90), r * 2, cp);

  path_element parc2(is_visible);
  string nw2 = make_path_arc_circumference(p20, p21, r * 2, 0, 1);
  path_element::data da2 = { nw2, 0 };
  parc2.start_element(arc_name + to_string(2));
  parc2.add_data(da2);
  parc2.add_style(s);
  parc2.finish_element();
  obj.add_element(parc2);

  // Put it together 2.
  text_element::data dt2 = { 0, 0, "START", typoleft };
  text_path_element tp2(arc_name + to_string(2), "0%", "left");
  tp2.start_element();
  tp2.add_data(dt2);
  tp2.finish_element();
  obj.add_element(tp2);


  // Make arc 3 text path
  point_2t p22 = get_circumference_point_d(zero_angle_north_cw(180), r * 2, cp);
  point_2t p23 = get_circumference_point_d(zero_angle_north_cw(270), r * 2, cp);
  path_element parc3(is_visible);
  string nw3 = make_path_arc_circumference(p22, p23, r * 2, 0, 1);
  path_element::data da3 = { nw3, 0 };
  parc3.start_element(arc_name + to_string(3));
  parc3.add_data(da3);
  parc3.add_style(s);
  parc3.finish_element();
  obj.add_element(parc3);

  // Put it together 3.
  text_element::data dt3 = { 0, 0, "END", typoleft };
  text_path_element tp3(arc_name + to_string(3), "0%", "right");
  tp3.start_element();
  tp3.add_data(dt3);
  tp3.finish_element();
  obj.add_element(tp3);
}


int main()
{
  test_arc("test-arc-label-2");
  return 0;
}
