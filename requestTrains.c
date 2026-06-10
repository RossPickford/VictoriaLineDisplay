#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

typedef enum Station
{
  D_BRIXTON = 1 << 0,
  D_STOCKWELL = 1 << 1,
  D_VAUXHALL = 1 << 2,
  D_PIMLICO = 1 << 3,
  D_VICTORIA = 1 << 4,
  D_GREEN_PARK = 1 << 5,
  D_OXFORD_CIRCUS = 1 << 6,
  D_WARREN_STREET = 1 << 7,
  D_EUSTON = 1 << 8,
  D_KINGS_CROSS = 1 << 9,
  D_HIGHBURY_ISLINGTON = 1 << 10,
  D_FINSBURY_PARK = 1 << 11,
  D_SEVEN_SISTERS = 1 << 12,
  D_TOTTENHAM_HALE = 1 << 13,
  D_BLACKHORSE_ROAD = 1 << 14,
  D_WALTHAMSTOW_CENTRAL = 1 << 15,
} Station;

typedef struct Train
{
  Station stops;
  uint16_t id;
  uint16_t time;

} Train;

void parseData(char *filename)
{
  FILE *file = fopen(filename, "r");

  if (!file)
  {
    printf("failed to open file");
    return;
  }

  char c;
  char quoteSkipCount = 0;
  while (quoteSkipCount < 10)
    fgetc(file);

  
  
}

int main(void)
{
  system("curl https://api.tfl.gov.uk/Line/victoria/Arrivals/940GZZLUBXN -o southbound.json");
  system("curl https://api.tfl.gov.uk/Line/victoria/Arrivals/940GZZLUWWL -o northbound.json");

  FILE *northbound = fopen("northbound.json", "r");
  FILE *southbound = fopen("southbound.json", "r");

  if (!northbound || !southbound)
  {
    printf("failed to open files");
    return -1;
  }

  printf("Southbound: \n");

  char c;
  while (fgetc(southbound) == '[')
    ;

  char *key, *value;
  // fscanf(southbound, "%s", key);

  printf("%c\n", fgetc(southbound));

  /* char buffer[1000];

  // for (size_t i = 0; i < 100; i++)
  // buffer[i] = '\0';

  // fgets(buffer, sizeof(buffer), southbound);

  while (fgets(buffer, sizeof(buffer), southbound))
    printf("%s\n", buffer); */

  // printf("%s\n", buffer);

  fclose(northbound);
  fclose(southbound);

  return 0;
}

/*
  https://api.tfl.gov.uk/Line/victoria/Arrivals/940GZZLUBXN - gets all trains in reference to Brixton
  https://api.tfl.gov.uk/Line/victoria/Arrivals/940GZZLUWWL - gets all trains in reference to Walthamstow Central
*/