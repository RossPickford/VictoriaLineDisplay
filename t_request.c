/**
 * This collects train request data from the tfl servers, places it inside a .txt file
 * and once the request is finised, it will notify the binary file "request_finished"
 * by changing the first byte to 1
 */

#include <stdio.h>
#include <stdlib.h>

int main(void) {
  system("curl https://api.tfl.gov.uk/Line/victoria/Arrivals/ -o trains.txt &");

  FILE* text = fopen("requestcomplete.bin", "wb");
  char val = 1;
  fwrite(&val, 1, 1, text);
  fclose(text);

  return 0;
}