#include "t_renderer.h"

#define REQUEST_TIME 30000

int main(void) {
  t_data tdata = {0, NULL};
  imageData imgData;
  display disp = {NULL, NULL, 0, 0};

  t_data_init(&tdata);
  bool status = 1;
  t_rend_init(&imgData, &disp);

  // Collect initial data
  system("curl https://api.tfl.gov.uk/Line/victoria/Arrivals/ -o trains.txt ");
  parseTrains(&tdata);

  uint64_t previousTick = 0;
  uint64_t requestRefreshTime = 0;

  while (status) {
    uint64_t currentTick = SDL_GetTicks();
    float delta = (float)(currentTick - previousTick) / 1000.0f;  // convert from ms to seconds
    requestRefreshTime += currentTick - previousTick;
    previousTick = currentTick;

    if (requestRefreshTime >= REQUEST_TIME) {
      requestRefreshTime = requestRefreshTime % REQUEST_TIME;
      system("curl https://api.tfl.gov.uk/Line/victoria/Arrivals/ -o trains.txt ");
      system("curl https://api.tfl.gov.uk/Line/victoria/Arrivals/ -o trains.json ");
      parseTrains(&tdata);

      printf("\n=============================\n");
      // print details about incoming train data - mostly for debugging purposes
      for (size_t i = 0; i < tdata.len; i++) {
        printf(
            "id: %u\n station name: %s\n direction: %d\n time to station: %u\n"
            " next station: %s\n alter station: %u\n status: %u\n"
            " data collected: %s\n data misses: %u\n"
            " x pos: %f, speed %f delta time: %f delta x: %f\n\n",
            tdata.data[i].id, getstationliteral(tdata.data[i].stationName), tdata.data[i].direction,
            tdata.data[i].timeToStation, getstationliteral(tdata.data[i].nextStation),
            tdata.data[i].alterStation, tdata.data[i].status,
            tdata.data[i].dataCollected ? "yes" : "no", tdata.data[i].dataMisses, tdata.data[i].x,
            tdata.data[i].speed, delta, tdata.data[i].speed * delta);
      }
    }

    updateTrainPosition(&tdata, delta);

    t_rend_drawPixels(&imgData, disp.renderer, tdata, delta);
    status = t_rend_event();
    // printf("finished a cycle\n");
    // status = false;
  }

  t_data_quit(&tdata);
  t_rend_quit(&imgData, &disp);
  return 0;
}