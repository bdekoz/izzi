 #include "a60-svg.h"

using namespace svg;

void
augment_place(svg_element& obj, const point_2t pt, svg::style s, const space_type r)
{
  point_to_circle(obj, pt, s, r);
  const auto [ px, py ] = pt;

  std::ostringstream oss;
  oss << "[" << px << "," << py << "]";

  const style bstyl = { color::blue, 1.0, color::blue, 0.0, 4 };
  typography typo(k::apercu_typo);
  typo._M_style = bstyl;
  styled_text(obj, oss.str(), pt, typo);
}

void
test_video(std::string ofile)
{
  using namespace std;
  using namespace svg;

  const svg::area<> page = { 1920, 1080 };
  svg_element obj(ofile, page);


  const style pstyl = { color::asamapink, 0.0, color::asamapink, 1.0, 4 };
  const style rstyl = { color::red, 1.0, color::red, 1.0, 4 };
  const style gstyl = { color::green, 1.0, color::red, 0.0, 4 };
  const style grystyl = { color::wcag_gray, 0.0, color::wcag_gray, 1.0, 2 };

  const string isrc="asama-0-oo-ee-oo-loop-5l.3.concat-all-ascii-white-1s.1080p-960x540.mp4";
  const string vsrc = "image/" + isrc;

  const area<> av(960, 540); // frame size of video file
  const area<> arect(480, 270); // size of video in svg
  auto [ rwidth, rheight ] = arect;
  point_2t cp = obj.center_point();
  auto [ cpx, cpy ] = cp;
  point_2t vp = { cpx - rwidth/2, cpy - rheight/2 }; // top left of video
  auto [ vpx, vpy ] = vp;

  // outline rect
  point_to_rect_centered(obj, cp, grystyl, page._M_width, page._M_height);

  // centered rect in green
  point_to_rect_centered(obj, cp, gstyl, rwidth, rheight);

  // red dot markers, black dot at center
  point_to_circle(obj, cp, k::b_style, 8);

  // center mark in pink overlay
  point_to_crossed_lines(obj, cp, pstyl, 250, 45);

  // video in center
  foreign_element fe;
  fe.start_element(vp, av, arect);

  video_element ve;
  ve.start_element();
  ve.add_data(arect, vsrc);
  ve.finish_element();
  fe.add_raw(ve.str());

  fe.finish_element();
  obj.add_element(fe);

  // video edge mark in pink overlay
  point_to_crossed_lines(obj, make_tuple(rwidth/2, rheight/2), pstyl, 50);
  point_to_crossed_lines(obj, make_tuple(rwidth, rheight), pstyl, 50);

  // coordinates
  augment_place(obj, std::make_tuple(vpx, cpy), rstyl, 4);
  augment_place(obj, std::make_tuple(cpx + rwidth/2, cpy), rstyl, 4);
  augment_place(obj, std::make_tuple(cpx, vpy), rstyl, 4);
  augment_place(obj, std::make_tuple(cpx, cpy + rheight/2), rstyl, 4);

  // video rect annotation
  std::ostringstream oss;
  oss << "[" << arect._M_width << "," << arect._M_height << "]";
  styled_text(obj, oss.str(), make_tuple(cpx, 40), k::apercu_typo);
}


int main()
{
  test_video("video-2");
  return 0;
}
