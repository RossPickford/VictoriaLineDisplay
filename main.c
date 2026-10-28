#include "t_renderer.h"

#define REQUEST_TIME 15000

int main(void) {
  t_data tdata = {0, NULL};
  imageData imgData;
  SDL_Window* window = NULL;
  SDL_Renderer* renderer = NULL;

  pixelDataExtended** tBuffer = NULL;

  t_data_init(&tdata);
  bool status = t_rend_init(&window, &renderer, &imgData, &tBuffer);

  // Collect initial data
  system("start t_request");
  system("curl https://api.tfl.gov.uk/Line/victoria/Arrivals/ -o trains.txt ");
  parseTrains(&tdata);

  FILE* requeststatus = fopen("requestcomplete.bin", "r+");
  uint8_t requestcomplete = 0;

  uint64_t previousTick = 0;
  uint64_t requestRefreshTime = 0;

  while (status) {
    uint64_t currentTick = SDL_GetTicks();
    float delta = (float)(currentTick - previousTick) /
                  1000.0f;  // convert from ms to seconds
    requestRefreshTime += currentTick - previousTick;
    previousTick = currentTick;

    // collect the value from requestcomplete.bin and reset file stream
    fgets(&requestcomplete, 2, requeststatus);
    fseek(requeststatus, 0, SEEK_SET);
    assert(requestcomplete == 0 || requestcomplete == 1);

    // request data from tfl server and then parse it once recieved
    /*  if (!requestcomplete && requestRefreshTime >= REQUEST_TIME) {
       system("start t_request");
       requestRefreshTime = requestRefreshTime % REQUEST_TIME;
     } else  */
    if (requestcomplete) {
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

      uint8_t zero = 0;
      fwrite(&zero, 1, 1, requeststatus);
      fseek(requeststatus, 0, SEEK_SET);
    }

    updateTrainPosition(&tdata, delta);

    status = t_rend_event() &&
             t_rend_drawPixels(&imgData, renderer, tdata, tBuffer, delta);
    // printf("finished a cycle\n");
    // status = false;
  }

  // Singalling to the requst executable that the program is finished
  char two = 2;
  fwrite(&two, 1, 1, requeststatus);
  fclose(requeststatus);

  t_data_quit(&tdata);
  // t_nodes_quit(tNodes);
  t_rend_quit(window, renderer, &imgData, &tBuffer);
  return 0;
}