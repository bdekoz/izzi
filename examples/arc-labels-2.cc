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

  point_2t p0 = get_circumference_point_d(align_angle_to_north(0), r, cp);
  point_2t p1 = get_circumference_point_d(align_angle_to_north(90), r, cp);
  point_2t p2 = get_circumference_point_d(align_angle_to_north(180), r, cp);
  point_2t p3 = get_circumference_point_d(align_angle_to_north(270), r, cp);

  string ne = make_path_arc_circumference(p0, p1, r);
  string se = make_path_arc_circumference(p1, p2, r);
  string sw = make_path_arc_circumference(p2, p3, r);
  string nw = make_path_arc_circumference(p3, p0, r);

  // Make arc vector path
  string arc_namev("arc-v");
  path_element parcv(true);
  path_element::data dav = { make_path_arc_circumference(p0, p3, r), 0 };
  parcv.start_element(arc_namev);
  parcv.add_data(dav);
  parcv.add_style(s);
  parcv.finish_element();
  obj.add_element(parcv);

  // Make arc text path
  const bool is_visible = false;
  string arc_name("arc-text");
  path_element parc(is_visible);
  path_element::data da = { nw, 0 };
  parc.start_element(arc_name);
  parc.add_data(da);
  parc.add_style(s);
  parc.finish_element();
  obj.add_element(parc);

  // Make label text and style it.
  string text("web vitals 2020");
  typography typo = k::apercu_typo;
  typo._M_size = 10;
  typo._M_a = typography::anchor::start;
  typo._M_align = typography::align::left;

  // Put it together.
  text_element::data dt = { 0, 0, text, typo };
  text_path_element tp(arc_name, "30%");
  tp.start_element();
  tp.add_data(dt);
  tp.finish_element();
  obj.add_element(tp);
}


int main()
{
  test_arc("test-arc-label-2");
  return 0;
}
