 #include "a60-svg.h"

void
test_image(std::string ofile)
{
  using namespace std;
  using namespace svg;

  svg_element obj(ofile, k::letter_096_v);
  point_2t cp = obj.center_point();
  auto [ cpx, cpy ] = cp;

  const string isrc = "image/floor-angle-shadow-blur.jpg";
  image_element i;
  image_element::data di = { isrc, cpx, cpy, 403, 302 };
  i.start_element();
  i.add_data(di, "", "visible", "");
  i.finish_element();
  obj.add_element(i);
}


int main()
{
  test_image("image-2");
  return 0;
}
