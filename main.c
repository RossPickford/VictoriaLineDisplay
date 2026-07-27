#include "t_renderer.h"

int main()
{
    TrainData *tData = NULL;
    uint8_t tDataLen = 0;

    trainNode *tNodes = NULL;

    SDL_Window *window = NULL;
    SDL_Renderer *renderer = NULL;
    imageData imgData;

    t_data_init();
    bool status = t_nodes_init(&tNodes) && t_rend_init(&window, &renderer, &imgData);

    // Collect initial data
    tData = requestTrains(&tDataLen);
    updateTrainNode(tNodes, tData, tDataLen);
    SDL_free(tData);

    uint64_t previousTick = 0;
    uint64_t requestRefreshTime = 0;
    printf("into the loop\n");
    while (status)
    {
        uint64_t currentTick = SDL_GetTicks();
        float delta = (float)(currentTick - previousTick) / 1000.0f;
        requestRefreshTime += currentTick - previousTick;
        previousTick = currentTick;

        /* if ((requestRefreshTime % 30000) > 0)
        {
            requestRefreshTime -= 30000;
            tData = requestTrains(&tDataLen);
            updateTrainNode(tNodes, tData, tDataLen);
            SDL_free(tData);
        } */

        // updateTrainPosition(tNodes, delta);

        status = t_rend_event() && t_rend_drawPixels(&imgData, renderer, tNodes, tNodeOffset + 1);
    }

    t_data_quit();
    t_nodes_quit(tNodes);
    t_rend_quit(window, renderer, &imgData);
    return 0;
}