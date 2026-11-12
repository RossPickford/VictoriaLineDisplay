#include <string.h>
#include <time.h>

#include "t_renderer.h"

#define REQUEST_TIME 30

int main(void) {
  t_data tdata = {0, NULL};
  imageData imgData;

  display disp = {NULL, NULL, 0, 0};
  struct RGBLedMatrixOptions options;
  struct RGBLedRuntimeOptions runtimeoptions;

  memset(&options, 0, sizeof(options));
  options.rows = 32;
  options.cols = 128;
  options.chain_length = 1;
  options.parallel = 2;

  memset(&runtimeoptions, 0, sizeof(runtimeoptions));
  runtimeoptions.drop_privileges = -1;

  /* This supports all the led commandline options. Try --led-help */
  disp.matrix =
      led_matrix_create_from_options_and_rt_options(&options, &runtimeoptions);
  if (disp.matrix == NULL) return 1;

  disp.canvas = led_matrix_create_offscreen_canvas(disp.matrix);
  led_canvas_get_size(disp.canvas, &disp.width, &disp.height);

  // Collect initial data
  system(
      "sudo curl https://api.tfl.gov.uk/Line/victoria/Arrivals/ -o "
      "trains.txt ");
  printf("getting initial trains...\n\n");

  t_data_init(&tdata);
  bool status = t_rend_init(&imgData);

  assert(status);

  parseTrains(&tdata);

  float previousTick = 0.0f, timeelapsed = 0.0f;

  while (status) {
    float currentTick = (float)clock() / (float)CLOCKS_PER_SEC;
    float delta =
        (float)(currentTick - previousTick);  // convert from ms to seconds
    previousTick = currentTick;

    timeelapsed += delta;

    // if t_request has the most up to date train data (which hasn't been read)
    // - update the train nodes
    if (timeelapsed >= REQUEST_TIME) {
      printf("\ntime elapsed\n");
      timeelapsed = (float)((int)timeelapsed % REQUEST_TIME);

      system(
          "sudo curl https://api.tfl.gov.uk/Line/victoria/Arrivals/ -o "
          "trains.txt ");
      parseTrains(&tdata);

      printf("\n=============================\n");
      // print details about incoming train data - mostly for debugging purposes
      for (size_t i = 0; i < tdata.len; i++) {
        printf(
            "id: %u\n station name: %s\n direction: %d\n time to station: %u\n"
            " next station: %s\n alter station: %u\n status: %u\n"
            " data collected: %s\n data misses: %u\n"
            " x pos: %f, speed %f delta time: %f delta x: %f\n\n",
            tdata.data[i].id, getstationliteral(tdata.data[i].stationName),
            tdata.data[i].direction, tdata.data[i].timeToStation,
            getstationliteral(tdata.data[i].nextStation),
            tdata.data[i].alterStation, tdata.data[i].status,
            tdata.data[i].dataCollected ? "yes" : "no",
            tdata.data[i].dataMisses, tdata.data[i].x, tdata.data[i].speed,
            delta, tdata.data[i].speed * delta);
      }
    }

    updateTrainPosition(&tdata, delta);

    status = t_rend_drawPixels(&imgData, &disp, tdata, delta);
  }

  t_data_quit(&tdata);
  t_rend_quit(&imgData);

  led_matrix_delete(disp.matrix);

  return 0;
}