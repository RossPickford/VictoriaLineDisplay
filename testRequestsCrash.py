import trainDataTest

import time

previousdata = trainDataTest.requestTrainData()
timeto = {}
while(True):
    data = trainDataTest.requestTrainData()
    print("========================")
    for d in data:
        for cd in previousdata:
            if (d[0] == cd[0] and d[3] == cd[3]):
                if d[0] in timeto:
                    timeto[d[0]] += 1
                else:
                    timeto.update({d[0] : 1})
                print(f"id: {d[0]} | time to: {d[3]} | amount: {timeto[d[0]]}")
            elif d[0] in timeto.keys():
                timeto[d[0]] = 0

    previousdata = data.copy()
    time.sleep(30)