// radial arc -*- mode: C++ -*-

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

#ifndef MiL_SVG_RADIAL_DIRECTION_ARC_GLYPH_H
#define MiL_SVG_RADIAL_DIRECTION_ARC_GLYPH_H 1


namespace svg {


/// Arc + arrow glyph that traces path of start to finish trajectory.
void
direction_arc_at(svg_element& obj, const point_2t origin,
		 const double rr, svg::style s, const double spacer = 10)
{
  // Adjust style so the stroke color matches the fill color.
  s._M_stroke_color = s._M_fill_color;

  const double r = rr - spacer;
  const auto [ mindeg, maxdeg ] = get_radial_range();
  auto anglemin = zero_angle_north_cw(mindeg);
  auto anglemax = zero_angle_north_cw(maxdeg);

  // Define arc.
  const bool largearcp = abs(anglemax - anglemin) > 180;
  const bool cwp = true;
  point_2t p0 = get_circumference_point_d(anglemin, r, origin);
  point_2t p4 = get_circumference_point_d(anglemax, r, origin);
  string sarc = make_path_arc_circumference(p0, p4, r, largearcp, cwp);

  // Style for stroke/line.
  s._M_fill_opacity = 0;
  s._M_stroke_opacity = 1;

  // Draw arc path.
  path_element parc;
  path_element::data da = { sarc, 0 };
  parc.start_element();
  parc.add_data(da);
  parc.add_style(s);
  parc.finish_element();
  obj.add_element(parc);

  // Define marker/arrow/triangle base points.
  double rspacer = std::max(5.0, spacer - 2);
  point_2t p5 = get_circumference_point_d(anglemax, r + rspacer, origin);
  point_2t p7 = get_circumference_point_d(anglemax, r - rspacer, origin);

  // ... and the tip of the triangle.
  point_2t p6 = get_circumference_point_d(anglemax - 90, rspacer, p4);


  // Define closed triangle/arrow path.
  std::ostringstream ossm;
  ossm << "M" << k::space << to_string(p4) << k::space;
  ossm << "L" << k::space;
  ossm << to_string(p5) << k::space;
  ossm << to_string(p6) << k::space;
  ossm << to_string(p7) << k::space;
  ossm << to_string(p4) << k::space;

  // Style so that fill will be shown, and stroke hidden.
  s._M_fill_opacity = 1;
  s._M_stroke_opacity = 0;

  // End marker path.
  path_element pmarker;
  path_element::data dm = { ossm.str(), 0 };
  pmarker.start_element();
  pmarker.add_data(dm);
  pmarker.add_style(s);
  pmarker.finish_element();
  obj.add_element(pmarker);
}


/// Title on same arc.
void
direction_arc_title_at(svg_element& obj, const point_2t origin,
		       const int radius, const typography typo,
		       const string title, const uint pcnt = 30)
{
  // Make arc text path
  const auto [ mindeg, maxdeg ] = get_radial_range();
  auto mina = zero_angle_north_cw(mindeg);
  auto maxa = zero_angle_north_cw(maxdeg);

  point_2t pmin = get_circumference_point_d(mina, radius, origin);
  point_2t pmax = get_circumference_point_d(maxa, radius, origin);

  string titlearc = make_path_arc_circumference(pmax, pmin, radius);

  string arc_name("arc-text");
  path_element parc;
  path_element::data da = { titlearc, 0 };
  parc.start_element(arc_name);
  parc.add_data(da);
  parc.add_style(typo._M_style);
  parc.finish_element();
  obj.store_element(parc);

  // Put it together.
  text_element::data dt = { 0, 0, title, typo };
  text_path_element tp(arc_name, std::to_string(pcnt) + "%");
  tp.start_element();
  tp.add_data(dt);
  tp.finish_element();
  obj.add_element(tp);
}

} // namespace svg

#endif
