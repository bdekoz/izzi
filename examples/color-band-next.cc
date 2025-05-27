#include "a60-svg.h"

void
test_color(std::string)
{
  const svg::color_qi klr_red(svg::color::red);
  
  const svg::colorband& cb = svg::cband_r;
  svg::color_qi klr = next_in_color_band(cb, 400);

  if (klr == klr_red)
    throw std::runtime_error("test_color:: wrong color");
}


int main()
{
  test_color("color-band-next");
  return 0;
}
