#include "t_renderer.h"

#define REQUEST_TIME 30000

int main(void) {
  t_data tdata = {0, NULL};
  imageData imgData;
  SDL_Window* window = NULL;
  SDL_Renderer* renderer = NULL;

  pixelDataExtended** tBuffer = NULL;

  t_data_init(&tdata);
  bool status = t_rend_init(&window, &renderer, &imgData, &tBuffer);

  // Collect initial data
  system("t_request");
  parseTrains(&tdata);
  // updateTrainNode(tNodes, tData, &tNodeLen, tDataLen);

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
    if (!requestcomplete && requestRefreshTime >= REQUEST_TIME) {
      system("start t_request");
      requestRefreshTime = requestRefreshTime % REQUEST_TIME;
    } else if (requestcomplete) {
      parseTrains(&tdata);

      uint8_t zero = 0;
      fwrite(&zero, 1, 1, requeststatus);
      fseek(requeststatus, 0, SEEK_SET);
    }

    updateTrainPosition(&tdata, delta);

    status =
        t_rend_event() && t_rend_drawPixels(&imgData, renderer, tdata, tBuffer);
    // printf("finished a cycle\n");
    // status = false;
  }

  t_data_quit(&tdata);
  // t_nodes_quit(tNodes);
  t_rend_quit(window, renderer, &imgData, &tBuffer);
  return 0;
}