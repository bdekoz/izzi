// alpha60 vector titles, legends, and glyphs (svg) -*- mode: C++ -*-
// alpha60
// bittorrent x scrape x data + analytics

// Copyright (c) 2016-2019, Benjamin De Kosnik <b.dekosnik@gmail.com>

// This file is part of the alpha60 library.  This library is free
// software; you can redistribute it and/or modify it under the terms
// of the GNU General Public License as published by the Free Software
// Foundation; either version 3, or (at your option) any later
// version.

// This library is distributed in the hope that it will be useful, but
// WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// General Public License for more details.

#ifndef a60_SVG_TITLES_AND_LEGENDS_H
#define a60_SVG_TITLES_AND_LEGENDS_H 1

#include "a60-io.h"

namespace a60
{
  using namespace svg;

string
strip_dir_to_timestamp(string dir, string match)
{
  string trimdir(dir);
  auto dtrimpos = trimdir.find(match);
  if (dtrimpos != string::npos)
    trimdir = trimdir.substr(dtrimpos + match.size());
  return trimdir;
}


const string
collection_size_and_partition(const collection& cll, const glyphs& glys)
{
  std::ostringstream oss;
  if (glys.size() == 1)
      oss << "ONE TORRENT";
  else
    oss << std::to_string(glys.size()) << " TORRENTS";
  if (!cll.partition_name().empty())
    oss << " IN [ " << cll.partition_name() << " ] PARTITION";
  return oss.str();
}


/// Basic torrent collection title
void
augment_collection_titles_1(svg_element& obj, svg::typography typo,
			    string titlestr, uint xoffs = 0)
{
  // Typographic defaults.
  svg::typography typotxt = typo;
  typotxt._M_a = svg::typography::anchor::start;
  typotxt._M_align = svg::typography::align::left;
  typotxt._M_w = svg::typography::weight::medium;
  typotxt._M_p = svg::typography::property::normal;

  int height = obj._M_area._M_height;

  // Media title.
  svg::typography typog = typotxt;
  typog._M_style._M_fill_color = svg::colore::gray50;
  sized_text(obj, typog, pt_to_px(24), titlestr,
		  carto::frame_margin() + xoffs,
		  height - carto::frame_margin());
}


/// Whole earth, 1x, summary style (classic)
void
augment_collection_titles_2(svg_element& obj, svg::typography typo,
			    string title, string dir, glyphs& glys,
			    swarm_mode m, const collection& cll)
{
  // Typographic defaults.
  svg::typography typotxt = typo;
  typotxt._M_a = svg::typography::anchor::start;
  typotxt._M_align = svg::typography::align::left;
  typotxt._M_w = svg::typography::weight::medium;
  typotxt._M_p = svg::typography::property::normal;

  const auto [ width, height ] = obj._M_area;

  // Media title sans slice.
  svg::typography typog = typotxt;
  typog._M_style._M_fill_color = svg::colore::gray50;
  string motitle = cll.media_object_name(false);

  int fmargin = carto::frame_margin();
  if (cll.matchslice.empty())
    {
      // Big title is media object.
      sized_text(obj, typog, pt_to_px(64), motitle,
		       fmargin, height - fmargin);
    }
  else
    {
      // Big title is slice name, postfix.
      string sname = cll.matchslice;
      if (!cll.postfix.empty())
	sname += " " + cll.postfix;
      sized_text(obj, typog, pt_to_px(64), sname, fmargin,
		       height - fmargin);

      // Small title is media object.
      typog._M_a = svg::typography::anchor::end;
      typog._M_align = svg::typography::align::right;
      sized_text_r(obj, typog, pt_to_px(14), motitle,
			 fmargin, height - (2 * fmargin), svg::transform(), 90);

    }

  // Summary numbers.
  augment_sum_glyphs_2(obj, typo, glys, m, width - fmargin,
		       height - fmargin);

  auto yoff = fmargin + pt_to_px(54)/2 + 10;

  // Time information.
  string trimdir = strip_dir_to_timestamp(dir, cll.match);
  svg::typography typoo = typotxt;
  typoo._M_style = svg::k::w_style;
  typoo._M_style._M_stroke_opacity = 1.0;
  typoo._M_style._M_stroke_size = 0.3;
  typoo._M_a = svg::typography::anchor::end;
  typoo._M_w = svg::typography::weight::normal;
  sized_text(obj, typoo, pt_to_px(54), trimdir, width - fmargin, yoff);

  // Title.
  svg::typography typov = typotxt;
  typov._M_a = svg::typography::anchor::end;
  typov._M_w = svg::typography::weight::xbold;
  sized_text(obj, typov, pt_to_px(12), title, width - fmargin, yoff + 22);
}


void
augment_collection_color_bar(svg_element& obj, swarm_mode m, svg::colore klr)
{
  bool peerp = m == swarm_mode::peer;

  const auto [ width, height ] = obj._M_area;

  // Put down solid color on bottom edge.
  rect_element rsvg;
  int fmargin = carto::frame_margin();
  rect_element::data dr = { 0, height - fmargin, width, fmargin };

  svg::style styl = peerp ? svg::k::b_style : svg::k::w_style;
  styl._M_fill_opacity = 1.0;
  styl._M_fill_color = klr;

  rsvg.start_element();
  rsvg.add_data(dr);
  rsvg.add_style(styl);
  rsvg.finish_element();
  obj.add_element(rsvg);
}


// Viz-65 flip-book style.
void
augment_collection_titles_3(svg_element& obj, svg::typography typo,
			    string title, string dir, glyphs& glys,
			    swarm_mode m, const collection& cll)
{
  // Typographic defaults.
  svg::typography typotxt = typo;
  typotxt._M_a = svg::typography::anchor::start;
  typotxt._M_align = svg::typography::align::left;
  typotxt._M_w = svg::typography::weight::medium;
  typotxt._M_p = svg::typography::property::normal;

  const auto [ width, height ] = obj._M_area;
  int fmargin = carto::frame_margin();

  // Visualization title.
  svg::typography typov = typotxt;
  typov._M_a = svg::typography::anchor::start;
  typov._M_w = svg::typography::weight::xbold;
  sized_text(obj, typov, pt_to_px(30), title, fmargin, height - fmargin);


  // Media title.
  svg::typography typog = typotxt;
  typog._M_style._M_fill_color = svg::colore::gray25;
  sized_text_r(obj, typog, pt_to_px(64), cll.media_object_name(),
		     fmargin, fmargin, svg::transform(), 90);

  // Summary numbers.
  augment_sum_glyphs_2(obj, typo, glys, m, width - fmargin,
		       height - fmargin);

  // Time information.
  string trimdir = strip_dir_to_timestamp(dir, cll.match);
  svg::typography typoo = typotxt;
  typoo._M_style = svg::k::b_style;
  typoo._M_a = svg::typography::anchor::middle;
  typoo._M_w = svg::typography::weight::xlight;
  sized_text(obj, typoo, pt_to_px(24), trimdir, width/2,
		   height - fmargin);
}


// Smashed minimial style
// by-* titles, filename, partition
void
augment_collection_titles_4(svg_element& obj, svg::typography typo,
			    string title, const collection& cll)
{
  // Typographic defaults.
  svg::typography typotxt = typo;
  typotxt._M_a = svg::typography::anchor::middle;
  typotxt._M_align = svg::typography::align::center;
  typotxt._M_w = svg::typography::weight::medium;
  typotxt._M_p = svg::typography::property::normal;

  int height = obj._M_area._M_height;
  int fmargin = carto::frame_margin();

  // Media title.
  // +28 is top of viz title
  svg::typography typog = typotxt;
  typog._M_w = svg::typography::weight::xbold;
  typog._M_style._M_stroke_color = svg::colore::gray40;
  typog._M_style._M_stroke_size = 0.5;
  typog._M_style._M_stroke_opacity = 1.0;
  typog._M_style._M_fill_opacity = 0.0;
  sized_text_r(obj, typog, pt_to_px(54), cll.media_object_name(),
		     fmargin + 36, height/2, svg::transform(), 90);

  // Visualization title.
  svg::typography typov = typotxt;
  typov._M_w = svg::typography::weight::xbold;
  sized_text_r(obj, typov, pt_to_px(30), title, fmargin, height/2,
		     svg::transform(), 90);
}


// By sample, by hour style.
// Media property and episode, date and time stamp with PST time zone.
void
augment_collection_titles_5(svg_element& obj, svg::typography typo,
			    string titlestr, uint xoffs = 0)
{
  // Typographic defaults.
  svg::typography typotxt = typo;
  typotxt._M_a = svg::typography::anchor::start;
  typotxt._M_align = svg::typography::align::left;
  typotxt._M_w = svg::typography::weight::medium;
  typotxt._M_p = svg::typography::property::normal;

  int height = obj._M_area._M_height;
  int fmargin = carto::frame_margin();

  // Time information.
  svg::typography typoo = typotxt;
  typoo._M_w = svg::typography::weight::xlight;
  typoo._M_a = svg::typography::anchor::end;
  typoo._M_align = svg::typography::align::right;

  sized_text_r(obj, typoo, pt_to_px(16), titlestr,
		     fmargin + xoffs, height - fmargin - carto::fspacer,
		     svg::transform(), 90);
}


// Ovoid, topn style
void
augment_collection_titles_6(svg_element& obj, const cached_collection& wcll)
{
  string smode = to_string(wcll.sort_mode);
  bool peerp = wcll.sort_mode == swarm_mode::peer;
  const auto [ width, height ] = obj._M_area;
  int fmargin = carto::frame_margin();

  // Common typographics.
  svg::typography typo = svg::k::apercu_typo;
  typo._M_style = peerp ? svg::k::b_style : svg::k::w_style;
  typo._M_align = svg::typography::align::left;
  typo._M_p = svg::typography::property::normal;
  typo._M_a = svg::typography::anchor::end;
  typo._M_w = svg::typography::weight::xbold;
  typo._M_style._M_fill_color = svg::colore::gray25;

  // Media title.
  sized_text_r(obj, typo, pt_to_px(30), wcll.media_object_name(),
		     fmargin, height - fmargin, svg::transform(), 90);

  // Datestamp.
  sized_text_r(obj, typo, pt_to_px(30), wcll.datestamp,
		     width - fmargin - pt_to_px(30)/2,
		     height - fmargin, svg::transform(), 90);
}


void
augment_collection_titles(svg_element& obj, svg::typography typo, string title,
			  string dir, glyphs& glys, swarm_mode m,
			  const collection& cll)
{ augment_collection_titles_2(obj, typo, title, dir, glys, m, cll); }



auto
corner_logo_size()
{
  // 1x
  //auto iw = 2092;
  //auto ih = 292;

  // .5x
  //auto iw = 1046;
  //auto ih = 146;

  // .25x
  //auto iw = 523;
  //auto ih = 73;

  // .125x
  //auto iw = 261;
  //auto ih = 36;

  // .1x
  auto iw = 209;
  auto ih = 29;

  return std::make_tuple(iw, ih);
}

void
augment_corner_logo(svg_element& obj, cached_collection& wcll,
		    const int xoff = carto::frame_margin())
{
  // Alpha60 / We Scrape U Logo
  const auto height = obj._M_area._M_height;

  const auto [iw, ih] = corner_logo_size();

  string a60file("identity/a60-logo-scrapeu-smash-x");
  if (wcll.sort_mode == swarm_mode::seed)
    a60file += "-inverse";

  auto& rtr = io::get_run_time_resources();
  const string ifile(io::end_path(rtr.data) + a60file + ".svg");

  auto yoff = height - carto::frame_margin();
  image_element i;
  image_element::data di = { ifile, xoff, yoff, iw, ih };
  i.start_element();
  i.add_data(di);
  i.finish_element();
  obj.add_element(i);
}

/// Make weighed title.
void
augment_weighed_title(svg_element& obj, cached_collection& wcll, bool lp,
		      const string prefix = "")
{
  // Swarm type.
  swarm_mode mode = wcll.sort_mode;
  string smode = to_string(mode);
  bool peerp = mode == swarm_mode::peer;

  // Common typographics.
  svg::typography typo = svg::k::apercu_typo;
  typo._M_a = svg::typography::anchor::middle;
  typo._M_style = peerp ? svg::k::b_style : svg::k::w_style;

  // Legends
  glyphs g1 = make_glyphs(wcll);
  glyphs g2 = remove_empty_glyphs(g1, mode);
  if (lp)
    augment_torrent_collection_glyphs_v3(obj, typo, g2, mode);

  // Unique peer/seed and prefix.
  string modprefix;
  if (!prefix.empty())
    {
      std::ostringstream oss;
      oss << prefix << " ";
      oss << "UNIQUE ";
      for (char& c: smode)
	oss << std::toupper(c, std::locale(""));
      oss << "S FOR ";
      modprefix = oss.str();
    }

  string sub = modprefix + collection_size_and_partition(wcll, g2);
  augment_collection_titles_2(obj, typo, sub, wcll.datestamp, g2,
			      wcll.sort_mode, wcll);
}


/// Demi- and Sub-titles: maps details, logos, sample details.
void
augment_weighed_demititle(svg_element& obj, cached_collection& wcll,
			  const string d)
{
  const auto height = obj._M_area._M_height;
  const int fmargin = carto::frame_margin();

  using svg::k::b_style;
  using svg::k::w_style;
  svg::typography typo = svg::k::apercu_typo;
  typo._M_p = svg::typography::property::normal;
  typo._M_w = svg::typography::weight::bold;
  typo._M_style = wcll.sort_mode == swarm_mode::peer ? b_style : w_style;

  svg::typography typotxt = typo;
  // typotxt._M_style._M_stroke_opacity = 0.8;
  // typotxt._M_style._M_stroke_size = 0.283;
  // typotxt._M_style._M_stroke_size = 0.5;
  // typotxt._M_style._M_stroke_size = 2.0;

  // Title.
  sized_text_r(obj, typo, pt_to_px(12), d, fmargin, height - height/4,
	       svg::transform(), 90);

  // Name, Slice Name, Episode Name
  typotxt._M_w = svg::typography::weight::medium;
  sized_text_r(obj, typotxt, pt_to_px(10), wcll.media_object_name(),
	       fmargin, height/2, svg::transform(), 90);

  // TC size
  glyphs g2 = cached_glyphs(wcll);
  string tcsize = collection_size_and_partition(wcll, g2);
  sized_text_r(obj, typotxt, pt_to_px(9), tcsize,
	       fmargin, height/4, svg::transform(), 90);

  // Date
  svg::typography typod = typo;
  typod._M_a = svg::typography::anchor::start;
  typod._M_align = svg::typography::align::left;
  sized_text_r(obj, typod, pt_to_px(9), wcll.datestamp,
	       fmargin, fmargin, svg::transform(), 90);

  augment_corner_logo(obj, wcll);
}

/// Demi- and Sub-titles: maps details, logos, sample details.
void
augment_weighed_demititle_corner(svg_element& obj, cached_collection& wcll,
				 const string d, const int xoff)
{
  const auto height = obj._M_area._M_height;
  const int fmargin = carto::frame_margin();
  auto [x, y] = corner_logo_size();

  augment_corner_logo(obj, wcll, xoff);

  using svg::k::b_style;
  using svg::k::w_style;
  svg::typography typo = svg::k::apercu_typo;
  typo._M_p = svg::typography::property::normal;
  typo._M_w = svg::typography::weight::bold;
  typo._M_a = svg::typography::anchor::start;
  typo._M_align = svg::typography::align::left;
  typo._M_style = wcll.sort_mode == swarm_mode::peer ? b_style : w_style;

  svg::typography typotxt = typo;
  // typotxt._M_style._M_stroke_opacity = 0.8;
  // typotxt._M_style._M_stroke_size = 0.283;
  // typotxt._M_style._M_stroke_size = 0.5;
  // typotxt._M_style._M_stroke_size = 2.0;

  // center of logo == center of interest == xoff +  minus half the logo width
  const auto xcenter = xoff + x / 2;

  // Title.
  const auto yoff = height - fmargin - y - 12;
  typo._M_w = svg::typography::weight::medium;
  sized_text_r(obj, typo, pt_to_px(8), d,
	       xcenter + 12, yoff, svg::transform(), -90);

  // Name, Slice Name, Episode Name
  typotxt._M_w = svg::typography::weight::bold;
  sized_text_r(obj, typotxt, pt_to_px(10), wcll.media_object_name(),
	       xcenter, yoff, svg::transform(), -90);

  // Date
  svg::typography typod = typo;
  typod._M_w = svg::typography::weight::light;
  sized_text_r(obj, typod, pt_to_px(8), wcll.datestamp,
	       xcenter - 12, yoff, svg::transform(), -90);
}

} // namespace a60
#endif
