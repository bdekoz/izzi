 #include "a60-svg.h"

void
test_video(std::string ofile)
{
  using namespace std;
  using namespace svg;

  const svg::area<> page = { 1920, 1080 };
  svg_element obj(ofile, page);
  point_2t cp = obj.center_point();
  auto [ cpx, cpy ] = cp;

  // video in center
  const string isrc="asama-0-oo-ee-oo-loop-5l.3.concat-all-ascii-white-1s.1080p-960x540.mp4";
  const string vsrc = "../docs/image/" + isrc;
  const area<> av(960, 540); // frame size of video file
  const area<> arect(480, 260); // size of video in svg
  rect_element::data rd = { cpx - av._M_width/2, cpy - av._M_height/2,
			    av._M_width, av._M_height};

  foreign_element fe;
  fe.start_element(av, rd);

  video_element ve;
  ve.start_element(av, vsrc, rd);
  ve.finish_element();
  fe.add_raw(ve.str());

  fe.finish_element();
  obj.add_element(fe);

  // red dot at center
  const style rstyl = { color::red, 1.0, color::red, 0.0, 2 };
  point_to_circle(obj, cp, rstyl, 4);
}


int main()
{
  test_video("video-2");
  return 0;
}
