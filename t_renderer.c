#include "t_renderer.h"

#include <math.h>

#define APP_END 0
#define APP_CONTINUE 1

#define BYTE_SIZE 8

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

  // original pixel assignment.
  /* for (size_t i = 0, col = 0, row = 1; i < pixelDataSize; i += 3) {
    if ((row * pixelRowLength) - i < 3 && pixelRowLength > 0) {
      i += padding;
      row++;
      col = 0;
    }

    pixelData pData = *(*(imgData->pixelData + (imgData->height - row)) + col++)
  =
        *(pixelData*)(rawPixelData + i);
    // printf("r: %u, g: %u, b: %u\n", pData.r, pData.g, pData.b);
  } */

  for (size_t i = 0, col = imgData->width - 1, row = 0; i < pixelDataSize;
       i += 3) {
    if (((row + 1) * pixelRowLength) - i < 3 && pixelRowLength > 0) {
      i += padding;
      row++;
      col = imgData->width - 1;
    }

    pixelData pData = *(*(imgData->pixelData + row) + col--) =
        *(pixelData*)(rawPixelData + i);
    // printf("r: %u, g: %u, b: %u\n", pData.r, pData.g, pData.b);
  }

  free(rawPixelData);
  fclose(image);
  return APP_CONTINUE;
}

bool t_rend_init(imageData* imgData) {
  if (!extractPixelDataFromFile(imgData)) {
    printf("couldn't load bitmap image\n");
    return APP_END;
  }

  return APP_CONTINUE;
}

void t_rend_quit(imageData* imgData) {
  for (size_t i = 0; i < imgData->height; i++) free((imgData->pixelData + i));
  free(imgData->pixelData);
}

bool t_rend_drawPixels(imageData* imgData, display* disp, t_data tdata,
                       float delta) {
  // printf("rendering\n");

  if (!imgData) {
    printf("A pointer variables inserted is/are null\n");
    return APP_END;
  }

  // printf("drawing background\n");
  for (size_t y = 0; y < imgData->height; y++)
    for (size_t x = 0; x < imgData->width; x++) {
      pixelData pdata = *(*(imgData->pixelData + y) + x);

      led_canvas_set_pixel(disp->canvas, x, y, pdata.r, pdata.g, pdata.b);
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
      int y = train.direction == NORTHBOUND ? NORTHBOUND_Y : SOUTHBOUND_Y;

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
      led_canvas_set_pixel(disp->canvas, xInt, y, 255, 0, 0);
      led_canvas_set_pixel(disp->canvas, xInt, y + 1, 255, 0, 0);
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

  // printf("presenting pixels\n");
  disp->canvas = led_matrix_swap_on_vsync(disp->matrix, disp->canvas);

  return APP_CONTINUE;
}