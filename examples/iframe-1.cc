#include "a60-svg.h"

void
test_iframe(std::string ofile)
{
  using namespace std;
  using namespace svg;

  svg_element obj(ofile, svg::k::letter_096_v);

  area<> ai = {403, 302};    
  const string isrc = "image/floor-angle-shadow-blur.jpg";
  iframe_element i;
  i.start_element();
  i.add_data(ai, isrc, "raster");
  i.finish_element();
  obj.add_element(i);
}


int main()
{
  test_iframe("iframe-1");
  return 0;
}
