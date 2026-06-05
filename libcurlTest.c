#include <stdio.h>
#include "curl/curl.h"

int main(void)
{
  printf("hello!\n");
  CURL *curl;

  // CURLcode result = curl_global_init(CURL_GLOBAL_ALL);
  CURLcode result;
  if (result != CURLE_OK)
  {
    printf("welp");
    // return (int)result;
  }

  curl = curl_easy_init();
  if (!curl)
  {
    fprintf(stderr, "failed to easy init");
    return -1;
  }

  /* curl_easy_setopt(curl, CURLOPT_URL, "https://api.tfl.gov.uk/Line/victoria/Arrivals");
  // example.com is redirected, so we tell libcurl to follow redirection
  // curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);

  // Perform the request, result gets the return code
  result = curl_easy_perform(curl);
  // Check for errors
  if (result != CURLE_OK)
    fprintf(stderr, "curl_easy_perform() failed: %s\n",
            curl_easy_strerror(result));

  // always cleanup
  curl_easy_cleanup(curl);
 */
  // curl_global_cleanup();
  return 0;
}