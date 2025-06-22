#include "a60-svg.h"


void
test_object(std::string ofile)
{
  using namespace std;
  using namespace svg;

  svg_element obj(ofile, svg::k::letter_096_v);
  auto cp = obj.center_point();

  area<> ai = {4032, 3024}; // image pixel size
  area<> arect = {403, 302};  // svg display size
  const string isrc = "image/floor-angle-shadow-blur.jpg";

  foreign_element fe;
  fe.start_element(cp, ai, arect);


  object_element i;
  i.start_element("raster_image");
  i.add_data(arect, isrc, "image/jpeg");
  i.finish_element();
  fe.add_element(i);

  fe.finish_element();
  obj.add_element(fe);
}


int main()
{
  test_object("object-1");
  return 0;
}
