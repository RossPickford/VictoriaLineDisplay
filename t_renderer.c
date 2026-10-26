#include "t_renderer.h"

#define APP_END false
#define APP_CONTINUE true

#define NORTHBOUND_Y 15
#define SOUTHBOUND_Y 17

#define ARENA_MAX 50
#define NODE_PIXELS_MAX 6

char sfps[10];

bool extractPixelDataFromFile(imageData* imgData) {
  FILE* image = fopen("./media/background_128x32_single.bmp", "rb");
  uint8_t header[14];
  uint8_t DIBHeader[40];

  if (image == NULL) {
    printf("Unable to load image file");
    return APP_END;
  }

  if (fread(header, 1, 14, image) != 14) {
    fprintf(stderr, "ERROR: Cannot read file header\n");
    fclose(image);
    return APP_END;
  }

  if (header[0] != 'B' && header[1] != 'M') {
    printf("\nthis is not a bmp file");
    fclose(image);
    return APP_END;
  }

  if (fread(DIBHeader, 1, 40, image) != 40) {
    fprintf(stderr, "ERROR: Cannot read file DIB header\n");
    fclose(image);
    return APP_END;
  }

  // all of these key address points were on the wiki for bmp files
  uint32_t headerSize = *(uint32_t*)&DIBHeader[0];
  imgData->width = *(int32_t*)&DIBHeader[4];
  imgData->height = *(int32_t*)&DIBHeader[8];
  uint32_t pixelOffset = *(uint32_t*)&header[10];
  uint16_t bitsPerPixel = *(uint16_t*)&DIBHeader[14];

  printf("Header size in bytes: %u\n", headerSize);
  printf("Pixel Data offset: %u\n", pixelOffset);
  printf("width: %d, height: %d\n", imgData->width, imgData->height);
  printf("Bits per pixel: %u\n", bitsPerPixel);

  if (bitsPerPixel != 24) {
    printf("BPP is not 24, please re-format");
    return APP_END;
  }

  // Has to be a multiple of 4 bytes, hence padding
  uint32_t pixelRowLength = (((bitsPerPixel * imgData->width) + 31) / 32) * 4;
  uint32_t pixelDataSize = pixelRowLength * imgData->height;

  uint8_t padding = pixelRowLength % (bitsPerPixel * 8);

  printf("Row size in bytes: %u | pixel data size: %u\n", pixelRowLength,
         pixelDataSize);

  uint8_t* rawPixelData = (uint8_t*)malloc(pixelDataSize);
  if (!rawPixelData) {
    printf("failed to allocate memory for pixel data\n");
    return APP_END;
  }

  if (!fread(rawPixelData, 1, pixelDataSize, image)) {
    fprintf(stderr, "ERROR: cannot read file pixel data\n");
    return APP_END;
  }

  imgData->pixelData =
      (pixelData**)malloc(imgData->height * sizeof(pixelData*));

  for (size_t i = 0; i < imgData->height; i++)
    *(imgData->pixelData + i) =
        (pixelData*)malloc(imgData->width * sizeof(pixelData));

  for (size_t i = 0, c = 0, r = 1; i < pixelDataSize; i += 3) {
    if ((r * pixelRowLength) - i < 3 && pixelRowLength > 0) {
      i += padding;
      r++;
      c = 0;
    }

    pixelData pData = *(*(imgData->pixelData + (imgData->height - r)) + c++) =
        *(pixelData*)(rawPixelData + i);
    // printf("r: %u, g: %u, b: %u\n", pData.r, pData.g, pData.b);
  }

  free(rawPixelData);
  fclose(image);
  return APP_CONTINUE;
}

bool t_rend_init(SDL_Window** window, SDL_Renderer** renderer,
                 imageData* imgData, pixelDataExtended*** tBuffer) {
  if (!SDL_Init(SDL_INIT_VIDEO)) {
    SDL_Log("Failed to initialise video: %s", SDL_GetError());
    return APP_END;
  }

  if (!extractPixelDataFromFile(imgData)) {
    printf("couldn't load bitmap image\n");
    return APP_END;
  }

  if (!SDL_CreateWindowAndRenderer("VL Display", 10 * imgData->width,
                                   10 * imgData->height, SDL_WINDOW_RESIZABLE,
                                   window, renderer)) {
    SDL_Log("Failed to create window and renderer: %s", SDL_GetError());
    return APP_END;
  }

  SDL_SetRenderLogicalPresentation(*renderer, imgData->width, imgData->height,
                                   SDL_LOGICAL_PRESENTATION_LETTERBOX);

  *tBuffer =
      (pixelDataExtended**)malloc(sizeof(pixelDataExtended*) * ARENA_MAX);
  assert(*tBuffer);

  for (size_t i = 0; i < ARENA_MAX; i++) {
    *(*tBuffer + i) =
        (pixelDataExtended*)malloc(sizeof(pixelDataExtended) * NODE_PIXELS_MAX);
    assert(*(*tBuffer + i));
  }

  return APP_CONTINUE;
}

bool t_rend_event() {
  // printf("event\n");
  SDL_Event event;
  while (SDL_PollEvent(&event)) {
    if (event.type == SDL_EVENT_KEY_DOWN &&
        event.key.scancode == SDL_SCANCODE_ESCAPE)
      return APP_END;

    if (event.type == SDL_EVENT_QUIT) return APP_END;
  }

  return APP_CONTINUE;
}

void t_rend_quit(SDL_Window* window, SDL_Renderer* renderer, imageData* imgData,
                 pixelDataExtended*** tBuffer) {
  for (size_t i = 0; i < imgData->height; i++) free((imgData->pixelData + i));

  // printf("freeing buffer\n");
  for (size_t i = 0; i < ARENA_MAX; i++) free(*(*tBuffer + i));

  free(*tBuffer);
  free(imgData->pixelData);
  SDL_DestroyRenderer(renderer);
  SDL_DestroyWindow(window);
  SDL_QuitSubSystem(SDL_INIT_VIDEO);
  SDL_Quit();
}

uint8_t ulerp(uint8_t minVal, uint8_t maxVal, float t) {
  assert(t <= 1.0f && t >= 0.0f);
  uint8_t diff = maxVal - minVal;
  float change = (float)diff * t;
  return minVal + (uint8_t)change;
}

uint8_t uinvslerp(uint8_t minVal, uint8_t maxVal, float t) {
  // float newT = sqrtf((2 * t) - (t * t));
  assert(t <= 1.0f && t >= 0.0f);
  float newT = 1.0f - sqrtf(1.0f - (t * t));
  uint8_t diff = maxVal - minVal;
  float change = (float)diff * newT;
  return minVal + (uint8_t)change;
}

bool t_rend_drawPixels(imageData* imgData, SDL_Renderer* renderer, t_data tdata,
                       pixelDataExtended** tBuff, float delta) {
  // printf("rendering\n");

  if (!imgData | !renderer) {
    printf("A pointer variables inserted is/are null\n");
    return APP_END;
  }

  SDL_SetRenderDrawColor(renderer, 0, 0, 0, SDL_ALPHA_OPAQUE);
  SDL_RenderClear(renderer);

  // printf("drawing background\n");
  for (size_t y = 0; y < imgData->height; y++)
    for (size_t x = 0; x < imgData->width; x++) {
      pixelData pData = *(*(imgData->pixelData + y) + x);
      // printf("r: %u, g: %u, b: %u\n", pData.r, pData.g, pData.b);

      SDL_FRect pixel = {x, y, 1.0f, 1.0f};
      SDL_SetRenderDrawColor(renderer, pData.r, pData.g, pData.b,
                             SDL_ALPHA_OPAQUE);
      SDL_RenderFillRect(renderer, &pixel);
    }

  if (tdata.len > 0) {
    for (size_t i = 0; i < tdata.len; i++) {
      train_object train = tdata.data[i];

      /**
       * ensure the x value is within the confines of the display
       * and that it is not nan.
       */
      assert(train.x >= 0.0f && train.x <= 128.0f);
      assert(!isnan(train.x));

      uint64_t xInt = (uint64_t)train.x;
      float y = train.direction == NORTHBOUND ? NORTHBOUND_Y : SOUTHBOUND_Y;

      float fract = train.x - (float)xInt;
      /**
       * Invert the x value and fractional for Northbound trains.
       * The x coords decrease for Northbound trains.
       */
      if (fract > 0.0f && train.direction == NORTHBOUND) {
        xInt++;
        fract = 1.0f - fract;
      }

      // Draw the pixel to screen
      SDL_FRect pxltop = {(float)xInt, y, 1.0f, 1.0f};
      SDL_FRect pxlbtm = {(float)xInt, y + 1, 1.0f, 1.0f};

      SDL_SetRenderDrawColor(renderer, 255, 0, 0, SDL_ALPHA_OPAQUE);
      SDL_RenderRect(renderer, &pxltop);
      SDL_RenderRect(renderer, &pxlbtm);
    }
  }

  size_t i = 0;
  for (uint32_t fps = (uint32_t)(1.0f / delta); fps != 0 && i < sizeof(sfps);
       fps /= 10) {
    sfps[i++] = (fps % 10) + '0';
  }

  for (size_t j = 0; j < i; j++) {
    char hold = sfps[j];
    sfps[j] = sfps[i-(j+1)];
    sfps[i-(j+1)] = hold;
  }
  sfps[i] = '\0';

  SDL_SetRenderDrawColor(renderer, 0, 255, 0, SDL_ALPHA_OPAQUE);
  SDL_SetRenderScale(renderer, 0.5f, 0.5f);
  assert(SDL_RenderDebugText(renderer, 210, 56, sfps));
  SDL_SetRenderScale(renderer, 1.0f, 1.0f);

  // printf("presenting pixels\n");
  SDL_RenderPresent(renderer);

  return APP_CONTINUE;
}