#include "t_renderer.h"

#define REQUEST_TIME 30000

int main(void)
{
    TrainData *tData = NULL;
    uint8_t tDataLen = 0;

    trainNode *tNodes = NULL;
    uint8_t tNodeLen = 0;

    imageData imgData;
    SDL_Window *window = NULL;
    SDL_Renderer *renderer = NULL;

    pixelDataExtended **tBuffer = NULL;

    t_data_init(&tData);
    bool status = t_nodes_init(&tNodes) && t_rend_init(&window, &renderer, &imgData, &tBuffer);

    // Collect initial data
        requestTrains(tData, &tDataLen);
        updateTrainNode(tNodes, tData, &tNodeLen, tDataLen);

    uint64_t previousTick = 0;
    uint64_t requestRefreshTime = 0;
    while (status)
    {
        uint64_t currentTick = SDL_GetTicks();
        float delta = (float)(currentTick - previousTick) / 1000.0f;
        requestRefreshTime += currentTick - previousTick;
        previousTick = currentTick;

        if (requestRefreshTime >= REQUEST_TIME)
        {
            requestRefreshTime -= REQUEST_TIME;
            requestTrains(tData, &tDataLen);
            updateTrainNode(tNodes, tData, &tNodeLen, tDataLen);
        }

        updateTrainPosition(tNodes, tNodeLen, delta);

        status = t_rend_event() && t_rend_drawPixels(&imgData, renderer, tNodes, tNodeLen, tBuffer);
        // printf("finished a cycle\n");
        // status = false;
    }

    t_data_quit(&tData);
    t_nodes_quit(tNodes);
    t_rend_quit(window, renderer, &imgData, &tBuffer);
    return 0;
}