 #include "a60-svg.h"
 #include "a60-svg-curves-grignani.h"

// 2 ripple
void
test_curve(std::string ofile)
{
  using namespace std;
  using namespace svg;

  area<> a = k::letter_096_v;
  svg_element obj(ofile, a);

  const int cols = 6;
  const int rows = 6;
  const double cell_w = 280.0;
  const double cell_h = 220.0;
  const double margin_left = 220.0; // Space for labels
  double total_w = margin_left + (cols * cell_w);

  const double radius = 200.0;

  // Using 6 strands for clarity in small grid cells
  int rsize = 1; // 8, 6

  // 1 @ r 200 == 0.2645 mm
  // 6 @ r 200 == 1.587 mm
  // implies
  // 1.89 @ r 200 = 0.5 mm
  // 3.78 @ r 200 = 1.0 mm
  // 7.56 @ r 200 = 2.0 mm
  double rwidth = 1.89; // 1.89[0-8] == 5.03 // 1.88[6-9] = .497

  // Baseline Config
  ripple_config base;
  base.amplitude = 40.0;
  base.frequency = 1.5;
  base.phase_shift = 0.0;
  base.decay = 0.0;
  base.view_tilt_x = 0.5;
  base.view_tilt_y = 0.2; // Shallow angle

  array<string, 6> labels =
    {
      "Amplitude (Height)",
      "Frequency (Waves)",
      "Phase Shift (Offset)",
      "Decay (Taper)",
      "View Tilt X (Pitch)",
      "View Tilt Y (Yaw)"
    };

  for (int r = 0; r < rows; ++r)
    {
      // Draw Row Label
      double label_y = r * cell_h + (cell_h / 2.0);
      string lty = format("<text x=\"20\" y=\"{}\" font-size=\"18\" fill=\"#333\" font-weight=\"bold\">{}</text>\n", label_y, labels[r]);
      obj.add_raw(lty);

      // Divider
      string ly = format("<line x1=\"0\" y1=\"{}\" x2=\"{}\" y2=\"{}\" stroke=\"#ddd\" stroke-width=\"1\" />\n", (r+1)*cell_h, total_w, (r+1)*cell_h);
      obj.add_raw(ly);

      for (int c = 0; c < cols; ++c)
	{
	  ripple_config config = base;
	  double t = static_cast<double>(c) / (cols - 1);
	  double val_debug = 0.0;

	  // Parametric variations
	  switch(r)
	    {
	    case 0: // Amplitude: 10 to 100
	      config.amplitude = lerp(10.0, 100.0, t);
	      val_debug = config.amplitude;
	      break;
	    case 1: // Frequency: 0.5 to 5.0
	      config.frequency = lerp(0.5, 5.0, t);
	      val_debug = config.frequency;
	      break;
	    case 2: // Phase: 0 to 2*Pi
	      config.phase_shift = lerp(0.0, 2.0 * numbers::pi, t);
	      val_debug = config.phase_shift;
	      break;
	    case 3: // Decay: 0.0 to 1.5 (1.5 means it disappears before end)
	      config.decay = lerp(0.0, 1.5, t);
	      val_debug = config.decay;
	      break;
	    case 4: // Tilt X: 0.0 to 1.5
	      config.view_tilt_x = lerp(0.0, 1.5, t);
	      val_debug = config.view_tilt_x;
	      break;
	    case 5: // Tilt Y: -1.0 to 1.0
	      config.view_tilt_y = lerp(-1.0, 1.0, t);
	      val_debug = config.view_tilt_y;
	      break;
	    }

	  double cx = margin_left + (c * cell_w) + (cell_w / 2.0);
	  double cy = (r * cell_h) + (cell_h / 2.0);

	  // Generate Ribbon
	  // Length 200, 8 strands, width 6
	  double rwidthv = rwidth + c * rwidth;
	  string ribbon = make_ripple_ribbon(cx, cy, radius, rsize, rwidthv, config);
	  obj.add_raw(ribbon);

	  // Debug Text
	  string dtxt = format("<text x=\"{}\" y=\"{}\" font-size=\"12\" fill=\"#666\" text-anchor=\"middle\">{:.2f}</text>\n", cx, cy + 80.0, val_debug);
	  obj.add_raw(dtxt);
	}
    }
}


int main()
{
  test_curve("curves-grignani-2");
  return 0;
}
