#include <stdio.h>
#include <stdint.h>

#include "led-matrix-c.h"

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

#define APP_END false
#define APP_CONTINUE true

#define NORTHBOUND_Y 15
#define SOUTHBOUND_Y 17

#define ARENA_MAX 50
#define NODE_PIXELS_MAX 6

bool extractPixelDataFromFile(imageData *imgData)
{
    FILE *image = fopen("./media/background_128x32.bmp", "rb");
    uint8_t header[14];
    uint8_t DIBHeader[40];

    if (image == NULL)
    {
        printf("Unable to load image file");
        return APP_END;
    }

    if (fread(header, 1, 14, image) != 14)
    {
        fprintf(stderr, "ERROR: Cannot read file header\n");
        fclose(image);
        return APP_END;
    }

    if (header[0] != 'B' && header[1] != 'M')
    {
        printf("\nthis is not a bmp file");
        fclose(image);
        return APP_END;
    }

    if (fread(DIBHeader, 1, 40, image) != 40)
    {
        fprintf(stderr, "ERROR: Cannot read file DIB header\n");
        fclose(image);
        return APP_END;
    }

    uint32_t headerSize = *(uint32_t *)&DIBHeader[0];
    imgData->width = *(int32_t *)&DIBHeader[4];
    imgData->height = *(int32_t *)&DIBHeader[8];
    uint32_t pixelOffset = *(uint32_t *)&header[10];
    uint16_t bitsPerPixel = *(uint16_t *)&DIBHeader[14];

    printf("Header size in bytes: %u\n", headerSize);
    printf("Pixel Data offset: %u\n", pixelOffset);
    printf("width: %d, height: %d\n", imgData->width, imgData->height);
    printf("Bits per pixel: %u\n", bitsPerPixel);

    if (bitsPerPixel != 24)
    {
        printf("BPP is not 24, please re-format");
        return APP_END;
    }

    // Has to be a multiple of 4 bytes, hence padding
    uint32_t pixelRowLength = (((bitsPerPixel * imgData->width) + 31) / 32) * 4;
    uint32_t pixelDataSize = pixelRowLength * imgData->height;

    uint8_t padding = pixelRowLength % (bitsPerPixel * 8);

    printf("Row size in bytes: %u | pixel data size: %u\n", pixelRowLength, pixelDataSize);

    uint8_t *rawPixelData = (uint8_t *)malloc(pixelDataSize);
    if (!rawPixelData)
    {
        printf("failed to allocate memory for pixel data\n");
        return APP_END;
    }

    if (!fread(rawPixelData, 1, pixelDataSize, image))
    {
        fprintf(stderr, "ERROR: cannot read file pixel data\n");
        return APP_END;
    }

    imgData->pixelData = (pixelData **)malloc(imgData->height * sizeof(pixelData *));

    for (size_t i = 0; i < imgData->height; i++)
        *(imgData->pixelData + i) = (pixelData *)malloc(imgData->width * sizeof(pixelData));

    for (size_t i = 0, c = 0, r = 1; i < pixelDataSize; i += 3)
    {
        if ((r * pixelRowLength) - i < 3 && pixelRowLength > 0)
        {
            i += padding;
            r++;
            c = 0;
        }

        pixelData pData = *(*(imgData->pixelData + (imgData->height - r)) + c++) = *(pixelData *)(rawPixelData + i);
        // printf("r: %u, g: %u, b: %u\n", pData.r, pData.g, pData.b);
    }

    free(rawPixelData);
    fclose(image);
    return APP_CONTINUE;
}

int main(int argc, char **argv)
{
    imageData iData;
    extractPixelDataFromFile(&iData);

    struct RGBLedMatrixOptions options;
    struct RGBLedMatrix *matrix;
    struct LedCanvas *offscreen_canvas;
    int width, height;
    int x, y, i;

    memset(&options, 0, sizeof(options));
    options.rows = 32;
    options.cols = 128;
    options.chain_length = 2;

    /* This supports all the led commandline options. Try --led-help */
    matrix = led_matrix_create_from_options(&options, &argc, &argv);
    if (matrix == NULL)
        return 1;

    /* Let's do an example with double-buffering. We create one extra
     * buffer onto which we draw, which is then swapped on each refresh.
     * This is typically a good approach for animations and such.
     */
    offscreen_canvas = led_matrix_create_offscreen_canvas(matrix);

    led_canvas_get_size(offscreen_canvas, &width, &height);

    fprintf(stderr, "Size: %dx%d. Hardware gpio mapping: %s\n",
            width, height, options.hardware_mapping);

    for (i = 0; i < 1000; ++i)
    {
        for (y = 0; y < height; ++y)
        {
            for (x = 0; x < width; ++x)
            {
                pixelData pixel = *(*(iData.pixelData + y) + x);
                led_canvas_set_pixel(offscreen_canvas, x, y, pixel.r, pixel.g, pixel.b);
            }
        }

        /* Now, we swap the canvas. We give swap_on_vsync the buffer we
         * just have drawn into, and wait until the next vsync happens.
         * we get back the unused buffer to which we'll draw in the next
         * iteration.
         */
        offscreen_canvas = led_matrix_swap_on_vsync(matrix, offscreen_canvas);
    }

    /*
     * Make sure to always call led_matrix_delete() in the end to reset the
     * display. Installing signal handlers for defined exit is a good idea.
     */
    led_matrix_delete(matrix);

    for (size_t i = 0; i < iData.height; i++)
        free((iData.pixelData + i));

    free(iData.pixelData);

    return 0;
}