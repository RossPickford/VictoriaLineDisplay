#include "t_dataCollection.h"
#include <stdlib.h>

char readBuff[1028];

int main(void)
{
    system("curl https://api.tfl.gov.uk/Line/victoria/Arrivals/ -o trains.txt");

    FILE *trains = fopen("trains.txt", "rb");

    while (fgets(readBuff, sizeof(readBuff), trains))
        printf("%s", readBuff);

    return 0;
}