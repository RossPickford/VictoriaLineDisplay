#include <stdlib.h>
#include <string.h>

#include "t_dataCollection.h"

#define DATA_ARENA_SIZE 50
#define KEY_VALUE_BUFF_SIZE 64
#define STREAM_BUFF_SIZE 300000

static char stream[STREAM_BUFF_SIZE];

static char k_buff[KEY_VALUE_BUFF_SIZE];
static char v_buff[KEY_VALUE_BUFF_SIZE];

void t_data_init(t_data* tdata) {
  tdata->data = (train_object*)malloc(sizeof(train_object) * DATA_ARENA_SIZE);
  memset(tdata->data, 0, sizeof(train_object) * DATA_ARENA_SIZE);
}

void t_data_quit(t_data* tdata) { free(tdata->data); }

/**
 * Compare strings to a certain length. If the given length is larger than
 * either string's length then it returns a false
 */
uint8_t strComp(String str1, String str2, size_t len) {
  if (len > str1.len && len > str2.len) return 0;

  for (size_t i = 0; i < len; i++)
    if (str1.data[i] != str2.data[i]) return 0;

  return 1;
}

int is_data_null(train_object data) {
  return data.id == 0 && data.direction == 0 && data.nextStation == 0 &&
         data.state == 0 && data.stationName == 0 && data.timeToStation == 0;
}

void requestTrains(t_data* tdata) {
  system("curl https://api.tfl.gov.uk/Line/victoria/Arrivals/ -o trains.txt");
  // system("curl https://api.tfl.gov.uk/Line/victoria/Arrivals/ -o
  // trains.json");

  FILE* trains = fopen("trains.txt", "rb");

  if (!trains) {
    printf("failed to open files");
    return;
  }

  for (size_t depth = 0; fgets(stream, sizeof(stream), trains); depth++)
    if (depth > 1) printf("stream buffer reached capacity before completion\n");

  char* stream_pos = stream;

  if (*stream_pos++ != '[') {
    fprintf(stderr, "expected to begin with array, instead recieved %c\n",
            stream[0]);
    return;
  }

  /**
   * Looping through each train object
   */
  while (*stream_pos != ']') {
    if (*stream_pos != '{') {
      fprintf(stderr, "expected object type, instead recieved %c\n",
              *stream_pos);
      return;
    }

    ++stream_pos;

    train_object temp_data;
    memset(&temp_data, 0, sizeof(temp_data));

    /**
     * Looping through each key-value pair in a train object
     */
    while (*stream_pos != '}') {
      if (*stream_pos != '"') {
        fprintf(stderr, "expected string type, instead recieved %c\n",
                *stream_pos);
        return;
      }

      String k_str = {0, k_buff};
      String v_str = {0, v_buff};
      KeyType k_type = KT_NONE;

      ++stream_pos;

      /**
       * storing current key into a buffer
       */
      while (*stream_pos != '"') {
        assert(k_str.len < KEY_VALUE_BUFF_SIZE);
        k_str.data[k_str.len++] = *stream_pos++;
      }

      /**
       * Switch determines if key is one of specific keys with the necessary
       * train data
       */
      switch (k_str.data[0]) {
        default:
          k_type = KT_NONE;
          break;
        case 'v':
          k_type = KT_ID;
          break;
        case 's':
          k_type = KT_STATIONNAME;
          break;
        case 'c':
          k_type = KT_CURRENTLOCATION;
          break;
        case 't':
          char* timeto_literal = "timeToS";
          String timeto = {sizeof(timeto_literal) - 1, timeto_literal};
          if (k_str.data[1] == 'o')
            k_type = KT_TOWARDS;
          else if (strComp(k_str, timeto, timeto.len))
            k_type = KT_TIMETOSTATION;
          break;
      }

      if (*++stream_pos != ':') {
        fprintf(stderr,
                "expected colon separating key and value, instead "
                "recieved %c",
                *stream_pos);
        return;
      }

      ++stream_pos;

      /**
       * If the key and it's respective value are found to be unnecassary, this
       * will cycle through until the next key.
       */
      if (k_type == KT_NONE) {
        for (; *stream_pos != ',' && *stream_pos != '}'; stream_pos++) {
          if (*stream_pos == '"') {
            while (*++stream_pos != '"');
          } else if (*stream_pos == '{')
            while (*++stream_pos != '}');
        }
      } else {
        switch (k_type) {
          case KT_ID:  // Vehicle ID
            if (*stream_pos != '"') {
              fprintf(stderr, "expected key string, instead recieved %c",
                      *stream_pos);
              return;
            }

            ++stream_pos;

            /**
             * Store the value into value buffer -
             * whilst moving stream_pos to the end of the key-value pair (i.e.
             * comma ',')
             *  */
            while (*stream_pos != '"') v_str.data[v_str.len++] = *stream_pos++;

            ++stream_pos;

            if (v_str.data[0] < 0x30 || v_str.data[0] > 0x39) {
              fprintf(stderr,
                      "expected valid number for ID, instead recieved %c",
                      v_str.data[0]);
              return;
            }

            /**
             * Convert numerical string into integer
             */
            for (size_t i = 0; i < v_str.len; i++) {
              temp_data.id = (temp_data.id * 10u) + (v_str.data[i] - 48u);
            }

            break;

          case KT_STATIONNAME:
            if (*stream_pos != '"') {
              fprintf(stderr, "expected key string, instead recieved %c",
                      *stream_pos);
              return;
            }

            ++stream_pos;

            while (*stream_pos != '"') {
              v_str.data[v_str.len++] = *stream_pos++;
            }

            ++stream_pos;

            switch (v_str.data[0]) {
              case 'T':
                temp_data.stationName = V_TOTTENHAM;
                break;
              case 'F':
                temp_data.stationName = V_FINSBURYPARK;
                break;
              case 'H':
                temp_data.stationName = V_HIGHBURY;
                break;
              case 'K':
                temp_data.stationName = V_KINGSCROSS;
                break;
              case 'E':
                temp_data.stationName = V_EUSTON;
                break;
              case 'O':
                temp_data.stationName = V_OXFORDCIRCUS;
                break;
              case 'G':
                temp_data.stationName = V_GREENPARK;
                break;
              case 'P':
                temp_data.stationName = V_PIMLICO;
                break;
              case 'W':
                temp_data.stationName =
                    v_str.data[2] == 'l' ? V_WALTHAMSTOW : V_WARRENSTREET;
                break;
              case 'B':
                temp_data.stationName =
                    v_str.data[1] == 'l' ? V_BLACKHORSE : V_BRIXTON;
                break;
              case 'S':
                temp_data.stationName =
                    v_str.data[1] == 'e' ? V_SEVENSISTERS : V_STOCKWELL;
                break;
            }
            break;

          default:
            for (; *stream_pos != ',' && *stream_pos != '}'; stream_pos++) {
              if (*stream_pos == '"') {
                while (*++stream_pos != '"');
              } else if (*stream_pos == '{')
                while (*++stream_pos != '}');
            }
        }  // end of switch
      }

      if (*stream_pos != ',' && *stream_pos != '}') {
        fprintf(stderr, "expected end of key-value pair, instead recieved %c",
                *stream_pos);
        return;
      } else if (*stream_pos == ',')
        ++stream_pos;

    }  // end of key-value pair loop inside train object

    // move to beginning of next train object or the end of the array
    while (*stream_pos != '{' && *stream_pos != ']') ++stream_pos;

    uint8_t train_exists = 0;

    for (size_t i = 0; i < tdata->len; i++)
      if (tdata->data[i].id == temp_data.id) train_exists = 1;

    if (train_exists) {
    } else if (!is_data_null(temp_data)) {
      tdata->data[tdata->len++] = temp_data;
    }

  }  // end of main train object array loop

  for (size_t i = 0; i < tdata->len; i++) {
    printf("id: %u\n station name: %u\n\n", tdata->data[i].id,
           tdata->data[i].stationName);
  }

  printf("number of unique trains: %u", tdata->len);

  fclose(trains);
}

int main(void) {
  t_data tdata;
  tdata.len = 0;
  t_data_init(&tdata);
  requestTrains(&tdata);
  return 0;
}