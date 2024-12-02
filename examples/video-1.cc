 #include "a60-svg.h"

void
test_tri(std::string ofile)
{
  using namespace std;
  using namespace svg;

  svg_element obj(ofile, k::letter_096_v);
  //  point_2t cp = obj.center_point();

  const area<> a(270, 480);
  rect_element::data rd = { 540, 960, 1080, 1920};
     
  foreign_element fe;
  fe.start_element(a, rd);
  obj.add_element(fe);

  const string vsrc = "../docs/image/star-x-0-blink-0.2s.1080p.mkv";
  video_element vb;
  vb.start_element(a, vsrc, rd);
  obj.add_element(vb);

  video_element vf;
  vf.finish_element();
  obj.str(vf.str());

  foreign_element fef;
  fef.finish_element();
  obj.str(fef.str());  
}


int main()
{
  test_tri("video-1");
  return 0;
}
