#include "a60-svg.h"
#include <iostream>

void
test_color(std::string ofile)
{
  using namespace std;
  using namespace svg;

  // Strong yellow.
  color_qi yi1(253, 234, 0);
  color_qf yf1(yi1);
  const bool checkh1 = yf1.h < 56 && yf1.h > 54;
  const bool checks1 = yf1.s == 1.0;
  const bool checkv1 = yf1.v > .99;
  std::cout << to_string(yi1) << std::endl;
  std::cout << to_string(yf1) << std::endl;
  if (!checkh1 || !checks1 || !checkv1)
    throw std::logic_error("yf1 rgb/hsv fail");

  // Strong blue
  color_qi yi2(0, 149, 255);
  color_qf yf2(yi2);
  const bool checkh2 = yf2.h < 206 && yf2.h > 204;
  const bool checks2 = yf2.s == 1.0;
  const bool checkv2 = yf2.v == 1.0;
  std::cout << to_string(yi2) << std::endl;
  std::cout << to_string(yf2) << std::endl;
  if (!checkh2 || !checks2 || !checkv2)
    throw std::logic_error("yf2 rgb/hsv fail");

  // Strong red
  color_qi yi3(255, 0, 24);
  color_qf yf3(yi3);
  const bool checkh3 = yf3.h < 355 && yf3.h > 353;
  const bool checks3 = yf3.s == 1.0;
  const bool checkv3 = yf3.v == 1.0;
  std::cout << to_string(yi3) << std::endl;
  std::cout << to_string(yf3) << std::endl;
  if (!checkh3 || !checks3 || !checkv3)
    throw std::logic_error("yf3 rgb/hsv fail");

  // Gray
  color_qi yi4(239, 239, 239);
  color_qf yf4(yi4);
  const bool checkh4 = yf4.h == 0;
  const bool checks4 = yf4.s == 0;
  const bool checkv4 = yf4.v < .94 && yf4.v > .93;
  std::cout << to_string(yi4) << std::endl;
  std::cout << to_string(yf4) << std::endl;
  if (!checkh4 || !checks4 || !checkv4)
    throw std::logic_error("yf4 rgb/hsv fail");
  
}


int main()
{
  test_color("color-rgb-to-hsv-1");
  return 0;
}
