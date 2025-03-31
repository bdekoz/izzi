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
serialize_id_types_json(jsonstream& writer, const auto& ids, string tag)
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
deserialize_json_string_to_dom(string& json)
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
deserialize_json_to_dom(string input_file)
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
deserialize_json_to_dom_object(string input_file)
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
deserialize_json_to_dom_array(string input_file)
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

} // namespace svg
#endif
