#include "t_renderer.h"

#define REQUEST_TIME 30000

int main()
{
    TrainData *tData = NULL;
    uint8_t tDataLen = 0;

    trainNode *tNodes = NULL;
    uint8_t tNodeLen = 0;

    imageData imgData;
    SDL_Window *window = NULL;
    SDL_Renderer *renderer = NULL;

    pixelDataExtended **tBuffer = NULL;

    t_data_init();
    bool status = t_nodes_init(&tNodes) && t_rend_init(&window, &renderer, &imgData, &tBuffer);

    // Collect initial data
    tData = requestTrains(&tDataLen);
    updateTrainNode(tNodes, tData, &tNodeLen, tDataLen);

    if (tData)
        free(tData);

    for (size_t i = 0; i < tNodeLen; i++)
    {
        printf("Train id: %d | ", (tNodes + i)->id);
        printf("direction: %d |", (tNodes + i)->dir);
        printf("station: %d ", (tNodes + i)->nextStop);
        printf("| x coord: %f ", (tNodes + i)->x);
        printf("| speed: %f\n", (tNodes + i)->speed);
    }

    uint64_t previousTick = 0;
    uint64_t requestRefreshTime = 0;
    while (status)
    {
        uint64_t currentTick = SDL_GetTicks();
        double delta = (double)(currentTick - previousTick) / 1000.0f;
        requestRefreshTime += currentTick - previousTick;
        previousTick = currentTick;

        if (requestRefreshTime >= REQUEST_TIME)
        {
            requestRefreshTime -= REQUEST_TIME;
            tData = requestTrains(&tDataLen);
            updateTrainNode(tNodes, tData, &tNodeLen, tDataLen);

            if (tData)
                free(tData);

            for (size_t i = 0; i < tNodeLen; i++)
            {
                printf("Train id: %d | ", (tNodes + i)->id);
                printf("direction: %d |", (tNodes + i)->dir);
                printf("station: %d ", (tNodes + i)->nextStop);
                printf("| x coord: %f ", (tNodes + i)->x);
                printf("| speed: %f\n", (tNodes + i)->speed);
            }
        }

        updateTrainPosition(tNodes, tNodeLen, delta);

        // printf("tnodeoffset before loop: %d\n", tNodeOffset);
        status = t_rend_event() && t_rend_drawPixels(&imgData, renderer, tNodes, tNodeLen, tBuffer);
        // printf("finished a cycle\n");
        // status = false;
    }

    t_data_quit();
    t_nodes_quit(tNodes);
    t_rend_quit(window, renderer, &imgData);
    return 0;
}