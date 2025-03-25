// alpha60 codecvt utf8 -*- mode: C++ -*-

// alpha60
// bittorrent x scrape x data + analytics

// Copyright (c) 2016-2024, Benjamin De Kosnik <b.dekosnik@gmail.com>

// This file is part of the alpha60 library.  This library is free
// software; you can redistribute it and/or modify it under the terms
// of the GNU General Public License as published by the Free Software
// Foundation; either version 3, or (at your option) any later
// version.

// This library is distributed in the hope that it will be useful, but
// WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// General Public License for more details.

#ifndef a60_SVG_CODECVT_H
#define a60_SVG_CODECVT_H 1

#include <ext/codecvt_specializations.h>


namespace svg {

/// Convert from ISO-8859-1 encoding to UTF-8, required for XML.
string
convert_8859_to_utf8(string in)
{
  using namespace std;

  using result = codecvt_base::result;
  using int_type = char;
  using ext_type = char;
  using state_type = __gnu_cxx::encoding_state;
  using ccutf8_codecvt = codecvt<int_type, ext_type, state_type>;

  string out;
  const int_type* internal_ptr;
  ext_type* external_ptr;
  ext_type* buf = new int_type[in.size() * 3];

  // Use iconv on linux, ie `iconv --list`

  // Internal encoding is ISO 8859 and external is UTF-8.
  state_type state("ISO-8859-16", "UTF-8", 0, 0);

  ccutf8_codecvt* cvtf8 = new ccutf8_codecvt;
  locale loc(locale::classic(), cvtf8);
  const ccutf8_codecvt& cvt = use_facet<ccutf8_codecvt>(loc);
  result r = cvt.out(state, in.c_str(), in.c_str() + in.size(), internal_ptr,
		     buf, buf + in.size() * 3, external_ptr);
  if (r == codecvt_base::ok)
    out = string(buf, external_ptr);
  else
    {
      if (r == codecvt_base::partial)
	{
	  ext_type* n_ptr = external_ptr;
	  while (n_ptr > external_ptr)
	    {
	      external_ptr = n_ptr;
	      r = cvt.in(state, in.c_str(), in.c_str() + in.size(),
			 internal_ptr, buf, buf + in.size() * 3, n_ptr);
	    }

	  if (r == codecvt_base::ok)
	    out = string(buf, n_ptr);
	  else
	    {
	      std::cerr << "convert_to_utf8: partial at "
			<< ulong(n_ptr - in.c_str()) << " "
			<< in << std::endl;
	      out = in;
	    }
	}
      if (r == codecvt_base::error)
	{
	  std::cerr << "convert_to_utf8: error " << in << std::endl;
	  out = in;
	}
    }

  // Any escape characters may have to be wrapped for HTML with &#[0][0][0];
  //delete cvtf8;
  delete [] buf;

  return out;
}


string
convert_to_utf8(string in)
{ return convert_8859_to_utf8(in); }

} // namespace svg
#endif
