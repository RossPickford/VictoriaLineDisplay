#include <stdlib.h>
#include <string.h>

#include "t_dataCollection.h"

#define DATA_ARENA_SIZE 50
#define STREAM_BUFF_SIZE 300000

static char stream[STREAM_BUFF_SIZE];

static char k_buff[32];
static char v_buff[32];

void t_data_init(t_data *tdata)
{
    tdata->data = (train_object *)malloc(sizeof(train_object) * DATA_ARENA_SIZE);
    memset(tdata->data, 0, sizeof(train_object) * DATA_ARENA_SIZE);
}

void t_data_quit(t_data *tdata) { free(tdata->data); }

uint8_t strComp(char *str1, char *str2, size_t len)
{
    assert(len < sizeof(str1) && len < sizeof(str2));

    for (size_t i = 0; i < len; i++)
        if (str1[i] != str2[i])
            return 0;

    return 1;
}

int is_data_null(train_object data)
{
    return data.id == 0 && data.direction == 0 && data.nextStation == 0 && data.state == 0 && data.stationName == 0 && data.timeToStation == 0;
}

void requestTrains(t_data *tdata)
{
    system("curl https://api.tfl.gov.uk/Line/victoria/Arrivals/ -o trains.txt");

    FILE *trains = fopen("trains.txt", "rb");

    if (!trains)
    {
        printf("failed to open files");
        return;
    }

    for (size_t depth = 0; fgets(stream, sizeof(stream), trains); depth++)
        if (depth > 1)
            printf("stream buffer reached capacity before completion\n");

    char *stream_pos = stream;

    if (*stream_pos++ != '[')
    {
        fprintf(stderr, "expected to begin with array, instead recieved %c\n",
                stream[0]);
        return;
    }

    /**
     * Looping through each train object
     */
    while (*stream_pos != ']')
    {
        if (*stream_pos != '{')
        {
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
        while (*stream_pos != '}')
        {
            if (*stream_pos != '"')
            {
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
            while (*stream_pos != '"')
            {
                assert(k_str.len < sizeof(k_str.data));
                k_str.data[k_str.len++] = *stream_pos++;
            }

            switch (k_str.data[0])
            {
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
                char *timeTo = "timeToS";
                if (k_str.data[1] == 'o')
                    k_type = KT_TOWARDS;
                else if (strComp(k_str.data, timeTo, sizeof(timeTo) - 1))
                    k_type = KT_TIMETOSTATION;
                break;
            }

            if (*++stream_pos != ':')
            {
                fprintf(stderr,
                        "expected colon separating key and value, instead "
                        "recieved %c",
                        *stream_pos);
                return;
            }

            ++stream_pos;

            if (k_type == KT_NONE)
            {
                while (*stream_pos != ',')
                {
                    if (*stream_pos++ == '"')
                        while (*stream_pos++ != '"')
                            ;
                }
            }
            else
            {
                switch (k_type)
                {
                case KT_ID: // Vehicle ID
                    if (*stream_pos != '"')
                    {
                        fprintf(stderr, "expected key string, instead recieved %c",
                                *stream_pos);
                        return;
                    }

                    ++stream_pos;

                    while (*stream_pos != '"')
                        v_str.data[v_str.len++] = *stream_pos++;

                    ++stream_pos;

                    if (v_str.data[0] < 0x30 || v_str.data[0] > 0x39)
                    {
                        fprintf(stderr,
                                "expected valid number for ID, instead recieved %c",
                                v_str.data[0]);
                        return;
                    }

                    /**
                     * Convert numerical string into real numerical
                     */
                    for (size_t i = 0; i < v_str.len; i++)
                        temp_data.id = (temp_data.id * 10u) + (v_str.data[i] - 48u);

                    break;
                default:
                    while (*stream_pos != ',')
                    {
                        if (*stream_pos++ == '"')
                            while (*stream_pos++ != '"')
                                ;
                    }
                } // end of switch
            }

            if (*stream_pos != ',' && *stream_pos != '}')
            {
                fprintf(stderr, "expected end of key-value pair, instead recieved %c",
                        *stream_pos);
                return;
            }
            else if (*stream_pos == ',')
                ++stream_pos;

        } // end of key-value pair loop inside train object

        uint8_t train_exists = 0;

        for (size_t i = 0; i < tdata->len; i++)
            if (tdata->data[i].id == temp_data.id)
                train_exists = 1;

        if (train_exists)
        {
        }
        else if (!is_data_null(temp_data))
        {
            tdata->data[tdata->len++] = temp_data;
        }

    } // end of main train object array loop

    for (size_t i = 0; i < tdata->len; i++)
        printf("id: %u\n", tdata->data[i].id);

    fclose(trains);
}

int main(void)
{
    t_data tdata;
    tdata.len = 0;
    t_data_init(&tdata);
    requestTrains(&tdata);
    return 0;
}