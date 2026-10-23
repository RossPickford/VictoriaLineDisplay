#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int main(void)
{
  system("tx");
  FILE *text = fopen("test.txt", "r+");
  char val = 0;

  clock_t deltatime = 0;
  clock_t previoustime = 0;
  while (1)
  {
    clock_t curtime = clock();
    deltatime += curtime - previoustime;
    previoustime = curtime;

    fgets(&val, 2, text);
    fseek(text, 0, SEEK_SET);

    if (!val && deltatime > (5 * CLOCKS_PER_SEC))
    {
      system("start tx");
      printf("Calling tx with value: %u and time: %f\n", val, (float)curtime / (float)CLOCKS_PER_SEC);
      deltatime = deltatime % 5;
    }

    if (val)
    {
      printf("resetting by rx with value: %u and time: %f\n", val, (float)curtime / (float)CLOCKS_PER_SEC);
      char zero = 0;
      fwrite(&zero, 1, 1, text);
      fseek(text, 0, SEEK_SET);
    }
  }

  fclose(text);
  return 0;
}