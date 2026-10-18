#include "t_renderer.h"

#define REQUEST_TIME 30000

int main(void)
{
    t_data tdata = {0, NULL};
    imageData imgData;
    SDL_Window *window = NULL;
    SDL_Renderer *renderer = NULL;

    pixelDataExtended **tBuffer = NULL;

    t_data_init(&tdata);
    bool status = t_rend_init(&window, &renderer, &imgData, &tBuffer);

    // Collect initial data
        requestTrains(&tdata);
        // updateTrainNode(tNodes, tData, &tNodeLen, tDataLen);

    uint64_t previousTick = 0;
    uint64_t requestRefreshTime = 0;
    while (status)
    {
        uint64_t currentTick = SDL_GetTicks();
        float delta = (float)(currentTick - previousTick) / 1000.0f; //convert from ms to seconds
        requestRefreshTime += currentTick - previousTick;
        previousTick = currentTick;

        if (requestRefreshTime >= REQUEST_TIME)
        {
            requestRefreshTime -= REQUEST_TIME;
            requestTrains(&tdata);
            // updateTrainNode(tNodes, tData, &tNodeLen, tDataLen);
        }

        updateTrainPosition(&tdata, delta);

        status = t_rend_event() && t_rend_drawPixels(&imgData, renderer, tdata, tBuffer);
        // printf("finished a cycle\n");
        // status = false;
    }

    t_data_quit(&tdata);
    // t_nodes_quit(tNodes);
    t_rend_quit(window, renderer, &imgData, &tBuffer);
    return 0;
}