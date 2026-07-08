#include <stdio.h>
#include <stdint.h>

int main(void)
{
    FILE* image = fopen("./media/background.bmp", "rb");
    uint8_t header[14];
    uint8_t DIBHeader[40];

    if (image == NULL)
    {
        printf("Unable to load image file");
        return -1;
    }


    if (fread(header, 1, 14, image) != 14)
    {
        fprintf(stderr, "ERROR: Cannot read file header\n");
        fclose(image);
        return -1;
    }

    if (header[0] != 'B' && header[1] != 'M')
    {
        printf("\nthis is not a bmp file");
        fclose(image);
        return 0;
    }

    if (fread(DIBHeader, 1, 40, image) != 40)
    {
        fprintf(stderr, "ERROR: Cannot read file DIB header\n");
        fclose(image);
        return -1;
    }
    
    uint32_t headerSize = *(uint32_t*)&DIBHeader[0];
    int32_t width = *(int32_t*)&DIBHeader[4];
    int32_t height = *(int32_t*)&DIBHeader[8];

    printf("Header size in bytes: %u", headerSize);
    printf("width: %d, height: %d\n", width, height);

    return 0;
}