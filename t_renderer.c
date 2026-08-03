#include "t_renderer.h"

#define APP_END false
#define APP_CONTINUE true

#define NORTHBOUND_Y 15
#define SOUTHBOUND_Y 17

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

    uint8_t *rawPixelData = (uint8_t *)SDL_malloc(pixelDataSize);
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

    imgData->pixelData = (pixelData **)SDL_malloc(imgData->height * sizeof(pixelData *));

    for (size_t i = 0; i < imgData->height; i++)
        *(imgData->pixelData + i) = (pixelData *)SDL_malloc(imgData->width * sizeof(pixelData));

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

    SDL_free(rawPixelData);
    fclose(image);
    return APP_CONTINUE;
}

bool t_rend_init(SDL_Window **window, SDL_Renderer **renderer, imageData *imgData)
{
    if (!SDL_Init(SDL_INIT_VIDEO))
    {
        SDL_Log("Failed to initialise video: %s", SDL_GetError());
        return APP_END;
    }

    if (!extractPixelDataFromFile(imgData))
    {
        printf("couldn't load bitmap image\n");
        return APP_END;
    }

    if (!SDL_CreateWindowAndRenderer("VL Display", imgData->width, imgData->height, SDL_WINDOW_FULLSCREEN, window, renderer))
    {
        SDL_Log("Failed to create window and renderer: %s", SDL_GetError());
        return APP_END;
    }

    SDL_SetRenderLogicalPresentation(*renderer, imgData->width, imgData->height, SDL_LOGICAL_PRESENTATION_LETTERBOX);

    return APP_CONTINUE;
}

bool t_rend_event()
{
    // printf("event\n");
    SDL_Event event;
    while (SDL_PollEvent(&event))
    {
        if (event.type == SDL_EVENT_KEY_DOWN && event.key.scancode == SDL_SCANCODE_ESCAPE)
            return APP_END;
    }

    return APP_CONTINUE;
}

void t_rend_quit(SDL_Window *window, SDL_Renderer *renderer, imageData *imgData)
{
    for (size_t i = 0; i < imgData->height; i++)
        SDL_free((imgData->pixelData + i));

    SDL_free(imgData->pixelData);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_QuitSubSystem(SDL_INIT_VIDEO);
    SDL_Quit();
}

uint8_t ulerp(uint8_t minVal, uint8_t maxVal, float t)
{
    uint8_t diff = maxVal - minVal;
    float change = (float)diff * t;
    return minVal + (uint8_t)change;
}

uint8_t uinvslerp(uint8_t minVal, uint8_t maxVal, float t)
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

void drawTrainNode(pixelData **pxlMtrx, float x, int8_t dir, pixelDataExtended **tBuff, uint8_t tBuffOffset)
{
    if (x > (float)UINT64_MAX || x < 0.0f)
    {
        // printf("t_renderer: X float coordinate beyond uint64_t scope - %f\n", x);
        return;
    }
    else if (x == NAN)
    {
        // printf("t_renderer: x float nat a number\n");
        return;
    }

    uint64_t xInt = (uint64_t)x;
    uint64_t y = dir == NORTHBOUND ? NORTHBOUND_Y : SOUTHBOUND_Y;

    float fract = x - (float)xInt;
    if (fract > 0.0f && dir == NORTHBOUND)
    {
        xInt++;
        fract = 1.0f - fract;
    }

    // Front pixels
    tBuff[tBuffOffset][0].y = y;
    tBuff[tBuffOffset][0].x = xInt;
    updatePixel(&tBuff[tBuffOffset][0].pxlData, UINT8_MAX, 0, 0);

    // (*(tBuff + tBuffOffset) + 0)->y = y;
    // (*(tBuff + tBuffOffset) + 0)->x = xInt;
    // updatePixel(&(*(tBuff + tBuffOffset) + 0)->pxlData, UINT8_MAX, 0, 0);

    tBuff[tBuffOffset][1].y = y + 1;
    tBuff[tBuffOffset][1].x = xInt;
    updatePixel(&tBuff[tBuffOffset][1].pxlData, UINT8_MAX, 0, 0);

    pixelData backPix = *(*(pxlMtrx + y) + xInt - dir);

    uint8_t newR = uinvslerp(backPix.r, UINT8_MAX, (1.0f - fract));
    uint8_t newG = uinvslerp(0, backPix.g, fract);
    uint8_t newB = uinvslerp(0, backPix.b, fract);

    // Back pixels
    tBuff[tBuffOffset][2].y = y;
    tBuff[tBuffOffset][2].x = xInt - dir;
    updatePixel(&tBuff[tBuffOffset][2].pxlData, newR, newG, newB);

    tBuff[tBuffOffset][3].y = y + 1;
    tBuff[tBuffOffset][3].x = xInt - dir;
    updatePixel(&tBuff[tBuffOffset][3].pxlData, newR, newG, newB);

    pixelData forwardPix = *(*(pxlMtrx + y) + xInt + dir);

    newR = uinvslerp(forwardPix.r, UINT8_MAX, fract);
    newG = uinvslerp(0, forwardPix.g, (1.0f - fract));
    newB = uinvslerp(0, forwardPix.b, (1.0f - fract));

    // Forward Pixels (future ones)
    tBuff[tBuffOffset][4].y = y;
    tBuff[tBuffOffset][4].x = xInt + dir;
    updatePixel(&tBuff[tBuffOffset][4].pxlData, newR, newG, newB);

    tBuff[tBuffOffset][5].y = y + 1;
    tBuff[tBuffOffset][5].x = xInt + dir;
    updatePixel(&tBuff[tBuffOffset][5].pxlData, newR, newG, newB);
}

bool t_rend_drawPixels(imageData *imgData, SDL_Renderer *renderer, trainNode *tNodes, int16_t tNodeLength)
{
    // printf("rendering\n");

    if (!imgData | !renderer)
    {
        printf("A pointer variables inserted is/are null\n");
        return APP_END;
    }

    SDL_SetRenderDrawColor(renderer, 0, 0, 0, SDL_ALPHA_OPAQUE);
    SDL_RenderClear(renderer);

    // printf("drawing background\n");
    for (size_t y = 0; y < imgData->height; y++)
        for (size_t x = 0; x < imgData->width; x++)
        {
            pixelData pData = *(*(imgData->pixelData + y) + x);
            // printf("r: %u, g: %u, b: %u\n", pData.r, pData.g, pData.b);

            SDL_FRect pixel = {x, y, 1.0f, 1.0f};
            SDL_SetRenderDrawColor(renderer, pData.r, pData.g, pData.b, SDL_ALPHA_OPAQUE);
            SDL_RenderFillRect(renderer, &pixel);
        }

    // uint64_t currentTick = SDL_GetTicks();
    // static uint64_t previousTick = 0;
    // double time = (double)(currentTick - previousTick) / 1000.0f;
    // previousTick = currentTick;

    // printf("creating and drawing train pixels\n");
    if (tNodeLength > 0 && tNodes)
    {
        // printf("t_renderer: drawing nodes - %u\n", tNodeLength);
        pixelDataExtended **tBuff = (pixelDataExtended **)SDL_malloc(sizeof(pixelDataExtended *) * tNodeLength);

        if (!tBuff)
        {
            printf("cannot allocate memory for train pixel buffer\n");
            return APP_END;
        }

        // printf("allocating memory for buffer\n");
        for (size_t i = 0; i < tNodeLength; i++)
        {
            *(tBuff + i) = (pixelDataExtended *)SDL_malloc(sizeof(pixelDataExtended) * 6);
            if (!*(tBuff + i))
            {
                printf("cannot allocate memory for train pixel buffer\n");
                return APP_END;
            }
        }

        uint8_t tBuffOffset = 0;

        // printf("filling train buffer\n");
        for (size_t i = 0; i < tNodeLength; i++) // Fill train buffer with node data
        {
            drawTrainNode(imgData->pixelData, (tNodes + i)->x, (tNodes + i)->dir, tBuff, i);
            // float delta = time * (tNodes + i)->speed * (tNodes + i)->dir;
            // (tNodes + i)->x += delta;

            // if ((tNodes + i)->dir == NORTHBOUND && (tNodes + i)->x < (tNodes + i)->nextStop)
            //     (tNodes + i)->x = (float)(tNodes + i)->nextStop;
            // else if ((tNodes + i)->dir == SOUTHBOUND && (tNodes + i)->x > (tNodes + i)->nextStop)
            //     (tNodes + i)->x = (float)(tNodes + i)->nextStop;
        }

        // printf("drawing buffer pixels\n");
        for (uint8_t i = 0; i < tNodeLength; i++)
            for (uint8_t j = 0; j < 6; j++)
            {
                SDL_FRect pxl = {(float)tBuff[i][j].x, (float)tBuff[i][j].y, 1.0f, 1.0f};
                pixelData trainPxl = tBuff[i][j].pxlData;

                SDL_SetRenderDrawColor(renderer, trainPxl.r, trainPxl.g, trainPxl.b, SDL_ALPHA_OPAQUE);
                SDL_RenderRect(renderer, &pxl);
            }

        // printf("freeing buffer\n");
        for (size_t i = 0; i < tNodeLength; i++)
            SDL_free(*(tBuff + i));

        SDL_free(tBuff);
    }

    // printf("presenting pixels\n");
    SDL_RenderPresent(renderer);

    return APP_CONTINUE;
}