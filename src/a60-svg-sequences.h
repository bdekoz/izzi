// -*- mode: C++ -*-

// Copyright (C) 2014-2020 Benjamin De Kosnik <b.dekosnik@gmail.com>

// This file is part of the alpha60-MiL SVG library.  This library is
// free software; you can redistribute it and/or modify it under the
// terms of the GNU General Public License as published by the Free
// Software Foundation; either version 3, or (at your option) any
// later version.

// This library is distributed in the hope that it will be useful, but
// WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// General Public License for more details.

#ifndef a60_SVG_SEQUENCES_H
#define a60_SVG_SEQUENCES_H 1

#include <sstream> //stringstream
#include <fstream>
#include <string>
#include <tuple>
#include <array>
#include <random>
#include <unistd.h>

#include "a60-svg.h"


namespace svg {

/**
   Fade rectangle fill from transparent to color, of duration sec, given fps
   starts on white (klr), fades to klr color in sec
   can use with image behind
*/
strings
fade_to_color_seq(const rect_element::data& dr, const color klr,
		  size_t fps = 30, double sec = 1.0, double maxopac = 1.0)
{
  // Calc number of frames needed.
  size_t framesn = fps * sec;
  double step = maxopac / framesn;

  // Start rectangle as transparent, fade to color.
  strings ret;
  style sty = { klr, 0.0, klr, 0.0, 0 };
  for (size_t i = 0; i < framesn; ++i)
    {
      // Foreground rectangle.
      rect_element rsvg;

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


/// Start rectangle as transparent, fade to color.
strings
fade_from_color_seq(const rect_element::data& dr, const color klr,
		    size_t fps = 30, double sec = 1.0, double minopac = 0)
{
  strings ret = fade_to_color_seq(dr, klr, fps, sec, minopac);
  std::reverse(ret.begin(), ret.end());
  return ret;
}


/// blink sequence of duration sec, given fps
/// starts on background (backgf)
/// at duration twhen
/// blinks specific frame (blinkf) times (nblinks) for seconds (blinksec)
/// can use with image behind
strings
blink_to_color_seq(const rect_element::data& dr, const color klr,
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

  strings ret;
  double opaque(1.0);
  double transparent(0.0);
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


/// wink sequence of duration sec, given fps to color klr
/// starts on background transparent, so can use with an image behind.
/// at duration twhen
/// winks specific rectangle (r) this much (maxclose) for seconds (winksec)
strings
wink_to_color_seq(const rect_element::data& dr, const color klr,
		  size_t fps, double sec,
		  double twhen, double maxclose, double winksec)
{
  const int rows = dr._M_height;

  // Calc number of frames needed.
  size_t framesn = fps * sec;
  size_t startn = fps * twhen;
  size_t winkframesn = (fps * winksec) + 1;
  size_t winkframeshalfn = winkframesn / 2;

  // End with backgf.
  if (startn + winkframesn >= framesn)
    throw std::logic_error("wink:: desired frames exceed total duration");

  strings ret;
  double opaque(1.0);
  double transparent(0.0);
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

  // Make two "eyelid" rectangles, one upper and the other lower.
  double ymid = dr._M_height / 2;
  double ymax = ymid * maxclose;

  // grow down
  rect_element::data dhi = { dr._M_x_origin, dr._M_y_origin, dr._M_width, 0 };

  // grow up
  rect_element::data dlo = { dr._M_x_origin, dr._M_y_origin + rows,
			     dr._M_width, 0 };

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
      dlo._M_y_origin = dr._M_y_origin + rows - offsetn;
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
      dlo._M_y_origin = dr._M_y_origin + rows - offsetn;
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


/// Simulated vertical roll, with fades.
/// r == frame size
strings
vertical_sync_roll_seq(const rect_element::data& drin, const color klr,
		       size_t /*fps = 30*/, double step = 10,
		       int blursz = 200, int solidsz = 133, double opac = 0.6)
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
  double x = (drin._M_height + (2 * blursz) + (3 * blurr)) / step;
  size_t framesn = static_cast<size_t>(x);

  style sty = { klr, 1.0, klr, 0.0, 0 };

  // rect_elementangles start at bottom off-screen and sweep up.
  // SVG rect area is rect 00 top left
  // SVG rect is drawn down and left from starting position.

  // Blur rectangles. The SVG gaussian blur gives a hard edge on the
  // top corner. So, to have an even fade, double it and center under
  // the solid rectangle.
  rect_element::data drblurb(drin);
  drblurb._M_y_origin = drin._M_y_origin + drin._M_height;
  drblurb._M_height = blursz;

  rect_element::data drblurt(drin);
  drblurt._M_y_origin = drin._M_y_origin + drin._M_height - blursz;
  drblurt._M_height = blursz;

  // Solid rectangle.
  rect_element::data drs(drin);
  drs._M_y_origin = drin._M_y_origin + drin._M_height - vsolidsz;
  drs._M_height = solidsz;

  strings ret;
  for (size_t i = 0; i < framesn; ++i)
    {
      double stepn = i * step;

      // Dual blur.
      sty._M_fill_opacity = opac;

      rect_element rsvg1;
      size_type yblurb = static_cast<int>(drblurb._M_y_origin - stepn);
      rsvg1.start_element();
      rsvg1.add_data(drblurb);
      rsvg1.add_filter("20y");
      rsvg1.add_style(sty);
      rsvg1.finish_element();

      rect_element rsvg3;
      // size_type yblurt = static_cast<int>(drblurt._M_y_origin - stepn);
      rsvg3.start_element();
      rsvg3.add_data(drblurt);
      rsvg3.add_filter("20y");

      std::ostringstream ostrt;
      ostrt << "rotate(180, 960, " << yblurb - vblursz << ")";
      rsvg3.add_transform(ostrt.str());
      rsvg3.add_style(sty);
      rsvg3.finish_element();

      string scene(rsvg1.str() + rsvg3.str());

      // Foreground rectangle.
      sty._M_fill_opacity = opac + 0.20;
      rect_element rsvgfg;
      // size_type y = static_cast<int>(drs._M_y_origin - stepn);
      rsvgfg.start_element();
      rsvgfg.add_data(drs);
      rsvgfg.add_style(sty);
      rsvgfg.finish_element();

      ret.push_back(scene + rsvgfg.str());
    }

  return ret;
}


/// Randomly create grid of [maxwidth] x [maxheight] grid, and fill with dots.
string
dot_grid_seq(const rect_element::data& drin, const color klr,
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
  size_type yinit = drin._M_height - radius - ystart;

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


/**
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
strings
optical_sound_dots_seq(const rect_element::data& drin, const color klr,
		       size_t fps = 30, double sec = 1.0,
		       const int radius = 80,
		       const int maxw = 8, const int maxh = 4,
		       const int xstart = 40, const int ystart = 100)
{
  strings ret;
  size_t framesn = fps * sec;   // Calc number of frames needed.
  size_t step_size = 4 + 17 + 4;
  for (size_t i = 1; i + step_size < framesn; i += step_size)
    {
      string dotpattern1 = dot_grid_seq(drin, klr, radius, maxw, maxh,
					xstart, ystart);
      ret.push_back(dotpattern1);
      string dotpattern2 = dot_grid_seq(drin, klr, radius, maxw, maxh,
					xstart, ystart);
      ret.push_back(dotpattern2);

      string empty;
      ret.insert(ret.end(), 17, empty);

      string dotpattern3 = dot_grid_seq(drin, klr, radius, maxw, maxh,
					xstart, ystart);
      ret.push_back(dotpattern3);
      string dotpattern4 = dot_grid_seq(drin, klr, radius, maxw, maxh,
					xstart, ystart);
      ret.push_back(dotpattern4);
    }

  return ret;
}


/// 1 channel
/// start with image, background color is klr
/// each "frame" == string in returned strings has an image and a background
strings
swipe_left_seq(const rect_element::data& drin, const string imgf,
	       const color klr = color::white, size_t fps = 30, double sec = 9)
{
  // Watch for horizontal tearing, tricky.
  // double sec = 5; // swipesec implies 384 pix/sec landscape
  // double sec = 8; // swipesec implies 240 pix/sec landscape
  // double sec = 9; // swipesec implies ~640 pix/sec landscape ie 21 pix/frame
  const style styl = { klr, 1.0, klr, 0.0, 0 };
  const size_t framesn = fps * sec;
  const auto width = drin._M_width;
  const auto offset(width / framesn);

  strings ret;
  for (size_t i = 0; i < framesn; ++i)
    {
      rect_element r;
      r.start_element();
      r.add_data(drin);
      r.add_style(styl);
      r.finish_element();

      int x = 0 - (i * offset);
      image_element img;
      image_element::data di = { imgf, x, 0, width, drin._M_height };
      img.start_element();
      img.add_data(di);
      img.finish_element();

      ret.push_back(r.str() + img.str());
    }
  return ret;
}

} // namespace svg

#endif
