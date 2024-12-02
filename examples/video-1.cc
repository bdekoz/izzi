 #include "a60-svg.h"

void
test_video(std::string ofile)
{
  using namespace std;
  using namespace svg;

  svg_element obj(ofile, k::letter_096_v);
  point_2t cp = obj.center_point();
  auto [ cpx, cpy ] = cp;

  const area<> a(270, 480);
  rect_element::data rd = { cpx, cpy, 1080, 1920};

  foreign_element fe;
  fe.start_element(a, rd);

  const string vsrc = "../docs/image/star-x-0-blink-0.2s.1080p.mkv";
  video_element ve;
  ve.start_element(a, vsrc, rd);
  ve.finish_element();
  fe.add_raw(ve.str());

  fe.finish_element();
  obj.add_element(fe);
}


int main()
{
  test_video("video-1");
  return 0;
}
