 #include "a60-svg.h"

void
test_video(std::string ofile)
{
  using namespace std;
  using namespace svg;

  svg_element obj(ofile, k::letter_096_v);
  auto cp = obj.center_point();
  
  const area<> av(1080, 1920);
  const area<> arect(270, 480);

  foreign_element fe;
  fe.start_element(cp, av, arect);

  const string vsrc = "image/star-x-0-blink-0.2s.1080p.mkv";
  video_element ve;
  ve.start_element("star-x-blink");
  ve.add_data(arect, vsrc, "video/mp4");
  ve.finish_element();
  fe.add_element(ve);

  fe.finish_element();
  obj.add_element(fe);
}


int main()
{
  test_video("video-1");
  return 0;
}
