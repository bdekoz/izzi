/// -*- mode: C++ -*-
/// Copyright (C) 2014-2017 Benjamin De Kosnik <b.dekosnik@gmail.com>
/// 2014-10-25


#ifndef MiL_SVG_INSCRIBE_H
#define MiL_SVG_INSCRIBE_H 1

#include "a60-svg.h"

#include <sstream> //stringstream
#include <fstream>
#include <string>
#include <tuple>
#include <array>
#include <random>
#include <unistd.h>


using namespace svg;


// fade rectangle fill from transparent to color, of duration sec, given fps
// starts on white (klr), fades to klr color in sec
// can use with image behind
string_vector
fade_to_color_vector(const Rect& r, const colore klr,
		     size_t fps = 30, double sec = 1.0, double maxopac = 1.0)
{
  // Calc number of frames needed.
  size_t framesn = fps * sec;
  double step = maxopac / framesn;

  // Start rectangle as transparent, fade to color.
  string_vector ret;
  style sty = { klr, 0.0, klr, 0.0, 0 };
  for (size_t i = 0; i < framesn; ++i)
    {
      // Foreground rectangle.
      rect_element rsvg;
      rect_element::data dr = { r.x, r.y, r.width, r.height };

      double opacity =  i * step;
      sty._M_fill_opacity = opacity;

      rsvg.start_element();
      rsvg.add_data(dr);
      rsvg.add_style(sty);
      rsvg.finish_element();
      ret.push_back(rsvg.str());
    }

  return ret;
}


string_vector
fade_from_color_vector(const Rect& r, const colore klr,
		       size_t fps = 30, double sec = 1.0, double minopac = 0)
{
  // Calc number of frames needed.
  size_t framesn = fps * sec;
  double step = (1.0 - minopac)/ framesn;

  // Start rectangle as transparent, fade to color.
  string_vector ret;
  style sty = { klr, 0.0, klr, 0.0, 0 };
  for (size_t i = 0; i < framesn; ++i)
    {
      // Foreground rectangle.
      rect_element rsvg;
      rect_element::data dr = { r.x, r.y, r.width, r.height };

      double opacity =  i * step;
      sty._M_fill_opacity = 1.0 - opacity;

      rsvg.start_element();
      rsvg.add_data(dr);
      rsvg.add_style(sty);
      rsvg.finish_element();
      ret.push_back(rsvg.str());
    }

  return ret;
}


// blink sequence of duration sec, given fps
// starts on background (backgf)
// at duration twhen
// blinks specific frame (blinkf) times (nblinks) for seconds (blinksec)
// can use with image behind
string_vector
blink_to_color_vector(const Rect& r, const colore klr,
		      size_t fps, double sec,
		      double twhen, size_t nblinks, double blinksec)
{
  // Calc number of frames needed.
  size_t framesn = fps * sec;
  size_t startn = fps * twhen;
  size_t blinkframesn = (fps * blinksec) + 1; // End with backgf.
  size_t blinkn = blinkframesn * nblinks;

  if (startn + blinkn > framesn)
    throw std::logic_error("blink:: desired frames exceed total duration");

  string_vector ret;
  double opaque(1.0);
  double transparent(0.0);
  rect_element::data dr = { r.x, r.y, r.width, r.height };
  style sty = { klr, 0.0, klr, 0.0, 0 };

  // Generate intro.
  for (size_t i = 0; i < startn; ++i)
    {
      sty._M_fill_opacity = transparent;
      rect_element rsvg;
      rsvg.start_element();
      rsvg.add_data(dr);
      rsvg.add_style(sty);
      rsvg.finish_element();
      ret.push_back(rsvg.str());
    }

  // Blink.
  for (size_t i = 0; i < nblinks; ++i)
    {
      for (size_t j = 0; j < blinkframesn - 1; ++j)
	{
	  sty._M_fill_opacity = opaque;
	  rect_element rsvg;
	  rsvg.start_element();
	  rsvg.add_data(dr);
	  rsvg.add_style(sty);
	  rsvg.finish_element();
	  ret.push_back(rsvg.str());
	}

      sty._M_fill_opacity = transparent;
      rect_element rsvg;
      rsvg.start_element();
      rsvg.add_data(dr);
      rsvg.add_style(sty);
      rsvg.finish_element();
      ret.push_back(rsvg.str());
    }

  // Generate outtro.
  for (size_t i = startn + blinkn; i < framesn; ++i)
    {
      sty._M_fill_opacity = opaque;
      rect_element rsvg;
      rsvg.start_element();
      rsvg.add_data(dr);
      rsvg.add_style(sty);
      rsvg.finish_element();
      ret.push_back(rsvg.str());
    }

  return ret;
}


// wink sequence of duration sec, given fps
// starts on background (backgf)
// at duration twhen
// winks specific rectangle (r) this much (maxclose) for seconds (winksec)
// can use with image behind
string_vector
wink_to_color_vector(const Rect& r, const colore klr,
		     size_t fps, double sec,
		     double twhen, double maxclose, double winksec)
{
  const int rows = r.height;

  // Calc number of frames needed.
  size_t framesn = fps * sec;
  size_t startn = fps * twhen;
  size_t winkframesn = (fps * winksec) + 1;
  size_t winkframeshalfn = winkframesn / 2;

  // End with backgf.
  if (startn + winkframesn >= framesn)
    throw std::logic_error("wink:: desired frames exceed total duration");

  string_vector ret;
  double opaque(1.0);
  double transparent(0.0);
  style sty = { klr, 0.0, klr, 0.0, 0 };

  rect_element::data dr = { r.x, r.y, r.width, r.height };

  // Generate intro.
  for (size_t i = 0; i < startn; ++i)
    {
      sty._M_fill_opacity = transparent;
      rect_element rsvg;
      rsvg.start_element();
      rsvg.add_data(dr);
      rsvg.add_style(sty);
      rsvg.finish_element();
      ret.push_back(rsvg.str());
    }

  // Make two "eyelid" rectangles, one upper and the other lower.
  double ymid = r.height / 2;
  double ymax = ymid * maxclose;
  rect_element::data dhi = { r.x, r.y, r.width, 0 }; // grow down
  rect_element::data dlo = { r.x, r.y + rows, r.width, 0 }; // grow up

  // Wink.
  // Step is ammount to move from eyes open to closed and back again.
  const double step = 2 * ymax / winkframesn;

  // Wink down.
  for (size_t j = 0; j < winkframeshalfn; ++j)
    {
      sty._M_fill_opacity = opaque;
      double offsetn =  j * step;

      // Double rainbow, y'all.
      rect_element r1;
      r1.start_element();
      dhi._M_height = offsetn;
      r1.add_data(dhi);
      r1.add_style(sty);
      r1.finish_element();

      rect_element r2;
      r2.start_element();
      dlo._M_y_origin = r.y + rows - offsetn;
      dlo._M_height = offsetn;
      r2.add_data(dlo);
      r2.add_style(sty);
      r2.finish_element();

      ret.push_back(r1.str() + r2.str());
    }

  // Wink up.
  for (size_t j = 0; j < winkframeshalfn; ++j)
    {
      sty._M_fill_opacity = opaque;
      double offsetn =  (winkframeshalfn - j) * step;

      // Double rainbow, y'all.
      rect_element r1;
      r1.start_element();
      dhi._M_height = offsetn;
      r1.add_data(dhi);
      r1.add_style(sty);
      r1.finish_element();

      rect_element r2;
      r2.start_element();
      dlo._M_y_origin = r.y + rows - offsetn;
      dlo._M_height = offsetn;
      r2.add_data(dlo);
      r2.add_style(sty);
      r2.finish_element();

      ret.push_back(r1.str() + r2.str());
    }

  // Generate outtro.
  for (size_t i = startn + winkframesn; i < framesn; ++i)
    {
      sty._M_fill_opacity = transparent;
      rect_element rsvg;
      rsvg.start_element();
      rsvg.add_data(dr);
      rsvg.add_style(sty);
      rsvg.finish_element();
      ret.push_back(rsvg.str());
    }

  return ret;
}


// Simulated vertical roll, with fades.
// r == frame size
string_vector
vertical_sync_roll_vector(const Rect& rin, const colore klr,
			  size_t fps = 30, double step = 10,
			  int blursz = 200, int solidsz = 133,
			  double opac = 0.6)
{
  // Composition of two rectangular elements:
  // (background) 60 pixel wide rectangular blur 10 pixels, 60% opacity
  // (foreground) 20 pixels wide centered solid color overlay
  const int blurr = 20; // blur radius
  // const int blursz = 200; // 60, 100, 160, 200
  // const int solidsz = 133; // 20, 40, 80, 133
  const double vblursz = blursz / 2;
  const double vsolidsz = solidsz / 2;

  // One full sweep is half-bottom to offscreen top.
  double x = (rin.height + (2 * blursz) + (3 * blurr)) / step;
  size_t framesn = static_cast<size_t>(x);

  style sty = { klr, 1.0, klr, 0.0, 0 };

  // Rectangles start at bottom off-screen and sweep up.
  // SVG rect area is rect 00 top left
  // SVG rect is drawn down and left from starting position.

  // Blur rectangles. The SVG gaussian blur gives a hard edge on the
  // top corner. So, to have an even fade, double it and center under
  // the solid rectangle.
  Rect rblurb(rin);
  rblurb.y = rin.y + rin.height;
  rblurb.height = blursz;

  Rect rblurt(rin);
  rblurt.y = rin.y + rin.height - blursz;
  rblurt.height = blursz;

  // Solid rectangle.
  Rect rs(rin);
  rs.y = rin.y + rin.height - vsolidsz;
  rs.height = solidsz;

  string_vector ret;
  for (size_t i = 0; i < framesn; ++i)
    {
      double stepn = i * step;

      // Dual blur.
      sty._M_fill_opacity = opac;

      rect_element rsvg1;
      size_type yblurb = static_cast<int>(rblurb.y - stepn);
      rect_element::data drb1 = { rblurb.x, yblurb, rblurb.width, rblurb.height };
      rsvg1.start_element();
      rsvg1.add_data(drb1);
      rsvg1.add_filter("20y");
      rsvg1.add_style(sty);
      rsvg1.finish_element();

      rect_element rsvg3;
      size_type yblurt = static_cast<int>(rblurt.y - stepn);
      rect_element::data drb2 = { rblurt.x, yblurt, rblurt.width, rblurt.height };
      rsvg3.start_element();
      rsvg3.add_data(drb2);
      rsvg3.add_filter("20y");

      ostringstream ostrt;
      ostrt << "rotate(180, 960, " << yblurb - vblursz << ")";
      rsvg3.add_transform(ostrt.str());
      rsvg3.add_style(sty);
      rsvg3.finish_element();

      string scene(rsvg1.str() + rsvg3.str());

      // Foreground rectangle.
      sty._M_fill_opacity = opac + 0.20;
      rect_element rsvgfg;
      size_type y = static_cast<int>(rs.y - stepn);
      rect_element::data dr = { rs.x, y, rs.width, rs.height };
      rsvgfg.start_element();
      rsvgfg.add_data(dr);
      rsvgfg.add_style(sty);
      rsvgfg.finish_element();

      ret.push_back(scene + rsvgfg.str());
    }

  return ret;
}


// Randomly create grid of [maxwidth] x [maxheight] grid, and fill with dots.
string
dot_grid_vector(const Rect& rin, const colore klr,
		const int radius = 80,
		const int maxwidth = 8, const int maxheight = 4,
		const int xstart = 40, const int ystart = 100)
{
  // 1920x1080 landscape baselines.
  // 8 wide, 4 high
  // 160 pixel diameter, start at x 40, y 100, move 240.

  static std::mt19937_64 rg(std::random_device{}());
  auto distw = std::uniform_int_distribution<>(0, maxwidth);
  auto disth = std::uniform_int_distribution<>(0, maxheight);
  auto distb = std::uniform_int_distribution<>(0, 1);
  const int gwidth = distw(rg);
  const int gheight = disth(rg);

  // Start at bottom off-screen and go up.
  // These should all be arguments if this were to be parameterized.
  size_type negspace = 2 * radius * 1.5;
  size_type yinit = rin.height - radius - ystart;

  // style sty = { klr, 1.0, klr, 0.0, 0 };
  style sty = { klr, .9, klr, 0.0, 0 };

  string ret;
  for (auto iheight = 0; iheight < gheight; iheight++)
    {
	for (auto iwidth = 0; iwidth < gwidth; iwidth++)
	  {
	    const bool b = distb(rg);
	    if (b)
	      {
		circle_element csvg;
		size_type x = radius + xstart + (iwidth * negspace);
		size_type y = yinit - (iheight * negspace);
		circle_element::data dc = { x, y, radius };
		csvg.start_element();
		csvg.add_data(dc);
		csvg.add_style(sty);
		csvg.finish_element();
		ret += csvg.str();
	      }
	  }
    }

  return ret;
}


/*
  Simulated optical sound dots, or film processing punches,
  an experimental film stylistic.

  "The conclusion of a Warhol film is usually announced by the
  appearance of a flurry of white dots—an artifact of the
  identification number punched into the last few feet of the roll
  at the lab."  Kyle Westphal, Andy Warhol's Magic Trick

  Analyzing Warhol's Outer and Inner Space frame-by-frame, assume:
  1920 x 1080 frame size
  160 pixel diameter circles, with 240 pixel horiz/vert spacing
  100 pixel from edge

  At 30fps, sequence is:
  - 2 frames dot pattern 1
  - 2 frames dot pattern 2
  - 17 frames nothing
  - 2 frames dot pattern 3
  - 2 frames dot pattern 4

  Simplify to:
  - 2 frames dot pattern 1
  - 2 frames dot pattern 2
  - 20 frames blank

  compositions are no more than four across, and sometimes go off the frame

  r == frame size
*/
string_vector
optical_sound_dots_vector(const Rect& rin, const colore klr,
			  size_t fps = 30, double sec = 1.0,
			  const int radius = 80,
			  const int maxw = 8, const int maxh = 4,
			  const int xstart = 40, const int ystart = 100)
{
  string_vector ret;
  size_t framesn = fps * sec;   // Calc number of frames needed.
  size_t step_size = 4 + 17 + 4;
  for (size_t i = 1; i + step_size < framesn; i += step_size)
    {
      string dotpattern1 = dot_grid_vector(rin, klr, radius, maxw, maxh,
					   xstart, ystart);
      ret.push_back(dotpattern1);
      ret.push_back(dotpattern1);
      string dotpattern2 = dot_grid_vector(rin, klr, radius, maxw, maxh,
					   xstart, ystart);
      ret.push_back(dotpattern2);
      ret.push_back(dotpattern2);

      string empty;
      ret.insert(ret.end(), 17, empty);

      string dotpattern3 = dot_grid_vector(rin, klr, radius, maxw, maxh,
					   xstart, ystart);
      ret.push_back(dotpattern3);
      ret.push_back(dotpattern3);
      string dotpattern4 = dot_grid_vector(rin, klr, radius, maxw, maxh,
					   xstart, ystart);
      ret.push_back(dotpattern4);
      ret.push_back(dotpattern4);
    }

  return ret;
}


string
matte_rectangles_vector(const Rect& r1, const Rect& r2,
			const colore klr, double opac = 1.0)
{
  string ret;
  style sty = { klr, opac, klr, 0.0, 0 };

  // Foreground rectangle.
  rect_element rsvg1;
  rect_element::data dr1 = { r1.x, r1.y, r1.width, r1.height };
  rsvg1.start_element();
  rsvg1.add_data(dr1);
  rsvg1.add_style(sty);
  rsvg1.finish_element();

  rect_element rsvg2;
  rect_element::data dr2 = { r2.x, r2.y, r2.width, r2.height };
  rsvg2.start_element();
  rsvg2.add_data(dr2);
  rsvg2.add_style(sty);
  rsvg2.finish_element();

  return rsvg1.str() + rsvg2.str();
}



// 2 channel
// Only vector, color band roll
void
object_style_swipe_all_left_2_channel()
{
  using namespace boost::filesystem;

  string prefix("/home/bkoz/src/MiL.asama-loops-2/images/landscape/");
  string dir("images.composited.1080p.l/");
  colore kolor = colore::asamapink;
  string cprefix(prefix + dir + "pink/");
  string_vector input_files = populate_known_good(cprefix);

  // Output/Augment.
  const size_t nwidth(5);
  size_t fps = 30;

  // Watch for horizontal tearing, tricky.
  //  double sec = 5; // swipesec implies 384 pix/sec landscape
  //  double sec = 8; // swipesec implies 240 pix/sec landscape
  //  double sec = 9; // swipesec implies ~640 pix/sec landscape ie 21 pix/frame
  double sec = 16; // swipesec implies ~360 pix/sec landscape ie 12 pix/frame
  size_t framesn = fps * sec;

  const string base("gen-odtct");
  ostringstream ostr;
  ostr << get_output_directory() << base;
  string outprefix(ostr.str());

  // Assume 2 channel
# if 0
  // Portrait
  int cols = 1080;
  int rows = 1920;
#else
  // Landscape
  int cols = 1920;
  int rows = 1080;
#endif
  Rect r = { 0, 0, 2 * cols, rows };

  int totalhsz = 3 * cols;
  double stepsz = totalhsz / framesn;
  size_t n = 0; // total count

  for (size_t j = 0; j < input_files.size(); ++j)
    {
      string infile(input_files[j]);
      boost::filesystem::path pinput(infile);
      string mangle(pinput.stem().string());

      ostr.str("");
      ostr << svg::k::space << mangle;
      string outsuffixmn(ostr.str());

      // Swipe each image.
      for (size_t jj = 0; jj < framesn; ++jj)
	{
	  ostr.str("");
	  ostr << svg::k::space << setw(nwidth) << setfill('0') << n;
	  string outbasei(outprefix + ostr.str() + outsuffixmn);
	  ++n;

	  // SVG object.
	  svg_element obj = make_svg_2_channel(cols, rows, outbasei);

	  rect_element backg;
	  rect_element::data rbackg = { r.x, r.y, r.width, r.height };
	  style rsty = { kolor, 1.0, colore::white, 0.0, 0 };
	  backg.start_element();
	  backg.add_data(rbackg);
	  backg.add_style(rsty);
	  backg.finish_element();
	  obj.add_element(backg);

	  // Image is rightmost, and higest so as to ride over color.
	  image_element i;
	  int xoff = 2 * cols - ( jj  * stepsz);
	  image_element::data di = { infile, xoff, 0, cols, rows };
	  i.start_element();
	  i.add_data(di);
	  i.finish_element();
	  obj.add_element(i);
	}
    }
}


// 2 channel
// Only vector, color band roll
void
object_style_blink_2_channel()
{
  using namespace boost::filesystem;

  string dir("images.composited.1080p.l/");
  string prefix("/home/bkoz/src/MiL.asama-loops-2/images/landscape/" + dir);
  string colorf("blue");
  string positivef = prefix + "asama-oo-ee-oo-" + colorf + "-0-flush.png";
  string testf = prefix + "asama-oo-ee-oo-" + colorf + "-0-flush-flip.png";
  boost::filesystem::path ppos(positivef);
  boost::filesystem::path ptest(testf);

  // Output/Augment.
  const size_t nwidth(5);
  size_t fps = 30;

  const string base("gen-odtct");
  ostringstream ostr;
  ostr << get_output_directory() << base;
  string outprefix(ostr.str());

  string mangle("asama-oo-ee-oo-" + colorf + "-f");
  ostr.str("");
  ostr << svg::k::space << mangle;
  string outsuffixmn(ostr.str());

  // Assume 2 channel
# if 0
  // Portrait
  int cols = 1080;
  int rows = 1920;
#else
  // Landscape
  int cols = 1920;
  int rows = 1080;
#endif
  Rect r = { 0, 0, 2 * cols, rows };
  string_vector bv = blink_to_color_vector(r, colore::asamaorange, fps,
					   10, 7.0, 2, 0.5);

  for (size_t j = 0; j < bv.size(); ++j)
    {
      ostr.str("");
      ostr << svg::k::space << setw(nwidth) << setfill('0') << j;
      string outbasei(outprefix + ostr.str() + outsuffixmn);

      // SVG object.
      svg_element obj = make_svg_2_channel(cols, rows, outbasei);

      // Test image is rightmost, and lowest so that left swipes push it under.
      image_element i;
      image_element::data di = { testf, cols, 0, cols, rows };
      i.start_element();
      i.add_data(di);
      i.finish_element();
      obj.add_element(i);

      // Positive key image is leftmost, and on top.
      image_element ik;
      image_element::data dik = { positivef, 0, 0, cols, rows };
      ik.start_element();
      ik.add_data(dik);
      ik.finish_element();
      obj.add_element(ik);

      // Roll fade.
      rect_element blinkf;
      blinkf.str(bv[j]);
      obj.add_element(blinkf);
    }
}


// Only vector, one color band rolls up
void
object_style_vertical_band_1_channel()
{
  using namespace boost::filesystem;

  // Output/Augment.
  string dir("images.composited-v2.1080p.l/");

  // Input
  string prefix("/home/bkoz/src/MiL.asama-loops-3/image/" + dir);
  string_vector input_files = populate_known_good(prefix);
  cout << "input size: " << input_files.size() << endl;

  const string base("gen-odtct");
  ostringstream ostr;
  ostr << get_output_directory() << base;
  string outprefix(ostr.str());

  // Setup.
  size_t step = 5; // pix/frame step size

  const size_t nwidth(5);
  size_t fps = 30;

  // Assume 2 channel
# if 0
  // Portrait
  int cols = 1080;
  int rows = 1920;
#else
  // Landscape
  int cols = 1920;
  int rows = 1080;
#endif
  int blurr = 20;
  int blursz = 160;
  int solidsz = 80;
  double opac = 0.8;

  Rect r1 = { 0, 0 + blursz + blurr, cols, rows };
  string_vector vroll11 = vertical_sync_roll_vector(r1, colore::white,
						    fps, step,
						    blursz, solidsz, opac);

#if 0
  Rect r2 = { 0, 0 + blursz/2 + blurr, cols, rows };
  string_vector vroll21 = vertical_sync_roll_vector(r2, colore::asamaorange,
						    fps, step / 2,
						    blursz * 2, solidsz * 2,
						    opac / 2);
#endif

  for (size_t i = 0; i < input_files.size(); ++i)
    {
      string resfile(input_files[i]);
      boost::filesystem::path p1(resfile);

      string mangle(p1.stem().string());
      ostr.str("");
      ostr << mangle;
      string outsuffixmn(ostr.str());

      cout << i << svg::k::space << resfile << endl; // XXX
      cout << "size 1: " << vroll11.size() << endl;
      //cout << "size 2: " << vroll21.size() << endl;

      for (size_t j = 0; j < vroll11.size(); ++j)
	{
	  ostr.str("");
	  ostr << svg::k::space <<setw(nwidth) << setfill('0') << j;
	  string outbasei(outprefix + outsuffixmn + ostr.str());

	  // SVG object.
	  area<> a = { cols, rows };
	  svg_element obj(outbasei, a);

	  // Image.
	  image_element i;
	  image_element::data di = { resfile, 0, 0, cols, rows };
	  i.start_element();
	  i.add_data(di);
	  i.finish_element();
	  obj.add_element(i);

	  // Roll fade 1.
	  rect_element rollf11;
	  rollf11.str(vroll11[j]);
	  obj.add_element(rollf11);
	}

#if 0
      for (size_t j = 0; j < vroll21.size(); ++j)
	{
	  ostr.str("");
	  ostr << svg::k::space << "c"
	       << svg::k::space <<setw(nwidth) << setfill('0') << j;
	  string outbasei(outprefix + ostr.str() + outsuffixmn);

	  // SVG object.
	  area<> a = { unit::pixel, cols, rows };
	  svg_element obj(outbasei, a);

	  // Image.
	  image_element i;
	  image_element::data di = { resfile, 0, 0, cols, rows };
	  i.start_element();
	  i.add_data(di);
	  i.finish_element();
	  obj.add_element(i);

	  // Roll fade 2.
	  rect_element rollf21;
	  rollf21.str(vroll21[j]);
	  obj.add_element(rollf21);
	}
#endif
    }
}


// Only vector, one color band rolls up
//  string dir("images.composited-v2.1080p.l/");
//  string prefix("/home/bkoz/src/MiL.asama-loops-3/image/" + dir);

struct render_state
{
  int blurr;
  int blursz;
  int solidsz;
  double opac;
};

// step == pix/frame step size)
void
object_style_vertical_roji_band_1_channel(const string inputdir,
					  const size_t fps,
					  const double sec,
					  const double step,
					  ::render_state rs)
{
  using namespace boost::filesystem;

  // Output/Augment.
  string_vector input_files = populate_known_good(inputdir);
  cout << "roji base images input size: " << input_files.size() << endl;

  const string base("generated-roji-");
  ostringstream ostr;
  ostr << get_output_directory() << base;
  string outprefix(ostr.str());

  // Assume 2 channel
# if 0
  // Portrait
  int cols = 1080;
  int rows = 1920;
#else
  // Landscape
  int cols = 1920;
  int rows = 1080;
#endif


  // Dots
  static std::mt19937_64 rg(std::random_device{}());
  auto distb = std::uniform_int_distribution<>(0, 1);

  Rect rd = { 0, 0, cols, rows };
  string_vector vdot1 = optical_sound_dots_vector(rd, colore::white,
						  fps, sec, 50, 50, 20);
  string_vector vdot2 = optical_sound_dots_vector(rd, colore::black,
						  fps, sec / 2, 20, 33, 33);
  string_vector vdot3 = optical_sound_dots_vector(rd, colore::hellayellow,
						  fps, sec, 100, 80, 80);

  // Rolls
  cout << "roji roll (fps, sec, step): "
       << fps << " " << sec << " " << step << endl;
  Rect r1 = { 0, 0 + rs.blursz + rs.blurr, cols, rows };

  double rdist = (1080 + 200) / (fps * sec); // One swipe.
  string_vector vroll1 = vertical_sync_roll_vector(r1, colore::white,
						   fps, rdist, rs.blursz,
						   rs.solidsz, rs.opac);
  string_vector vroll1r(vroll1);
  std::reverse(vroll1r.begin(), vroll1r.end());


  string_vector vroll2e = vertical_sync_roll_vector(r1, colore::blue,
						   fps, rdist * 2,
						   rs.blursz * 1.5,
						   rs.solidsz * 1.25, rs.opac / 2);

  string_vector vroll3 = vertical_sync_roll_vector(r1, colore::black,
						   fps,
						   rdist / 2, rs.blursz * 2,
						   rs.solidsz, 0.4);

  // Half the time, double the distance.
  string_vector vroll2(vroll2e);
  copy(vroll2e.begin(), vroll2e.end(), back_inserter(vroll2));
  string_vector vroll2r(vroll2);
  std::reverse(vroll2r.begin(), vroll2r.end());
  cout << "roji roll 1: " << vroll1.size() << endl;
  cout << "roji roll 2: " << vroll2.size() << endl;
  cout << "roji roll 3: " << vroll3.size() << endl;

  const auto quarter1 = vroll1.size() / 4;
  const auto quarter4 = 3 * quarter1;
  for (string_vector::size_type i = 0; i < input_files.size(); ++i)
    {
      string resfile(input_files[i]);
      boost::filesystem::path p1(resfile);

      cout << i << svg::k::space << resfile << endl;

      ostringstream ostr;
      ostr << setw(5) << setfill('0') << i;
      string outbasei(outprefix + ostr.str());

      // SVG object.
      area<> a = { cols, rows };
      svg_element obj(outbasei, a);

      // Image.
      image_element img;
      image_element::data di = { resfile, 0, 0, cols, rows };
      img.start_element();
      img.add_data(di);
      img.finish_element();
      obj.add_element(img);

      // Rolls
      if (i < vroll3.size())
	{
	  // Roll fade 1.
	  rect_element rollf1;
	  rollf1.str(vroll3[i]);
	  obj.add_element(rollf1);

	  rect_element rollf1r;
	  rollf1r.str(vroll1r[i]);
	  obj.add_element(rollf1r);
	}

      if (i < vroll1.size())
	{
	  // Roll fade 1.
	  rect_element rollf1;
	  rollf1.str(vroll1[i]);
	  obj.add_element(rollf1);

	  rect_element rollf1r;
	  rollf1r.str(vroll1r[i]);
	  obj.add_element(rollf1r);
	}

#if 1
      ulong k = 0;
      if (i < vroll2.size() && i >= k + 75)
	{
	  // Roll fade 2.
	  rect_element rollf2;
	  rollf2.str(vroll2[k]);
	  obj.add_element(rollf2);

	  // Roll fade 2.
	  rect_element rollf2r;
	  rollf2r.str(vroll2r[k]);
	  obj.add_element(rollf2r);

	  ++k;
	}
#endif

      if (i < vroll2.size())
	{
	  // Roll fade 2.
	  rect_element rollf2;
	  rollf2.str(vroll2[i]);
	  obj.add_element(rollf2);

	  rect_element rollf2r;
	  rollf2r.str(vroll2r[i]);
	  obj.add_element(rollf2r);
	}


      // DOT
      // Steady every 1 sec
      const size_type steadysec = 4;
      const bool skip_till_p(i == 0 || !(i % (steadysec * fps)));
      if (i > quarter1 && i < quarter4)
	{
	  // Dot grid 1.

	  // For longer-duration dots.
	  string dot1s;
	  string dot2s;
	  string dot3s;
	  if (distb(rg) && i < vdot1.size())
	    {
	      string dot1p = vdot1[i];
	      if (skip_till_p)
		dot1s = dot1p;
	      circle_element dots1;
	      dots1.str(dot1s);
	      obj.add_element(dots1);
	    }

	  if (distb(rg) && i < vdot2.size())
	    {
	      string dot2p = vdot2[i];
	      if (skip_till_p)
		dot2s = dot2p;
	      circle_element dots2;
	      dots2.str(dot2s);
	      obj.add_element(dots2);
	    }
	  if (distb(rg) && i < vdot3.size())
	    {
	      string dot3p = vdot3[i];
	      if (skip_till_p)
		dot3s = dot3p;
	      circle_element dots3;
	      dots3.str(dot3s);
	      obj.add_element(dots3);
	    }
	}

    }
}


// 2 channel
// Only vector, one color band rolls up
void
object_style_vertical_band_2_channel()
{
  using namespace boost::filesystem;

  string colorp("pink");
  string dir("images.composited.1080p.l.orig/");
  string prefix("/home/bkoz/src/MiL.asama-loops-2/images/landscape/" + dir);
  string positivef = prefix + "asama-oo-ee-oo-" + colorp + "-0-flush.png";
  string testf = prefix + "asama-oo-ee-oo-" + colorp + "-0-flush-flip.png";
  boost::filesystem::path ppos(positivef);
  boost::filesystem::path ptest(testf);

  // Output/Augment.
  const size_t nwidth(5);
  size_t fps = 30;

  const string base("gen-odtct");
  ostringstream ostr;
  ostr << get_output_directory() << base;
  string outprefix(ostr.str());

  string mangle("asama-oo-ee-oo-" + colorp + "-f");
  ostr.str("");
  ostr << svg::k::space << mangle;
  string outsuffixmn(ostr.str());

  // Assume 2 channel
# if 0
  // Portrait
  int cols = 1080;
  int rows = 1920;
#else
  // Landscape
  int cols = 1920;
  int rows = 1080;
#endif
  Rect r = { 0, 0, 2 * cols, rows };
  string_vector v = vertical_sync_roll_vector(r, colore::asamaorange, fps, 6);

  for (size_t j = 0; j < v.size(); ++j)
    {
      ostr.str("");
      ostr << svg::k::space << setw(nwidth) << setfill('0') << j;
      string outbasei(outprefix + ostr.str() + outsuffixmn);

      // SVG object.
      svg_element obj = make_svg_2_channel(cols, rows, outbasei);

      // Test image is rightmost, and lowest so that left swipes push it under.
      image_element i;
      image_element::data di = { testf, cols, 0, cols, rows };
      i.start_element();
      i.add_data(di);
      i.finish_element();
      obj.add_element(i);

      // Positive key image is leftmost, and on top.
      image_element ik;
      image_element::data dik = { positivef, 0, 0, cols, rows };
      ik.start_element();
      ik.add_data(dik);
      ik.finish_element();
      obj.add_element(ik);

      // Roll fade.
      rect_element rollf;
      rollf.str(v[j]);
      obj.add_element(rollf);
    }
}


void
object_style_random_dot_grid_1_channel()
{
  using namespace boost::filesystem;

  string colorp("pink");
  string dir("images.composited-v2.1080p.l.all/");
  string prefix("/home/bkoz/src/MiL.asama-loops-3/image/" + dir);
  // string testf = prefix + "asama-oeo-v5-" + colorp + "-v-z.png";
  //  string testf = prefix + "asama-oeo-v5-" + colorp + "-buddah-z.png";
  //string testf = prefix + "asama-oeo-v5-" + colorp + "-mushroom-z.png";
    string testf = prefix + "asama-oeo-v5-" + colorp + "-xmegamerge-z.png";
  // string testf = prefix + "asama-oeo-v5-" + colorp + "-corner-z.png";
  boost::filesystem::path ptest(testf);

  // Output/Augment.
  const size_t nwidth(5);
  size_t fps = 30;
  double sec = 60;

  const string base("gen-odtct");
  ostringstream ostr;
  ostr << get_output_directory() << base;
  string outprefix(ostr.str());

  string mangle("asama-oo-ee-oo-" + colorp + "-f");
  ostr.str("");
  ostr << svg::k::space << mangle;
  string outsuffixmn(ostr.str());

  // Assume 1 channel
# if 0
  // Portrait
  int cols = 1080;
  int rows = 1920;
#else
  // Landscape
  int cols = 1920;
  int rows = 1080;
#endif
  Rect r = { 0, 0, cols, rows };
  string_vector v = optical_sound_dots_vector(r, colore::asamaorange, fps, sec);

  for (size_t j = 0; j < v.size(); ++j)
    {
      ostr.str("");
      ostr << svg::k::space << setw(nwidth) << setfill('0') << j;
      string outbasei(outprefix + ostr.str() + outsuffixmn);

      // SVG object.
      area<> a = { cols, rows };
      svg_element obj(outbasei, a);

      // Positive image is lowest.
      image_element i;
      image_element::data di = { testf, 0, 0, cols, rows };
      i.start_element();
      i.add_data(di);
      i.finish_element();
      obj.add_element(i);

      // Dot grid.
      circle_element dots;
      dots.str(v[j]);
      obj.add_element(dots);
    }
}


void
object_style_fade_to_wink_fade_from_1_channel(const string inputdir,
					      const size_t fps,
					      const double sec,
					      ::render_state rs)
{
  using namespace boost::filesystem;
  using size_type = string_vector::size_type;

 // Output/Augment.
  string_vector input_files = populate_known_good(inputdir);
  cout << "roji base images input size: " << input_files.size() << endl;

  const string base("generated-roji");
  ostringstream ostr;
  ostr << get_output_directory() << base;
  string outprefix(ostr.str());

  // Assume 2 channel
# if 0
  // Portrait
  int cols = 1080;
  int rows = 1920;
#else
  // Landscape
  int cols = 1920;
  int rows = 1080;
#endif

  size_type count = 0;

  // 2.5 sec fade in, fade out vector mattes.
  double fadesec = 2.5;
  Rect r = { 0, 0, cols, rows };
  string_vector fto = fade_to_color_vector(r, colore::white, fps, fadesec);
  string_vector ffrom(fto);
  std::reverse(ffrom.begin(), ffrom.end());

  // Build up a 25 sec repitore of blinks, copy to sum blinks.
  string_vector winkv;
  string_vector winkv1 = wink_to_color_vector(r, colore::white, fps,
					      5, 2, 100.0, 1);
  string_vector winkv2 = wink_to_color_vector(r, colore::blue, fps,
					      5, 3, 66.0, 0.5);
  string_vector winkv2r(winkv2);
  std::reverse(winkv2r.begin(), winkv2r.end());

  string_vector winkv3 = wink_to_color_vector(r, colore::white, fps,
					      5, 1, 95.0, 2);
  std::copy(winkv1.begin(), winkv1.end(), std::back_inserter(winkv));
  std::copy(winkv2.begin(), winkv2.end(), std::back_inserter(winkv));
  std::copy(winkv2r.begin(), winkv2r.end(), std::back_inserter(winkv));
  std::copy(winkv3.begin(), winkv3.end(), std::back_inserter(winkv));
  std::copy(winkv3.begin(), winkv3.end(), std::back_inserter(winkv));
  cout << "roji wink size: " << winkv.size() << endl;

  // Fade in
  string startimg(input_files.front());
  for (size_type i = 0; i < ffrom.size(); ++i)
    {
      ostr.str("");
      ostr << svg::k::space << setw(5) << setfill('0') << count++;
      string outbasei(outprefix + ostr.str());

      // SVG object.
      area<> a = { cols, rows };
      svg_element obj(outbasei, a);

      // Image
      image_element img;
      image_element::data di = { startimg, 0, 0, cols, rows };
      img.start_element();
      img.add_data(di);
      img.finish_element();
      obj.add_element(img);

      // Vector Layer
      rect_element r;
      r.str(ffrom[i]);
      obj.add_element(r);
    }

  // Wink
  for (size_type i = 0; i < input_files.size(); ++i)
    {
      ostr.str("");
      ostr << svg::k::space << setw(5) << setfill('0') << count++;
      string outbasei(outprefix + ostr.str());

      // SVG object.
      area<> a = { cols, rows };
      svg_element obj(outbasei, a);

      // Positive image is lowest.
      image_element img;
      image_element::data di = { input_files[i], 0, 0, cols, rows };
      img.start_element();
      img.add_data(di);
      img.finish_element();
      obj.add_element(img);

      // Vector Layer (wink)
      if (i < winkv.size())
	{
	  rect_element r;
	  r.str(winkv[i]);
	  obj.add_element(r);
	}
    }

  // Fade out
  string endimg(input_files.back());
  for (size_type i = 0; i < fto.size(); ++i)
    {
      ostr.str("");
      ostr << svg::k::space << setw(5) << setfill('0') << count++;
      string outbasei(outprefix + ostr.str());

      // SVG object.
      area<> a = { cols, rows };
      svg_element obj(outbasei, a);

      // Image
      image_element img;
      image_element::data di = { endimg, 0, 0, cols, rows };
      img.start_element();
      img.add_data(di);
      img.finish_element();
      obj.add_element(img);

      // Vector Layer
      rect_element r;
      r.str(fto[i]);
      obj.add_element(r);
    }
}


// 1 channel
// Only vector, sync band vert rolls, ie old television recording stylistic.
//
// Simulated vertical roll between two or more frames, sync, pure
// color with fades.  r == frame size
// Assume single-channel, ie svg base coordinates are frame-size.
//
// Sketch.
//
// input image 1, image 2
// input color of vector rectangle/fade that will "roll"
// input speed of roll? duration of roll? trajectory?
//
// start frame 1, no effects for editing continunity
// roll up color
// roll up second color
// roll up frame 2
// roll up third color
// roll up frame 3
// roll up forth color
void
object_style_vertical_sync_roll_1_channel()
{
  using namespace boost::filesystem;
  using size_type = string_vector::size_type;

  string colorp("pink");
  string dir("images.composited-v2.1080p.l/");
  string prefix("/home/bkoz/src/MiL.asama-loops-3/image/" + dir);
  string_vector input_files = populate_known_good(prefix);

  cout << "input size: " << input_files.size() << endl;

  // Watch for vertical tearing, tricky but not has hard as horizontal.
  // Some basics:
  // 1080 pixels means
  // 06 pix/frame x 30 frames/sec == 180 pixels/sec -> 13.5 sec to scroll
  // 08 pix/frame x 30 frames/sec == 240 pixels/sec -> 4.5 sec to scroll
  // 10 pix/frame x 30 frames/sec == 300 pixels/sec -> 3.6 sec to scroll
  // 16 pix/frame x 30 frames/sec == 480 pixels/sec -> 2.25 sec to scroll
  // 18 pix/frame x 30 frames/sec == 540 pixels/sec -> 2.0 sec to scroll
  // 20 pix/frame x 30 frames/sec == 600 pixels/sec -> 1.8 sec to scroll
  auto step = 6; // pix/frame step size

  // Output/Augment.
  const size_t nwidth(5);
  size_t fps = 30;
  double sec = 800; // 8 step
  //  double sec = 300; // 10 step
  size_t framesn = fps * sec;

  const string base("gen-odtct");
  ostringstream ostr;
  ostr << get_output_directory() << base;
  string outprefix(ostr.str());

  string mangle("asama-oo-ee-oo-" + colorp + "-f");
  ostr.str("");
  ostr << svg::k::space << mangle;
  string outsuffixmn(ostr.str());

  // Assume 1 channel
# if 0
  // Portrait
  int cols = 1080;
  int rows = 1920;
#else
  // Landscape
  int cols = 1920;
  int rows = 1080;
#endif

  // For the dots, a couple of solutions.
  //
  // 1
  // full frame, 1920x1080 landscape baselines.
  // 8 wide, 4 high
  // 160 pixel diameter, start at x 40, y 100, move 240.


  // 2
  // can break the frame into a grid of four.
  // 0000 x 0480 x 0960 x 1440 x 1920
  // To get the edges of a 1920 pixel wide frame, use
  //   left  = 0000 - 0480
  //   right = 1440 - 1920
  //
  // -> 80 pixel radius, 240 space, xi = 0,

  Rect rd = { 0, 0, cols, rows };
#if 0
  // 1 Full
  string_vector vdot1 = optical_sound_dots_vector(rd, colore::white, fps, sec,
						  40, 16, 8);
  string_vector vdot2 = optical_sound_dots_vector(rd, colore::black,
						  fps, sec, 20, 32, 16);
#else
  // 2 Grid Variant
  static std::mt19937_64 rg(std::random_device{}());
  auto distb = std::uniform_int_distribution<>(0, 1);
  string_vector vdot1 = optical_sound_dots_vector(rd, colore::black,
						  fps, sec, 20, 8, 16, 0);
  string_vector vdot2 = optical_sound_dots_vector(rd, colore::white,
						  fps, sec, 80, 2, 4, 0);
  string_vector vdot3 = optical_sound_dots_vector(rd, colore::black,
						  fps, sec, 20, 8, 16, 1440);
  string_vector vdot4 = optical_sound_dots_vector(rd, colore::white,
						  fps, sec, 40, 4, 8, 1440);
#endif
  int blursz = 160;
  int solidsz = 80;
  double opac = 0.7;

  Rect r1 = { 0, 0, cols, rows };
  Rect r2 = { 0, 0 - rows, cols, rows };
  Rect r3 = { 0, 0 + rows, cols, rows };
  string_vector vroll11 = vertical_sync_roll_vector(r1, colore::white,
						    fps, step,
						    blursz, solidsz, opac);
  string_vector vroll12 = vertical_sync_roll_vector(r2, colore::white,
						    fps, step,
						    blursz, solidsz, opac);
  string_vector vroll13 = vertical_sync_roll_vector(r3, colore::white,
						    fps, step,
						    blursz, solidsz, opac);

  string_vector vroll21 = vertical_sync_roll_vector(r1, colore::asamaorange,
						    fps, step,
						    blursz / 2, solidsz /2,
						    opac);
  string_vector vroll22 = vertical_sync_roll_vector(r2, colore::asamaorange,
						    fps, step,
						    blursz / 2, solidsz / 2,
						    opac);
  string_vector vroll23 = vertical_sync_roll_vector(r3, colore::asamaorange,
						    fps, step,
						    blursz / 2, solidsz / 2,
						    opac);

  // Here, i is the index to raster frames.
  // This may run over specified time, but not overly so.
  size_t currentf = 1; // Should not be more than maxframes.
  for (size_type i = 0; i + 1 < input_files.size() && currentf < framesn; ++i)
    {
      string resfile(input_files[i]);
      boost::filesystem::path p1(resfile);

      string syncfile(input_files[i + 1]);
      boost::filesystem::path p2(syncfile);

      for (size_type j = 0; size_type(j * step) <= size_type(rows); j++)
	{
	  ostr.str("");
	  ostr << svg::k::space << setw(nwidth) << setfill('0') << currentf;
	  string outbasei(outprefix + ostr.str() + outsuffixmn);

	  // SVG object.
	  area<> a = { cols, rows };
	  svg_element obj(outbasei, a);

	  int offset = j * step;

	  // Resident image is lowest.
	  image_element i;
	  image_element::data di = { resfile, 0, 0 - offset, cols, rows };
	  i.start_element();
	  i.add_data(di);
	  i.finish_element();
	  obj.add_element(i);

	  // Sync image is next.
	  image_element i2;
	  image_element::data di2 = { syncfile, 0, 0 + rows - offset, cols, rows };
	  i2.start_element();
	  i2.add_data(di2);
	  i2.finish_element();
	  obj.add_element(i2);

	  // Slow down dot grid
	  if (j * step < rows / 2.5)
	    {
	      // Dot grid 1.
	      if (distb(rg))
		{
		  circle_element dots1;
		  dots1.str(vdot1[currentf]);
		  obj.add_element(dots1);
		}
	      if (distb(rg))
		{

		  circle_element dots2;
		  dots2.str(vdot1[currentf]);
		  obj.add_element(dots2);
		}

	      // Dot grid 2.
	      if (distb(rg))
		{
		  circle_element dots1;
		  dots1.str(vdot3[currentf]);
		  obj.add_element(dots1);
		}
	      if (distb(rg))
		{
		  circle_element dots2;
		  dots2.str(vdot4[currentf]);
		  obj.add_element(dots2);
		}
	    }

	  // Roll fade 1.
	  if (j < vroll11.size())
	    {
	      rect_element rollf11;
	      rollf11.str(vroll11[j]);
	      obj.add_element(rollf11);

	      rect_element rollf12;
	      rollf12.str(vroll12[j]);
	      obj.add_element(rollf12);

	      rect_element rollf13;
	      rollf13.str(vroll13[j]);
	      obj.add_element(rollf13);
	    }

	  // Roll fade echo 1
	  if (j < vroll21.size())
	    {
	      rect_element rollf21;
	      rollf21.str(vroll21[j]);
	      obj.add_element(rollf21);

	      rect_element rollf22;
	      rollf22.str(vroll22[j]);
	      obj.add_element(rollf22);

	      rect_element rollf23;
	      rollf23.str(vroll23[j]);
	      obj.add_element(rollf23);
	    }

	  ++currentf;
	  cout << currentf << " of " << framesn << " by " << j << endl;
	}
    }
}


#endif // MiL_SVG_INSCRIBE_H
