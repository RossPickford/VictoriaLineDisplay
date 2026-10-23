#include <stdio.h>

int main(void) {
  FILE* text = fopen("test.txt", "wb");
  char val = 1;
  fwrite(&val, 1, 1, text);
  fclose(text);
  return 0;
}