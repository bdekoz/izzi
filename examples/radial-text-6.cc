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
      radial_text_r(obj, std::to_string(d), typo, r1, cp, d);
    }

  // Middle radial
  const int r2 = 120;
  typo._M_size = pt_to_px(24);

  for (uint i = 0; i < 12; ++i)
    {
      auto d = i * 30;
      radial_text_r(obj, std::to_string(d), typo, r2, cp, d);
    }

  // Outer radial
  const int r3 = 180;
  typo._M_size = pt_to_px(10);

  for (uint i = 0; i < 12; ++i)
    {
      auto d = i * 30;
      radial_text_r(obj, std::to_string(d), typo, r3, cp, d);
    }


  // Put in 0/180 and 90/270 guides.
  const int r4 = 200;
  const style rstyl = { color::red, 1.0, color::red, 1.0, 2 };
  point_to_crossed_lines(obj, rstyl, cp, r4);
}


int main()
{
  test_arc("radial-text-6");
  return 0;
}
