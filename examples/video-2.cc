 #include "a60-svg.h"

void
test_video(std::string ofile)
{
  using namespace std;
  using namespace svg;

  const svg::area<> aa = { 1200, 900 };  
  svg_element obj(ofile, aa);
  point_2t cp = obj.center_point();
  auto [ cpx, cpy ] = cp;

  // video in center
  const area<> a(270, 480);
  rect_element::data rd = { cpx - a._M_width/2, cpy - a._M_height/2,
			    1080, 1920};

  foreign_element fe;
  fe.start_element(a, rd);

  const string vsrc = "../docs/image/star-x-0-blink-0.2s.1080p.mkv";
  video_element ve;
  ve.start_element(a, vsrc, rd);
  ve.finish_element();
  fe.add_raw(ve.str());

  fe.finish_element();
  obj.add_element(fe);

  // red dot at center
  point_to_circle(obj, cp, svg::k::b_style, 4);  
}


int main()
{
  test_video("video-2");
  return 0;
}
