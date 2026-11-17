#include "t_renderer.h"

#include <math.h>

#define APP_END 0
#define APP_CONTINUE 1

#define BYTE_SIZE 8

#if NORTHBOUND == -1

#define NORTHBOUND_Y 15
#define SOUTHBOUND_Y 17

#else

#define NORTHBOUND_Y 15
#define SOUTHBOUND_Y 13

#endif

#define ARENA_MAX 50
#define NODE_PIXELS_MAX 6

char sfps[10];

bool extractPixelDataFromFile(imageData* imgData) {
  FILE* image =
      fopen("./media/background_128x32_single.bmp", "rb");
  uint8_t header[14];
  uint8_t DIBHeader[40];

  assert(image);
  assert(fread(header, 1, 14, image) == 14);
  assert(header[0] == 'B' && header[1] == 'M');
  assert(fread(DIBHeader, 1, 40, image) == 40);

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

  assert(bitsPerPixel == 24);

  /**
   * The row length is repesented in bytes - it must be a mutliple of 4 bytes
   * This utilises the floor function - you add on the highest fraction below 1
   * that is a multiple of 1/32 (i.e 31/32) to the row length (measured in 4
   * bytes). The floor function thus ensures the result is a multiple of 4 bytes
   * above the remainder. The final multiplication of 4 converts it back to
   * bytes.
   */
  uint32_t pixelRowLength = (((bitsPerPixel * imgData->width) + 31) / 32) * 4;
  uint32_t pixelDataSize = pixelRowLength * imgData->height;

  /**
   * The padding is how many bytes need to be added on at the end of each row
   * as a row must contain a full pixel dataset (3 bytes in this case)
   *  */
  uint8_t padding = pixelRowLength % (bitsPerPixel / BYTE_SIZE);

  printf("Row size in bytes: %u | pixel data size: %u\n", pixelRowLength,
         pixelDataSize);

  uint8_t* rawPixelData = (uint8_t*)malloc(pixelDataSize);
  assert(rawPixelData);
  assert(fread(rawPixelData, 1, pixelDataSize, image));

  imgData->pixelData =
      (pixelData**)malloc(imgData->height * sizeof(pixelData*));

  assert(imgData->pixelData);

  for (size_t i = 0; i < imgData->height; i++) {
    *(imgData->pixelData + i) =
        (pixelData*)malloc(imgData->width * sizeof(pixelData));
    assert(*(imgData->pixelData + i));
  }

  // original pixel assignment.
  for (size_t i = 0, col = 0, row = 1; i < pixelDataSize; i += 3) {
    if ((row * pixelRowLength) - i < 3 && pixelRowLength > 0) {
      i += padding;
      row++;
      col = 0;
    }

    pixelData pData =
        *(*(imgData->pixelData + (imgData->height - row)) + col++) =
            *(pixelData*)(rawPixelData + i);
    // printf("r: %u, g: %u, b: %u\n", pData.r, pData.g, pData.b);
  }

  // flipped for LED matrix
  /* for (size_t i = 0, col = imgData->width - 1, row = 0; i < pixelDataSize;
       i += 3) {
    if (((row + 1) * pixelRowLength) - i < 3 && pixelRowLength > 0) {
      i += padding;
      row++;
      col = imgData->width - 1;
    }

    *(*(imgData->pixelData + row) + col--) = *(pixelData*)(rawPixelData + i);
  } */

  free(rawPixelData);
  fclose(image);
  return APP_CONTINUE;
}

void t_rend_init(imageData* imgData, display* disp) {
  assert(extractPixelDataFromFile(imgData));
  assert(SDL_Init(SDL_INIT_VIDEO));
  assert(SDL_CreateWindowAndRenderer("VL Display", 10 * imgData->width,
                                     10 * imgData->height, SDL_WINDOW_RESIZABLE,
                                     &disp->window, &disp->renderer));

  assert(SDL_SetRenderLogicalPresentation(disp->renderer, imgData->width,
                                          imgData->height,
                                          SDL_LOGICAL_PRESENTATION_LETTERBOX));
}

bool t_rend_event()
{
    // printf("event\n");
    SDL_Event event;
    while (SDL_PollEvent(&event))
    {
        if (event.type == SDL_EVENT_KEY_DOWN && event.key.scancode == SDL_SCANCODE_ESCAPE)
            return APP_END;

        if (event.type == SDL_EVENT_QUIT)
            return APP_END;
    }

    return APP_CONTINUE;
}

void t_rend_quit(imageData* imgData, display* disp) {
  for (size_t i = 0; i < imgData->height; i++) {
    free((imgData->pixelData + i));
  }

  free(imgData->pixelData);
  SDL_DestroyRenderer(disp->renderer);
  SDL_DestroyWindow(disp->window);
  SDL_QuitSubSystem(SDL_INIT_VIDEO);
  SDL_Quit();
}

bool t_rend_drawPixels(imageData* imgData, SDL_Renderer* renderer, t_data tdata,
                       float delta) {
  assert(imgData);

  SDL_RenderClear(renderer);

  // printf("drawing background\n");
  for (size_t y = 0; y < imgData->height; y++)
    for (size_t x = 0; x < imgData->width; x++) {
      pixelData pdata = *(*(imgData->pixelData + y) + x);

      SDL_FRect rect = {x, y, 1.0f, 1.0f};
      SDL_SetRenderDrawColor(renderer, pdata.r, pdata.g, pdata.b, SDL_ALPHA_OPAQUE);
      SDL_RenderRect(renderer, &rect);
    }

  if (tdata.len > 0) {
    for (size_t i = 0; i < tdata.len; i++) {
      train_object train = tdata.data[i];

      /**
       * ensure the x value is within the confines of the display
       * and that it is not nan.
       */
      /*       printf("train x = %f\n\n", train.x);
            printf(
                "id: %u\n station name: %s\n direction: %d\n time to station:
         %u\n" " next station: %s\n alter station: %u\n status: %u\n" " data
         collected: %s\n data misses: %u\n" " x pos: %f, speed %f delta time: %f
         delta x: %f\n\n", train.id, getstationliteral(train.stationName),
         train.direction, train.timeToStation,
         getstationliteral(train.nextStation), train.alterStation, train.status,
         train.dataCollected ? "yes" : "no", train.dataMisses, train.x,
         train.speed, delta, train.speed * delta); */
      assert(!isnan(train.x));
      assert(train.x >= 0.0f && train.x <= 128.0f);

      uint64_t xInt = (uint64_t)train.x;
      int y = train.direction == NORTHBOUND ? NORTHBOUND_Y : SOUTHBOUND_Y;

      float fract = train.x - (float)xInt;
      /**
       * Invert the x value and fractional for Northbound trains.
       * The x coords decrease for Northbound trains.
       */
      if (fract > 0.0f && train.direction == -1) {
        xInt++;
        fract = 1.0f - fract;
      }

      SDL_FRect top = {(float)xInt, (float)y, 1.0f, 1.0f};
      SDL_FRect bottom = {(float)xInt, (float)y + 1.0f, 1.0f, 1.0f};

      SDL_SetRenderDrawColor(renderer, 255, 0, 0, SDL_ALPHA_OPAQUE);
      SDL_RenderRect(renderer, &top);
      SDL_RenderRect(renderer, &bottom);
    }
  }

  // ===============================================
  // Drawing the fps
  /*   size_t i = 0;
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
    SDL_SetRenderScale(renderer, 1.0f, 1.0f); */

  //===================================================

  SDL_RenderPresent(renderer);
  return APP_CONTINUE;
}