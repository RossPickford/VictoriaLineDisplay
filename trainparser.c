#include <stdio.h>

int main(int argv, char** argc) { 
    FILE* stationinfo;
    system("curl https://api.tfl.gov.uk/TrackerNet/PredictionDetailed/V/VIC?app_key=1af1e69a0f734e779134a71357b06073 -o trainsvic.xml ");
    stationinfo = fopen("trainsvix.xml", "rb");

    char* stream[100000];

    while(fgets(stream, sizeof(stream), stationinfo)) {
        
    }
}