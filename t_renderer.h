#include "led-matrix-c.h"
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
    struct RGBLedMatrix *matrix;
    struct LedCanvas *canvas;
    int width;
    int height;
} display;

bool t_rend_init(imageData* imgData);
bool t_rend_drawPixels(imageData* imgData, display* disp, t_data tdata,
                       float delta);
void t_rend_quit(imageData* imgData);
