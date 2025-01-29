#include "a60-svg.h"

void
test_text(std::string ofile)
{
  using namespace std;
  using namespace svg;

  area<> a = k::letter_096_v;
  const string url("https://bdekoz.github.io/izzi/");
  typography typo = k::apercu_typo;
  typo._M_size = 24;

  svg_element obj(ofile, a);
  point_2t cp = obj.center_point();
  styled_text_link(obj, "lynxxxxxx", cp, typo, url);
}


int main()
{
  test_text("text-link");
  return 0;
}
