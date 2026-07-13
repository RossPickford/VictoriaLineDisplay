#include <stdio.h>
#include <stdint.h>
#include <math.h>
#include <SDL3/SDL.h>

#define APP_END false
#define APP_CONTINUE true

#define NORTHBOUND -1
#define SOUTHBOUND 1

#define WALTHAMSTOW_BOUND 4
#define BRIXTON_BOUND 123

#define NORTHBOUND_Y 15
#define SOUTHBOUND_Y 17

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

typedef struct trainNode
{
    int8_t dir;
    uint16_t id;
    float x;
    float speed;
} trainNode;

typedef struct pixelDataExtended
{
    uint8_t x;
    uint8_t y;
    pixelData pxlData;
} pixelDataExtended;

SDL_Window *window = NULL;
SDL_Renderer *renderer = NULL;

uint32_t width = 0, height = 0;

trainNode *trains = NULL;

bool extractPixelDataFromFile(imageData *imgData)
{
    FILE *image = fopen("./media/background.bmp", "rb");
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
    imgData->width = width = *(int32_t *)&DIBHeader[4];
    imgData->height = height = *(int32_t *)&DIBHeader[8];
    uint32_t pixelOffset = *(uint32_t *)&header[10];
    uint16_t bitsPerPixel = *(uint16_t *)&DIBHeader[14];

    printf("Header size in bytes: %u\n", headerSize);
    printf("Pixel Data offset: %u\n", pixelOffset);
    printf("width: %d, height: %d\n", width, height);
    printf("Bits per pixel: %u\n", bitsPerPixel);

    if (bitsPerPixel != 24)
    {
        printf("BPP is not 24, please re-format");
        return APP_END;
    }

    // Has to be a multiple of 4 bytes, hence padding
    uint32_t pixelRowLength = (((bitsPerPixel * width) + 31) / 32) * 4;
    uint32_t pixelDataSize = pixelRowLength * height;

    uint8_t padding = pixelRowLength % (bitsPerPixel * 8);

    printf("Row size in bytes: %u | pixel data size: %u\n", pixelRowLength, pixelDataSize);

    uint8_t rawPixelData[pixelDataSize];

    if (!fread(rawPixelData, 1, pixelDataSize, image))
    {
        fprintf(stderr, "ERROR: cannot read file pixel data\n");
        return APP_END;
    }

    imgData->pixelData = (pixelData **)SDL_malloc(height * sizeof(pixelData *));

    for (size_t i = 0; i < height; i++)
        *(imgData->pixelData + i) = (pixelData *)SDL_malloc(width * sizeof(pixelData));

    for (size_t i = 0, c = 0, r = 1; i < pixelDataSize; i += 3)
    {
        if ((r * pixelRowLength) - i < 3 && pixelRowLength > 0)
        {
            i += padding;
            r++;
            c = 0;
        }

        *(*(imgData->pixelData + (height - r)) + c++) = *(pixelData *)(rawPixelData + i);
        // printf("r: %u, g: %u, b: %u\n", pData.r, pData.g, pData.b);
    }

    fclose(image);
    return APP_CONTINUE;
}

bool AppInit()
{
    if (!SDL_Init(SDL_INIT_VIDEO))
    {
        SDL_Log("Failed to initialise video: %s", SDL_GetError());
        return APP_END;
    }

    if (!SDL_CreateWindowAndRenderer("VL Display", width, height, SDL_WINDOW_FULLSCREEN, &window, &renderer))
    {
        SDL_Log("Failed to create window and renderer: %s", SDL_GetError());
        return APP_END;
    }

    SDL_SetRenderLogicalPresentation(renderer, width, height, SDL_LOGICAL_PRESENTATION_LETTERBOX);

    return APP_CONTINUE;
}

bool AppEvent()
{
    SDL_Event event;
    while (SDL_PollEvent(&event))
    {
        if (event.type == SDL_EVENT_KEY_DOWN && event.key.scancode == SDL_SCANCODE_ESCAPE)
            return APP_END;
    }

    return APP_CONTINUE;
}

uint8_t ulerp(uint8_t minVal, uint8_t maxVal, float t)
{
    uint8_t diff = maxVal - minVal;
    float change = (float)diff * t;
    return minVal + (uint8_t)change;
}

uint8_t uslerp(uint8_t minVal, uint8_t maxVal, float t)
{
    // float newT = sqrtf((2 * t) - (t * t));
    float newT = 1.0f - sqrtf(1.0f - (t * t));
    uint8_t diff = maxVal - minVal;
    float change = (float)diff * newT;
    return minVal + (uint8_t)change;
}

void updatePixel(pixelData *pxl, uint8_t r, uint8_t g, uint8_t b)
{
    pxl->r = r;
    pxl->g = g;
    pxl->b = b;
}

pixelDataExtended drawTrainBuffer[10][6];
uint8_t trainBufferOffset = 0;
bool drawTrainNode(pixelData **pxlMtrx, float x, int8_t dir)
{
    if (x > (float)UINT64_MAX || x < 0.0f)
    {
        fprintf(stderr, "X float coordinate beyond uint64_t scope");
        return false;
    }

    uint64_t xInt = (uint64_t)x;
    uint64_t y = dir == NORTHBOUND ? NORTHBOUND_Y : SOUTHBOUND_Y;

    float fract = x - (float)xInt;
    if (fract > 0.0f && dir == NORTHBOUND)
    {
        xInt++;
        fract = 1.0f - fract;
    }

    // front pixels
    drawTrainBuffer[trainBufferOffset][0].y = y;
    drawTrainBuffer[trainBufferOffset][0].x = xInt;
    updatePixel(&drawTrainBuffer[trainBufferOffset][0].pxlData, UINT8_MAX, 0, 0);

    drawTrainBuffer[trainBufferOffset][1].y = y + 1;
    drawTrainBuffer[trainBufferOffset][1].x = xInt;
    updatePixel(&drawTrainBuffer[trainBufferOffset][1].pxlData, UINT8_MAX, 0, 0);

    pixelData backPix = *(*(pxlMtrx + y) + xInt - dir);

    uint8_t newR = uslerp(backPix.r, UINT8_MAX, (1.0f - fract));
    uint8_t newG = uslerp(0, backPix.g, fract);
    uint8_t newB = uslerp(0, backPix.b, fract);

    // back pixels
    drawTrainBuffer[trainBufferOffset][2].y = y;
    drawTrainBuffer[trainBufferOffset][2].x = xInt - dir;
    updatePixel(&drawTrainBuffer[trainBufferOffset][2].pxlData, newR, newG, newB);

    drawTrainBuffer[trainBufferOffset][3].y = y + 1;
    drawTrainBuffer[trainBufferOffset][3].x = xInt - dir;
    updatePixel(&drawTrainBuffer[trainBufferOffset][3].pxlData, newR, newG, newB);

    pixelData forwardPix = *(*(pxlMtrx + y) + xInt + dir);

    newR = uslerp(forwardPix.r, UINT8_MAX, fract);
    newG = uslerp(0, forwardPix.g, (1.0f - fract));
    newB = uslerp(0, forwardPix.b, (1.0f - fract));

    drawTrainBuffer[trainBufferOffset][4].y = y;
    drawTrainBuffer[trainBufferOffset][4].x = xInt + dir;
    updatePixel(&drawTrainBuffer[trainBufferOffset][4].pxlData, newR, newG, newB);

    drawTrainBuffer[trainBufferOffset][5].y = y + 1;
    drawTrainBuffer[trainBufferOffset][5].x = xInt + dir;
    updatePixel(&drawTrainBuffer[trainBufferOffset][5].pxlData, newR, newG, newB);

    trainBufferOffset++;
}

uint64_t previousTick = 0;
bool AppIterate(imageData *imgData)
{
    SDL_SetRenderDrawColor(renderer, 0, 255, 0, SDL_ALPHA_OPAQUE);
    SDL_RenderClear(renderer);

    for (size_t y = 0; y < imgData->height; y++)
        for (size_t x = 0; x < imgData->width; x++)
        {
            pixelData pData = *(*(imgData->pixelData + y) + x);
            // printf("r: %u, g: %u, b: %u\n", pData.r, pData.g, pData.b);

            SDL_FRect pixel = {x, y, 1.0f, 1.0f};
            SDL_SetRenderDrawColor(renderer, pData.r, pData.g, pData.b, SDL_ALPHA_OPAQUE);
            SDL_RenderFillRect(renderer, &pixel);
        }

    uint64_t currentTick = SDL_GetTicks();
    float time = (float)(currentTick - previousTick) / 1000.0f;
    previousTick = currentTick;
    for (size_t i = 0; i < 2; i++)
    {
        drawTrainNode(imgData->pixelData, (trains + i)->x, (trains + i)->dir);
        float delta = time * (trains + i)->speed * (trains + i)->dir;
        (trains + i)->x += delta;
    }

    for (uint8_t i = 0; i < trainBufferOffset; i++)
        for (uint8_t j = 0; j < 6; j++)
        {
            SDL_FRect pxl = {(float)drawTrainBuffer[i][j].x, (float)drawTrainBuffer[i][j].y, 1.0f, 1.0f};
            pixelData trainPxl = drawTrainBuffer[i][j].pxlData;

            SDL_SetRenderDrawColor(renderer, trainPxl.r, trainPxl.g, trainPxl.b, SDL_ALPHA_OPAQUE);
            SDL_RenderRect(renderer, &pxl);
        }

    trainBufferOffset = 0;
    SDL_RenderPresent(renderer);

    return APP_CONTINUE;
}

int main(int argc, char *argv[])
{
    imageData imgData;

    trains = SDL_malloc(sizeof(trainNode) * 2);

    trains->dir = SOUTHBOUND;
    trains->x = (float)WALTHAMSTOW_BOUND;
    trains->id = 202;
    trains->speed = 3.0f;
    (trains + 1)->dir = NORTHBOUND;
    (trains + 1)->x = (float)BRIXTON_BOUND;
    (trains + 1)->id = 200;
    (trains + 1)->speed = 0.5f;

    if (!extractPixelDataFromFile(&imgData))
        return -1;

    bool status = AppInit();

    // While status is true it will loop
    while (status)
        status = AppEvent() && AppIterate(&imgData);

    for (size_t i = 0; i < imgData.height; i++)
        SDL_free(*(imgData.pixelData + i));

    SDL_free(imgData.pixelData);
    SDL_free(trains);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_QuitSubSystem(SDL_INIT_VIDEO);
    SDL_Quit();

    return 0;
}