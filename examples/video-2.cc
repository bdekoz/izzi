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

  const style rstyl = { color::red, 1.0, color::red, 0.0, 4 };
  const style gstyl = { color::green, 1.0, color::red, 0.0, 4 };
  const style grystyl = { color::wcag_gray, 0.0, color::wcag_gray, 1.0, 2 };

  const string isrc="asama-0-oo-ee-oo-loop-5l.3.concat-all-ascii-white-1s.1080p-960x540.mp4";
  const string vsrc = "../docs/image/" + isrc;
  const area<> av(960, 540); // frame size of video file
  const area<> arect(480, 270); // size of video in svg
  auto [ rwidth, rheight ] = arect;

  // outline rect
  point_to_rect_centered(obj, cp, grystyl, page._M_width, page._M_height);

  // center rect
  point_to_rect_centered(obj, cp, gstyl, rwidth, rheight);

  // center mark
  point_to_crossed_lines(obj, cp, k::b_style, 2 * std::max(rwidth, rheight), 45.0);
  
  // video in center
  foreign_element fe;
  fe.start_element(av, arect);

  video_element ve;
  rect_element::data rd = { rwidth/2, rheight/2, rwidth, rheight};
  ve.start_element(arect, vsrc, rd);
  ve.finish_element();
  fe.add_raw(ve.str());

  fe.finish_element();
  obj.add_element(fe);

  // red dot markers, black dot at center
  point_to_circle(obj, cp, k::b_style, 8);
  point_to_circle(obj, std::make_tuple(rwidth/2, cpy), rstyl, 4);
  point_to_circle(obj, std::make_tuple(rwidth/2, rheight/2), rstyl, 2);
  point_to_circle(obj, std::make_tuple(cpx + rwidth/2, cpy), rstyl, 4);
  point_to_circle(obj, std::make_tuple(cpx + rwidth/2, rheight/2), rstyl, 2);
}


int main()
{
  test_video("video-2");
  return 0;
}
