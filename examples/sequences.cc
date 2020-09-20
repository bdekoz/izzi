#include "a60-svg-sequences.h"

void
test_seq(std::string ofile)
{
  using namespace std;
  using namespace svg;

  area<> a = k::letter_096_v;
  svg_element obj(ofile, a);
}


int main()
{
  test_seq("test-seq-1");
  return 0;
}
