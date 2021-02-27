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

#ifndef MiL_SVG_RADIAL_DIRECTION_ARC_H
#define MiL_SVG_RADIAL_DIRECTION_ARC_H 1


namespace svg {


/// Insert arc + arrow glyph that traces path of start to finish
/// trajectory.
svg_element
insert_direction_arc_at(svg_element& obj, const point_2t origin,
			const double rr, svg::style s,
			const uint spacer = 10)
{
  const double r = rr - spacer;
  const auto [ mindeg, maxdeg ] = get_radial_range();

  point_2t p0 = get_circumference_point_d(align_angle_to_north(mindeg),
					  r, origin);
  point_2t p4 = get_circumference_point_d(align_angle_to_north(maxdeg),
					  r, origin);

  // Define arc.
  std::ostringstream ossa;
  ossa << "M" << k::space << to_string(p0) << k::space;
  ossa << "A" << k::space;
  ossa << std::to_string(r) << k::comma << std::to_string(r) << k::space;
  ossa << align_angle_to_north(1) << k::space;
  ossa << "1, 1" << k::space;
  ossa << to_string(p4) << k::space;

  // Adjust style so the stroke color matches the fill color.
  s._M_stroke_color = s._M_fill_color;

  auto rspacer = spacer - 2;
  auto anglemax = align_angle_to_north(maxdeg);
  point_2t p5 = get_circumference_point_d(anglemax, r + rspacer, origin);
  point_2t p7 = get_circumference_point_d(anglemax, r - rspacer, origin);

  // Circumference arc length desired is radius times the angle of the
  // arc.
  auto theta = 2 * rspacer / r;
  auto thetad = theta * 180 / k::pi;
  auto alignd = align_angle_to_north(maxdeg + thetad);
  point_2t p6 = get_circumference_point_d(alignd, r, origin);

  // Define marker.
  std::ostringstream ossm;
  ossm << "M" << k::space << to_string(p4) << k::space;
  ossm << "L" << k::space;
  ossm << to_string(p5) << k::space;
  ossm << to_string(p6) << k::space;
  ossm << to_string(p7) << k::space;
  ossm << to_string(p4) << k::space;

  // Adjust style so that fill will be shown, and stroke hidden.
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

  // Reset style.
  s._M_fill_opacity = 0;
  s._M_stroke_opacity = 1;

  // Arc path.
  path_element parc;
  path_element::data da = { ossa.str(), 0 };
  parc.start_element();
  parc.add_data(da);
  parc.add_style(s);
  parc.finish_element();
  obj.add_element(parc);

  return obj;
}

} // namespace svg

#endif
