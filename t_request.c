/**
 * This collects train request data from the tfl servers, places it inside a
 * .txt file and once the request is finised, it will notify the binary file
 * "request_finished" by changing the first byte to 1
 */

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define REQUEST_INTERVAL 30000

int main(void) {
  FILE* text = fopen("requestcomplete.bin", "r+");
  char val = 1;

  clock_t time = 0;
  clock_t previous = clock();
  clock_t delta = 0;

  while (1) {
    time = clock();
    delta += time - previous;
    previous = time;

    fgets(&val, 2, text);
    fseek(text, 0, SEEK_SET);
    if (val == 2) {
      break;
    } else {
      val = 1;
    }

    if (delta >= REQUEST_INTERVAL) {
      delta = delta % REQUEST_INTERVAL;

      system(
          "curl https://api.tfl.gov.uk/Line/victoria/Arrivals/ -o trains.txt ");
      fwrite(&val, 1, 1, text);
    }
  }

  fclose(text);

  return 0;
}