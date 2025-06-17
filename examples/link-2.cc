 #include "a60-svg.h"

void
test_link(std::string ofile)
{
  using namespace std;
  using namespace svg;

  area<> a = k::letter_096_v;
  svg_element obj(ofile, a);
  point_2t cp = obj.center_point();

  const string isrc = "image/floor-angle-shadow-blur.jpg";

  link_element l;
  l.start_element();
  l.add_data(isrc);
  text_element t = style_text("place where it happened", cp, k::hyperl_typo);
  l.add_element(t);
  l.finish_element();
  obj.add_element(l);
}


int main()
{
  test_link("link-2");
  return 0;
}
