// alpha60/izzi serialize/deserialize to/from JSON -*- mode: C++ -*-

// Copyright (c) 2016-2025, Benjamin De Kosnik <b.dekosnik@gmail.com>

// This file is part of the alpha60 library.  This library is free
// software; you can redistribute it and/or modify it under the terms
// of the GNU General Public License as published by the Free Software
// Foundation; either version 3, or (at your option) any later
// version.

// This library is distributed in the hope that it will be useful, but
// WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// General Public License for more details.

#ifndef a60_JSON_BASICS_H
#define a60_JSON_BASICS_H 1

#include <iostream>
#include <sstream>
#include <fstream>

#define RAPIDJSON_HAS_STDSTRING 1
#include "rapidjson/prettywriter.h"
#include "rapidjson/pointer.h"
#include "rapidjson/filereadstream.h"
#include "rapidjson/reader.h"
#include "rapidjson/document.h"
#include "rapidjson/error/en.h"


namespace svg {

/// Aliases/using.
namespace rj = rapidjson;
using jsonstream = rj::PrettyWriter<rj::StringBuffer>;


/// Serialize set of id_type
void
serialize_id_types_json(jsonstream& writer, const auto& ids, const string tag)
{
  writer.String(tag);
  writer.StartArray();
  uint countp(1);
  for (const auto& id : ids)
    {
      auto [ count, name ] = id;

      writer.StartObject();

      writer.String("number");
      writer.Int(countp++);

      writer.String("id");
      writer.String(name);

      writer.String("count");
      writer.Int(count);

      writer.EndObject();
    }
  writer.EndArray();
}


/// Deserialize input string.
rj::Document
deserialize_json_string_to_dom(const string& json)
{
  // DOM
  rj::Document dom;
  dom.Parse(json.c_str());
  if (dom.HasParseError())
    {
      std::cerr << "error: cannot parse input string " << std::endl;
      std::cerr << rj::GetParseError_En(dom.GetParseError()) << std::endl;
      std::cerr << dom.GetErrorOffset() << std::endl;
    }
  return dom;
}


/// Deserialize input file.
rj::Document
deserialize_json_to_dom(const string input_file)
{
  // Deserialize input file.
  std::ifstream ifs(input_file);
  string json;
  if (ifs.good())
    {
      std::ostringstream oss;
      oss << ifs.rdbuf();
      json = oss.str();
    }
  else
    {
      std::cerr << "error: cannot open input file " << input_file << std::endl;
    }

  return deserialize_json_string_to_dom(json);
}


/// Load JSON file to in-memory DOM.
rj::Document
deserialize_json_to_dom_object(const string input_file)
{
  rj::Document dom = deserialize_json_to_dom(input_file);

  if (!dom.IsObject())
    {
      std::cerr << "error: expected document to be an object "
		<< "in file: "<< input_file << std::endl;
    }

  return dom;
}


/// Load JSON file to in-memory DOM array.
rj::Document
deserialize_json_to_dom_array(const string input_file)
{
  rj::Document dom = deserialize_json_to_dom(input_file);

  if (!dom.IsArray())
    {
      std::cerr << "error: expected document to be an array "
		<< "in file: "<< input_file << std::endl;
    }

  return dom;
}


/// Search DOM for string literals.
string
search_dom_for_string_field(const rj::Document& dom, const string finds)
{
  string found;
  if (!dom.HasParseError() && dom.HasMember(finds.c_str()))
    {
      const rj::Value& a = dom[finds.c_str()];
      if (a.IsString())
	found = a.GetString();
    }
  return found;
}


/// Search DOM for integer values.
int
search_dom_for_int_field(const rj::Document& dom, const string finds)
{
  int found(0);
  if (!dom.HasParseError() && dom.HasMember(finds.c_str()))
    {
      const rj::Value& a = dom[finds.c_str()];
      if (a.IsInt())
	found = a.GetInt();
    }
  return found;
}


/// Extract from raw JSON value to double.
double
extract_dom_value_to_double(const rj::Value& v)
{
  double ret(0.00123);
  const bool stringp = v.IsString();
  const bool numberp = v.IsNumber();
  const bool boolp = v.IsBool();
  if (numberp)
    {
      // https://miloyip.github.io/rapidjson/md_doc_tutorial.html#QueryNumber
      ret = v.GetDouble();
    }
  if (stringp)
    {
      string s = v.GetString();
      ret = std::stod(s);
    }
  if (boolp)
    ret = static_cast<double>(v.GetBool());

  return ret;
}


/// Deserialize json array,
/// extract specific fields from array objects,
/// return as vec of point_2t
///
/// jdata == input is path + filename of input JSON data file
/// afield == pointer in json file dom to specific array's data
/// field1 == x field to extract in array
/// field2 == y field to extract in array
vrange
deserialize_json_array_object_field_n(const string jdata, const string afield,
				      const string field1, const string field2,
				      const bool verbosep = true)
{
  vrange ret;

  // Load input JSON data file into DOM.
  // Assuming 2025-era mozilla pageload json input styles, aka
  // /home/bkoz/src/mozilla-a11y-data-visual-forms/data/2025-01-27-minimal.json
  rj::Document dom(deserialize_json_to_dom(jdata));
  rj::Value* ap = rj::Pointer(afield.c_str()).Get(dom);
  if (ap)
    {
      const rj::Value& av = *ap;
      if (av.IsArray())
	{
	  for (uint j = 0; j < av.Size(); ++j)
	    {
	      double x(0);
	      double y(0);
	      const rj::Value& vssub = av[j];
	      const bool f1p = vssub.HasMember(field1.c_str());
	      const bool f2p = vssub.HasMember(field2.c_str());
	      if (f1p)
		{
		  const rj::Value& vx = vssub[field1.c_str()];
		  x = extract_dom_value_to_double(vx);
		}
	      if (f2p)
		{
		  const rj::Value& vy = vssub[field2.c_str()];
		  y = extract_dom_value_to_double(vy);
		}

	      if (f1p && f2p)
		ret.push_back(std::make_tuple(x, y));
	      else
		{
		  string m("deserialize_json_array_object_field_n:: error ");
		  m += k::tab;
		  m += "iteration " + std::to_string(j);
		  m += " not found";
		  m += k::newline;

		  if (verbosep)
		    throw std::runtime_error(m);
		}
	    }
	}
    }

  return ret;
}

} // namespace svg
#endif
