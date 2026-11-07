#include <string.h>
#include <time.h>

#include "t_renderer.h"

#define REQUEST_TIME 15000

int main(void) {
  t_data tdata = {0, NULL};
  imageData imgData;

  display disp = {NULL, NULL, 0, 0};
  struct RGBLedMatrixOptions options;
  struct RGBLedRuntimeOptions runtimeoptions;

  memset(&options, 0, sizeof(options));
  options.rows = 32;
  options.cols = 64;
  options.chain_length = 2;

  memset(&runtimeoptions, 0, sizeof(runtimeoptions));
  runtimeoptions.drop_privileges = -1;

  /* This supports all the led commandline options. Try --led-help */
  disp.matrix =
      led_matrix_create_from_options_and_rt_options(&options, &runtimeoptions);
  if (disp.matrix == NULL) return 1;

  disp.canvas = led_matrix_create_offscreen_canvas(disp.matrix);
  led_canvas_get_size(disp.canvas, &disp.width, &disp.height);

  /**
   * This file holds the status of the t_request - 1 for it has completed the
   * request and the trains file is ready to be parsed, 0 is for the data has
   * been parsed, and 2 tells t_request that the program has been closed.
   */
  FILE* requeststatus = fopen("requestcomplete.txt", "w+");
  assert(requeststatus);
  printf("file opened\n\n");
  uint8_t requestcomplete = 0;

  fwrite(&requestcomplete, 1, 1, requeststatus);
  fseek(requeststatus, 0, SEEK_SET);

  printf("data written onto file\n\n");

  // Collect initial data
  system(
      "sudo curl https://api.tfl.gov.uk/Line/victoria/Arrivals/ -o "
      "trains.txt ");
  printf("getting initial trains...\n\n");
  // start the request program - collects train data every 30 seconds and then
  // informs the program
  system("sudo ./t_request &");
  printf("launching request program...\n\n");
  parseTrains(&tdata);

  t_data_init(&tdata);
  bool status = t_rend_init(&imgData);
  float previousTick = 0.0f;

  while (status) {
    float currentTick = (float)clock() / (float)CLOCKS_PER_SEC;
    float delta =
        (float)(currentTick - previousTick);  // convert from ms to seconds
    previousTick = currentTick;

    // collect the value from requestcomplete.bin and reset file stream
    fgets(&requestcomplete, 2, requeststatus);
    fseek(requeststatus, 0, SEEK_SET);
    assert(requestcomplete == 0 || requestcomplete == 1 ||
           requestcomplete == 2);

    // if t_request has the most up to date train data (which hasn't been read)
    // - update the train nodes
    if (requestcomplete == 1) {
      parseTrains(&tdata);

      /*       printf("\n=============================\n");
            // print details about incoming train data - mostly for debugging
         purposes for (size_t i = 0; i < tdata.len; i++) { printf( "id: %u\n
         station name: %s\n direction: %d\n time to station: %u\n" " next
         station: %s\n alter station: %u\n status: %u\n" " data collected: %s\n
         data misses: %u\n" " x pos: %f, speed %f delta time: %f delta x:
         %f\n\n", tdata.data[i].id,
         getstationliteral(tdata.data[i].stationName), tdata.data[i].direction,
         tdata.data[i].timeToStation,
                  getstationliteral(tdata.data[i].nextStation),
                  tdata.data[i].alterStation, tdata.data[i].status,
                  tdata.data[i].dataCollected ? "yes" : "no",
                  tdata.data[i].dataMisses, tdata.data[i].x,
         tdata.data[i].speed, delta, tdata.data[i].speed * delta);
            } */

      uint8_t zero = 0;
      fwrite(&zero, 1, 1, requeststatus);
      fseek(requeststatus, 0, SEEK_SET);
    }

    updateTrainPosition(&tdata, delta);

    status = t_rend_drawPixels(&imgData, &disp, tdata, delta);
  }

  // Singalling to the requst executable that the program is finished
  char two = 2;
  fwrite(&two, 1, 1, requeststatus);
  fclose(requeststatus);

  t_data_quit(&tdata);
  t_rend_quit(&imgData);

  led_matrix_delete(disp.matrix);

  return 0;
}