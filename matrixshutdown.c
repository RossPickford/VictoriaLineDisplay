#include <stdio.h>
#include <string.h>
#include "led-matrix-c.h"

int main(void) {
  struct RGBLedMatrix* matrix;
  struct LedCanvas* canvas;
  struct RGBLedMatrixOptions options;
  struct RGBLedRuntimeOptions runtimeoptions;

  memset(&options, 0, sizeof(options));
  options.rows = 32;
  options.cols = 128;
  options.chain_length = 1;
  options.parallel = 2;

  memset(&runtimeoptions, 0, sizeof(runtimeoptions));
  runtimeoptions.drop_privileges = -1;

  matrix =
      led_matrix_create_from_options_and_rt_options(&options, &runtimeoptions);
  canvas = led_matrix_create_offscreen_canvas(matrix);

  led_canvas_clear(canvas);
  led_matrix_delete(matrix);

  return 0;
}
