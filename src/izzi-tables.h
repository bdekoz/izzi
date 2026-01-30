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

/// Converts ISO datestamp to human-readable long form with month names.
string
iso_datestamp_string_to_html_time(const string ds)
{
  // Build the HTML time element
  string html_element;

  string ds_long("");
  std::chrono::year_month_day ymd;
  std::stringstream ss(ds);
  // %F is the standard specifier for YYYY-MM-DD (ISO 8601)
  if (std::chrono::from_stream(ss, "%F", ymd))
    ds_long = std::format("%B %A %Y", ymd);
  else
    {
      std::cerr << "datestamp_to_html_time:: error with datestamp '"
		<< ds << "'" << std::endl;
    }

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


// Image row in table.
string
serialize_row_2c_4f(string_view img1src, string_view img1alt,
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
  oss << serialize_row_2c_4f(img1, img1alt, img2, img2alt);
  oss << tblend << svg::k::newline;

  const string metricf = gtitlelc + "-image-table.html";
  std::ofstream ofs(metricf);
  if (ofs.good())
    ofs << oss.str() << svg::k::newline;
}


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
      string tts(gtitlelc);
      std::ranges::replace(tts, k::hyphen, k::space);
      ofs << "<caption>" << tts << "</caption>" << svg::k::newline;
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
