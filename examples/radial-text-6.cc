#include "a60-svg-radial.h"

void
test_arc(std::string ofile)
{
  using namespace std;
  using namespace svg;

  area<> a = k::letter_096_v;
  svg_element obj(ofile, a);

  typography typo = k::apercu_typo;

  point_2t cp = obj.center_point();

  // pt 10 -> rendered as pt 7.5

  // Inner radial
  const int r1 = 60;
  typo._M_size = pt_to_px(10);

  for (uint i = 0; i < 12; ++i)
    {
      auto d = i * 30;
      radial_text_r(obj, typo, std::to_string(d), r1, cp, d);
    }

  // Middle radial
  const int r2 = 120;
  typo._M_size = pt_to_px(24);

  for (uint i = 0; i < 12; ++i)
    {
      auto d = i * 30;
      radial_text_r(obj, typo, std::to_string(d), r2, cp, d);
    }

  // Outer radial
  const int r3 = 180;
  typo._M_size = pt_to_px(10);

  for (uint i = 0; i < 12; ++i)
    {
      auto d = i * 30;
      radial_text_r(obj, typo, std::to_string(d), r3, cp, d);
    }



  // Put in 0/180 and 90/270 guides.
  const int r4 = 200;
  const style rstyl = { color::red, 1.0, color::red, 1.0, 2 };

  auto d0 = zero_angle_north_cw(0);
  auto d6 = zero_angle_north_cw(180);
  auto d3 = zero_angle_north_cw(90);
  auto d9 = zero_angle_north_cw(270);
  point_2t p0 = get_circumference_point_d(d0, r4, cp);
  point_2t p6 = get_circumference_point_d(d6, r4, cp);
  point_2t p3 = get_circumference_point_d(d3, r4, cp);
  point_2t p9 = get_circumference_point_d(d9, r4, cp);
  points_to_line(obj, rstyl, p0, p6);
  points_to_line(obj, rstyl, p9, p3);
}


int main()
{
  test_arc("radial-text-6");
  return 0;
}
