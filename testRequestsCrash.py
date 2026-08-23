import trainDataTest

import time

while(True):
    data = trainDataTest.requestTrainData()

    for d in data:
        print(f"id: {d[0]} | direction: {d[1]} | station: {d[2]} | time to: {d[3]}")

    time.sleep(30)