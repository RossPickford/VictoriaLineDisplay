#include <string.h>

void getstringvalue(char **stream_pos, String *val);
uint8_t strComp(String str1, String str2, size_t len);
int8_t getstationfromstring(String str, size_t startpos);