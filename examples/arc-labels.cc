#include "a60-svg-radial.h"

using namespace std;
using namespace svg;


string
quarter_circular_arc(const point_2t& start, const point_2t& end, const int r)
{
  // Define arc.
  // A rx ry x-axis-rotation large-arc-flag sweep-flag x y
  std::ostringstream oss;
  oss << "M" << k::space << to_string(start) << k::space;
  oss << "A" << k::space;
  oss << std::to_string(r) << k::space << std::to_string(r) << k::space;
  oss << 0 << k::space << 0 << k::space << 1 << k::space;
  oss << to_string(end) << k::space;  
  return oss.str();
}


void
test_arc(std::string ofile)
{
  area<> a = k::letter_096_v;
  svg_element obj(ofile, a);
  style s = { color::white, 0.0, color::black, 1.0, 3 };

  point_2t cp = obj.center_point();
  auto [ x, y ] = cp;
  
  const int r = 60;

  point_2t p0 = get_circumference_point_d(align_angle_to_glyph(0), r, cp);
  point_2t p1 = get_circumference_point_d(align_angle_to_glyph(90), r, cp);
  point_2t p2 = get_circumference_point_d(align_angle_to_glyph(180), r, cp);
  point_2t p3 = get_circumference_point_d(align_angle_to_glyph(270), r, cp);

  string ne = quarter_circular_arc(p0, p1, r);
  string se = quarter_circular_arc(p1, p2, r);
  string sw = quarter_circular_arc(p2, p3, r);
  string nw = quarter_circular_arc(p3, p0, r);      
 
  // Make arc path
  string arc_name("arc");  
  path_element parc;
  path_element::data da = { nw, 0 };
  parc.start_element(arc_name);
  parc.add_data(da);
  parc.add_style(s);  
  parc.finish_element();
  obj.add_element(parc);


  string text("web vitals");
  typography typo = k::apercu_typo;
  typo._M_size = 12;
#if 1
  typo._M_a = typography::anchor::start;
  typo._M_align = typography::align::left;
#else
  typo._M_a = typography::anchor::end;
  typo._M_align = typography::align::right;
#endif
  
  text_element::data dt = { x, y, text, typo };
  text_path_element tp(arc_name, "30%");
  tp.start_element();
  tp.add_data(dt);
  tp.finish_element();
  obj.add_element(tp);
}



int main()
{
  test_arc("test-arc-1");
  return 0;
}
