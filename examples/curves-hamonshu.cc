// Render the complete Hamonshu catalogue and ten deterministic seed variants.
// -*- mode: C++ -*-

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

// Every catalogue entry occupies one column. Row zero preserves the seed used
// by make_motif_path(); the following rows sample ten alternate seeds. Keeping
// the boxes fixed makes changes in seeded rotation, phase, direction, density,
// row count, and repeat count directly comparable down each column.
constexpr std::size_t variant_count = 10;
constexpr std::size_t row_count = variant_count + 1;
constexpr double label_width = 120;
constexpr double header_height = 28;
constexpr double cell_width = 88;
constexpr double cell_height = 70;
constexpr double cell_padding = 7;

static_assert(std::numeric_limits<unsigned>::digits >= 32);

/// Avalanche an unsigned value so adjacent variant numbers exercise unrelated
/// residues in the many modulo-based Hamonshu curve parameters.
unsigned
mix_seed(unsigned value)
{
  value ^= value >> 16;
  value *= 0x7feb352dU;
  value ^= value >> 15;
  value *= 0x846ca68bU;
  value ^= value >> 16;
  return value;
}

/// Preserve the catalogue seed in row zero and derive ten repeatable samples.
unsigned
seed_for_row(const hamonshu::pattern_spec& spec, const std::size_t row)
{
  const unsigned canonical = hamonshu::pattern_seed(spec);
  if (row == 0)
    return canonical;
  return mix_seed(canonical ^ (0x9e3779b9U * static_cast<unsigned>(row)));
}

/// Dispatch a motif with an explicit seed instead of the catalogue seed.
std::string
make_seeded_motif_path(const hamonshu::pattern_spec& spec,
                       const hamonshu::pattern_box& box,
                       const unsigned seed)
{
  using enum hamonshu::motif_kind;

  hamonshu::validate_pattern_spec(spec);
  if (!box.valid())
    throw std::runtime_error("Hamonshu motif box must be finite and positive");

  const hamonshu::pattern_context context {box, seed};
  std::string data;
  switch (hamonshu::classify_pattern(spec))
    {
    case waterline: hamonshu::make_waterlines(data, context); break;
    case crest: hamonshu::make_crests(data, context); break;
    case spiral: hamonshu::make_spirals(data, context); break;
    case spray: hamonshu::make_spray(data, context); break;
    case arc: hamonshu::make_arcs(data, context); break;
    case lattice: hamonshu::make_lattice(data, context); break;
    case bubble: hamonshu::make_bubbles(data, context); break;
    case scroll: hamonshu::make_scrolls(data, context); break;
    case fan: hamonshu::make_fans(data, context); break;
    case breaker: hamonshu::make_breakers(data, context); break;
    case braid: hamonshu::make_braids(data, context); break;
    case cascade: hamonshu::make_cascade(data, context); break;
    case ripple: hamonshu::make_ripples(data, context); break;
    case fountain: hamonshu::make_fountains(data, context); break;
    case cloud: hamonshu::make_clouds(data, context); break;
    case cell: hamonshu::make_cells(data, context); break;
    }

  if (data.empty())
    throw std::runtime_error("Hamonshu motif generated no SVG path data");
  return data;
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
cell_markup(const double left, const double top, const std::string_view fill)
{
  std::ostringstream output;
  output << "<rect x=\"" << left
         << "\" y=\"" << top
         << "\" width=\"" << cell_width
         << "\" height=\"" << cell_height
         << "\" fill=\"" << fill
         << "\" stroke=\"#cbd5da\" stroke-width=\"0.35\"/>\n";
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
column_label(const hamonshu::pattern_spec& spec)
{
  std::string label = hamonshu::zero_padded(spec.first_page, 3);
  if (spec.last_page != spec.first_page)
    label += "-" + hamonshu::zero_padded(spec.last_page, 3);
  label += "/" + hamonshu::zero_padded(spec.motif, 2);
  return label;
}

void
render_parameter_grid(const std::string& output_name)
{
  const double grid_width = hamonshu::pattern_specs.size() * cell_width;
  const svg::area<> canvas {
    label_width + grid_width,
    header_height + row_count * cell_height,
  };
  svg::svg_element document(
    output_name,
    "Mori Yuzan Hamonshu volume 2: canonical curves and ten seed variants",
    canvas);

  svg::defs_element definitions;
  definitions.start_element();
  for (std::size_t row = 0; row != row_count; ++row)
    for (std::size_t column = 0;
         column != hamonshu::pattern_specs.size(); ++column)
      {
        const std::string clip_id
          = "clip-hamonshu-column-" + hamonshu::zero_padded(
              static_cast<unsigned>(column + 1), 3)
            + "-row-" + hamonshu::zero_padded(
              static_cast<unsigned>(row), 2);
        const double left = label_width + column * cell_width;
        const double top = header_height + row * cell_height;
        definitions.add_raw(clip_path_markup(clip_id, left, top));
      }
  definitions.finish_element();
  document.add_element(definitions);

  svg::group_element grid;
  grid.start_element("hamonshu-parameter-grid");
  grid.add_raw(text_markup(label_width - 8, header_height - 9,
                           "seed sample", "end", 9));
  for (std::size_t column = 0;
       column != hamonshu::pattern_specs.size(); ++column)
    {
      const double center_x
        = label_width + column * cell_width + cell_width / 2;
      grid.add_raw(text_markup(
        center_x, header_height - 9,
        column_label(hamonshu::pattern_specs[column]),
        "middle", 7));
    }

  const std::array ink_colors {
    svg::color_qi {24, 54, 68},
    svg::color_qi {61, 72, 125},
    svg::color_qi {100, 67, 121},
    svg::color_qi {133, 64, 105},
    svg::color_qi {153, 72, 77},
    svg::color_qi {151, 95, 49},
    svg::color_qi {126, 119, 43},
    svg::color_qi {77, 130, 62},
    svg::color_qi {41, 127, 104},
    svg::color_qi {31, 111, 137},
    svg::color_qi {39, 83, 139},
  };

  std::set<std::string> identifiers;
  std::size_t rendered_count = 0;
  for (std::size_t row = 0; row != row_count; ++row)
    {
      const double top = header_height + row * cell_height;
      const std::string row_label = row == 0
        ? "catalogue seed"
        : "variant " + hamonshu::zero_padded(
            static_cast<unsigned>(row), 2);
      grid.add_raw(text_markup(label_width - 8,
                               top + cell_height / 2 + 4,
                               row_label, "end", 10));

      const std::string_view background
        = row % 2 == 0 ? "#f7fafb" : "#eef4f6";
      const svg::style motif_style {
        svg::color::none, 0, ink_colors[row], 0.9, 0.6,
      };

      for (std::size_t column = 0;
           column != hamonshu::pattern_specs.size(); ++column)
        {
          const hamonshu::pattern_spec& spec
            = hamonshu::pattern_specs[column];
          const double left = label_width + column * cell_width;
          grid.add_raw(cell_markup(left, top, background));

          const hamonshu::pattern_box box {
            left + cell_padding,
            top + cell_padding,
            left + cell_width - cell_padding,
            top + cell_height - cell_padding,
          };
          const unsigned seed = seed_for_row(spec, row);
          const std::string path_data
            = make_seeded_motif_path(spec, box, seed);

          if (row == 0 && path_data != hamonshu::make_motif_path(spec, box))
            throw std::runtime_error(
              "canonical seeded dispatch diverged for "
              + hamonshu::pattern_id(spec));
          if (path_data.find("nan") != std::string::npos
              || path_data.find("inf") != std::string::npos)
            throw std::runtime_error(
              "non-finite Hamonshu path coordinate for "
              + hamonshu::pattern_id(spec));

          const std::string suffix
            = "-variant-" + hamonshu::zero_padded(
                static_cast<unsigned>(row), 2);
          const std::string id = hamonshu::pattern_id(spec) + suffix;
          if (!identifiers.insert(id).second)
            throw std::runtime_error(
              "duplicate Hamonshu variant identifier: " + id);
          const std::string clip_id
            = "clip-hamonshu-column-" + hamonshu::zero_padded(
                static_cast<unsigned>(column + 1), 3)
              + "-row-" + hamonshu::zero_padded(
                static_cast<unsigned>(row), 2);

          svg::group_element motif;
          motif.start_element(id);
          motif.add_title(
            hamonshu::pattern_title(spec) + "; " + row_label
            + "; seed " + std::to_string(seed));
          motif.add_element(svg::make_path(
            path_data, motif_style, id + "-path", true,
            "clip-path=\"url(#" + clip_id
              + ")\" stroke-linecap=\"round\" "
                "stroke-linejoin=\"round\""));
          motif.finish_element();
          grid.add_element(motif);
          ++rendered_count;
        }
    }

  grid.finish_element();
  document.add_element(grid);

  const std::size_t expected_count
    = hamonshu::pattern_specs.size() * row_count;
  if (rendered_count != expected_count || identifiers.size() != expected_count)
    throw std::runtime_error("Hamonshu parameter grid is incomplete");
}

} // namespace

int
main(const int argc, char** argv)
{
  try
    {
      const std::string output_name
        = argc == 2 ? argv[1] : "curves-hamonshu-variants";
      render_parameter_grid(output_name);
      std::cout << "generated " << hamonshu::pattern_specs.size()
                << " canonical Hamonshu motifs and " << variant_count
                << " variants per motif in " << output_name << ".svg\n";
      return 0;
    }
  catch (const std::exception& error)
    {
      std::cerr << "curves-hamonshu: " << error.what() << '\n';
      return 1;
    }
}
