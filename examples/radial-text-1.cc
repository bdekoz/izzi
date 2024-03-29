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
      auto p = get_circumference_point_d(d, r, cp);
      radial_text_cw(obj, std::to_string(d), typo, p, d);
    }
}


int main()
{
  test_arc("radial-text-1");
  return 0;
}
