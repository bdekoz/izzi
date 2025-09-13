 #include "a60-svg.h"

void
test_hex(std::string ofile)
{
  using namespace std;
  using namespace svg;

  area<> a = k::letter_096_v;
  svg_element obj(ofile, a);
  point_2t cp = obj.center_point();

  const auto len = 20;

  // Hexagon honeycomb
  auto honey = make_hexagon_honeycomb(cp, len, 21, false, k::b_style);
  obj.add_element(honey);

  // Text honeycomb
  typography typo = k::apercu_typo;
  typo._M_style.set_colors(color::wcag_lgray);
  auto honeytxt = make_text_honeycomb(cp, len, 21, true, "-h88-", typo);
  obj.add_element(honeytxt);

  // Hexagon honeycomb 2
  auto [ x, y ] = cp;
  auto honey2 = make_hexagon_honeycomb({ x - 4 * len, y}, 2 * len, 4,
				      false, k::r_style);
  obj.add_element(honey2);  
}


int main()
{
  test_hex("hexagon-honeycomb");
  return 0;
}
