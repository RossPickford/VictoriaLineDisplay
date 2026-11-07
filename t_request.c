/**
 * This collects train request data from the tfl servers, places it inside a
 * .txt file and once the request is finised, it will notify the binary file
 * "request_finished" by changing the first byte to 1
 */

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define REQUEST_INTERVAL 30

int main(void) {
  FILE* text = fopen("requestcomplete.txt", "r+");
  char val = 1;

  int time = 0;
  int previous = clock() / CLOCKS_PER_SEC;
  int delta = 0;

  while (1) {
    time = clock() / CLOCKS_PER_SEC;
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
          "curl https://api.tfl.gov.uk/Line/victoria/Arrivals?app_key=580efc09100e43b6976feaefb4e33f12 -o trains.txt ");
      fwrite(&val, 1, 1, text);
    }
  }

  fclose(text);

  return 0;
}