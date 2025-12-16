#include "a60-svg.h"

void
test_color(std::string ofile)
{
  using namespace std;
  using namespace svg;

  area<> a = k::letter_096_v;
  auto [ width, height ] = a;  
  svg_element obj(ofile, a);
  point_2t cp = obj.center_point();

  const style sgray = { color::gray10, 1.0, color::gray10, 0.0, 2 };
    
  const uint dubois_n = 19;
  using dubois_array = std::array<color, dubois_n>;

  dubois_array spectrum =
    {
       color::duboisyellow1, color::duboisyellow2, color::duboisyellow3,
       color::duboisbrown1,  color::duboisbrown2,  color::duboisbrown3,
       color::duboisbrown4, color::duboisbrown5,
       color::duboisred1, color::duboisred2, color::duboisred3,
       color::duboisgreen1,  color::duboisgreen2,  color::duboisgreen3,
       color::duboisgreen4,
       color::duboisblue1, color::duboisblue2, color::duboisblue3,
       color::duboisblue4,
    };

   // Gray background.
  auto r = make_rect_centered(cp, sgray, {width, height});
  obj.add_element(r);
  svg_element emb = display_color_qis(spectrum, a, k::apercu_typo);
  obj.add_element(emb);
}


int main()
{
  test_color("color-palette-2");
  return 0;
}
