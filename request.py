import time
start = time.time()

station_ids = { "Brixton" : "940GZZLUBXN",
                "Stockwell" : "940GZZLUSKW",
                "Vauxhall" : "940GZZLUVXL",
                "Pimlico" : "940GZZLUPCO",
                "Victoria" : "940GZZLUVIC",
                "Green Park" : "940GZZLUGPK",
                "Oxford Circus" : "940GZZLUOXC",
                "Warren Street" : "940GZZLUWRR",
                "Euston" : "940GZZLUEUS",
                "Kings Cross St. Pancras" : "940GZZLUKSX",
                "Highbury & Islington" : "940GZZLUHAI",
                "Finsbury Park" : "940GZZLUFPK",
                "Seven Sisters" : "940GZZLUSVS",
                "Tottenham Hale" : "940GZZLUTMH",
                "Blackhorse Road" : "940GZZLUBLR",
                "Walthamstow Central" : "940GZZLUWWL"}



class TrainData:
    def __init__(self, direction, nextStation, timeToStation, state):
        self.direction = direction
        self.nextStation = nextStation
        self.timeToStation = timeToStation
        self.state = state

import subprocess
subprocess.run(["curl", "https://api.tfl.gov.uk/Line/victoria/Arrivals/", "-o", "trains.json"])
# subprocess.run(["curl", "https://api.tfl.gov.uk/Line/victoria/Arrivals/940GZZLUBXN", "-o", "southbound.json"])
# subprocess.run(["curl", "https://api.tfl.gov.uk/Line/victoria/Arrivals/940GZZLUWWL", "-o", "northbound.json"])

from sys import path
import link
path.append(link.getRequestsAddress())

from requests import get
import nextStation

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
params = {"app_key": API_KEY}

train_data = {}

count = 0
for station_name, stop_id in station_ids.items():
    # request_start = time.time()
    url = base_url + stop_id
    request = get(url, params=params)
    data = request.json()
    # request_end = time.time()

    # print(f"Request took: {request_end - request_start}")

    temp_trainData = {}

    for train in data:
        id = train.get("vehicleId", "No vehicle Id")
        if not id.isdigit() or id in train_data.keys():
            continue

        location = train.get("currentLocation", "Location unknown")
        # print(location)
        station = location            
        timeToStation = 0
        state = "None"
        if "Between" in location:
            station = getStationFromLocation(location)
            state = "moving"
            timeToStation = train.get("timeToStation", "No time given")
        elif "Approaching" in location:
            station = location.replace("Approaching ", "")
            state = "moving"
            timeToStation = train.get("timeToStation", "No time given")
        elif "At Platform" in location:
            station = station_name
            state = "idle"
        elif "At" in location:
            station = location.replace("At ", "")
            state = "idle"
        elif "Departing" in location or "Departed" in location or "Left" in location: 
            destination = train.get("towards", "no destination")
            a = 1 if destination == "Brixton" else -1
            previousStation = list(station_ids.keys())[count + a]
            print(f"{id} | {destination} : {location} : {previousStation}")
            if previousStation in location:
                station = station_name
                state = "moving"
            else:
                continue
        elif "Area" in location and "Brixton" in location:
            station = "Brixton"
            state = "moving"
        else:
            print(f"Unknown Location Value: {location}")

        if station == "Highbury & Isl":
            station = "Highbury & Islington"
        elif station == "Kings Cross St. P":
            station = "Kings Cross St. Pancras"

        if station != station_name:
            continue

        direction = train.get("towards","No direction")

        direction = "northbound" if direction == "Walthamstow Central" else "southbound"

        if id not in temp_trainData.keys():
            temp_trainData.update({id : TrainData(direction, station, timeToStation, state)})
        elif temp_trainData[id].nextStation != station:
            print(f"Error 01: Different Location for train {id}")
        elif timeToStation < temp_trainData[id].timeToStation: 
                temp_trainData[id].timeToStation = timeToStation
    
    train_data.update(temp_trainData)
    count += 1


import trainTimes

print("Trains")
for id, loc in train_data.items():
        print(f"ID: {id} | {loc.direction} | {"next" if loc.state == "moving" else "current"} Stop: {loc.nextStation} | time to: {loc.timeToStation} / {trainTimes.getStationTime(loc.direction, loc.nextStation)}")

end = time.time()

print(f"Program took {end - start} seconds to run")



    
