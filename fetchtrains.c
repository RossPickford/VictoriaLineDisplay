#include <stdlib.h>
#include <stdio.h>

void fetchtrains() {
    system("start curl https://api.tfl.gov.uk/TrackerNet/PredictionDetailed/V/WAL?app_key=1af1e69a0f734e779134a71357b06073 -o trainswal.xml ");
    system("start curl https://api.tfl.gov.uk/TrackerNet/PredictionDetailed/V/BHR?app_key=1af1e69a0f734e779134a71357b06073 -o trainsbhr.xml ");
    system("start curl https://api.tfl.gov.uk/TrackerNet/PredictionDetailed/V/TTH?app_key=1af1e69a0f734e779134a71357b06073 -o trainstth.xml ");
    system("start curl https://api.tfl.gov.uk/TrackerNet/PredictionDetailed/V/SVS?app_key=1af1e69a0f734e779134a71357b06073 -o trainssvs.xml ");
    system("start curl https://api.tfl.gov.uk/TrackerNet/PredictionDetailed/V/FPK?app_key=1af1e69a0f734e779134a71357b06073 -o trainsfpk.xml ");
    system("start curl https://api.tfl.gov.uk/TrackerNet/PredictionDetailed/V/HBY?app_key=1af1e69a0f734e779134a71357b06073 -o trainshby.xml ");
    system("start curl https://api.tfl.gov.uk/TrackerNet/PredictionDetailed/V/KXX?app_key=1af1e69a0f734e779134a71357b06073 -o trainskxx.xml ");
    system("start curl https://api.tfl.gov.uk/TrackerNet/PredictionDetailed/V/EUS?app_key=1af1e69a0f734e779134a71357b06073 -o trainseus.xml ");
    system("start curl https://api.tfl.gov.uk/TrackerNet/PredictionDetailed/V/WST?app_key=1af1e69a0f734e779134a71357b06073 -o trainswst.xml ");
    system("start curl https://api.tfl.gov.uk/TrackerNet/PredictionDetailed/V/OXC?app_key=1af1e69a0f734e779134a71357b06073 -o trainsoxc.xml ");
    system("start curl https://api.tfl.gov.uk/TrackerNet/PredictionDetailed/V/GPK?app_key=1af1e69a0f734e779134a71357b06073 -o trainsgpk.xml ");
    system("start curl https://api.tfl.gov.uk/TrackerNet/PredictionDetailed/V/VIC?app_key=1af1e69a0f734e779134a71357b06073 -o trainsvic.xml ");
    system("start curl https://api.tfl.gov.uk/TrackerNet/PredictionDetailed/V/PIM?app_key=1af1e69a0f734e779134a71357b06073 -o trainspim.xml ");
    system("start curl https://api.tfl.gov.uk/TrackerNet/PredictionDetailed/V/VUX?app_key=1af1e69a0f734e779134a71357b06073 -o trainsvux.xml ");
    system("start curl https://api.tfl.gov.uk/TrackerNet/PredictionDetailed/V/STK?app_key=1af1e69a0f734e779134a71357b06073 -o trainsstk.xml ");
    system("start curl https://api.tfl.gov.uk/TrackerNet/PredictionDetailed/V/BRX?app_key=1af1e69a0f734e779134a71357b06073 -o trainsbrx.xml ");
}

int main(int argc, char** argv) {
    fetchtrains();
    return 0;
}