from sys import path
import link
path.append()

from requests import get


API_KEY = "580efc09100e43b6976feaefb4e33f12"
url = "https://api.tfl.gov.uk/Line/victoria/Arrivals"
params = {"app_key": API_KEY}

response = get(url, params=params)
data = response.json()

trainData = {}

print("Southbound Trains: ")
for train in data:
    vehicle_id = train.get("vehicleId", "Unknown")
    stationName = train.get("stationName", "No Station Available")
    stationName = stationName.replace(" Underground Station", "")
    location = train.get("currentLocation", "No location available")
    direction = train.get("towards", "No Direction Available")
    if direction == "Brixton":
        direction = "southbound"
    else:
        direction = "northbound"
    time = train.get("timeToStation", "Unknown")
    if stationName in location:
        if vehicle_id in trainData:
            if trainData[vehicle_id][2] > time:
                 trainData[vehicle_id][2] = time
        else:
            trainData.update({vehicle_id : [stationName, direction, time]})

import trainTimes

for train_id, data in trainData.items():
    print(f"Train {train_id} → {data[0]} | direction: {data[1]} | time to station: {data[2]} / {trainTimes.getStationTime(data[1], data[0])}")