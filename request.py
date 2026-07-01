import time
start = time.time()

station_id_lookup = { "Stockwell" : "940GZZLUSKW",
                     "Vauxhall" : "940GZZLUVXL",
                     "Pimlico" : "940GZZLUPCO",
                     "Victoria" : "940GZZLUVIC",
                     "Green Park" : "940GZZLUGPK",
                     "Oxford Circus" : "940GZZLUOXC",
                     "Warren Street" : "940GZZLUWRR",
                     "Euston" : "940GZZLUEUS",
                     "Kings Cross St. Pancras" : "940GZZLUKSX",
                     "Kings Cross St. P" : "940GZZLUKSX",
                     "Highbury & Islington" : "940GZZLUHAI",
                     "Highbury & Isl" : "940GZZLUHAI",
                     "Finsbury Park" : "940GZZLUFPK",
                     "Seven Sisters" : "940GZZLUSVS",
                     "Tottenham Hale" : "940GZZLUTMH",
                     "Blackhorse Road" : "940GZZLUBLR"}

class TrainData:
    def __init__(self, nextStation, timeToStation, state):
        self.nextStation = nextStation
        self.timeToStation = timeToStation
        self.state = state

import subprocess
# subprocess.run(["curl", "https://api.tfl.gov.uk/Line/victoria/Arrivals/", "-o", "trains.json"])
subprocess.run(["curl", "https://api.tfl.gov.uk/Line/victoria/Arrivals/940GZZLUBXN", "-o", "southbound.json"])
subprocess.run(["curl", "https://api.tfl.gov.uk/Line/victoria/Arrivals/940GZZLUWWL", "-o", "northbound.json"])

from sys import path
import link
path.append(link.getRequestsAddress())

train_data = {}
train_list = []

from requests import get
import nextStation

"""API_KEY = "580efc09100e43b6976feaefb4e33f12"
url = "https://api.tfl.gov.uk/Line/victoria/Arrivals"
params = {"app_key": API_KEY}

response = get(url, params=params)
data = response.json()

print("Trains: ")
for train in data:
    id = train.get("vehicleId", "Unknown")
    if id not in trainList:
        trainList.append(id)
    location = train.get("currentLocation", "No location available")
    stationName = train.get("stationName", "No Station Available")
    stationName = stationName.replace(" Underground Station", "")
    direction = train.get("towards", "No Direction Available")
    if direction == "Brixton":
        direction = "southbound"
    else:
        direction = "northbound"
    if "Leaving" in location or "Left" in location:
        print("train Leaving")
    elif "Between" in location or "Approaching" in location:

        time = train.get("timeToStation", "Unknown")
        if stationName in location:
            if id in trainData:
                if trainData[id][3] > time:
                     trainData[id][3] = time
            else:
                trainData.update({id : [stationName, direction, location, time]})
    elif "At" in location or "At Platform" == location:
        if stationName in location:
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
    print(f"Train {train_id} → {data[0]} | direction: {data[1]} | location: {data[2]} | time to station: {data[3]} / {trainTimes.getStationTime(data[1], data[0])}") """

def getStationFromLocation(location):
    buf = []
    and_count = 0
    for i in range(len(location)):
        buf.append(location[i])
        if location[i] == 'a' and and_count == 0:
            and_count += 1
        elif location[i] == 'n' and and_count == 1:
            and_count += 1
        elif location[i] == 'd' and and_count == 2:
            buf.append(location[i + 1])
            break
        else:
            and_count = 0
    remove = "".join(buf)
    return location.replace(remove, "")

API_KEY = "580efc09100e43b6976feaefb4e33f12"
base_url = "https://api.tfl.gov.uk/Line/victoria/Arrivals/"
northbound_url = "https://api.tfl.gov.uk/Line/victoria/Arrivals/940GZZLUWWL"
southbound_url = "https://api.tfl.gov.uk/Line/victoria/Arrivals/940GZZLUBXN"
params = {"app_key": API_KEY}

northbound_response = get(northbound_url, params=params)
northbound_data = northbound_response.json()

southbound_response = get(southbound_url, params=params)
southbound_data = southbound_response.json()

north_trains = {}
south_trains = {}
all_data = [[northbound_data, north_trains], [southbound_data, south_trains]]


for data in all_data:
    for train in data[0]:
        id = train.get("vehicleId", "No vehicle Id")
        if not id.isdigit():
            continue

        location = train.get("currentLocation", "Location unknown")
        station = location            
        timeToStation = 0
        state = "None"
        if "Between" in location:
            station = getStationFromLocation(location)
            state = "moving"
        elif "Approaching" in location:
            station = location.replace("Approaching ", "")
            state = "moving"
        elif "At Platform" in location:
            station = (train.get("stationName", "no station available")).replace(" Underground Station", "")
            state = "idle"
        elif "At" in location:
            station = location.replace("At ", "")
            state = "idle"

        station_url = base_url
        if state == "moving" and station in station_id_lookup.keys():
            timeToStation = 100000
            station_url += station_id_lookup[station]
            station_url += "?direction="
            station_url += "outbound" if data[0] == northbound_data else "inbound"
            station_request = get(station_url, params=params)
            station_data = station_request.json()
            for s in station_data:
                if id == s.get("vehicleId", "no id"):
                    t = s.get("timeToStation", "No Time Given")
                    if t < timeToStation:
                        timeToStation = t
        elif state == "moving" and (station == "Walthamstow Central" or station == "Brixton"):
            timeToStation = train.get("timeToStation", "No Time Given")
            
        if id not in data[1]:
            data[1].update({id : TrainData(station, timeToStation, state)})
        elif data[1][id].nextStation != station:
            print(f"Error 01: Different Location for train {id}")
        elif timeToStation < data[1][id].timeToStation: 
                data[1][id].timeToStation = timeToStation


import trainTimes

print("Northbound Trains")
for id, loc in north_trains.items():
        print(f"ID: {id} | {"next" if loc.state == "moving" else "current"} Stop: {loc.nextStation} | time to: {loc.timeToStation} / {trainTimes.getStationTime("northbound", loc.nextStation)}")

print("Southbound Trains")
for id, loc in south_trains.items():
     print(f"ID: {id} | {"next" if loc.state == "moving" else "current"} Stop: {loc.nextStation} | time to: {loc.timeToStation} / {trainTimes.getStationTime("southbound", loc.nextStation)}")

end = time.time()

print(f"Program took {end - start} seconds to run")



    
