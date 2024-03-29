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
  const int r = 160;

  // Have two concepts for rotation of objects around a circle.
  // 1. location of the radial points == clockwise starting at zero north
  // 2. radial text switches orientation at 180 degrees for readibility,
  //    aka 6'oclock south
  //     a. clockwise, 0 < d < 180
  //     b. counter clockwise 180 < d < 360
  for (uint i = 0; i < 12; ++i)
    {
      auto d = i * 30;

      if (d <= 180)
	{
	  auto dcw = zero_angle_north_cw(d);
	  auto p = get_circumference_point_d(dcw, r, cp);
	  radial_text_cw(obj, std::to_string(d), typo, p, dcw);
	}
      else
	{
	  // Mapping some values...

	  // d 210 cw -> d 150 ccw
	  // d 240 cw -> d 120 ccw
	  // d 270 cw -> d 90 ccw
	  // d 330 cw -> d 30 ccw

	  // 210 - 180 == 30 -> 180 - 30 == 150
	  // 240 - 180 == 60 -> 180 - 60 == 120
	  // 330 - 180 == 150 -> 180 - 150 == 30

	  auto dp = d - 180;
	  auto dccw = zero_angle_north_ccw(180 - dp);
	  auto p = get_circumference_point_d(dccw, r, cp);
	  radial_text_ccw(obj, std::to_string(d), typo, p, dccw);
	}
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
  test_arc("radial-text-4");
  return 0;
}
