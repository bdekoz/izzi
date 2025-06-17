 #include "a60-svg.h"


// image link around circle element with title.
void
test_link(std::string ofile)
{
  using namespace std;
  using namespace svg;

  // Raser image resource.
  const string isrc = "image/floor-angle-shadow-blur.jpg";
  
  area<> a = k::letter_096_v;
  svg_element obj(ofile, a);
  point_2t cp = obj.center_point();
  auto [ x, y] = cp;

  // First link, preload.
  link_element lp;
  lp.start_element();
  lp.add_data(isrc, "preload", "image");
  obj.add_element(lp);
  

  // Second link.
  link_element l;
  l.start_element();
  l.add_data(isrc);

  double r1 = 50;
  circle_element c;
  circle_element::data dc = { x - r1/2, y, r1 };
  c.start_element();
  c.add_data(dc);
  c.add_style(k::r_style);
  c.add_raw(element_base::finish_hard);
  c.add_title("red dot boom");
  c.add_raw(circle_element::pair_finish_tag);
  l.add_element(c);

  l.finish_element();
  obj.add_element(l);
}


int main()
{
  test_link("link-3");
  return 0;
}
