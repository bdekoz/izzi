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
      auto [ x, y ] = get_circumference_point_d(d, r, cp);
      radial_text_cw(obj, typo, std::to_string(d), x, y, d);
    }
}


int main()
{
  test_arc("test-radial-text-1");
  return 0;
}
