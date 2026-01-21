 #include "a60-svg.h"
#include "a60-svg-curves-grignani.h"

// 1 grignani
void
test_curve(std::string ofile)
{
  using namespace std;
  using namespace svg;

  area<> a = k::letter_096_v;
  svg_element obj(ofile, a);

  // Grid Setup
  const int cols = 6;
  const int rows = 6;
  const double cell_size = 300.0;
  const double margin = 50.0;
  const double radius = 100.0;

  // SVG Header
  double total_w = cols * cell_size + margin;
  //double total_h = rows * cell_size + margin;

  // Baseline Config (The "Control" group)
  ribbon_config base;
  base.fold_amplitude = 1.0;
  base.fold_frequency = 3.0;
  base.torsion = 0.8;
  base.roll_speed = 1.5;
  base.view_tilt_x = 0.5;
  base.view_tilt_y = 0.6;

  // Experiment Definitions
  array<string, 6> labels =
    {
      "Fold Amplitude (Loop Size)",
      "Fold Frequency (Complexity)",
      "Torsion (Z-Depth Wave)",
      "Roll Speed (Twist Amount)",
      "View Tilt X (Camera Pitch)",
      "View Tilt Y (Camera Yaw)"
    };

  for (int r = 0; r < rows; ++r)
    {
      // Draw Row Label
      double label_y = r * cell_size + 40;
      string lyt = format("<text x=\"20\" y=\"{}\" font-size=\"24\" fill=\"#333\" font-weight=\"bold\">{}:</text>\n", label_y, labels[r]);
      obj.add_raw(lyt);

      for (int c = 0; c < cols; ++c)
	{
	  ribbon_config config = base;

	  // Normalize column 0..1
	  double t = static_cast<double>(c) / (cols - 1);

	  // Apply parameter variations based on Row index
	  switch(r)
	    {
	    case 0: config.fold_amplitude = lerp(0.5, 2.0, t); break;
	    case 1: config.fold_frequency = lerp(1.0, 6.0, t); break;
	    case 2: config.torsion        = lerp(0.1, 2.5, t); break;
	    case 3: config.roll_speed     = lerp(0.0, 5.0, t); break;
	    case 4: config.view_tilt_x    = lerp(0.0, 3.14, t); break;
	    case 5: config.view_tilt_y    = lerp(0.0, 3.14, t); break;
	    }

	  double cx = c * cell_size + (cell_size / 2) + 20; // +20 for label offset
	  double cy = r * cell_size + (cell_size / 2) + 30;

	  // Generate ribbon
	  // Using 6 strands for clarity in small grid cells
	  string ribbon = make_rolling_ribbon(cx, cy, radius, 6, 0.04, config);
	  obj.add_raw(ribbon);

	  // Optional: Debug value text below each shape
	  double val = 0;
	  if (r==0)
	    val=config.fold_amplitude;
	  if (r==1)
	    val=config.fold_frequency;
	  // ... etc
	  string dbg = format("<text x=\"{}\" y=\"{}\" font-size=\"12\" text-anchor=\"middle\" fill=\"#888\">{:.2f}</text>", cx, cy + radius + 20, val);
	  obj.add_raw(dbg);
	}

      // Horizontal divider line
      if (r < rows - 1)
	{
	  double line_y = (r + 1) * cell_size;
	  string liney = format("<line x1=\"0\" y1=\"{}\" x2=\"{}\" y2=\"{}\" stroke=\"#eee\" stroke-width=\"2\" />\n", line_y, total_w, line_y);
	  obj.add_raw(liney);
	}
    }
}


int main()
{
  test_curve("curves-grignani-1");
  return 0;
}
