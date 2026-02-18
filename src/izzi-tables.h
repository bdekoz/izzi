// izzi HTML tables  -*- mode: C++ -*-

// Copyright (c) 2025-2026, Benjamin De Kosnik <b.dekosnik@gmail.com>

// This file is part of the alpha60 library.  This library is free
// software; you can redistribute it and/or modify it under the terms
// of the GNU General Public License as published by the Free Software
// Foundation; either version 3, or (at your option) any later
// version.

// This library is distributed in the hope that it will be useful, but
// WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// General Public License for more details.

#ifndef izzi_TABLES_H
#define izzi_TABLES_H 1

#include "a60-svg.h"
#include <string_view>

namespace svg {

using std::ostringstream;

/// Arithmetic on date stamps and date hour stamps.
/// Return time_point associated with that iso-date string and hour.
std::chrono::year_month_day
date_stamp_to_year_month_day(const string dstamp)
{
  string ddate = dstamp.substr(0, 10);

#ifndef __clang__
  std::chrono::year_month_day ymd;
  std::istringstream iss(ddate);
  iss >> std::chrono::parse("%F", ymd);
  if (iss.fail())
    {
      string m("date_stamp_to_year_month_day: failed to parse (");
      m += ddate;
      m += ")";
      m += k::newline;
      throw std::runtime_error(m);
    }
#else
  // emscripten
  // Extract parts as integers
  int y = std::stoi(ddate.substr(0, 4));
  int m = std::stoi(ddate.substr(5, 2));
  int d = std::stoi(ddate.substr(8, 2));

  // Construct the object directly
  std::chrono::year_month_day ymd {
    std::chrono::year{y},
    std::chrono::month(static_cast<unsigned int>(m)),
    std::chrono::day(static_cast<unsigned int>(d))
  };
#endif
  return ymd;
}


/// Converts ISO datestamp to HTML time element with accessible-readable
/// long form with month names, instead of just a list of digits.
string
iso_datestamp_string_to_html_time(const string ds)
{
  std::chrono::year_month_day ymd = date_stamp_to_year_month_day(ds);
  string ds_long = std::format("{:%B %d, %Y}", ymd);

  // Build the HTML time element
  string html_element;

  // <time datetime="YYYY-MM-DD" aria-label="Month Day, Year">Month Day, Year</time>
  html_element = "<time datetime=";
  html_element += k::quote;
  html_element += ds;
  html_element += k::quote;
  html_element += k::space;
  html_element += "aria-label=";
  html_element += k::quote;
  html_element += ds_long;
  html_element += k::quote;
  html_element += ">";
  html_element += k::space;
  html_element += ds;
  html_element += k::space;
  html_element += "</time>";

  return html_element;
}


/// Cleanup Pandas HTML table export.
/// Pandas html table export from properly constructed json is excellent.
/// However, knowing the data, it can be made more legible with a smarter table head.
/// So, strip thead and simplify names, make multi-column, multi-row for clarity.
string
simplify_pandas_table(const string minimetricf)
{
  string html;
  std::ifstream ifs(minimetricf);
  if (ifs.good())
    {
      /*
	<thead>
	  <tr style="text-align: right;">
	  <th>metric</th>
	  <th>firefox_median</th>
	  <th>firefox_rsd</th>
	  <th>chrome_median</th>
	  <th>chrome_rsd</th>
	  <th>difference</th>
	  </tr>
	</thead>
      */
      ostringstream oss;
      oss << ifs.rdbuf();
      html = oss.str();

      const string thead_start("<thead>");
      const string thead_end("</thead>");
      auto startpos = html.find(thead_start);
      auto endpos = html.find(thead_end);

      const string theadnu = R"_delimiter_(
	<thead>
	  <tr>
	    <th rowspan="2" width="25%">metric</th>
	    <th colspan="2" width="25%">firefox</th>
	    <th colspan="2" width="25%">chrome</th>
	    <th rowspan="2" width="25%">difference</th>
	  </tr>
	  <tr>
	    <th>median</th>
	    <th>rsd</th>
	    <th>median</th>
	    <th>rsd</th>
	   </tr>
	</thead>
      )_delimiter_";

      if (startpos != string::npos && endpos != string::npos)
	{
	  auto length = endpos + thead_end.size() - startpos;
	  html.replace(startpos, length, theadnu);
	}
      else
	std::cout << "cannot find table heads to swap" << std::endl;
    }
  return html;
}


/// Link row in table.
string
serialize_link_row_2c_4f(string_view uri1, string_view link1,
			 string_view uri2, string_view link2)
{
  // td elements == left
  // th elements == center

  // NB: Use {{}} for literal quoting, as src may have '/' and '&' etc.
  constexpr const char* tdblank = R"_delimiter_(<th style="padding: 5px;"><a href="{}"> {} </a></th>)_delimiter_";

  ostringstream oss;
  oss << "<tr>" << svg::k::newline;
  oss << std::format(tdblank, uri1, link1) << svg::k::newline;
  oss << std::format(tdblank, uri2, link2) << svg::k::newline;
  oss << "</tr>" << svg::k::newline;
  return oss.str();
}


/// Image row in table.
string
serialize_image_row_2c_4f(string_view img1src, string_view img1alt,
			  string_view img2src, string_view img2alt)
{
  // td elements == left
  // th elements == center

  // NB: Use {{}} for literal quoting, as src may have '/' and '&' etc.
  constexpr const char* tdblank = R"_delimiter_(<th style="padding: 5px;"><img src="{{}}" alt="{{}}" width="50%"></th>)_delimiter_";

  ostringstream oss;
  oss << "<tr>" << svg::k::newline;
  oss << std::format(tdblank, img1src, img1alt) << svg::k::newline;
  oss << std::format(tdblank, img2src, img2alt) << svg::k::newline;
  oss << "</tr>" << svg::k::newline;
  return oss.str();
}


/// Serialize borderless image table.
void
serialize_2_image_table(const string& gtitlelc)
{
  const string tblstart = R"_delimiter_(<table style="border-collapse: collapse; width: 100%;">)_delimiter_";
  const string tblend = "</table>";

  string img1 = "";
  string img1alt = "";
  string img2 = "";
  string img2alt = "";

  ostringstream oss;
  oss << tblstart << svg::k::newline;
  oss << serialize_image_row_2c_4f(img1, img1alt, img2, img2alt);
  oss << tblend << svg::k::newline;

  const string metricf = gtitlelc + "-image-table.html";
  std::ofstream ofs(metricf);
  if (ofs.good())
    ofs << oss.str() << svg::k::newline;
}

/// Caption element, both without styling and as H3 element in markdown.
const string cap = "<caption>";
const string capH3 = R"_delimiter_(<caption style="font-size: 1.17em; font-weight: bold; margin: 1em 0; text-align: left;">)_delimiter_";



/// Serialize information about the analysis passes as an html table.
/// Flatten all objects into one total number.
void
serialize_meta_collection_table(const string& gtitlelc, const string sdur,
				const uint btihasz,
				const uint dl, const uint ul)
{
  /*
    HTML table element, simplified as
    <table>
      <thead>
      </thead>
      <tbody>
	<tr>
	  <td>HTML tables</td>
	</tr>
      </tbody>
    </table>

    duration btihasz downloaders* uploaders*
    * = total, per btiha

    QED 4-column-6-field row
  */
  const string metricf = gtitlelc + "-meta-collection-table.html";
  std::ofstream ofs(metricf);
  if (ofs.good())
    {
      const string thead = R"_delimiter_(
	<thead>
	  <tr>
	    <th rowspan="2" width="25%">duration</th>
	    <th rowspan="2" width="25%">btiha size</th>
	    <th colspan="2" width="25%">downloaders</th>
	    <th colspan="2" width="25%">uploaders</th>
	  </tr>
	  <tr>
	    <th>total</th>
	    <th>per btiha</th>
	    <th>total</th>
	    <th>per btiha</th>
	   </tr>
	</thead>
      )_delimiter_";

      ofs.imbue(std::locale(""));
      ofs << std::fixed << std::setprecision(0);

      ofs << "<table>" << svg::k::newline;
      ofs << thead << svg::k::newline;

#if 0
      string tts(gtitlelc);
      std::ranges::replace(tts, k::hyphen, k::space);
      ofs << cap << tts << "</caption>" << svg::k::newline;
#else
      ofs << capH3 << "Aggregate Table" << "</caption>" << svg::k::newline;
#endif

      ofs << "<tbody>" << svg::k::newline;
      ofs << "<tr>" << svg::k::newline;

      ofs << "<td>" << sdur << "</td>" << svg::k::newline;
      ofs << "<th>" << btihasz << "</th>" << svg::k::newline;
      ofs << "<td>" << dl << "</td>" << svg::k::newline;
      ofs << "<td>" << dl / btihasz << "</td>" << svg::k::newline;
      ofs << "<td>" << ul << "</td>" << svg::k::newline;
      ofs << "<td>" << ul / btihasz << "</td>" << svg::k::newline;

      ofs << "</tr>" << svg::k::newline;
      ofs << "</tbody>" << svg::k::newline;
      ofs << "</table>" << svg::k::newline;
    }
}


/// Serialize row of media object information.
///
/// media_object btihasz downloaders* uploaders* #weeks sample_dates
/// * = total, per btiha
string
serialize_row_6c_8f(const string moname, const uint btihasz,
		    const uint dl, const uint ul,
		    const uint weeksn, const string sdates)
{
  ostringstream oss;
  oss.imbue(std::locale(""));
  oss << std::fixed << std::setprecision(0);

  oss << "<tr>" << svg::k::newline;

  // td elements == left
  // th elements == center
  oss << "<td>" << moname << "</td>" << svg::k::newline;
  oss << "<td>" << weeksn << "</td>" << svg::k::newline;
  oss << "<td>" << sdates << "</td>" << svg::k::newline;
  oss << "<td>" << btihasz << "</td>" << svg::k::newline;
  oss << "<td>" << dl << "</td>" << svg::k::newline;
  oss << "<td>" << dl / btihasz << "</td>" << svg::k::newline;
  oss << "<td>" << ul << "</td>" << svg::k::newline;
  oss << "<td>" << ul / btihasz << "</td>" << svg::k::newline;

  oss << "</tr>" << svg::k::newline;

  return oss.str();
}


} // namespace svg

#endif
