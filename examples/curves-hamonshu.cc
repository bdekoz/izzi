// Explore the form parameter space of Izzi's Hamonshu motif API.
// -*- mode: C++ -*-

#include <algorithm>
#include <array>
#include <cstddef>
#include <iostream>
#include <limits>
#include <set>
#include <sstream>
#include <stdexcept>
#include <string>
#include <string_view>

#include <a60-svg-curves-hamonshu.h>

namespace {

namespace hamonshu = svg::hamonshu;

struct motif_selection
{
  unsigned first_page;
  unsigned last_page;
  unsigned motif;
};

// The grey markers drawn beside titles in examples/curves-hamonshu.svg select
// these fifteen source motifs. Store source coordinates instead of catalogue
// offsets so insertions elsewhere in the catalogue cannot change the set.
constexpr std::array selected_motifs {
  motif_selection {1, 1, 1},
  motif_selection {2, 2, 1},
  motif_selection {3, 3, 1},
  motif_selection {3, 3, 2},
  motif_selection {6, 6, 2},
  motif_selection {6, 6, 3},
  motif_selection {9, 9, 1},
  motif_selection {17, 17, 3},
  motif_selection {18, 18, 4},
  motif_selection {20, 20, 4},
  motif_selection {23, 23, 2},
  motif_selection {39, 39, 2},
  motif_selection {40, 40, 1},
  motif_selection {46, 47, 2},
  motif_selection {51, 51, 5},
};

// Each column changes only curvature: wave height, curl radius, and transverse
// displacement. Density, phase, orientation, sampling, and every SVG color
// remain fixed, so differences across a row are differences in form alone.
constexpr std::array curvature_ratios {
  0.25, 0.45, 0.70, 1.0, 1.30, 1.65, 2.10,
};

constexpr double label_width = 310;
constexpr double header_height = 50;
constexpr double cell_width = 150;
constexpr double cell_height = 96;
constexpr double cell_padding = 11;

const hamonshu::pattern_spec&
selected_pattern(const motif_selection selection)
{
  const auto match = std::find_if(
    hamonshu::pattern_specs.begin(), hamonshu::pattern_specs.end(),
    [=](const hamonshu::pattern_spec& spec) {
      return spec.first_page == selection.first_page
        && spec.last_page == selection.last_page
        && spec.motif == selection.motif;
    });
  if (match == hamonshu::pattern_specs.end())
    throw std::runtime_error("selected Hamonshu motif is not in the catalogue");
  return *match;
}

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
cell_markup(const double left, const double top)
{
  std::ostringstream output;
  output << "<rect x=\"" << left
         << "\" y=\"" << top
         << "\" width=\"" << cell_width
         << "\" height=\"" << cell_height
         << "\" fill=\"#f7fafb\""
         << " stroke=\"#cbd5da\" stroke-width=\"0.45\"/>\n";
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
row_label(const hamonshu::pattern_spec& spec)
{
  std::string label = hamonshu::zero_padded(spec.first_page, 3);
  if (spec.last_page != spec.first_page)
    label += "-" + hamonshu::zero_padded(spec.last_page, 3);
  label += "/" + hamonshu::zero_padded(spec.motif, 2) + "  ";
  label += hamonshu::display_name(spec.name);
  return label;
}

std::string
configuration_title(const hamonshu::pattern_spec& spec,
                    const hamonshu::motif_config& config)
{
  return hamonshu::pattern_title(spec)
    + "; density=" + number(config.density)
    + "; curvature=" + number(config.curvature)
    + "; phase=" + number(config.phase)
    + "; rotation=" + number(config.rotation)
    + "; reflected=" + (config.reflected ? "true" : "false");
}

void
validate_public_api()
{
  const auto& spec = hamonshu::pattern_specs.front();
  const hamonshu::pattern_box box {0, 0, 120, 80};

  if (hamonshu::pattern_id(spec)
        != "hamonshu-page-001-motif-01-nested-current-scrolls"
      || hamonshu::pdf_scan_page(1) != 2
      || hamonshu::pdf_scan_page(51) != 27)
    throw std::runtime_error("Hamonshu source-index mapping changed");

  const std::string canonical_path = hamonshu::make_motif_path(spec, box);
  if (canonical_path
      != hamonshu::make_motif_path(spec, box, hamonshu::motif_config {}))
    throw std::runtime_error(
      "default Hamonshu configuration changed the canonical form");

  const auto expect_changed_form
    = [&](const hamonshu::motif_config& config,
          const std::string_view parameter) {
        if (canonical_path == hamonshu::make_motif_path(spec, box, config))
          throw std::runtime_error(
            "Hamonshu " + std::string(parameter)
            + " did not change the canonical path");
      };
  hamonshu::motif_config changed;
  changed.density = 1.5;
  expect_changed_form(changed, "density");
  changed = {};
  changed.curvature = 1.5;
  expect_changed_form(changed, "curvature");
  changed = {};
  changed.phase = 0.5;
  expect_changed_form(changed, "phase");
  changed = {};
  changed.rotation = 0.2;
  expect_changed_form(changed, "rotation");
  changed = {};
  changed.reflected = true;
  expect_changed_form(changed, "reflection");

  if (hamonshu::classify_pattern(
        hamonshu::pattern_spec {3, 3, 3, "towering-breaking-wave"})
        != hamonshu::motif_kind::breaker
      || hamonshu::classify_pattern(
           hamonshu::pattern_spec {49, 49, 2, "ringed-current"})
           != hamonshu::motif_kind::ripple)
    throw std::runtime_error(
      "Hamonshu classifier matched a token fragment instead of a word stem");

  expect_invalid_argument(
    [&] { (void)hamonshu::make_motif_path(spec, hamonshu::pattern_box {}); },
    "invalid Hamonshu pattern box");
  expect_invalid_argument(
    [&] {
      (void)hamonshu::make_motif_path(
        hamonshu::pattern_spec {50, 50, 1, "colophon"}, box);
    },
    "invalid Hamonshu catalogue entry");

  hamonshu::motif_config invalid;
  invalid.density = 0;
  expect_invalid_argument(
    [&] { (void)hamonshu::make_motif_path(spec, box, invalid); },
    "zero Hamonshu density");

  invalid = {};
  invalid.curvature = std::numeric_limits<double>::infinity();
  expect_invalid_argument(
    [&] { (void)hamonshu::make_motif_path(spec, box, invalid); },
    "non-finite Hamonshu curvature");

  invalid = {};
  invalid.samples_per_curve = 7;
  expect_invalid_argument(
    [&] { (void)hamonshu::make_motif_path(spec, box, invalid); },
    "undersampled Hamonshu motif");

  std::set<std::string> selected_ids;
  for (const auto selection : selected_motifs)
    {
      const auto& selected_spec = selected_pattern(selection);
      hamonshu::validate_pattern_spec(selected_spec);
      selected_ids.insert(hamonshu::pattern_id(selected_spec));
    }
  if (selected_ids.size() != selected_motifs.size())
    throw std::runtime_error(
      "Hamonshu motif selection contains a duplicate source entry");

  std::set<hamonshu::motif_kind> kinds;
  for (const auto& catalogue_spec : hamonshu::pattern_specs)
    {
      hamonshu::validate_pattern_spec(catalogue_spec);
      kinds.insert(hamonshu::classify_pattern(catalogue_spec));
    }
  constexpr std::size_t motif_kind_count = 16;
  if (kinds.size() != motif_kind_count)
    throw std::runtime_error(
      "Hamonshu catalogue does not exercise every motif family");
}

void
render_parameter_grid(const std::string& output_name)
{
  validate_public_api();

  const svg::area<> canvas {
    label_width + curvature_ratios.size() * cell_width,
    header_height + selected_motifs.size() * cell_height,
  };
  svg::svg_element document(
    output_name,
    "Mori Yuzan Hamonshu volume 2: curated monochrome curvature explorer",
    canvas);

  svg::defs_element definitions;
  definitions.start_element();
  for (std::size_t row = 0; row != selected_motifs.size(); ++row)
    for (std::size_t column = 0;
         column != curvature_ratios.size(); ++column)
      {
        const std::string clip_id
          = "clip-hamonshu-row-" + std::to_string(row)
            + "-column-" + std::to_string(column);
        definitions.add_raw(clip_path_markup(
          clip_id, label_width + column * cell_width,
          header_height + row * cell_height));
      }
  definitions.finish_element();
  document.add_element(definitions);

  const svg::style motif_style {
    svg::color::none, 0, svg::color_qi {31, 66, 79}, 0.94, 0.82,
  };

  svg::group_element grid;
  grid.start_element("hamonshu-parameter-grid");
  grid.add_raw(text_markup(label_width - 12, header_height - 17,
                           "source motif / form ratio", "end", 11));
  for (std::size_t column = 0;
       column != curvature_ratios.size(); ++column)
    {
      const double center_x
        = label_width + column * cell_width + cell_width / 2;
      const double curvature = curvature_ratios[column];
      grid.add_raw(text_markup(
        center_x, header_height - 17,
        "curvature = " + number(curvature)
          + (curvature == 1.0 ? " (canonical)" : ""),
        "middle", 10));
    }

  std::set<std::string> identifiers;
  std::size_t rendered_count = 0;
  for (std::size_t row = 0; row != selected_motifs.size(); ++row)
    {
      const auto& spec = selected_pattern(selected_motifs[row]);
      const double top = header_height + row * cell_height;
      grid.add_raw(text_markup(label_width - 12,
                               top + cell_height / 2 + 4,
                               row_label(spec), "end", 9));

      std::set<std::string> forms;
      const hamonshu::pattern_box comparison_box {
        0, 0,
        cell_width - 2 * cell_padding,
        cell_height - 2 * cell_padding,
      };
      for (std::size_t column = 0;
           column != curvature_ratios.size(); ++column)
        {
          const double left = label_width + column * cell_width;
          grid.add_raw(cell_markup(left, top));

          const hamonshu::pattern_box box {
            left + cell_padding,
            top + cell_padding,
            left + cell_width - cell_padding,
            top + cell_height - cell_padding,
          };
          hamonshu::motif_config config;
          config.curvature = curvature_ratios[column];
          const std::string path_data
            = hamonshu::make_motif_path(spec, box, config);
          const std::string form_signature
            = hamonshu::make_motif_path(spec, comparison_box, config);
          if (path_data.empty()
              || path_data.find("nan") != std::string::npos
              || path_data.find("inf") != std::string::npos
              || form_signature.empty()
              || form_signature.find("nan") != std::string::npos
              || form_signature.find("inf") != std::string::npos)
            throw std::runtime_error(
              "invalid Hamonshu path data for " + hamonshu::pattern_id(spec));
          if (!forms.insert(form_signature).second)
            throw std::runtime_error(
              "curvature produced a duplicate Hamonshu form for "
              + hamonshu::pattern_id(spec));

          if (curvature_ratios[column] == 1.0
              && path_data != hamonshu::make_motif_path(spec, box))
            throw std::runtime_error(
              "canonical Hamonshu parameter sample diverged for "
              + hamonshu::pattern_id(spec));

          const std::string id = hamonshu::pattern_id(spec)
            + "-curvature-" + std::to_string(column);
          if (!identifiers.insert(id).second)
            throw std::runtime_error(
              "duplicate Hamonshu parameter-cell identifier: " + id);
          const std::string clip_id
            = "clip-hamonshu-row-" + std::to_string(row)
              + "-column-" + std::to_string(column);

          svg::group_element cell;
          cell.start_element(id);
          cell.add_title(configuration_title(spec, config));
          cell.add_element(svg::make_path(
            path_data, motif_style, id + "-path", true,
            "clip-path=\"url(#" + clip_id
              + ")\" stroke-linecap=\"round\" "
                "stroke-linejoin=\"round\""));
          cell.finish_element();
          grid.add_element(cell);
          ++rendered_count;
        }
    }

  grid.finish_element();
  document.add_element(grid);

  const std::size_t expected_count
    = selected_motifs.size() * curvature_ratios.size();
  if (rendered_count != expected_count || identifiers.size() != expected_count)
    throw std::runtime_error("Hamonshu parameter grid is incomplete");
}

} // namespace

int
main(const int argc, char** argv)
{
  try
    {
      if (argc > 2)
        throw std::invalid_argument(
          "usage: curves-hamonshu [output-name-without-extension]");
      const std::string output_name
        = argc == 2 ? argv[1] : "curves-hamonshu";
      render_parameter_grid(output_name);
      std::cout << "generated "
                << selected_motifs.size() * curvature_ratios.size()
                << " monochrome Hamonshu form samples in "
                << output_name << ".svg\n";
      return 0;
    }
  catch (const std::exception& error)
    {
      std::cerr << "curves-hamonshu: " << error.what() << '\n';
      return 1;
    }
}
