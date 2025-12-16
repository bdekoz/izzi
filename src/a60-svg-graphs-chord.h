// alpha60 horizontal chord separations -*- mode: C++ -*-

// alpha60
// bittorrent x scrape x data + analytics

// Copyright (c) 2020-2021, Benjamin De Kosnik <b.dekosnik@gmail.com>

// This file is part of the alpha60 library.  This library is free
// software; you can redistribute it and/or modify it under the terms
// of the GNU General Public License as published by the Free Software
// Foundation; either version 3, or (at your option) any later
// version.

// This library is distributed in the hope that it will be useful, but
// WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// General Public License for more details.

#ifndef a60_SVG_CHORD_GRAPHS_H
#define a60_SVG_CHORD_GRAPHS_H 1

#include "a60-metadata.h"

namespace svg {

using namespace a60::metadata;

//// Distance between center and top/bottom block
const double ctopspace(60);

/// Distance between label and top/bottom of line or text
const double ltopspace(10);

const double hdenom(a60::k::age_values.size() + 1 + 1 + 1);

// Type sizes.
const auto lsz = 16; // large bold
const auto asz = 12; // sub headings
const auto ssz = 10; // sub sub headings

/// Compute xoffset.
uint
get_xoffset(const string& id, const uint xstart, const double hspace)
{ return xstart + (a60::k::age_value_to_multiple(id, true) * hspace); };


/// Grid, titles, labels for h_chord_graph.
/// @param yscale == how much to vertically scale the bar graph from center.
svg::svg_element
h_chord_graph_labels(const string aggname,
		     const svg::area<> a = svg::k::v1080p_h)
{
  const string viztitle("chord-graph-" + aggname);
  svg_element obj(viztitle, a);

  // Starting positions and vertical/horizontal offsets.
  const auto width = a._M_width;
  const double hspace((width - (a60::k::metamargin * 2)) / hdenom);
  const uint xstart(a60::k::metamargin + hspace + hspace);

  // Vertical have rect, vo number, filler, age, filler, vp number, rect.
  // Middle is age values.
  const auto height = a._M_height;
  const double vcenter(height / 2);

  typography typol = get_metadata_typo(lsz, true);
  sized_text(obj, typol, lsz, "age values", a60::k::metamargin, vcenter);

  typography typoli = get_metadata_typo(asz, false);
  typoli._M_p = svg::typography::property::italic;
  sized_text(obj, typoli, ssz, "birth age values per decile", a60::k::metamargin,
	     vcenter - ctopspace);
  string playingll("#" + a60::k::playing + " age values per decile");
  sized_text(obj, typoli, ssz, playingll, a60::k::metamargin,
	     vcenter + ctopspace + asz);

  // Draw horizontal labels over connecting lines.
  // Draw h labels, ages with v hair lines up and down at each value.
  typol._M_anchor = svg::typography::anchor::middle;
  typol._M_align = svg::typography::align::center;
  style styll = get_metadata_style(false);
  styll._M_stroke_size = 0.5;
  styll._M_stroke_opacity =  1.0;
  for (const auto& val : a60::k::age_values)
    {
      uint xoff = get_xoffset(val, xstart, hspace);
      sized_text(obj, typol, lsz, val, xoff, vcenter);

      // Top line.
      point_2t topstart(xoff, vcenter - lsz - ltopspace);
      point_2t topend(xoff, vcenter - ctopspace + ltopspace);
      auto l1 = make_line(topstart, topend, styll);
      obj.add_element(l1);

      // Bottom line.
      point_2t bstart(xoff, vcenter + lsz + ltopspace);
      point_2t bend(xoff, vcenter + ctopspace - ltopspace);
      auto l2 = make_line(bstart, bend, styll);
      obj.add_element(l2);
    }

  return obj;
}


/** A chord diagram, flattened into a linear horizontal form.

    Rendering set/subset of metadata with "playing" compound grammars.

    The Vertical (roughly, in thirds):
    top is actual age graph
    middle is ages plotted on horizontal axes
    bottom is playing age graph, inverted.

    And then a connecting line will be drawn from the actual age to
    the playing age.

    The Horizontal:
    Use linear scaling for age values, instead of bell shaped scaling for glyph.
    have 10 values (00-09, ..., 90-99) + 100+ + unknown +ambiguous -> 13.
*/
svg::svg_element
h_chord_graph(const vumids& cumulative, const string aggname,
	      const area<> a = svg::k::v1080p_h, const double yscale = 0.7,
	      const double rwidth = 10)
{
  using id_size = a60::id_size;

  // Draw labels, axes, ticks, etc.
  svg_element obj = h_chord_graph_labels(aggname, a);

  // Starting positions and vertical/horizontal offsets.
  const auto width = a._M_width;
  const double hspace((width - (a60::k::metamargin * 2)) / hdenom);
  const uint xstart(a60::k::metamargin + hspace + hspace);

  // Vertical have rect, vo number, filler, age, filler, vp number, rect.
  // Middle is age values.
  const auto height = a._M_height;
  const double vcenter(height / 2);

  /**
     Process cumulative age values 1-3.

     1. ages vo map
     2. ages playing map
     3. largest count value over all of the age values
  */
  const uint agesi = a60::k::find_id_index("ages");
  const umids& ages = cumulative[agesi];
  umids agesvo;
  umids agesvp;
  id_size maxno(0);
  for (const auto& val : ages)
    {
      auto [ id, count ] = val;

      if (id.find(a60::k::playing) != string::npos)
	agesvp.insert(val);
      else
	agesvo.insert(val);
      maxno = std::max(maxno, count);
    }
  const id_size maxn = maxno * 2;
  std::cout << "max n is: " << maxn << std::endl;

  /**
     Process cumulative age values 4.

     4. coalesce/flatten multi-playing values
     "50-59 playing 50-59, 60-69"
     "50-59 playing 10-19, 60-69

     to

     "50-59 playing 50-59" x 1
     "50-59 playing 10-19" x 1
     "50-59 playing 60-69" x 2
  */
  umids agesvpflat;
  for (const auto& val : agesvp)
    {
      auto [ id, count ] = val;

      // Pick out all age_values from playing age_values.
      strings ageso;
      strings agesplaying;
      split_playing_value(id, ageso, agesplaying);
      for (const string& valso : ageso)
	{
	  for (const string& valsp : agesplaying)
	    {
	      const string splayings = k::space + a60::k::playing + k::space;
	      const string valpflat = valso + splayings + valsp;
	      if (agesvpflat.count(valpflat))
		{
		  auto& val = agesvpflat.at(valpflat);
		  val += count;
		}
	      else
		agesvpflat.insert(make_pair(valpflat, count));
	    }
	}
    }

  // Setup.
  typography typol = get_metadata_typo(lsz, true);
  sized_text(obj, typol, lsz, "age values", a60::k::metamargin, vcenter);

  typography typov = get_metadata_typo(lsz, false, true);

  const uint ytotalgraphspace(vcenter - ctopspace - ltopspace - a60::k::metamargin);
  const uint ygraphdelta = std::round(yscale * ytotalgraphspace);

  // Size per two of largest values in one v space.
  const uint ydelta(ygraphdelta / 2);

  // Draw ages.
  // Starting point for above-the-age-values line, above summary count
  uint voffstart = vcenter - ctopspace - ltopspace - asz;

  // Vector of total age_value counts per decile for birth ages.
  using vul = a60::vul;
  vul vsum(a60::k::age_values.size(), 0);
  for (const auto& val : agesvo)
    {
      auto [ id, count ] = val;
      uint xoff = get_xoffset(id, xstart, hspace);

      // rect
      double rheight = scale_value_on_range(count, 1, maxn, 10, ydelta);
      double rystart = voffstart - (rheight / 2);
      rect_element r = make_rect_centered({xoff, rystart},
					  get_metadata_style(),
					  { rwidth, rheight });
      obj.add_element(r);

      // text
      sized_text(obj, typov, asz, std::to_string(count), xoff,
		 voffstart - rheight - ltopspace);

      const auto indx = a60::k::age_value_to_multiple(id, true);
      vsum[indx] += count;
    }

  // Draw ages playing.
  // Starting point for below-the-age-values line, below summary count.
  uint vpoffstart = vcenter + ctopspace + ltopspace + asz;

  style pstyl = { color::gray40, 1.0, color::gray40, 0.0, 0 };
  style pstyll = { color::gray40, 0.0, color::red, 1, 2 };

  // Vector of total age_value counts per decile for playing.
  vul vpsum(a60::k::age_values.size(), 0);

  // Vector of previous vertical offsets, how many "playing" slots already filled.
  vul vpyoffsets(a60::k::age_values.size(), 0);
  for (const auto& val : agesvpflat)
    {
      auto [ idp, count ] = val;

      // Pick out LHS age from RHS ages playing.
      strings ageso;
      strings agesplaying;
      split_playing_value(idp, ageso, agesplaying);

      // For each age playing value, keep track of the total number
      // from each actual age value.
      // NB: 40-49 playing 20-29, 30-39, 40-49 is counted as 1 actual 2 playing.
      //     And no 40-49 to 40-49 line
      for (const string& agepv : agesplaying)
	{
	  const auto indxp = a60::k::age_value_to_multiple(agepv, true);

	  // Check to see if ages playing value is the same as any age value.
	  const auto aend = ageso.end();
	  const bool agesamep = std::find(ageso.begin(), aend, agepv) != aend;
	  if (agesamep)
	    {
	      // Only add to birth age sum.
	      // NB: indxp and indx are the same for this value.
	      vsum[indxp] += count;
	    }
	  else
	    {
	      // Calculate vertical offset, including previous playing values.
	      ulong& yoffset = vpyoffsets[indxp];
	      uint xoff = get_xoffset(agepv, xstart, hspace);
	      uint yoff = vpoffstart + yoffset;

	      // rect
	      auto normalize_v_o_r = scale_value_on_range;
	      double rheight = normalize_v_o_r(count, 1, maxn, 10, ydelta);
	      auto rectcp = std::make_tuple(xoff, yoff + (rheight / 2));
	      rect_element r = make_rect_centered(rectcp, pstyl,
						  { rwidth, rheight });
	      obj.add_element(r);

	      // text
	      sized_text(obj, typov, ssz, std::to_string(count), xoff,
			 yoff + rheight + ltopspace + ssz);

	      // Add rectangle, space, text tile to offsets.
	      yoffset += rheight + ltopspace + ssz + ltopspace;

	      // Draw connnectors.
	      for (const string& agev : ageso)
		{
		  // Style from agepv.
		  style pstylage = pstyll;
		  auto klr = age_value_to_color_tint_shade(agepv);
		  pstylage._M_stroke_color = klr;

		  // Dash style if agepv == "ambiguous".
		  string dashstr;
		  const bool vback = agepv == a60::k::age_values.back();
		  const bool vfront = agepv == a60::k::age_values.front();
		  if (vback || vfront)
		    dashstr = "4 1";

		  const bool strokewidenp = false;
		  auto strokesz(0);
		  if (strokewidenp)
		    {
		      strokesz = pstylage._M_stroke_size * count;
		      pstylage._M_stroke_size = strokesz;
		    }

		  uint xoffvo = get_xoffset(agev, xstart, hspace);
		  point_2t bstart(xoffvo, vcenter + ltopspace);
		  point_2t bend(xoff, yoff + (pstylage._M_stroke_size / 2));

		  auto ln = make_line(bstart, bend, pstylage, dashstr);
		  obj.add_element(ln);

		  // Add to sum of playing and birth ages.
		  vpsum[indxp] += count;

		  const auto indx = a60::k::age_value_to_multiple(agev, true);
		  vsum[indx] += count;
		}
	    }
	}
    }

  // Print out sum of all ages per value, and sum total grey notch.
  for (const auto& val : a60::k::age_values)
    {
      auto indx = a60::k::age_value_to_multiple(val, true);
      uint xoff = get_xoffset(val, xstart, hspace);
      uint yoff = vcenter - ctopspace;
      uint total = vsum[indx];
      if (total > 0)
	{
	  sized_text(obj, typov, asz, std::to_string(total), xoff, yoff);

	  // Notch.
	  double rheight = scale_value_on_range(total, 1, maxn, 10, ydelta);
	  double ynotch = voffstart - rheight - ltopspace - asz;

	  // Top line.
	  point_2t topstart(xoff - (rwidth / 2), ynotch);
	  point_2t topend(xoff + (rwidth / 2), ynotch);

	  style nstyl = svg::k::w_style;
	  nstyl._M_stroke_size = 3;
	  nstyl._M_stroke_opacity = 1.0;
	  auto ln = make_line(topstart, topend, nstyl);
	  obj.add_element(ln);
	}
    }

  // Print out sum of all playing ages per value.
  for (const auto& val : a60::k::age_values)
    {
      auto indx = a60::k::age_value_to_multiple(val, true);
      uint xoff = get_xoffset(val, xstart, hspace);
      uint yoff = vpoffstart - ltopspace;
      uint total = vpsum[indx];
      if (total > 0)
	sized_text(obj, typov, asz, std::to_string(total), xoff, yoff);
    }

  // Total counts of birth ages and playing ages and "playing %"
  uint avocount = std::accumulate(vsum.begin(), vsum.end(), uint(0));
  uint avpcount = std::accumulate(vpsum.begin(), vpsum.end(), uint(0));
  double playingper = double(avpcount) / (avpcount + avocount);
  uint playingperi = static_cast<uint>(playingper * 100);
  string playingperstr = std::to_string(playingperi) + "% playing";

  const uint xsumoff = width - a60::k::metamargin;
  typol._M_anchor = svg::typography::anchor::end;
  typol._M_align = svg::typography::align::right;
  sized_text(obj, typol, lsz, "total values found", xsumoff, vcenter);

  typol._M_w = svg::typography::weight::normal;
  sized_text(obj, typol, asz, std::to_string(avocount), xsumoff,
	     vcenter - ctopspace);
  sized_text(obj, typol, asz, std::to_string(avpcount), xsumoff,
	     vcenter + ctopspace + asz);
  sized_text(obj, typol, asz, playingperstr, xsumoff,
	     vcenter + ctopspace + asz * 3);
  return obj;
}


/// Analyze metadata directory or subset of it and generate summary
/// output for all in directory.
void
render_metadata_aggregate_chord(const area<> a, const vumids& cumulative,
				const strings& namedkeyso, const string aggname)
{
  // Rank ids.
  vsids idsranked(a60::k::id_dimensions.size());
  for (uint index(0); index < a60::k::id_dimensions.size(); ++index)
    {
      sids& csids = idsranked[index];
      const umids& cumuids = cumulative[index];
      for (const auto& idp : cumuids)
	{
	  auto [ name, count ] = idp;
	  csids.insert(make_tuple(count, name));
	}
    }

  // Serialize.
  serialize_metadata_aggregate(idsranked, namedkeyso, aggname);

  // Render graph/chord element.
  const string outname = mangle_aggregate(aggname);
  svg_element obj = h_chord_graph(cumulative, outname, a);

  // Titles.
  auto tsz = 16;
  typography typot = get_metadata_typo(tsz, true, false);

  auto [ width, height ] = obj._M_area;
  auto xpos = width - a60::k::metamargin;
  auto ypos = height - a60::k::metamargin;

  // Ages / partition titles.
  string title("ages");
  if (!aggname.empty())
    title += string(a60::k::space + aggname);
  sized_text_r(obj, typot, tsz, title, xpos, ypos, -90);

  // Subset.
  render_metadata_aggregate_namekeys(obj, namedkeyso, xpos);
}


/// Analyze metadata directory or subset of it, and generate summary
/// output for all in directory.  Iff field is non zero, then subset
/// it to data files with field and match.
void
analyze_metadata_aggregate_chord(const area<> a,
				 const string field = "", const string match = "",
				 const string wfield = "", const string wvalue = "")
{
  // Set of files used for summary.
  const string idir = a60::io::get_run_time_resources().metadata;
  const strings files = field.empty() ?
			a60::io::populate_files(idir, ".json") :
			files_with_field_matching(field, match);

  vumids cumulative = cache_metadata(files, wfield, wvalue);

  // Get set of named keys.
  strings namedkeys;
  if (wfield.empty())
    namedkeys = files_with_field_matching(field, match, true);
  else
    namedkeys = files_with_fields_matching(files, wfield, wvalue, true);

  string outname;
  const string outname1 = mangle_metadata_aggregate(outname, field, match);
  const string outname2 = mangle_metadata_aggregate(outname1, wfield, wvalue);
  render_metadata_aggregate_chord(a, cumulative, namedkeys, outname2);
}


/// Analyze metadata directory or subset of it, and generate summary
/// output for all in directory.
void
analyze_metadata_aggregate_chord(const area<> a, const strings& namedkeys,
				 const string aggname,
				 const string wfield = "", const string wvalue = "")
{
  string idir = a60::io::get_run_time_resources().metadata;
  idir = a60::io::end_path(idir);

  // Vector with cumulative maps of name to count for each dimension
  // in a60::k::id_dimensions.
  strings files;
  for (const string& s: namedkeys)
    files.push_back(idir + s + ".json");

  vumids cumulative = cache_metadata(files, wfield, wvalue);

  // Match name_keys against wfield, if any.
  strings namedkeysedit;
  if (wfield.empty())
    namedkeysedit = namedkeys;
  else
    {
      strings namedkeys2 = files_with_fields_matching(files, wfield, wvalue, true);
      namedkeysedit = namedkeys2;
    }

  const string outname = mangle_metadata_aggregate(aggname, wfield, wvalue);
  render_metadata_aggregate_chord(a, cumulative, namedkeysedit, outname);
}

} // namepace svg

#endif
