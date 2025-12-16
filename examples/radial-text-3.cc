#include "a60-svg-radial.h"

void
test_arc(std::string ofile)
{
  using namespace std;
  using namespace svg;

  area<> a = k::letter_096_v;
  svg_element obj(ofile, a);

  typography typo = k::apercu_typo;
  typo._M_size = 24;

  point_2t cp = obj.center_point();
  const int r = 120;

  for (uint i = 0; i < 12; ++i)
    {
      auto d = i * 30;
      auto dn = zero_angle_north_ccw(d);
      auto p = get_circumference_point_d(dn, r, cp);
      radial_text_ccw(obj, std::to_string(d), typo, p, dn);
    }

  // Put in 0/180 and 90/270 guides.
  const style rstyl = { color::red, 1.0, color::red, 1.0, 2 };

  auto d0 = zero_angle_north_cw(0);
  auto d6 = zero_angle_north_cw(180);
  auto d3 = zero_angle_north_cw(90);
  auto d9 = zero_angle_north_cw(270);
  point_2t p0 = get_circumference_point_d(d0, r * 1.25, cp);
  point_2t p6 = get_circumference_point_d(d6, r * 1.25, cp);
  point_2t p3 = get_circumference_point_d(d3, r * 1.25, cp);
  point_2t p9 = get_circumference_point_d(d9, r * 1.25, cp);
  auto l1 = make_line(p0, p6, rstyl);
  auto l2 = make_line(p9, p3, rstyl);
  obj.add_element(l1);
  obj.add_element(l2);  
}


int main()
{
  test_arc("radial-text-3");
  return 0;
}
