#include <stdio.h>

int main(void) {
  FILE* text = fopen("test.txt", "wb");
  char val[2] = {1, 1};
  fwrite(&val, 1, 2, text);
  fclose(text);

  return 0;
}