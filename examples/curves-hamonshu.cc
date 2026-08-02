#include <iostream>
#include <set>
#include <stdexcept>
#include <string>
#include <string_view>

#include "a60-svg-curves-hamonshu.h"

namespace {

constexpr std::size_t column_count = 17;
constexpr double cell_width = 100;
constexpr double cell_height = 80;
constexpr double cell_margin = 7;

template<typename Function>
void
expect_runtime_error(Function function, const std::string_view operation)
{
  try
    { function(); }
  catch (const std::runtime_error&)
    { return; }
  throw std::runtime_error(std::string(operation) + " did not reject input");
}

void
test_curves_hamonshu(const std::string& output_name)
{
  using namespace svg::hamonshu;

  if (pattern_id(pattern_specs.front())
      != "hamonshu-page-001-motif-01-nested-current-scrolls"
      || pdf_scan_page(1) != 2 || pdf_scan_page(51) != 27)
    throw std::runtime_error("Hamonshu source-index mapping changed");
  expect_runtime_error(
    [&] { (void)make_motif_path(pattern_specs.front(), pattern_box {}); },
    "invalid Hamonshu pattern box");
  expect_runtime_error(
    [&] {
      (void)make_motif_path(
        pattern_spec {50, 50, 1, "colophon"}, pattern_box {0, 0, 1, 1});
    },
    "invalid Hamonshu catalogue entry");

  constexpr std::size_t row_count
    = (pattern_specs.size() + column_count - 1) / column_count;
  const svg::area<> canvas {column_count * cell_width,
                            row_count * cell_height};
  svg::svg_element document(
    output_name,
    "Mori Yuzan Hamonshu volume 2 procedural wave-pattern catalogue",
    canvas);

  const svg::style motif_style {
    svg::color::none, 0.0, svg::color::black, 1.0, 0.65
  };
  std::set<std::string> identifiers;
  svg::group_element catalogue;
  catalogue.start_element("hamonshu-volume-2-patterns");

  for (std::size_t index = 0; index != pattern_specs.size(); ++index)
    {
      const auto& spec = pattern_specs[index];
      validate_pattern_spec(spec);

      const double left = (index % column_count) * cell_width + cell_margin;
      const double top = (index / column_count) * cell_height + cell_margin;
      const pattern_box box {
        left, top, left + cell_width - 2 * cell_margin,
        top + cell_height - 2 * cell_margin
      };
      const std::string id = pattern_id(spec);
      if (!identifiers.insert(id).second)
        throw std::runtime_error("duplicate Hamonshu pattern identifier: " + id);

      const std::string path_data = make_motif_path(spec, box);
      if (path_data.find("nan") != std::string::npos
          || path_data.find("inf") != std::string::npos)
        throw std::runtime_error("non-finite Hamonshu path coordinate: " + id);

      svg::group_element motif;
      motif.start_element(id);
      motif.add_title(pattern_title(spec));
      motif.add_element(svg::make_path(path_data, motif_style));
      motif.finish_element();
      catalogue.add_element(motif);
    }

  catalogue.finish_element();
  document.add_element(catalogue);
}

} // namespace

int
main(const int argc, char** argv)
{
  try
    {
      const std::string output_name
        = argc == 2 ? argv[1] : "curves-hamonshu";
      test_curves_hamonshu(output_name);
      std::cout << "generated " << svg::hamonshu::pattern_specs.size()
                << " Hamonshu motifs in " << output_name << ".svg\n";
      return 0;
    }
  catch (const std::exception& error)
    {
      std::cerr << "curves-hamonshu: " << error.what() << '\n';
      return 1;
    }
}
