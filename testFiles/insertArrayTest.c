#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#define ARRAY_MAX 20

void updateTrainNode(uint32_t *arr1, uint32_t *arr2, uint8_t *arr1Size, uint8_t arr2Size)
{
    uint8_t iarr1 = 0, iarr2 = 0;
    uint8_t a1Size = *arr1Size;

    while (iarr1 < a1Size && iarr2 < arr2Size)
    {
        uint32_t t_arr1 = *(arr1 + iarr1);
        uint32_t t_arr2 = *(arr2 + iarr2);
        printf("arr1[%u]: %u | arr2[%u]: %u\n", iarr1, t_arr1, iarr2, t_arr2);

        if (t_arr1 == t_arr2) // There is a match in data IDs
        {
            printf("t_displayNodes: changing single node data\n");
        }
        else if (t_arr1 < t_arr2) // There is no data for current node
        {
            printf("node missing data\n");

            if (iarr1 < a1Size)
                iarr1++;

            continue;
        }
        else if ((a1Size + 1) < ARRAY_MAX) // Insert a new node into the current position
        {
            printf("inserting new node\n");

            for (size_t i = a1Size++; i > iarr1; i--)
                *(arr1 + i) = *(arr1 + i - 1);

            t_arr1 = t_arr2;
        }
        else
        {
            printf("Cannot assign another train node due to reaching Arena max - %u\n", a1Size);
            break;
        }

        *(arr1 + iarr1) = t_arr1;
        if (iarr1 < a1Size)
            iarr1++;
        if (iarr2 < arr2Size)
            iarr2++;
    }

    if (iarr2 < arr2Size && a1Size < ARRAY_MAX)
        while (iarr2 < arr2Size)
        {
            printf("t_displayNodes: filling node array\n");

            a1Size++;
            *(arr1 + iarr1++) = *(arr2 + iarr2++);
        }

    *arr1Size = a1Size;
}

int main()
{
    uint32_t *arr1 = (uint32_t *)malloc(sizeof(uint32_t) * ARRAY_MAX);

    uint8_t arr1Size = 1;
    arr1[0] = 5;
    // arr1[1] = 2;
    // arr1[2] = 4;
    // arr1[3] = 6;

    uint32_t arr2[] = {3};

    updateTrainNode(arr1, arr2, &arr1Size, 1);

    printf("===== final array =====\n");
    for (size_t i = 0; i < arr1Size; i++)
        printf("%u\n", arr1[i]);

    free(arr1);
}
