#include "t_renderer.h"

#define REQUEST_TIME 5000

int main()
{
    TrainData *tData = NULL;
    uint8_t tDataLen = 0;

    trainNode *tNodes = NULL;
    uint8_t tNodeLen = 0; // This represents the index, NOT the length

    imageData imgData;
    SDL_Window *window = NULL;
    SDL_Renderer *renderer = NULL;

    t_data_init();
    bool status = t_nodes_init(&tNodes) && t_rend_init(&window, &renderer, &imgData);

    // Collect initial data
    tData = requestTrains(&tDataLen);
    updateTrainNode(tNodes, tData, &tNodeLen, tDataLen);

    printf("T Data: %u\n", tDataLen);
    for (size_t i = 0; i < tDataLen; i++)
    {
        printf("id: %u\n", (tData + i)->id);
    }
    printf("T Nodes: %d\n", tNodeLen);
    for (size_t i = 0; i < tNodeLen; i++)
    {
        printf("id: %u\n", (tNodes + i)->id);
    }

    if (tData)
        free(tData);

    /* for (size_t i = 0; i < tNodeOffset; i++)
    {
        printf("Train id: %d | ", (tNodes + i)->id);
        printf("direction: %s |", (tNodes + i)->dir == -1 ? "Northbound" : "Southbound");
        printf("station: %d", (tNodes + i)->nextStop);
        printf("| x coord: %f\n", (tNodes + i)->x);
        printf("| speed: %f\n", (tNodes + i)->speed);
    } */

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
            printf("requesting data: %lu\n", requestRefreshTime);
            requestRefreshTime -= REQUEST_TIME;
            tData = requestTrains(&tDataLen);
            updateTrainNode(tNodes, tData, &tNodeLen, tDataLen);

            printf("T Data: %u\n", tDataLen);
            for (size_t i = 0; i < tDataLen; i++)
            {
                printf("id: %u\n", (tData + i)->id);
            }
            printf("T Nodes: %u\n", tNodeLen);
            for (size_t i = 0; i < tNodeLen; i++)
            {
                printf("id: %u\n", (tNodes + i)->id);
            }

            if (tData)
                free(tData);

            /*     for (size_t i = 0; i < tNodeOffset; i++)
                {
                    printf("Train id: %d | ", (tNodes + i)->id);
                    printf("direction: %s |", (tNodes + i)->dir == -1 ? "Northbound" : "Southbound");
                    printf("station: %d", (tNodes + i)->nextStop);
                    printf("| x coord: %f\n", (tNodes + i)->x);
                    printf("| speed: %f\n", (tNodes + i)->speed);
                } */
        }

        updateTrainPosition(tNodes, tNodeLen, delta);

        // printf("tnodeoffset before loop: %d\n", tNodeOffset);
        status = t_rend_event() && t_rend_drawPixels(&imgData, renderer, tNodes, tNodeLen);
        // status = false;
        printf("finished a cycle\n");
    }

    t_data_quit();
    t_nodes_quit(tNodes);
    t_rend_quit(window, renderer, &imgData);
    return 0;
}