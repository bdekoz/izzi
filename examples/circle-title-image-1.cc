#include "a60-svg.h"

using namespace svg;

void
test_tooltip(svg_element& obj)
{
  using namespace std;
  using namespace svg;

  point_2t cp = obj.center_point();
  auto [ x, y ] = cp;

  const double r = 10;
  const style styl1 = { color::wcag_lgray, 1.0, color::black, 1.0, .5 };

  // image tooltip
  const string imgid = "image-1";
  const string isrc = "image/floor-angle-shadow-blur.jpg";
  image_element i;
  image_element::data di = { isrc, 0, 0, 200, 150 };
  i.start_element(imgid);
  i.add_data(di, "hidden", "anonymous");
  i.finish_element();
  obj.add_element(i);

  
  // circle with text (title) tooltip, image (screenshot) tooltip
  circle_element c;
  circle_element::data dc = { x - 2 * r, y, r };
  c.start_element();
  c.add_data(dc);
  c.add_style(styl1);
  c.add_raw(script_element::tooltip_attribute(imgid));
  c.add_raw(element_base::finish_tag_hard);
  c.add_title("uss callister into infinity");
  c.add_raw(circle_element::pair_finish_tag);
  c.add_raw({k::newline});
  obj.add_element(c);

  // script
  script_element::scope context = script_element::scope::element;
  obj.add_element(script_element::tooltip_script(context));
}


int main()
{
  area<> a = k::v1080p_h;
  svg_element obj("circle-title-image-1", a);

  test_tooltip(obj);
  return 0;
}
 
