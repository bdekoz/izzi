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
      auto dn = zero_angle_north_cw(d);
      dn -= adjust_label_angle_for_text_height(typo);      
      auto [ x, y ] = get_circumference_point_d(dn, r, cp);
      radial_text_cw(obj, typo, std::to_string(d), x, y, dn);
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
  points_to_line(obj, rstyl, p0, p6);
  points_to_line(obj, rstyl, p9, p3);
}


int main()
{
  test_arc("test-radial-text-2");
  return 0;
}
