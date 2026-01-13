#include "a60-svg-radial.h"

void
test_arc(std::string ofile)
{
  using namespace std;
  using namespace svg;

  //  typography typo = k::apercu_typo;
  typography typo = k::hyperl_typo;
  const style rstyl = { color::red, 1.0, color::red, 1.0, 0.5 };

  area<> a = k::instagram_4ks;
  svg_element obj(ofile, a);
  point_2t cp = obj.center_point();

  // 1 Inner radial
  const int r1 = 60;
  typo._M_size = 10;
  for (uint i = 0; i < 12; ++i)
    {
      auto d = i * 30;
      string s = "she loves me " + std::to_string(d);
      radial_text_r(obj, s, typo, r1, cp, d);
    }

  // 2 Middle radial
  const int r2 = 120;
  typo._M_size = pt_to_px(24);
  for (uint i = 0; i < 12; ++i)
    {
      auto d = i * 30;
      string s = "she loves me not " + std::to_string(d);
      radial_text_r(obj, s, typo, r2, cp, d);
    }

  // 3 dot
  const int rd = 600;
  for (uint i = 0; i < 12; ++i)
    {
      auto d = i * 30;
      auto p = get_circumference_point_d(d, rd, cp);
      auto c = make_circle(p, rstyl, 6);
      obj.add_element(c);
    }

  // 4 Outer radial
  const int r3 = 700;
  typo._M_size = pt_to_px(18);
  for (uint i = 0; i < 12; ++i)
    {
      auto d = i * 30;
      string s = "absolutely " + std::to_string(d) + " percent";
      radial_text_r(obj, s, typo, r3, cp, d);
    }

  // Put in 0/180 and 90/270 guides.
  const int r4 = 1000;
  obj.add_element(make_path_center_mark(cp, rstyl, r4, 2));
  for (uint i = 0; i < 12; ++i)
    {
      auto d = i * 30;
      const point_2t pdbg = get_circumference_point_d(d, r4, cp);
      auto l = make_line(cp, pdbg, rstyl);
      obj.add_element(l);
    }
}


int main()
{
  test_arc("radial-text-7");
  return 0;
}
