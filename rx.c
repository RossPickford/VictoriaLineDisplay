#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int main(void) {
  system("./tx");
  FILE* text = fopen("test.txt", "rb");
  char val = 0;

  float deltatime = 0.0f;
  float previoustime = 0.0f;

  while (1) {
    float curtime = ((float)clock() / (float)CLOCKS_PER_SEC);
    deltatime += curtime - previoustime;
    previoustime = curtime;

    if (deltatime > 5.0f) {
      system("start ./tx");
      deltatime -= 5.0f;
    }

    if (val) {
      printf("value: %u\n", val);
      FILE* temp = fopen("test.txt", "wb");
      char zero = 0;
      fwrite(&zero, 1, 1, temp);
      fclose(temp);
    }

    fgets(&val, 1, text);
  }

  fclose(text);
  return 0;
}