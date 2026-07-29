#include "t_renderer.h"

extern int16_t tNodeOffset;


int main()
{
    TrainData *tData = NULL;
    uint8_t tDataLen = 0;
    
    trainNode *tNodes = NULL;
    
    imageData imgData;
    SDL_Window *window = NULL;
    SDL_Renderer *renderer = NULL;

    t_data_init();
    bool status = t_nodes_init(&tNodes) && t_rend_init(&window, &renderer, &imgData);

    // Collect initial data
    tData = requestTrains(&tDataLen);
    updateTrainNode(tNodes, tData, tDataLen);
    SDL_free(tData);

    for (size_t i = 0; i < tNodeOffset; i++)
    {
        printf("Train id: %d | ", (tNodes + i)->id);
        printf("direction: %s |", (tNodes + i)->dir == -1 ? "Northbound" : "Southbound");
        printf("station: %d", (tNodes + i)->nextStop);
        printf("| x coord: %f\n", (tNodes + i)->x);
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

        if (requestRefreshTime >= 30000)
        {
            printf("requesting data: %lu\n", requestRefreshTime);
            requestRefreshTime -= 30000;
            tData = requestTrains(&tDataLen);
            updateTrainNode(tNodes, tData, tDataLen);
            SDL_free(tData);
        }

        updateTrainPosition(tNodes, delta);

        // printf("tnodeoffset before loop: %d\n", tNodeOffset);
        status = t_rend_event() && t_rend_drawPixels(&imgData, renderer, tNodes, tNodeOffset + 1);
        // status = false;
    }

    t_data_quit();
    t_nodes_quit(tNodes);
    t_rend_quit(window, renderer, &imgData);
    return 0;
}