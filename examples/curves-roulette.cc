// Explore the parameter space of Izzi's roulette-curve path API.
// -*- mode: C++ -*-

#include <algorithm>
#include <array>
#include <cmath>
#include <cstddef>
#include <iostream>
#include <limits>
#include <numbers>
#include <optional>
#include <set>
#include <sstream>
#include <stdexcept>
#include <string>
#include <string_view>

#include <a60-svg-curves-roulette.h>

namespace {

struct experiment
{
  // A missing kind selects the straight-line trochoid API. The two enum
  // values select the public centered-roulette API directly.
  std::optional<svg::roulette_kind> kind;
  std::size_t fixed_radius;
  std::size_t rolling_radius;
  std::string_view label;
};

// Each column moves the tracing point from the rolling-circle center, through
// the cycloidal value d/r=1, and out into increasingly prolate curves.
constexpr std::array point_distance_ratios {
  0.0, 0.25, 0.50, 0.75, 1.0, 1.25, 1.50, 2.0, 3.0,
};

// Each row changes the rolling geometry. Named cycloids are identified only
// at d/r=1; the rest of their rows remain members of the broader trochoid
// family rather than being mislabeled as the named special case.
constexpr std::array experiments {
  experiment {std::nullopt, 0, 1, "line trochoid"},
  experiment {svg::roulette_kind::epitrochoid, 1, 1,
              "epi 1:1 (cardioid at d/r=1)"},
  experiment {svg::roulette_kind::epitrochoid, 2, 1,
              "epi 2:1 (nephroid at d/r=1)"},
  experiment {svg::roulette_kind::epitrochoid, 3, 1, "epi 3:1"},
  experiment {svg::roulette_kind::epitrochoid, 5, 2, "epi 5:2"},
  experiment {svg::roulette_kind::epitrochoid, 11, 7, "epi 11:7"},
  experiment {svg::roulette_kind::hypotrochoid, 2, 1,
              "hypo 2:1 (Tusi at d/r=1)"},
  experiment {svg::roulette_kind::hypotrochoid, 3, 1,
              "hypo 3:1 (deltoid at d/r=1)"},
  experiment {svg::roulette_kind::hypotrochoid, 4, 1,
              "hypo 4:1 (astroid at d/r=1)"},
  experiment {svg::roulette_kind::hypotrochoid, 5, 2, "hypo 5:2"},
  experiment {svg::roulette_kind::hypotrochoid, 11, 7, "hypo 11:7"},
};

constexpr double label_width = 285;
constexpr double header_height = 54;
constexpr double cell_width = 205;
constexpr double cell_height = 205;
constexpr double cell_padding = 18;

template<typename Function>
void
expect_invalid_argument(Function function, const std::string_view operation)
{
  try
    { function(); }
  catch (const std::invalid_argument&)
    { return; }
  throw std::runtime_error(
    std::string(operation) + " did not reject invalid input");
}

std::string
number(const double value)
{
  std::ostringstream output;
  output.precision(3);
  output << value;
  return output.str();
}

std::string
clip_path_markup(const std::string& id, const double left, const double top)
{
  std::ostringstream output;
  output << "<clipPath id=\"" << id
         << "\" clipPathUnits=\"userSpaceOnUse\">"
         << "<rect x=\"" << left + 0.5
         << "\" y=\"" << top + 0.5
         << "\" width=\"" << cell_width - 1
         << "\" height=\"" << cell_height - 1
         << "\"/></clipPath>\n";
  return output.str();
}

std::string
cell_markup(const double left, const double top, const bool alternate,
            const bool cycloidal)
{
  const std::string_view fill = cycloidal
    ? "#e2edf1" : alternate ? "#eef4f6" : "#f8fafb";
  std::ostringstream output;
  output << "<rect x=\"" << left
         << "\" y=\"" << top
         << "\" width=\"" << cell_width
         << "\" height=\"" << cell_height
         << "\" fill=\"" << fill
         << "\" stroke=\"#c8d2d7\" stroke-width=\"0.5\"/>\n";
  return output.str();
}

std::string
text_markup(const double x, const double y, const std::string_view text,
            const std::string_view anchor, const double size)
{
  std::ostringstream output;
  output << "<text x=\"" << x
         << "\" y=\"" << y
         << "\" text-anchor=\"" << anchor
         << "\" font-family=\"monospace\" font-size=\"" << size
         << "\" fill=\"#34434a\">" << text << "</text>\n";
  return output.str();
}

std::string
make_trochoid_experiment(const experiment& value,
                         const double distance_ratio,
                         const double left, const double top)
{
  svg::trochoid_config config;
  config.rolling_radius = static_cast<double>(value.rolling_radius);
  config.point_distance = distance_ratio * config.rolling_radius;
  config.turns = 3;
  config.samples_per_turn = 160;

  const double inner_width = cell_width - 2 * cell_padding;
  const double inner_height = cell_height - 2 * cell_padding;
  const double maximum_t = 2 * std::numbers::pi * config.turns;
  const double x_span = config.rolling_radius * maximum_t
                        + 2 * config.point_distance;
  const double y_span = std::max(2 * config.point_distance, 0.25);
  const double scale = std::min(inner_width / x_span,
                                inner_height / y_span);
  const svg::point_2t origin {
    left + cell_padding + scale * config.point_distance,
    top + cell_height / 2 + scale * config.rolling_radius,
  };
  return svg::make_trochoid_path(origin, scale, config);
}

std::string
make_centered_experiment(const experiment& value,
                         const double distance_ratio,
                         const double left, const double top)
{
  if (!value.kind)
    throw std::logic_error("centered roulette experiment has no kind");

  svg::roulette_config config;
  config.fixed_radius = value.fixed_radius;
  config.rolling_radius = value.rolling_radius;
  config.point_distance
    = distance_ratio * static_cast<double>(config.rolling_radius);
  config.samples_per_turn = 180;

  const double fixed = static_cast<double>(config.fixed_radius);
  const double rolling = static_cast<double>(config.rolling_radius);
  const double center_radius
    = *value.kind == svg::roulette_kind::epitrochoid
        ? fixed + rolling : fixed - rolling;
  const double extent = center_radius + config.point_distance;
  const double inner_size
    = std::min(cell_width, cell_height) - 2 * cell_padding;
  const double scale = inner_size / (2 * extent);
  const svg::point_2t origin {
    left + cell_width / 2,
    top + cell_height / 2,
  };
  return svg::make_roulette_path(origin, scale, *value.kind, config);
}

std::string
make_experiment_path(const experiment& value, const double distance_ratio,
                     const double left, const double top)
{
  if (!value.kind)
    return make_trochoid_experiment(value, distance_ratio, left, top);
  return make_centered_experiment(value, distance_ratio, left, top);
}

std::string
experiment_title(const experiment& value, const double distance_ratio)
{
  std::string title(value.label);
  title += ", tracing distance d/r=" + number(distance_ratio);
  if (value.kind)
    {
      svg::roulette_config config;
      config.fixed_radius = value.fixed_radius;
      config.rolling_radius = value.rolling_radius;
      title += ", closure turns="
               + std::to_string(svg::roulette_completion_turns(config));
    }
  else
    title += ", sampled turns=3";
  return title;
}

void
validate_public_api()
{
  const svg::point_2t origin {0, 0};

  svg::trochoid_config trochoid;
  trochoid.rolling_radius = 2;
  trochoid.point_distance = 2;
  if (svg::make_trochoid_path(origin, 1, trochoid)
      != svg::make_cycloid_path(origin, 1, trochoid))
    throw std::runtime_error("cycloid wrapper changed the canonical curve");

  svg::roulette_config closure;
  closure.fixed_radius = 111;
  closure.rolling_radius = 70;
  if (svg::roulette_completion_turns(closure) != 70)
    throw std::runtime_error("roulette ratio did not reduce to 111:70");

  svg::roulette_config centered;
  centered.fixed_radius = 11;
  centered.rolling_radius = 7;
  centered.point_distance = 7;
  if (svg::make_epitrochoid_path(origin, 1, centered)
      != svg::make_epicycloid_path(origin, 1, centered))
    throw std::runtime_error("epicycloid wrapper changed the canonical curve");

  centered.fixed_radius = 4;
  centered.rolling_radius = 1;
  centered.point_distance = 1;
  if (svg::make_hypotrochoid_path(origin, 1, centered)
      != svg::make_hypocycloid_path(origin, 1, centered))
    throw std::runtime_error("hypocycloid wrapper changed the canonical curve");

  svg::roulette_config invalid_centered;
  invalid_centered.fixed_radius = 0;
  expect_invalid_argument(
    [&] { (void)svg::roulette_completion_turns(invalid_centered); },
    "zero fixed radius");

  invalid_centered.fixed_radius = 1;
  invalid_centered.rolling_radius = 1;
  expect_invalid_argument(
    [&] {
      (void)svg::make_hypotrochoid_path(origin, 1, invalid_centered);
    },
    "non-interior hypotrochoid circle");

  svg::trochoid_config invalid_trochoid;
  invalid_trochoid.samples_per_turn = 7;
  expect_invalid_argument(
    [&] { (void)svg::make_trochoid_path(origin, 1, invalid_trochoid); },
    "undersampled trochoid");
  expect_invalid_argument(
    [&] {
      (void)svg::make_trochoid_path(
        origin, std::numeric_limits<double>::infinity());
    },
    "non-finite output scale");
}

void
render_parameter_grid(const std::string& output_name)
{
  validate_public_api();

  const svg::area<> canvas {
    label_width + point_distance_ratios.size() * cell_width,
    header_height + experiments.size() * cell_height,
  };
  svg::svg_element document(
    output_name,
    "Trochoid, epitrochoid, and hypotrochoid parameter explorer",
    canvas);

  svg::defs_element definitions;
  definitions.start_element();
  for (std::size_t row = 0; row != experiments.size(); ++row)
    for (std::size_t column = 0;
         column != point_distance_ratios.size(); ++column)
      {
        const std::string clip_id
          = "clip-roulette-row-" + std::to_string(row)
            + "-column-" + std::to_string(column);
        definitions.add_raw(clip_path_markup(
          clip_id, label_width + column * cell_width,
          header_height + row * cell_height));
      }
  definitions.finish_element();
  document.add_element(definitions);

  const std::array ink_colors {
    svg::color_qi {44, 74, 88},
    svg::color_qi {52, 85, 130},
    svg::color_qi {68, 77, 145},
    svg::color_qi {91, 70, 143},
    svg::color_qi {117, 68, 130},
    svg::color_qi {143, 68, 108},
    svg::color_qi {151, 82, 76},
    svg::color_qi {145, 105, 55},
    svg::color_qi {112, 126, 52},
    svg::color_qi {70, 133, 72},
    svg::color_qi {39, 126, 105},
  };

  svg::group_element grid;
  grid.start_element("roulette-parameter-grid");
  grid.add_raw(text_markup(label_width - 12, header_height - 18,
                           "curve / radius ratio", "end", 12));
  for (std::size_t column = 0;
       column != point_distance_ratios.size(); ++column)
    {
      const double center_x
        = label_width + column * cell_width + cell_width / 2;
      const double ratio = point_distance_ratios[column];
      grid.add_raw(text_markup(
        center_x, header_height - 18,
        "d/r = " + number(ratio) + (ratio == 1 ? " (cycloid)" : ""),
        "middle", 12));
    }

  std::set<std::string> identifiers;
  std::size_t curve_count = 0;
  for (std::size_t row = 0; row != experiments.size(); ++row)
    {
      const experiment& value = experiments[row];
      const double top = header_height + row * cell_height;
      grid.add_raw(text_markup(label_width - 12,
                               top + cell_height / 2 + 5,
                               value.label, "end", 12));

      for (std::size_t column = 0;
           column != point_distance_ratios.size(); ++column)
        {
          const double ratio = point_distance_ratios[column];
          const double left = label_width + column * cell_width;
          grid.add_raw(cell_markup(
            left, top, (row + column) % 2 != 0, ratio == 1));

          const std::string id
            = "roulette-row-" + std::to_string(row)
              + "-column-" + std::to_string(column);
          if (!identifiers.insert(id).second)
            throw std::runtime_error("duplicate roulette cell id: " + id);
          const std::string path_data
            = make_experiment_path(value, ratio, left, top);
          if (path_data.empty()
              || path_data.find("nan") != std::string::npos
              || path_data.find("inf") != std::string::npos)
            throw std::runtime_error(
              "invalid roulette path data in " + id);

          const svg::style curve_style {
            svg::color::none, 0, ink_colors[row], 0.92, 1.05,
          };
          const std::string clip_id
            = "clip-roulette-row-" + std::to_string(row)
              + "-column-" + std::to_string(column);
          svg::group_element cell;
          cell.start_element(id);
          cell.add_title(experiment_title(value, ratio));
          cell.add_element(svg::make_path(
            path_data, curve_style, id + "-path", true,
            "clip-path=\"url(#" + clip_id
              + ")\" stroke-linecap=\"round\" "
                "stroke-linejoin=\"round\""));
          cell.finish_element();
          grid.add_element(cell);
          ++curve_count;
        }
    }

  grid.finish_element();
  document.add_element(grid);

  const std::size_t expected_count
    = experiments.size() * point_distance_ratios.size();
  if (curve_count != expected_count || identifiers.size() != expected_count)
    throw std::runtime_error("roulette parameter grid is incomplete");
}

} // namespace

int
main(const int argc, char** argv)
{
  try
    {
      if (argc > 2)
        throw std::invalid_argument(
          "usage: curves-roulette [output-name-without-extension]");
      const std::string output_name
        = argc == 2 ? argv[1] : "curves-roulette";
      render_parameter_grid(output_name);
      std::cout << "generated "
                << experiments.size() * point_distance_ratios.size()
                << " roulette parameter samples in "
                << output_name << ".svg\n";
      return 0;
    }
  catch (const std::exception& error)
    {
      std::cerr << "curves-roulette: " << error.what() << '\n';
      return 1;
    }
}
