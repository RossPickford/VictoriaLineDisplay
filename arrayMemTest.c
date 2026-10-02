#include <stdio.h>
#include <time.h>
#include <string.h>
#include <stdlib.h>

#define MEM_SIZE 10000

char global[MEM_SIZE];
char *heap_g = NULL;

int main(void)
{
    long globalAv = 0;
    long heapAv = 0;

    char *heap = (char *)malloc(MEM_SIZE);
    heap_g = heap;

    for (size_t i = 0; i < 100000; i++)
    {

        memset(global, 0, sizeof(global));
        memset(heap, 0, sizeof(heap));

        clock_t ticks = clock();

        for (size_t i = 0; i < MEM_SIZE; i++)
        {
            global[i] = 'Y';
        }

        globalAv += clock() - ticks;

        ticks = clock();

        for (size_t i = 0; i < MEM_SIZE; i++)
        {
            heap_g[i] = 'Y';
        }

        heapAv += clock() - ticks;
    }

    globalAv = globalAv;
    heapAv = heapAv;

    printf("%ld\n", clock());

    printf("global mem took: %ld\n", globalAv);
    printf("heap mem took: %ld\n", heapAv);

    free(heap);

    return 0;
}