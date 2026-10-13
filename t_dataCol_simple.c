#include "t_dataCollection.h"
#include "t_parse.h"

#define DATA_ARENA_SIZE 50
#define KEY_VALUE_BUFF_SIZE 64
#define STREAM_BUFF_SIZE 300000

static char stream[STREAM_BUFF_SIZE];

static char k_buff[KEY_VALUE_BUFF_SIZE];
static char v_buff[KEY_VALUE_BUFF_SIZE];

static uint8_t traintotal = 0;

void requestTrains(t_data* tdata) {
  memset(tdata->data, 0, sizeof(train_object) * tdata->len);
  tdata->len = 0;

  system("curl https://api.tfl.gov.uk/Line/victoria/Arrivals/ -o trains.txt");
  system("curl https://api.tfl.gov.uk/Line/victoria/Arrivals/ -o trains.json");

  FILE* trains = fopen("trains.txt", "rb");

  if (!trains) {
    printf("failed to open files");
    return;
  }

  for (size_t depth = 0; fgets(stream, sizeof(stream), trains); depth++)
    if (depth > 1) printf("stream buffer reached capacity before completion\n");

  char* streampos = stream;

  if (*streampos++ != '[') {
    fprintf(stderr, "expected to begin with array, instead recieved %c\n",
            stream[0]);
    return;
  }

  /**
   * Looping through each train object
   */
  while (*streampos != ']') {
    if (*streampos != '{') {
      fprintf(stderr, "expected object type, instead recieved %c\n",
              *streampos);
      return;
    }

    ++streampos;

    train_object tempdata;
    memset(&tempdata, 0, sizeof(tempdata));

    /**
     * Looping through each key-value pair in a train object
     */
    while (*streampos != '}') {
      if (*streampos != '"') {
        fprintf(stderr, "expected string type, instead recieved %c\n",
                *streampos);
        return;
      }

      String k_str = {0, k_buff};
      String vstr = {0, v_buff};
      KeyType k_type = KT_NONE;

      ++streampos;

      /**
       * storing current key into a buffer
       */
      while (*streampos != '"') {
        assert(k_str.len < KEY_VALUE_BUFF_SIZE);
        k_str.data[k_str.len++] = *streampos++;
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

      if (*++streampos != ':') {
        fprintf(stderr,
                "expected colon separating key and value, instead "
                "recieved %c",
                *streampos);
        return;
      }

      ++streampos;

      /**
       * If the key and it's respective value are found to be unnecassary, this
       * will cycle through until the next key.
       */
      if (k_type == KT_NONE) {
        for (; *streampos != ',' && *streampos != '}'; streampos++) {
          if (*streampos == '"') {
            while (*++streampos != '"');
          } else if (*streampos == '{')
            while (*++streampos != '}');
        }
      } else {
        switch (k_type) {
          case KT_ID:  // Vehicle ID
            getstringvalue(&streampos, &vstr);

            if (vstr.data[0] < 0x30 || vstr.data[0] > 0x39) {
              fprintf(stderr,
                      "expected valid number for ID, instead recieved %c",
                      vstr.data[0]);
              return;
            }

            /**
             * Convert numerical string into integer
             */
            for (size_t i = 0; i < vstr.len; i++) {
              tempdata.id = (tempdata.id * 10u) + (vstr.data[i] - 48u);
            }

            break;

          case KT_STATIONNAME:
            getstringvalue(&streampos, &vstr);
            tempdata.stationName = (station_name)getstationfromstring(vstr, 0);
            break;
          case KT_TOWARDS:
            getstringvalue(&streampos, &vstr);

            switch (vstr.data[0]) {
              case 'B':
                tempdata.direction = 1;
                break;
              case 'W':
              case 'S':
                tempdata.direction = -1;
                break;
              default:
                fprintf(stderr, "unknown destination: %s\n", vstr.data);
                break;
            }

            // change the next station to the correct one based on direction
            tempdata.nextStation += tempdata.direction * tempdata.alterStation;

            if (tempdata.nextStation < V_WALTHAMSTOW ||
                tempdata.nextStation > V_BRIXTON) {
              fprintf(stderr, "invalid station value\n");
              return;
            }

            break;
          case KT_TIMETOSTATION:
            if (*streampos < 0x30 || *streampos > 0x39) {
              fprintf(stderr,
                      "expected value to be a number, instead recieved %c",
                      *streampos);
              return;
            }

            while (*streampos != ',') {
              vstr.data[vstr.len++] = *streampos++;
            }

            for (size_t i = 0; i < vstr.len; i++) {
              tempdata.timeToStation =
                  (tempdata.timeToStation * 10u) + (vstr.data[i] - 48u);
            }

            break;
          case KT_CURRENTLOCATION:
            getstringvalue(&streampos, &vstr);

            size_t index = 0;
            switch (vstr.data[0]) {
              /**
               * Either 'Departing' or 'Departed' hence whey the index to
               * collect the station name is dependant on which word the value
               * begins with
               */
              case 'D':
                // if the 9th byte in the value is ' ' then the first word must
                // be 'Departed'
                index = vstr.data[8] == ' ' ? 9 : 10;
                tempdata.nextStation =
                    (station_name)getstationfromstring(vstr, index);

                tempdata.alterStation = 1;
                break;
                /**
                 * If the value begins with 'Left'
                 */
              case 'L':
                tempdata.nextStation = getstationfromstring(vstr, 5);
                break;
              /**
               * This can have several options - beginning with 'At' or
               * 'Approaching', or the whole value can be 'At Platform' - hence
               * the secondary check if Pimlico is returned
               */
              case 'A':

                index = vstr.data[1] == 't' ? 3 : 12;
                tempdata.nextStation = getstationfromstring(vstr, index);
                if (tempdata.nextStation == V_PIMLICO &&
                    vstr.data[index + 1] == 'l')
                  tempdata.nextStation = tempdata.stationName;

                if (index == 3) {  // if the current locatain is 'At', then the
                                   // train is at a stop
                  tempdata.timeToStation = 0;
                }
                break;
              /**
               * This is either 'Brixton Area' or ' Between X and Y'
               * To get the correct station, the direction is added from
               * the station result to get the actual station the train is
               * heading to
               */
              case 'B':
                if (vstr.data[1] == 'r') {
                  tempdata.nextStation == V_BRIXTON;
                  break;
                } else if (vstr.data[1] != 'e') {
                  fprintf(stderr, "unexpected value for current location: %c\n",
                          vstr.data[1]);
                  return;
                }

                tempdata.nextStation =
                    (station_name)getstationfromstring(vstr, 8);
                tempdata.alterStation = 1;
                break;
              default:
                fprintf(stderr, "unknown current location\n");
                return;
            }
            break;
          default:
            for (; *streampos != ',' && *streampos != '}'; streampos++) {
              if (*streampos == '"') {
                while (*++streampos != '"');
              } else if (*streampos == '{')
                while (*++streampos != '}');
            }
        }  // end of data collection switch
      }

      if (*streampos != ',' && *streampos != '}') {
        fprintf(stderr, "expected end of key-value pair, instead recieved %c\n",
                *streampos);
        return;
      } else if (*streampos == ',')
        ++streampos;

    }  // end of key-value pair loop inside train object

    // move to beginning of next train object or the end of the array
    while (*streampos != '{' && *streampos != ']') ++streampos;

    // Adding the train information to the main arena of train data
    uint8_t train_exists = 0;
    size_t itrain = 0;

    for (size_t i = 0; i < tdata->len; i++)
      if (tdata->data[i].id == tempdata.id) {
        train_exists = 1;
        itrain = i;
      }

    /**
     *  if train is already in the main arena, only change the time to if it is
     * less than the currently saved time to.
     */
    if (train_exists &&
        tempdata.timeToStation < tdata->data[itrain].timeToStation) {
      tdata->data[itrain].timeToStation = tempdata.timeToStation;
    } else if (!isobjectempty(tempdata) &&
               tempdata.stationName == tempdata.nextStation) {
      tdata->data[tdata->len++] = tempdata;
      traintotal++;
    }

  }  // end of main train object array loop =======

  // sort trains into order of ids
  for (size_t i = 0; i < tdata->len - 1; i++)
    for (size_t j = i + 1; j < tdata->len; j++) {
      if (tdata->data[i].id > tdata->data[j].id) {
        train_object tempdata = tdata->data[i];
        tdata->data[i] = tdata->data[j];
        tdata->data[j] = tempdata;
      }
    }

  // print details about incoming train data - mostly for debugging purposes
  for (size_t i = 0; i < tdata->len; i++) {
    printf(
        "id: %u\n station name: %s\n direction: %d\n next station: %s\n time "
        "to station: %u\n\n",
        tdata->data[i].id, getstationliteral(tdata->data[i].stationName),
        tdata->data[i].direction, getstationliteral(tdata->data[i].nextStation),
        tdata->data[i].timeToStation);
  }

  printf("number of unique trains: %u\n", tdata->len);
  printf("total number of trains by ID: %u\n", traintotal);

  fclose(trains);
}

/**
 * Store the string value into value buffer -
 * whilst moving the incoming text stream positon to the end of the key-value
 * pair (i.e. comma ',' or '}')
 *  */
void getstringvalue(char** streampos, String* val) {
  if (**streampos != '"') {
    fprintf(stderr, "expected key string, instead recieved %c\n", **streampos);
    return;
  }

  ++(*streampos);

  while (**streampos != '"') {
    val->data[val->len++] = *((*streampos)++);
  }

  ++(*streampos);
}

int8_t getstationfromstring(String str, size_t startpos) {
  switch (str.data[startpos]) {
    case 'T':
      return (int8_t)V_TOTTENHAM;
    case 'F':
      return (int8_t)V_FINSBURYPARK;
    case 'H':
      return (int8_t)V_HIGHBURY;
    case 'K':
      return (int8_t)V_KINGSCROSS;
    case 'E':
      return (int8_t)V_EUSTON;
    case 'O':
      return (int8_t)V_OXFORDCIRCUS;
    case 'G':
      return (int8_t)V_GREENPARK;
    case 'P':
      return (int8_t)V_PIMLICO;
    case 'W':
      return (int8_t)(str.data[startpos + 2] == 'l' ? V_WALTHAMSTOW
                                                    : V_WARRENSTREET);
    case 'B':
      return (int8_t)(str.data[startpos + 1] == 'l' ? V_BLACKHORSE : V_BRIXTON);
    case 'S':
      return (int8_t)(str.data[startpos + 1] == 'e' ? V_SEVENSISTERS
                                                    : V_STOCKWELL);
    case 'V':
      return (int8_t)(str.data[startpos + 1] == 'i' ? V_VICTORIA : V_VAUXHALL);
    default:
      printf("unknown station\n");
      return (int8_t)V_NONE;
  }
}

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

/**
 * check if train object has no variables set
 */
int isobjectempty(train_object data) {
  return data.id == 0 && data.direction == 0 && data.nextStation == 0 &&
         data.state == 0 && data.stationName == 0 && data.timeToStation == 0;
}

char* getstationliteral(station_name station) {
  switch (station) {
    case V_WALTHAMSTOW:
      return "Walthamstow Central";
    case V_BLACKHORSE:
      return "Blackhorse Road";
    case V_TOTTENHAM:
      return "Tottenham Hale";
    case V_SEVENSISTERS:
      return "Seven Sisters";
    case V_FINSBURYPARK:
      return "Finsbury Park";
    case V_HIGHBURY:
      return "Highbury & Islington";
    case V_KINGSCROSS:
      return "Kings Cross";
    case V_EUSTON:
      return "Euston";
    case V_WARRENSTREET:
      return "Warren Street";
    case V_OXFORDCIRCUS:
      return "Oxford Circus";
    case V_GREENPARK:
      return "Green Park";
    case V_VICTORIA:
      return "Victoria";
    case V_PIMLICO:
      return "Pimlico";
    case V_VAUXHALL:
      return "Vauxhall";
    case V_STOCKWELL:
      return "Stockwell";
    case V_BRIXTON:
      return "Brixton";
    default:
      return "Unknown Station";
  }
}

/**
 * main function ========================================
 */
int main(void) {
  t_data tdata;
  tdata.len = 0;
  t_data_init(&tdata);
  requestTrains(&tdata);
  t_data_quit(&tdata);
  return 0;
}

void t_data_init(t_data* tdata) {
  tdata->data = (train_object*)malloc(sizeof(train_object) * DATA_ARENA_SIZE);
  memset(tdata->data, 0, sizeof(train_object) * DATA_ARENA_SIZE);
}

void t_data_quit(t_data* tdata) { free(tdata->data); }