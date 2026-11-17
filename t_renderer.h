#include <SDL3/SDL.h>
#include "t_dataCollection.h"

#pragma once

typedef struct pixelData {
  uint8_t b;
  uint8_t g;
  uint8_t r;
} pixelData;

typedef struct imageData {
  pixelData** pixelData;
  int32_t width;
  int32_t height;
} imageData;

typedef struct display {
  SDL_Window* window;
  SDL_Renderer* renderer;
  int width;
  int height;
} display;

void t_rend_init(imageData* imgData, display* disp);
bool t_rend_drawPixels(imageData* imgData, SDL_Renderer* renderer, t_data tdata,
                       float delta);
void t_rend_quit(imageData* imgData, display* disp);
