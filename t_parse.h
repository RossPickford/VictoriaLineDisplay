#include <string.h>

typedef enum KeyType {
  KT_NONE,
  KT_ID,
  KT_STATIONNAME,
  KT_TIMETOSTATION,
  KT_CURRENTLOCATION,
  KT_TOWARDS,
  KT_LIMBO
} KeyType;

void getstringvalue(char** stream_pos, String* val);
uint8_t strComp(String str1, String str2, size_t len);
int8_t getstationfromstring(String str, size_t startpos);