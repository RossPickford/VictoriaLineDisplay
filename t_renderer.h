#include "t_displayNodes.h"

typedef struct pixelData
{
    uint8_t b;
    uint8_t g;
    uint8_t r;
} pixelData;

typedef struct imageData
{
    pixelData **pixelData;
    int32_t width;
    int32_t height;
} imageData;

typedef struct pixelDataExtended
{
    uint8_t x;
    uint8_t y;
    pixelData pxlData;
} pixelDataExtended;

bool t_renderer_init(SDL_Window *window, SDL_Renderer *renderer, imageData *imgData);
bool t_renderer_event();
bool t_rend_drawPixels(imageData *imgData, SDL_Renderer *renderer, trainNode *tNodes, uint8_t tNodeSize);
void t_rend_quit(SDL_Window *window, SDL_Renderer *renderer, imageData *imgData);

