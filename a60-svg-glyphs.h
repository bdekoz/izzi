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

#ifndef a60_SVG_GLYPHS_H
#define a60_SVG_GLYPHS_H 1

#include "a60-carto.h"
#include "a60-torrent.h"

namespace a60
{
  using namespace svg;

/**
   Graphic element for torrent details. Aggregated, they form a legend
   to the given torrent map.
*/
struct glyph
{
  using colort = svg::colorq;

  string		name;
  string		sname;
  string		infohash;
  ulong			tid;
  resolution_mode	resolution;
  encoding_mode         encoding;
  colort		klr;

  string		d1name;
  string		d2name;
  string		d3name;
  ulong			d1; // peers
  ulong			d2; // seeds
  ulong			d3; // downloaded

  // If zero, then unused. If not, then part of a larger set of
  // glyphs, and this value represents the normalized value.
  ulong			weight;

  ulong			bytes;
  bool			is_private;
  bool			is_ssl;
  bool			is_i2p;

  glyph(const string n, string ih, ulong t, resolution_mode res,
	encoding_mode enc, colort c, string d1, string d2, string d3,
	ulong pd1, ulong pd2, ulong pd3, ulong w)
  : name(n), sname(""), infohash(ih), tid(t), resolution(res), encoding(enc),
    klr(c), d1name(d1), d2name(d2), d3name(d3), d1(pd1), d2(pd2), d3(pd3),
    weight(w), bytes(0), is_private(false), is_ssl(false), is_i2p(false)
  { }

  glyph(const string n, colort c, string d1, string d2, string d3,
	ulong pd1, ulong pd2, ulong pd3)
  : name(n), sname(""), infohash(""), tid(0),
    resolution(resolution_mode::none), encoding(encoding_mode::none), klr(c),
    d1name(d1), d2name(d2), d3name(d3), d1(pd1), d2(pd2), d3(pd3),
    weight(0), bytes(0), is_private(false), is_ssl(false), is_i2p(false)
  { }

  glyph(const string n, colort c, string d1, string d2, ulong pd1, ulong pd2)
  : name(n), sname(""), infohash(""), tid(0),
    resolution(resolution_mode::none), encoding(encoding_mode::none), klr(c),
    d1name(d1), d2name(d2), d3name(""), d1(pd1), d2(pd2), d3(0),
    weight(0), bytes(0), is_private(false), is_ssl(false), is_i2p(false)
  { }

  glyph()
  : name(""), sname(""), infohash(""), tid(0),
    resolution(resolution_mode::none), encoding(encoding_mode::none),
    klr(svg::colore::black),
    d1name(""), d2name(""), d3name(""), d1(0), d2(0), d3(0),
    weight(0), bytes(0), is_private(false), is_ssl(false), is_i2p(false)
  { }

  ulong
  value(const swarm_mode mode) const
  {
    ulong ret(0);
    if (mode == swarm_mode::peer)
      ret = d1;
    if (mode == swarm_mode::seed)
      ret = d2;
    return ret;
  }
};

/// Plural glyphs.
using glyphs = std::vector<glyph>;


/// Shell for titles page.
glyphs
make_glyphs_basic(const collection& cll)
{
  string tdir = cll.torrent_directory();

  // Load up torrent and decode metadata.
  lt::session_proxy sp;
  lt::settings_pack settings = make_settings_pack();
  lt::session s(settings);
  configure_session(s, settings);

  lt::torrent_handle th;
  lt::add_torrent_params p;
  p.save_path = io::get_output_directory().c_str();

  const render_state& tr = get_render_state();

  glyphs glys;
  for (uint i = 0; i < cll.members.size(); ++i)
    {
      const collection::tmarkup& mrkup = cll.members[i];

      // Full filename to trimmed, partial name.
      string filef = mrkup.name;
      string filetrim = cll.trim_string(filef, cll.trim, cll.multiepisodep);

      glyph g(filef, tr.get_color(filef), "peers", "seeds", 0, 0);

      // Add peer glyph info.
      g.name = filef;
      g.sname = filetrim;
      g.tid = mrkup.tid;
      g.resolution = mrkup.resolution;
      g.encoding = mrkup.encoding;

      // If this is a synthetic torrent, strip filename to torrent base.
      auto fpos = filef.find("multi-btih-");
      if (fpos != string::npos)
	{
	  // Strip filef to after "multi-bih-" + .. + "x-" + torrent name.
	  auto fpos2 = filef.find("x-");
	  if (fpos2 != string::npos)
	    filef = filef.substr(fpos2 + 2);
	}

      // Find the rest from torrent meta data.
      string fqfilef(io::end_path(tdir) + filef + ".torrent");
      lt::error_code ec;
      p.ti = std::make_shared<lt::torrent_info>(fqfilef, std::ref(ec), 0);
      if (ec)
	{
	  std::string s("make_glyphs_basic:: error or invalid torrent file");
	  std::cout << s << std::endl << fqfilef << std::endl;
	  std::cout <<  ec.message() << std::endl;
	}
      else
	{
	  th = s.add_torrent(p, ec);
	  auto& ti = *(th.torrent_file());

	  std::ostringstream oss;
	  oss << ti.info_hash();
	  g.infohash = oss.str();
	  g.is_private = ti.priv();
	  g.is_ssl = ti.ssl_cert() != "";
	  g.is_i2p = ti.is_i2p();
	  g.bytes = ti.total_size();

	  s.remove_torrent(th);
	}
      glys.push_back(g);
    }
  return glys;
}


/// Expensive. Should this be part of a weighed collection?
glyphs
make_glyphs(const cached_collection& wcll)
{
  glyphs glys = make_glyphs_basic(wcll);
  for (uint i = 0; i < wcll.members.size(); ++i)
    {
      glyph& g = glys[i];
      g.d1 = wcll.peer_totals[i];
      g.d2 = wcll.seed_totals[i];
    }
  return glys;
}


/// Eraser.
glyphs
remove_empty_glyphs(glyphs& glysin, swarm_mode m)
{
  // Only augment non-zero size glyphs.
  auto lnonzeropeer = [](const glyph& g) { return g.d1 > 0; };
  auto lnonzeroseed = [](const glyph& g) { return g.d2 > 0; };
  auto lnonzero = m == swarm_mode::peer ? lnonzeropeer : lnonzeroseed;

  glyphs glys;
  copy_if(glysin.begin(), glysin.end(), back_inserter(glys), lnonzero);
  return glys;
}


/// Cache glyphs.
const glyphs&
cached_glyphs(cached_collection& wcll)
{
  static glyphs glys;
  if (glys.empty())
    {
      glyphs g1 = make_glyphs(wcll);
      glyphs g2 = remove_empty_glyphs(g1, wcll.sort_mode);
      glys = g2;
    }
  return glys;
}


/// Sum over vector glyphs, return as new summary glyph.
glyph
sum_glyphs(const glyphs& glys, const string name = "all")
{
  // Add in summary info for combined torrent collection.
  glyph sumg = { };
  sumg.name = name;
  sumg.klr = svg::colore::gray50;

  if (!glys.empty())
    {
      for (const glyph& g: glys)
	{
	  sumg.d1 += g.d1; // peer
	  sumg.d2 += g.d2; // seed
	  sumg.d3 += g.d3; // downloads

	  sumg.bytes += g.d1 * g.bytes;
	}
      sumg.d1name = glys[0].d1name;
      sumg.d2name = glys[0].d2name;
      sumg.d3name = glys[0].d3name;
    }

  return sumg;
}


/// Weigh a vector of glyphs via the number of current peers.
void
weigh_glyphs_via_mode(glyphs& glys, const swarm_mode mode,
		      const uint maxh, const uint maxw)
{
  auto lglyphascending = [mode](const glyph& g1, const glyph& g2)
			 { return g1.value(mode) > g2.value(mode); };
  std::sort(glys.begin(), glys.end(), lglyphascending);

  // Remove zero element glyph elements.
  glyphs glysnu;
  for (glyph& g: glys)
    {
      if (g.value(mode) != 0)
	glysnu.push_back(g);
    }
  glys = glysnu;

  uint maxv = glys.front().value(mode);
  uint minv = glys.back().value(mode);
  uint nceil = std::max(maxh, maxw);
  uint nfloor = 10;

  // Weigh.
  for (glyph& g: glys)
    {
      ulong v = g.value(mode);
      auto n = normalize_on_range(v, minv, maxv, nfloor, nceil);
      g.weight = static_cast<ulong>(n);
    }

  // Sort glyphs in decreasing size, so that biggest layer is lowest.
  auto lglyphweigh = [](const glyph& g1, const glyph& g2)
    { return g1.weight > g2.weight; };
  std::sort(glys.begin(), glys.end(), lglyphweigh);
}


constexpr uint bndlsize = 10;

/// bndlsize is the subgrouping of names into bundles of 10
/// max bundles per row is 5 for vertical stacks, 1 for horizontal stacks.
ulong
compute_number_of_glyph_rows(const ulong ntorrents, const uint maxbundlesrow)
{
  const uint maxperrow = bndlsize * maxbundlesrow;
  double dtorrents(ntorrents);
  double d(dtorrents / maxperrow);
  ulong dceil = std::ceil(d);

  std::cout << "d: " << d << " dceil: " << dceil
	    << " ntorrents: " << ntorrents
	    << " maxperrow: " << maxperrow << std::endl;
  return dceil;
}



/// Circle size proportional to area.
void
augment_weighed_glyph_v1(svg_element& obj, const glyph& g, int tx, int ty)
{
  svg::style styl = svg::k::b_style;
  styl._M_fill_color = g.klr;
  styl._M_fill_opacity = 1.0;

  styl._M_stroke_color = g.klr;
  styl._M_stroke_opacity = 0.0;
  styl._M_stroke_size = 0.20;

  uint radius = g.weight / 2;

  string xform = svg::transform::rotate(1, tx, ty);
  point_2d_to_circle(obj, tx - radius, ty, styl, radius, xform);
}


/// For ovoid representation.
void
augment_weighed_glyph_v2(svg_element& obj, const glyph& g, ulong vmax,
			 bool textp)
{
  svg::typography typo = svg::k::apercu_typo;

  // Dead center.
  auto [ midx, midy ] = obj.center_point();
  uint radius = g.weight / 2;

  if (textp)
    {
      // Find percentage.
      uint perct = g.d1 * 100 / vmax;

      // Draw circle.
      svg::style tstyl = svg::k::w_style;
      tstyl._M_fill_opacity = 0.0;
      tstyl._M_stroke_opacity = 0.2;
      tstyl._M_stroke_size = 0.5;
      point_2d_to_circle(obj, midx, midy, tstyl, radius);

      // Name
      svg::typography typon = typo;
      typon._M_a = svg::typography::anchor::end;
      typon._M_align = svg::typography::align::right;
      sized_text_r(obj, typon, 14, g.name,
			 midx - radius + carto::fspacer, midy - carto::fspacer,
			 svg::transform(), 90);

      // Percentage
      svg::typography typog = typo;
      typog._M_a = svg::typography::anchor::start;
      typog._M_align = svg::typography::align::left;
      typog._M_style._M_fill_color = svg::colore::gray50;
      std::ostringstream outs;
      outs << perct << '%';
      sized_text_r(obj, typog, 28, outs.str(),
			 midx - radius + carto::fspacer, midy + carto::fspacer,
			 svg::transform(), 90);
    }
  else
    {
      svg::style styl = svg::k::b_style;
      styl._M_fill_color = g.klr;
      styl._M_fill_opacity = 1.0;

      styl._M_stroke_color = g.klr;
      styl._M_stroke_opacity = 0.0;
      styl._M_stroke_size = 0.20;

      point_2d_to_circle(obj, midx, midy, styl, radius);
    }
}


/// "Classic" old-type map legend feel, square color name centered, for
/// grid layout.
void
augment_torrent_glyph_v1(svg_element& obj, svg::typography& typo, const glyph& g,
		      int tx, int ty)
{
  svg::style styl = svg::k::b_style;
  styl._M_fill_opacity = 1.0;
  styl._M_fill_color = g.klr;

  group_element gpre;
  gpre.start_element("torrent-glyph", styl);
  obj.add_element(gpre);

  point_2d_to_rect(obj, tx - 14 / 2, ty, styl, 14, 14);

  ty += 28;
  typo._M_size = 12;
  typo._M_w = svg::typography::weight::light;
  typo._M_p = svg::typography::property::italic;
  text_element::data dtl2 = { tx, ty, g.name, typo };
  text_element tl2;
  tl2.start_element();
  tl2.add_data(dtl2);
  tl2.finish_element();
  obj.add_element(tl2);

  ty += 12;
  typo._M_size = 10;
  typo._M_w = svg::typography::weight::normal;
  typo._M_p = svg::typography::property::normal;
  std::ostringstream ossa;
  ossa.imbue(std::locale(""));
  ossa << std::fixed << g.d1;
  text_element::data dtl1s = { tx, ty, ossa.str(), typo };
  text_element tl1s;
  tl1s.start_element();
  tl1s.add_data(dtl1s);
  tl1s.finish_element();
  obj.add_element(tl1s);

  if (g.d2 > 0)
    {
      ty += 10;
      std::ostringstream ossc;
      ossc.imbue(std::locale(""));
      ossc << std::fixed << g.d2;
      text_element::data dtl2s = { tx, ty, ossc.str(), typo };
      text_element tl2s;
      tl2s.start_element();
      tl2s.add_data(dtl2s);
      tl2s.finish_element();
      obj.add_element(tl2s);
    }

  if (g.d3 > 0)
    {
      ty += 10;
      std::ostringstream ossk;
      ossk.imbue(std::locale(""));
      ossk << std::fixed << g.d3;
      text_element::data dtl3s = { tx, ty, ossk.str(), typo };
      text_element tl3s;
      tl3s.start_element();
      tl3s.add_data(dtl3s);
      tl3s.finish_element();
      obj.add_element(tl3s);
    }

  group_element gpost;
  gpost.finish_element();
  obj.add_element(gpost);
}


void
augment_torrent_glyph_numbers(svg_element& obj, svg::typography& typo,
			      const glyph& g, int tx, int ty, swarm_mode m)
{
  // Numeric components of glyph, seeds, peers, downloads.
  const string sp4 = "    ";
  const string sp6 = "      ";
  std::ostringstream oss;

  if (g.d1 > 0 && (m == swarm_mode::peer || m == swarm_mode::unknown))
    {
      std::ostringstream ossa;
      ossa.imbue(std::locale(""));
      ossa << std::fixed << g.d1;
      oss << ossa.str() << sp4;
    }

  if (g.d2 > 0 && (m == swarm_mode::seed || m == swarm_mode::unknown))
    {
      std::ostringstream ossc;
      ossc.imbue(std::locale(""));
      ossc << std::fixed << g.d2;
      oss << ossc.str() << sp4;
    }

  if (g.d3 > 0)
    {
      std::ostringstream ossk;
      ossk.imbue(std::locale(""));
      ossk << std::fixed << g.d3;
      oss << ossk.str();
    }

  sized_text(obj, typo, pt_to_px(10), oss.str(), tx + 20, ty);
}


/// Marian Bantje conversation starts here.
/// Stacked layout with torrent name vertical, then color, and
/// peer/seed numbers. Assumed that stack goes L to R, some delta.
void
augment_torrent_glyph_v2(svg_element& obj, svg::typography& typo,
			 const glyph& g, int tx, int ty, swarm_mode m)
{
  svg::style styl = svg::k::b_style;
  styl._M_fill_opacity = 1.0;
  styl._M_fill_color = g.klr;

  const auto typesz = 10;

  // Text.
  svg::typography typotxt = typo;
  typotxt._M_size = typesz;
  typotxt._M_a = svg::typography::anchor::start;
  typotxt._M_align = svg::typography::align::left;
  typotxt._M_w = svg::typography::weight::medium;
  typotxt._M_p = svg::typography::property::normal;

  svg::typography typotxtr = typo;
  typotxtr._M_size = typesz;
  typotxtr._M_a = svg::typography::anchor::end;
  typotxtr._M_w = svg::typography::weight::light;
  typotxtr._M_p = svg::typography::property::normal;

  group_element gpre;
  gpre.start_element("torrent-glyph", styl);
  obj.add_element(gpre);

  point_2d_to_rect(obj, tx, ty, styl, 14, 14);


  sized_text_r(obj, typotxtr, pt_to_px(typesz), g.name, tx, ty - 10,
		     svg::transform(), 90);

  // Use tspan to make horizontal divisions.
  ty += 12;

  // Numbers, if any.
  augment_torrent_glyph_numbers(obj, typotxt, g, tx, ty, m);

  group_element gpost;
  gpost.finish_element();
  obj.add_element(gpost);
}


/// Marian Bantje conversation picks up steam here.
/// Compact stacked layout with torrent name vertical, then color, and
/// peer/seed numbers. Assumed that stack goes L to R, some delta.
void
augment_torrent_glyph_v3(svg_element& obj, svg::typography& typo,
			 const glyph& g, int tx, int ty, swarm_mode m)
{
  bool peerp = m == swarm_mode::peer;
  svg::style styl = peerp ? svg::k::b_style : svg::k::w_style;
  styl._M_fill_opacity = 1.0;
  styl._M_fill_color = g.klr;

  const auto typesz = 10;

  // Text.
  svg::typography typotxt = typo;
  typotxt._M_size = typesz;
  typotxt._M_a = svg::typography::anchor::start;
  typotxt._M_align = svg::typography::align::left;
  typotxt._M_w = svg::typography::weight::medium;
  typotxt._M_p = svg::typography::property::normal;

  svg::typography typotxtr = typo;
  typotxtr._M_size = typesz;
  typotxtr._M_a = svg::typography::anchor::end;
  typotxtr._M_w = svg::typography::weight::light;
  typotxtr._M_p = svg::typography::property::normal;

  group_element gpre;
  gpre.start_element("torrent-glyph", styl);
  obj.add_element(gpre);

  point_2d_to_rect(obj, tx, ty - 10, styl, 12, 12);

  std::ostringstream osst;
  osst << "t" << std::setfill('0') << std::setw(3) << g.tid;

  sized_text(obj, typotxtr, pt_to_px(typesz), osst.str(), tx - 10, ty);

  // Numbers, if any.
  augment_torrent_glyph_numbers(obj, typotxt, g, tx, ty, m);

  group_element gpost;
  gpost.finish_element();
  obj.add_element(gpost);
}


/// Ziggurat style for infohash datum layout: tid - color - name/btih/private.
void
augment_torrent_glyph_v4v(svg_element& obj, svg::typography& typo,
			  const glyph& g, int tx, int ty, swarm_mode m)
{
  // Get info hash and acess metadata for glyph.
  svg::style styl = svg::k::b_style;
  styl._M_fill_opacity = 1.0;
  styl._M_fill_color = g.klr;

  // Text.
  svg::typography typobtih = typo;
  typobtih._M_a = svg::typography::anchor::start;
  typobtih._M_align = svg::typography::align::left;
  typobtih._M_p = svg::typography::property::normal;
  typobtih._M_size = 10;
  typobtih._M_w = svg::typography::weight::xlight;

  svg::typography typotxt = typobtih;
  typotxt._M_face = svg::k::shsans;
  typotxt._M_size = 11;
  typotxt._M_w = svg::typography::weight::medium;

  svg::typography typotxtr = typo;
  typotxtr._M_a = svg::typography::anchor::end;
  typotxt._M_align = svg::typography::align::right;
  typotxtr._M_w = svg::typography::weight::bold;
  typotxtr._M_p = svg::typography::property::normal;
  typotxtr._M_size = 14;

  group_element gpre;
  gpre.start_element("torrent-glyph", styl);
  obj.add_element(gpre);

  // Torrent id.
  std::ostringstream osst;
  osst << "t" << std::setfill('0') << std::setw(3) << g.tid;

  sized_text_r(obj, typotxtr, pt_to_px(14), osst.str(), tx + 6, ty,
		     svg::transform(), 90);

  // Rectangle of color.
  ty += 10;
  point_2d_to_rect(obj, tx, ty, styl, 22, 22);

  // Torrent name.
  ty += 34;
  tx += 13;
  sized_text_r(obj, typotxt, pt_to_px(12), g.name, tx, ty,
		     svg::transform(), 90);

  // Torrent infohash and access private/public. (Append '-' if private.)
  tx -= 13;
  string ih = g.infohash;
  if (g.is_private)
    {
      ih.push_back(' ');
      ih.push_back('-');
    }

  sized_text_r(obj, typobtih, pt_to_px(10), ih, tx, ty,
		     svg::transform(), 90);

  // Numbers, if any.
  augment_torrent_glyph_numbers(obj, typobtih, g, tx, ty, m);

  group_element gpost;
  gpost.finish_element();
  obj.add_element(gpost);
}


/// Ziggurat style for infohash datum layout: tid - color - name/btih/private.
void
augment_torrent_glyph_v4h(svg_element& obj, svg::typography& typo,
			  const glyph& g, int tx, int ty)
{
  // Get info hash and acess metadata for glyph.
  svg::style styl = svg::k::b_style;
  styl._M_fill_opacity = 1.0;
  styl._M_fill_color = g.klr;

  // Text.
  svg::typography typobtih = typo;
  typobtih._M_a = svg::typography::anchor::start;
  typobtih._M_align = svg::typography::align::left;
  typobtih._M_p = svg::typography::property::normal;
  typobtih._M_size = 10;
  typobtih._M_w = svg::typography::weight::xlight;

  svg::typography typotxt = typobtih;
  typotxt._M_face = svg::k::shsans;
  typotxt._M_size = 11;
  typotxt._M_w = svg::typography::weight::medium;

  svg::typography typotxtr = typo;
  typotxtr._M_a = svg::typography::anchor::end;
  typotxt._M_align = svg::typography::align::right;
  typotxtr._M_w = svg::typography::weight::bold;
  typotxtr._M_p = svg::typography::property::normal;
  typotxtr._M_size = 14;

  group_element gpre;
  gpre.start_element("torrent-glyph", styl);
  obj.add_element(gpre);

  // Torrent id.
  std::ostringstream osst;
  osst << "t" << std::setfill('0') << std::setw(3) << g.tid;
  sized_text(obj, typotxtr, pt_to_px(14), osst.str(), tx - 8, ty + 6);

  // Rectangle of color.
  point_2d_to_rect(obj, tx, ty - 11, styl, 22, 22);

  // Torrent name.
  tx += 34;
  sized_text(obj, typotxt, pt_to_px(12), g.name, tx, ty);

  // Torrent infohash and access private/public. (Append '-' if private.)
  ty += 13;
  string ih = g.infohash;
  if (g.is_private)
    {
      ih.push_back(' ');
      ih.push_back('-');
    }

  sized_text(obj, typobtih, pt_to_px(10), ih, tx, ty);

  group_element gpost;
  gpost.finish_element();
  obj.add_element(gpost);
}


/// Swarm_mode dominates.
void
augment_sum_glyph(svg_element& obj, svg::typography typo, glyph& sumg,
		  swarm_mode m, int tx, int ty)
{
  typo._M_a = svg::typography::anchor::end;
  typo._M_w = svg::typography::weight::medium;
  typo._M_align = svg::typography::align::right;
  typo._M_p = svg::typography::property::normal;

  // Major mode
  bool peerp = m == swarm_mode::peer;
  svg::typography typomj = typo;

#if OUTLINETEXTMODE
  typomj._M_style = svg::k::w_style;
  typomj._M_style._M_stroke_opacity = 1.0;
  typomj._M_style._M_stroke_size = 0.2;
#else
  typomj._M_style = peerp ? svg::k::b_style : svg::k::w_style;
#endif

  // Minor
  svg::typography typom = typo;
  typom._M_style._M_fill_color = svg::colore::gray50;

  string peern;
  {
    std::ostringstream oss;
    oss.imbue(std::locale(""));
    if (sumg.d1 > 0)
      oss << sumg.d1;
    peern = oss.str();
  }

  string seedn;
  {
    std::ostringstream oss;
    oss.imbue(std::locale(""));
    if (sumg.d2 > 0)
      oss << sumg.d2;
    seedn = oss.str();
  }

  string peert = to_string(swarm_mode::peer);
  string peerT;
  for (char& c: peert)
    peerT += std::toupper(c, std::locale(""));

  string seedt = to_string(swarm_mode::seed);
  string seedT;
  for (char& c: seedt)
    seedT += std::toupper(c, std::locale(""));

  // Switch.
  string majorn;
  string majort("UNIQUE ");
  string minorn;
  string minort("UNIQUE ");
  if (m == swarm_mode::peer)
    {
      majorn = peern;
      majort += peerT;
      minorn = seedn;
      minort += seedT;
    }
  else
    {
      majorn = seedn;
      majort += seedT;
      minorn = peern;
      minort += peerT;
    }

  majort += "S";
  minort += "S";

  sized_text(obj, typomj, pt_to_px(32), majorn + " " + majort, tx, ty);

  ty -= 36;
  if (!minorn.empty())
    sized_text(obj, typom, pt_to_px(16), minorn + " " + minort, tx, ty);
}


/// Just the text field names.
void
augment_sum_glyphs_1(svg_element& obj, svg::typography typo, glyphs& glys,
		     int tx, int ty)
{
  if (glys.empty())
    return;
  glyph sumg = sum_glyphs(glys);

  typo._M_size = 10;

  svg::typography typonum = typo;
  typonum._M_a = svg::typography::anchor::end;
  typonum._M_p = svg::typography::property::normal;
  typonum._M_w = svg::typography::weight::normal;

  svg::typography typotxt = typo;
  typotxt._M_a = svg::typography::anchor::start;
  typotxt._M_p = svg::typography::property::italic;
  typotxt._M_w = svg::typography::weight::light;

  const int delta = 2;

  // All peers, with numer on the left and label on the right.
  std::ostringstream ossc;
  ossc.imbue(std::locale(""));
  ossc << sumg.d1;
  text_element::data dtc = { tx - delta, ty, ossc.str(), typonum };
  text_element tc;
  tc.start_element();
  tc.add_data(dtc);
  tc.finish_element();
  obj.add_element(tc);

  text_element::data dtl1s = { tx + delta, ty, sumg.d1name, typotxt };
  text_element tl1s;
  tl1s.start_element();
  tl1s.add_data(dtl1s);
  tl1s.finish_element();
  obj.add_element(tl1s);

  // Seeds.
  if (sumg.d2 > 0)
    {
      ty += 10;
      std::ostringstream osss;
      osss.imbue(std::locale(""));
      osss << sumg.d2;
      text_element::data dts = { tx - delta, ty, osss.str(), typonum };
      text_element ts;
      ts.start_element();
      ts.add_data(dts);
      ts.finish_element();
      obj.add_element(ts);

      text_element::data dtl2s = { tx + delta, ty, sumg.d2name, typotxt };
      text_element tl2s;
      tl2s.start_element();
      tl2s.add_data(dtl2s);
      tl2s.finish_element();
      obj.add_element(tl2s);
    }

  // Downloads, as reported by tracker.
  if (sumg.d3 > 0)
    {
      ty += 10;
      std::ostringstream ossd;
      ossd.imbue(std::locale(""));
      ossd << sumg.d3;
      text_element::data dtd = { tx - delta, ty, ossd.str(), typonum };
      text_element td;
      td.start_element();
      td.add_data(dtd);
      td.finish_element();
      obj.add_element(td);

      text_element::data dtl3s = { tx + delta, ty, sumg.d3name, typotxt };
      text_element tl3s;
      tl3s.start_element();
      tl3s.add_data(dtl3s);
      tl3s.finish_element();
      obj.add_element(tl3s);
    }
}


/// Swarm_mode dominates.
void
augment_sum_glyphs_2(svg_element& obj, svg::typography typo, glyphs& glys,
		     swarm_mode m, int tx, int ty)
{
  if (glys.empty())
    return;
  glyph sumg = sum_glyphs(glys);
  augment_sum_glyph(obj, typo, sumg, m, tx, ty);
}


/**
  Two different approaches in this mongoloid.
  Text is scaled proportionately to total summary.
  Vector is scaled proportionately to largest item.
 */
void
make_torrent_collection_ovoid_glyph(svg_element& obj, swarm_mode m,
				    glyphs& glys, bool textp)
{
  // Weighted peer glyphs.
  if (!glys.empty())
    {
      const auto [ width, height ] = obj._M_area;

      std::cout << std::fixed;
      std::cout << "make_torrent_collection_ovoid_glyph: " << glys.size()
		<< std::endl;

      // Summary glyph.
      glyph sumg = sum_glyphs(glys);
      double maxv = sumg.value(m);
      double maxspace = std::max(width, height);
      if (maxv == 0 || maxspace == 0)
	{
	  string msg("make_torrent_collection_ovoid_glyph:: error ");
	  msg += "mode: " + to_string(m) + " ";
	  msg += "maxv: " + a60::to_string(maxv) + " ";
	  msg += "maxspace: " + a60::to_string(maxspace) + " ";
	  throw std::runtime_error(msg);
	}

      // Set up transformation that max value (maxv) is the maximum
      // space (maxspace). Actual values are porportionate.
      double r = maxspace / maxv;

      std::cout << "maxv " << maxv << std::endl;
      std::cout << "ratio " << r << std::endl;

      if (!textp)
	weigh_glyphs_via_mode(glys, m, height, width);

      for (glyph& g: glys)
	{
	  double v = g.value(m);
	  if (textp)
	    g.weight = double(r * v);
	  augment_weighed_glyph_v2(obj, g, maxv, textp);
	}
    }
}



/// v2
/// Left to right glyphs that staircase up and to the right.
void
augment_torrent_collection_glyphs_v2(svg_element& obj, svg::typography typo,
				     glyphs& glys, swarm_mode m)
{
  // Offset (delta) and calculated initial position for legend (lx).
  const int delta = 22;

  // Torrent glyphs.
  group_element gpre;
  gpre.start_element();
  obj.add_element(gpre);

  const uint maxlen(10);
  const uint maxrowlen(5);
  int tx = 2 * carto::frame_margin();
  int ty = obj._M_area._M_height - 2 * carto::frame_margin();
  int lx = tx;
  int ly = ty;
  for (uint n(0); n < glys.size();)
    {
      // Walk portrait landscape across horizontal.
      for (uint sn(0); sn < maxlen && n < glys.size(); ++sn)
	{
	  glyph& g = glys[n++];
	  augment_torrent_glyph_v2(obj, typo, g, lx, ly, m);
	  lx += delta;
	  ly -= delta;
	}

      if (n == maxlen * maxrowlen)
	{
	  lx = 2 * carto::frame_margin();
	  ly = obj._M_area._M_height / 2;
	}
      else
	{
	  lx += 4 * delta;
	  ly = ty;
	}
    }

  group_element gpost;
  gpost.finish_element();
  obj.add_element(gpost);
}


/// v3
/// by day/week/hour
// Compact steep left to right glyphs that staircase up and to the right.
// Assume that there is only one row of glyphs, but each column has a
// variable size, perhaps pertaining to a partition size.
// Uses torrent info instead of name.
void
augment_torrent_collection_glyphs_v3(svg_element& obj, svg::typography typo,
				     glyphs& glys, swarm_mode m,
				     const int yoff = 2)
{
  // Offset (delta) and calculated initial position for legend (lx).
  const int xdelta = 10;
  const int ydelta = 20;
  const int xdeltarow = 130;

  // Torrent glyphs.
  group_element gpre;
  gpre.start_element();
  obj.add_element(gpre);

  // Height range from 10 to 25.
  uint maxheight(25);
  static std::mt19937_64 rg(std::random_device{}());
  auto disti = std::uniform_int_distribution<>(10, maxheight);
  uint maxlen = disti(rg);

  // Maximum number of staggered groups in one row.
  uint groupsn(0);
  const uint maxgroupsinrow(13);

  int tx = 2 * carto::frame_margin();

  // Vertical offset.
  int ty = obj._M_area._M_height - (yoff * carto::frame_margin());

  int lx = tx;
  int ly = ty;

  int xcol0 = lx;
  for (uint n(0); n < glys.size();)
    {
      // Walk portrait landscape across horizontal.
      for (uint sn(0); sn < maxlen && n < glys.size(); ++sn)
	{
	  glyph& g = glys[n++];
	  augment_torrent_glyph_v3(obj, typo, g, lx, ly, m);
	  lx += xdelta;
	  ly -= ydelta;
	}
      ++groupsn;

      if (groupsn == maxgroupsinrow)
	{
	  ty -= ((ydelta * maxheight) + (carto::frame_margin()));
	  ly = ty;
	  lx = 2 * carto::frame_margin();
	  xcol0 = lx;
	  groupsn = 0;
	}
      else
	{
	  ly = ty;
	  lx = xcol0 + xdeltarow;
	  xcol0 = lx;
	}

      // Margin requires shorter length for the last element in a row.
      if (groupsn == maxgroupsinrow - 1)
	maxlen = 10;
      else
	maxlen = disti(rg);
    }

  group_element gpost;
  gpost.finish_element();
  obj.add_element(gpost);
}


/// v4v, vertical fill
/// stagger step info hash
void
augment_torrent_collection_glyphs_v4v(svg_element& obj, svg::typography typo,
				      glyphs& glys, swarm_mode m)
{
  // Offset (delta) and calculated initial position for legend (lx).
  const int delta = 30;

  // Torrent glyphs.
  group_element gpre;
  gpre.start_element();
  obj.add_element(gpre);

  // Create one horizontal row of glyphs, split into groups of 10 columns each.
  const uint maxbundlesrow = 5;
  const uint maxperrow = bndlsize * maxbundlesrow;

  int tx = 2 * carto::frame_margin();

  // Set initial vertical position.
  // For one column, start at midpoint of page.
  // For two column, start at bottom row then move to top row.
  int ty = obj._M_area._M_height;
  if (glys.size() > maxperrow)
    ty = ty/1.5 - (bndlsize * delta);
  else
    ty = ty/2 - (bndlsize * delta);

  int lx = tx;
  int ly = ty;
  for (uint n(0); n < glys.size();)
    {
      // Walk portrait landscape across horizontal.
      for (uint sn(0); sn < bndlsize && n < glys.size(); ++sn)
	{
	  glyph& g = glys[n++];
	  augment_torrent_glyph_v4v(obj, typo, g, lx, ly, m);

	  // Increment for next.
	  lx += delta;
	  ly += delta;
	}

      if (n == maxperrow)
	{
	  // Shift to new vertical row.
	  lx = tx;
	  ty = carto::frame_margin() + delta;
	  ly = ty;
	}
      else
	{
	  // Shift to new horizonal group.
	  lx += 2 * delta;
	  ly = ty;
	}
    }

  group_element gpost;
  gpost.finish_element();
  obj.add_element(gpost);
}


/// v4h, horizontal fill
/// stagger step info hash long file names
void
augment_torrent_collection_glyphs_v4h(svg_element& obj, svg::typography typo,
				      glyphs& glys)
{
  // Offset (delta) and calculated initial position for legend (lx).
  const int delta = 30;

  // Torrent glyphs.
  group_element gpre;
  gpre.start_element();
  obj.add_element(gpre);

  // Create one horizontal row of glyphs, split into groups of 10 columns each.
  const uint maxbundlesrow = 1;
  const uint maxperrow = bndlsize * maxbundlesrow;

  int tx = (2 * carto::frame_margin()) + (1.5 * delta);

  // Set initial vertical position.
  int ty = tx;

  int lx = tx;
  int ly = ty;
  for (uint n(0); n < glys.size();)
    {
      // Walk portrait landscape across horizontal.
      for (uint sn(0); sn < bndlsize && n < glys.size(); ++sn)
	{
	  glyph& g = glys[n++];
	  augment_torrent_glyph_v4h(obj, typo, g, lx, ly);

	  lx += delta;
	  ly += delta;
	}

      // Only one column, so shift to new vertical group.
      if (n == maxperrow)
	{
	  lx = tx;
	  ty = ly + (2 * delta);
	  ly = ty;
	}
    }

  group_element gpost;
  gpost.finish_element();
  obj.add_element(gpost);
}


/// v5
/// short files: stagger step info hash
/// long names: 90 rotate top column.
/// NB: Assumed that multi-page details are already computed.
void
augment_torrent_collection_glyphs_v5(svg_element& obj, svg::typography typo,
				     glyphs& sglys, glyphs& lglys, swarm_mode m)
{
  if (sglys.size() > 0)
    {
      // Potentially mixed short and long.
      augment_torrent_collection_glyphs_v4v(obj, typo, sglys, m);
      if (lglys.size() > 0 && lglys.size() <= 8)
	augment_torrent_collection_glyphs_v4h(obj, typo, lglys);
    }
  else
    {
      // Just long.
      if (lglys.size() > 0)
	augment_torrent_collection_glyphs_v4h(obj, typo, lglys);
    }
}


void
augment_torrent_collection_glyphs(svg_element& obj, svg::typography typo,
				  glyphs& glys, swarm_mode m)
{ augment_torrent_collection_glyphs_v3(obj, typo, glys, m); }


/// Tie in both of the above.
void
augment_torrent_collection_multi_glyphs(svg_element& obj, svg::typography typo,
					glyphs& glysin, swarm_mode m)
{
  // First row of glyphs cannot have more than 9 total.
  glyphs glys = remove_empty_glyphs(glysin, m);
  augment_torrent_collection_glyphs(obj, typo, glys, m);
}

} // namespace a60
#endif
