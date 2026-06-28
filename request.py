from sys import path
import link
path.append(link.getRequestsAddress())

from requests import get
import nextStation

API_KEY = "580efc09100e43b6976feaefb4e33f12"
url = "https://api.tfl.gov.uk/Line/victoria/Arrivals"
params = {"app_key": API_KEY}

response = get(url, params=params)
data = response.json()

trainData = {}

print("Southbound Trains: ")
for train in data:
    vehicle_id = train.get("vehicleId", "Unknown")
    location = train.get("currentLocation", "No location available")
    stationName = train.get("stationName", "No Station Available")
    stationName = stationName.replace(" Underground Station", "")
    if direction == "Brixton":
        direction = "southbound"
    else:
        direction = "northbound"
    if "Leaving" in location or "Left" in location:
        print("train Leaving")
    elif "Between" in location or "Approaching" in location:
        direction = train.get("towards", "No Direction Available")
        if direction == "Brixton":
            direction = "southbound"
        else:
            direction = "northbound"
        time = train.get("timeToStation", "Unknown")
        if stationName in location:
            if vehicle_id in trainData:
                if trainData[vehicle_id][3] > time:
                     trainData[vehicle_id][3] = time
            else:
                trainData.update({vehicle_id : [stationName, direction, location, time]})
    elif "At" in location or "At Platform" == location:
        if id in trainData:
            if direction == "northbound":
                _nextStation = nextStation.northbound[stationName]
            else:
                _nextStation = nextStation.southbound[stationName]

            if trainData[id][0] != _nextStation:
                trainData[id] = [stationName, direction, location, 0]
        else:
            trainData[id] = [stationName, direction, location, 0]

import trainTimes

for train_id, data in trainData.items():
    print(f"Train {train_id} → {data[0]} | direction: {data[1]} | location: {data[2]} | time to station: {data[3]} / {trainTimes.getStationTime(data[1], data[0])}")